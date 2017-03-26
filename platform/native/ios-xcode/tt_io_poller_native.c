/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_io_poller_native.h>

#include <memory/tt_memory_alloc.h>
#include <os/tt_fiber.h>
#include <os/tt_task.h>

#include <tt_sys_error.h>
#include <tt_util_native.h>

#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

// larger __KQ_EV_NUM could save time for calling kevent, but may
// descrease timer accuracy and vice vesa
#define __KQ_EV_NUM 1

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef tt_bool_t (*__io_handler_t)(IN tt_io_poller_ntv_t *sys_iop);

enum
{
    __POLLER_EXIT,

    __POLLER_EV_NUM
};

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_bool_t __worker_io(IN tt_io_poller_ntv_t *sys_iop);

static tt_bool_t __poller_io(IN tt_io_poller_ntv_t *sys_iop);

static __io_handler_t __io_handler[TT_IO_NUM] = {
    __worker_io, __poller_io,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_io_poller_component_init_ntv()
{
    return TT_SUCCESS;
}

tt_result_t tt_io_poller_create_ntv(IN tt_io_poller_ntv_t *sys_iop)
{
    int kq;
    struct kevent kev;

    tt_u32_t __done = 0;
#define __PC_P_LOCK (1 << 0)
#define __PC_W_LOCK (1 << 1)
#define __PC_KQ (1 << 2)

    tt_dlist_init(&sys_iop->poller_ev);
    tt_dlist_init(&sys_iop->worker_ev);

    if (!TT_OK(tt_spinlock_create(&sys_iop->poller_lock, NULL))) {
        TT_ERROR("fail to create poller poller_lock");
        goto fail;
    }
    __done |= __PC_P_LOCK;

    if (!TT_OK(tt_spinlock_create(&sys_iop->worker_lock, NULL))) {
        TT_ERROR("fail to create poller worker_lock");
        goto fail;
    }
    __done |= __PC_W_LOCK;

    kq = kqueue();
    if (kq < 0) {
        TT_ERROR_NTV("fail to create kqueue fd");
        goto fail;
    }
    sys_iop->kq = kq;
    __done |= __PC_KQ;

    EV_SET(&kev, TT_IO_POLLER, EVFILT_USER, EV_ADD | EV_CLEAR, 0, 0, NULL);
again_p:
    if (kevent(kq, &kev, 1, NULL, 0, NULL) != 0) {
        if (errno == EINTR) {
            goto again_p;
        }

        TT_ERROR_NTV("fail to add rd thread ev mark");
        goto fail;
    }

    EV_SET(&kev, TT_IO_WORKER, EVFILT_USER, EV_ADD | EV_CLEAR, 0, 0, NULL);
again_w:
    if (kevent(kq, &kev, 1, NULL, 0, NULL) != 0) {
        if (errno == EINTR) {
            goto again_w;
        }

        TT_ERROR_NTV("fail to add rd thread ev mark");
        goto fail;
    }

    return TT_SUCCESS;

fail:
    if (__done & __PC_KQ) {
        __RETRY_IF_EINTR(close(sys_iop->kq) != 0);
    }

    if (__done & __PC_W_LOCK) {
        tt_spinlock_destroy(&sys_iop->worker_lock);
    }

    if (__done & __PC_P_LOCK) {
        tt_spinlock_destroy(&sys_iop->poller_lock);
    }

    return TT_FAIL;
}

void tt_io_poller_destroy_ntv(IN tt_io_poller_ntv_t *sys_iop)
{
    if (!tt_dlist_empty(&sys_iop->poller_ev)) {
        TT_FATAL("poller poller_ev list is not empty");
    }

    if (!tt_dlist_empty(&sys_iop->worker_ev)) {
        TT_FATAL("poller worker_ev list is not empty");
    }

    tt_spinlock_destroy(&sys_iop->poller_lock);

    tt_spinlock_destroy(&sys_iop->worker_lock);

    __RETRY_IF_EINTR(close(sys_iop->kq) != 0);
}

tt_bool_t tt_io_poller_run_ntv(IN tt_io_poller_ntv_t *sys_iop,
                               IN tt_s64_t wait_ms)
{
    struct timespec t, *timeout;
    struct kevent kev[__KQ_EV_NUM];
    int nev;

    if (wait_ms != TT_TIME_INFINITE) {
        t.tv_sec = wait_ms / 1000;
        t.tv_nsec = (wait_ms % 1000) * 1000000;
        timeout = &t;
    } else {
        timeout = NULL;
    }

    nev = kevent(sys_iop->kq, NULL, 0, kev, __KQ_EV_NUM, timeout);
    if (nev > 0) {
        tt_u32_t i;
        for (i = 0; i < nev; ++i) {
            if (!__io_handler[kev[i].ident](sys_iop)) {
                return TT_FALSE;
            }
        }
    } else if ((nev != 0) && (errno != EINTR)) {
        TT_ERROR_NTV("kevent fail");
        return TT_FALSE;
    }

    return TT_TRUE;
}

tt_result_t tt_io_poller_exit_ntv(IN tt_io_poller_ntv_t *sys_iop)
{
    tt_io_ev_t *io_ev;
    struct kevent kev;

    io_ev = tt_malloc(sizeof(tt_io_ev_t));
    if (io_ev == NULL) {
        TT_ERROR("no mem for poller exit ev");
        return TT_FAIL;
    }

    io_ev->src = NULL;
    io_ev->dst = NULL;
    tt_dnode_init(&io_ev->node);
    io_ev->io = TT_IO_POLLER;
    io_ev->ev = __POLLER_EXIT;

    tt_spinlock_acquire(&sys_iop->poller_lock);
    tt_dlist_push_tail(&sys_iop->poller_ev, &io_ev->node);
    tt_spinlock_release(&sys_iop->poller_lock);

    EV_SET(&kev, TT_IO_POLLER, EVFILT_USER, 0, NOTE_TRIGGER, 0, NULL);

again:
    if (kevent(sys_iop->kq, &kev, 1, NULL, 0, NULL) == 0) {
        return TT_SUCCESS;
    } else if (errno == EINTR) {
        goto again;
    } else {
        TT_ERROR_NTV("fail to send poller exit");
        // no need to care io_ev, as it may already be processed
        return TT_FAIL;
    }
}

tt_result_t tt_io_poller_finish_ntv(IN tt_io_poller_ntv_t *sys_iop,
                                    IN tt_io_ev_t *io_ev)
{
    struct kevent kev;

    tt_spinlock_acquire(&sys_iop->worker_lock);
    tt_dlist_push_tail(&sys_iop->worker_ev, &io_ev->node);
    tt_spinlock_release(&sys_iop->worker_lock);

    EV_SET(&kev, TT_IO_WORKER, EVFILT_USER, 0, NOTE_TRIGGER, 0, NULL);

again:
    if (kevent(sys_iop->kq, &kev, 1, NULL, 0, NULL) == 0) {
        return TT_SUCCESS;
    } else if (errno == EINTR) {
        goto again;
    } else {
        TT_ERROR_NTV("fail to send poller finish");
        return TT_FAIL;
    }
}

tt_result_t tt_io_poller_send_ntv(IN tt_io_poller_ntv_t *sys_iop,
                                  IN tt_io_ev_t *io_ev)
{
    struct kevent kev;

    tt_spinlock_acquire(&sys_iop->poller_lock);
    tt_dlist_push_tail(&sys_iop->poller_ev, &io_ev->node);
    tt_spinlock_release(&sys_iop->poller_lock);

    EV_SET(&kev, TT_IO_POLLER, EVFILT_USER, 0, NOTE_TRIGGER, 0, NULL);

again:
    if (kevent(sys_iop->kq, &kev, 1, NULL, 0, NULL) == 0) {
        return TT_SUCCESS;
    } else if (errno == EINTR) {
        goto again;
    } else {
        TT_ERROR_NTV("fail to send to poller");
        return TT_FAIL;
    }
}

tt_bool_t __worker_io(IN tt_io_poller_ntv_t *sys_iop)
{
    tt_dlist_t dl;
    tt_dnode_t *node;

    tt_dlist_init(&dl);
    tt_spinlock_acquire(&sys_iop->worker_lock);
    tt_dlist_move(&dl, &sys_iop->worker_ev);
    tt_spinlock_release(&sys_iop->worker_lock);

    while ((node = tt_dlist_pop_head(&dl)) != NULL) {
        tt_io_ev_t *io_ev = TT_CONTAINER(node, tt_io_ev_t, node);

        TT_ASSERT(io_ev->src != NULL);
        tt_fiber_resume(io_ev->src);
    }

    return TT_TRUE;
}

tt_bool_t __poller_io(IN tt_io_poller_ntv_t *sys_iop)
{
    tt_dlist_t dl;
    tt_dnode_t *node;

    tt_dlist_init(&dl);
    tt_spinlock_acquire(&sys_iop->poller_lock);
    tt_dlist_move(&dl, &sys_iop->poller_ev);
    tt_spinlock_release(&sys_iop->poller_lock);

    while ((node = tt_dlist_pop_head(&dl)) != NULL) {
        tt_io_ev_t *io_ev = TT_CONTAINER(node, tt_io_ev_t, node);

        if ((io_ev->io == TT_IO_POLLER) && (io_ev->ev == __POLLER_EXIT)) {
            tt_free(io_ev);
            return TT_FALSE;
        }

        // a message to this fiber
        TT_ASSERT(&io_ev->dst->fs->thread->task->iop.sys_iop == sys_iop);
        // todo: add to fiber and awake the fiber if needed

        if (io_ev->src != NULL) {
            tt_task_finish(io_ev->dst->fs->thread->task, io_ev);
        } else {
            tt_free(io_ev);
        }
    }

    return TT_TRUE;
}
