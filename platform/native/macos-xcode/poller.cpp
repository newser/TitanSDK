/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/native/poller.h>

#include <misc/tt_util.h>
#include <tt/io/event.h>
#include <tt/log/manager.h>
#include <tt/misc/rollback.h>
#include <tt/os/fiber.h>
#include <tt/time/def.h>

#include <sys/errno.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

namespace tt::native {

class poller_ev: public io::ev
{
public:
    enum
    {
        e_wakeup,
        e_exit,

        id_num
    };

    poller_ev(uint32_t id): io::ev(io::e_poller, id) { assert(id < id_num); }

    std::pair<bool, bool> handle(native::poller &p) override;
};

class worker_ev: public io::ev
{
public:
    enum
    {
        e_wakeup,
        e_exit,

        id_num
    };

    worker_ev(uint32_t id): io::ev(io::e_worker, id) { assert(id < id_num); }

    std::pair<bool, bool> handle(native::poller &p) override;
};

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static poller_ev s_poller_ev{poller_ev::e_wakeup};

static worker_ev s_worker_ev{worker_ev::e_wakeup};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

poller::~poller()
{
    while (!poller_ev_.empty()) {
        io::ev *ev = poller_ev_.front();
        poller_ev_.pop_front();

        if (ev->is(io::e_poller, poller_ev::e_exit)) {
            // saft to delete poller exit
            delete ev;
        }
    }

    if (!worker_ev_.empty()) {
        log::fatal("poller worker_ev list is not empty");
    }

    close(kq_);
}

bool poller::init()
{
    kq_ = kqueue();
    if (kq_ < 0) { return false; }
    auto _1 = make_rollback([this]() {
        close(kq_);
        kq_ = -1;
    });

    struct kevent kev;
    EV_SET(&kev, io::e_poller, EVFILT_USER, EV_ADD | EV_CLEAR, 0, 0,
           &s_poller_ev);
ag_poller:
    if (kevent(kq_, &kev, 1, nullptr, 0, nullptr) != 0) {
        if (errno == EINTR) { goto ag_poller; }
        return false;
    }

    EV_SET(&kev, io::e_worker, EVFILT_USER, EV_ADD | EV_CLEAR, 0, 0,
           &s_worker_ev);
ag_worker:
    if (kevent(kq_, &kev, 1, nullptr, 0, nullptr) != 0) {
        if (errno == EINTR) { goto ag_worker; }
        return false;
    }

    _1.dismiss();
    return true;
}

bool poller::run(uint64_t wait_ms)
{
    struct timespec t, *timeout;
    struct kevent kev[k_kev_num];
    int nev;

    if (wait_ms != time::k_inf) {
        t.tv_sec = wait_ms / 1000;
        t.tv_nsec = (wait_ms % 1000) * 1000000;
        timeout = &t;
    } else {
        timeout = nullptr;
    }

    nev = kevent(kq_, nullptr, 0, kev, k_kev_num, timeout);
    if (nev > 0) {
        for (int i = 0; i < nev; ++i) {
            io::ev *ev = (io::ev *)kev[i].udata;
            uint16_t flags = kev[i].flags;

            if (flags & EV_ERROR) {
                ev->err(err::e_fail);
            } else if (flags & EV_EOF) {
                ev->err(err::e_end);
            } else {
                ev->err(err::e_ok);
            }

            ev->kev_flags(flags);

            auto [del, end] = ev->handle(*this);
            if (del) { delete ev; }
            if (end) { return false; }
        }
    } else if ((nev != 0) && (errno != EINTR)) {
        // TT_ERROR_NTV("kevent fail");
        return false;
    }

    return true;
}

bool poller::exit()
{
    struct poller_exit: public poller_ev
    {
        poller_exit(): poller_ev(poller_ev::e_exit) {}

        std::pair<bool, bool> handle(native::poller &p) override
        {
            return std::make_pair(false, true);
        }
    };
    static poller_exit ev;

    {
        std::scoped_lock<tt::spinlock> s(poller_lock_);
        poller_ev_.push_back(&ev);
    }

    struct kevent kev;
    EV_SET(&kev, io::e_poller, EVFILT_USER, 0, NOTE_TRIGGER, 0, &s_poller_ev);
ag:
    if (kevent(kq_, &kev, 1, NULL, 0, NULL) == 0) {
        return true;
    } else if (errno == EINTR) {
        goto ag;
    } else {
        // TT_ERROR_NTV("fail to send poller exit");
        // no need to care io_ev, as it may already be processed
        return false;
    }
}

bool poller::worker_done(io::ev &ev)
{
    {
        std::scoped_lock<tt::spinlock> s(worker_lock_);
        worker_ev_.push_back(&ev);
    }

    struct kevent kev;
    EV_SET(&kev, io::e_worker, EVFILT_USER, 0, NOTE_TRIGGER, 0, &s_worker_ev);
ag:
    if (kevent(kq_, &kev, 1, NULL, 0, NULL) == 0) {
        return true;
    } else if (errno == EINTR) {
        goto ag;
    } else {
        // TT_ERROR_NTV("fail to send worker exit");
        // no need to care io_ev, as it may already be processed
        return false;
    }
}

bool poller::recv(io::ev &ev)
{
    {
        std::scoped_lock<tt::spinlock> s(poller_lock_);
        poller_ev_.push_back(&ev);
    }

    struct kevent kev;
    EV_SET(&kev, io::e_poller, EVFILT_USER, 0, NOTE_TRIGGER, 0, &s_poller_ev);
ag:
    if (kevent(kq_, &kev, 1, NULL, 0, NULL) == 0) {
        return true;
    } else if (errno == EINTR) {
        goto ag;
    } else {
        // TT_ERROR_NTV("fail to send poller exit");
        // no need to care io_ev, as it may already be processed
        return false;
    }
}

std::pair<bool, bool> poller_ev::handle(native::poller &p)
{
    std::list<io::ev *> l;
    p.get_poller_ev(l);

    while (!l.empty()) {
        io::ev *ev = l.front();
        l.pop_front();

        auto [del, end] = ev->handle(p);
        if (del) { delete ev; }
        if (end) { return std::make_pair(false, true); }

#if 0
        if (io_ev->io == TT_IO_POLLER) {
            if (io_ev->ev == __POLLER_EXIT) {
                tt_free(io_ev);
                return TT_FALSE;
            }
            
            if (io_ev->src != NULL) {
                tt_task_finish(io_ev->dst->fs->thread->task, io_ev);
            } else {
                tt_free(io_ev);
            }
        } else if (io_ev->io == TT_IO_FIBER) {
            tt_fiber_t *dst = io_ev->dst;
            TT_ASSERT(&dst->fs->thread->task->iop.sys_iop == sys_iop);
            tt_dlist_push_tail(&dst->ev, &io_ev->node);
            if (dst->recving) { tt_fiber_resume(dst, TT_FALSE); }
        } else {
            TT_ASSERT(io_ev->io == TT_IO_TASK);
            tt_task_poller_io(io_ev);
        }
#endif
    }

    return std::make_pair(false, false);
}

std::pair<bool, bool> worker_ev::handle(native::poller &p)
{
    std::list<io::ev *> l;
    p.get_worker_ev(l);

    while (!l.empty()) {
        io::ev *ev = l.front();
        l.pop_front();

        fiber *fb = ev->src_fiber();
        assert(fb != nullptr);
        fiber_mgr &fb_mgr = fb->mgr();
        assert(fb_mgr.current_is_main());
        fb->resume_cautious(fb_mgr.main(), false);
    }

    return std::make_pair(false, false);
}

}
