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

#include <io/tt_io_event.h>
#include <memory/tt_memory_alloc.h>
#include <os/tt_fiber.h>
#include <os/tt_task.h>

#include <tt_ipc_native.h>
#include <tt_socket_native.h>
#include <tt_sys_error.h>
#include <tt_util_native.h>

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

// larger __EP_EV_NUM could save time for calling kevent, but may
// descrease timer accuracy and vice vesa
#define __EP_EV_NUM 1

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef tt_bool_t (*__io_handler_t)(IN tt_io_ev_t *io_ev,
                                    IN tt_io_poller_ntv_t *sys_iop);

enum
{
    __POLLER_EXIT,

    __POLLER_EV_NUM,
};

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_bool_t __worker_io(IN tt_io_ev_t *io_ev,
                             IN tt_io_poller_ntv_t *sys_iop);

static tt_bool_t __poller_io(IN tt_io_ev_t *io_ev,
                             IN tt_io_poller_ntv_t *sys_iop);

static tt_bool_t __skt_io(IN tt_io_ev_t *io_ev, IN tt_io_poller_ntv_t *sys_iop);

static tt_bool_t __ipc_io(IN tt_io_ev_t *io_ev, IN tt_io_poller_ntv_t *sys_iop);

static __io_handler_t __io_handler[TT_IO_NUM] = {
    __worker_io, __poller_io, NULL, __skt_io, __ipc_io,
};

static tt_io_ev_t __s_poller_io_ev;

static tt_io_ev_t __s_worker_io_ev;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_io_poller_component_init_ntv()
{
    // poller io ev
    __s_poller_io_ev.src = NULL;
    __s_poller_io_ev.dst = NULL;
    tt_dnode_init(&__s_poller_io_ev.node);
    __s_poller_io_ev.io = TT_IO_POLLER;
    __s_poller_io_ev.ev = 0;

    // worker io ev
    __s_worker_io_ev.src = NULL;
    __s_worker_io_ev.dst = NULL;
    tt_dnode_init(&__s_worker_io_ev.node);
    __s_worker_io_ev.io = TT_IO_WORKER;
    __s_worker_io_ev.ev = 0;

    return TT_SUCCESS;
}

tt_result_t tt_io_poller_create_ntv(IN tt_io_poller_ntv_t *sys_iop)
{
    int ep, evfd;
    struct epoll_event ev = {0};

    tt_u32_t __done = 0;
#define __PC_P_LOCK (1 << 0)
#define __PC_W_LOCK (1 << 1)
#define __PC_EP (1 << 2)
#define __PC_P_EVFD (1 << 3)
#define __PC_W_EVFD (1 << 4)

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

    ep = epoll_create(10000);
    if (ep < 0) {
        TT_ERROR_NTV("fail to create epoll fd");
        goto fail;
    }
    sys_iop->ep = ep;
    __done |= __PC_EP;

    evfd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (evfd < 0) {
        TT_ERROR_NTV("fail to create poller event fd");
        goto fail;
    }
    sys_iop->poller_evfd = evfd;
    __done |= __PC_P_EVFD;

    ev.events = EPOLLIN;
    ev.data.ptr = &__s_poller_io_ev;
    if (epoll_ctl(ep, EPOLL_CTL_ADD, evfd, &ev) != 0) {
        TT_ERROR_NTV("fail to add poller event fd");
        goto fail;
    }

    evfd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (evfd < 0) {
        TT_ERROR_NTV("fail to create worker event fd");
        goto fail;
    }
    sys_iop->worker_evfd = evfd;
    __done |= __PC_W_EVFD;

    ev.events = EPOLLIN;
    ev.data.ptr = &__s_worker_io_ev;
    if (epoll_ctl(ep, EPOLL_CTL_ADD, evfd, &ev) != 0) {
        TT_ERROR_NTV("fail to add worker event fd");
        goto fail;
    }

    return TT_SUCCESS;

fail:
    if (__done & __PC_W_EVFD) {
        __RETRY_IF_EINTR(close(sys_iop->worker_evfd) != 0);
    }

    if (__done & __PC_P_EVFD) {
        __RETRY_IF_EINTR(close(sys_iop->poller_evfd) != 0);
    }

    if (__done & __PC_EP) {
        __RETRY_IF_EINTR(close(sys_iop->ep) != 0);
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
        TT_FATAL("poller ev list is not empty");
    }

    if (!tt_dlist_empty(&sys_iop->worker_ev)) {
        TT_FATAL("worker ev list is not empty");
    }

    tt_spinlock_destroy(&sys_iop->poller_lock);

    tt_spinlock_destroy(&sys_iop->worker_lock);

    __RETRY_IF_EINTR(close(sys_iop->ep) != 0);

    __RETRY_IF_EINTR(close(sys_iop->poller_evfd) != 0);

    __RETRY_IF_EINTR(close(sys_iop->worker_evfd) != 0);
}

tt_bool_t tt_io_poller_run_ntv(IN tt_io_poller_ntv_t *sys_iop,
                               IN tt_s64_t wait_ms)
{
    int timeout;
    struct epoll_event ev[__EP_EV_NUM];
    int nev;

    if (wait_ms == TT_TIME_INFINITE) {
        timeout = -1;
    } else if (wait_ms >= 0x7FFFFFFF) {
        timeout = -1;
    } else {
        timeout = (int)wait_ms;
    }

    nev = epoll_wait(sys_iop->ep, ev, __EP_EV_NUM, timeout);
    if (nev > 0) {
        int i;
        for (i = 0; i < nev; ++i) {
            tt_io_ev_t *io_ev = (tt_io_ev_t *)ev[i].data.ptr;

            if (!__io_handler[io_ev->io](io_ev, sys_iop)) {
                return TT_FALSE;
            }
        }
    } else if ((nev != 0) && (errno != EINTR)) {
        TT_ERROR_NTV("epoll fail");
        return TT_FALSE;
    }

    return TT_TRUE;
}

tt_result_t tt_io_poller_exit_ntv(IN tt_io_poller_ntv_t *sys_iop)
{
    tt_io_ev_t *io_ev;
    const uint64_t sig = 1;

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

again:
    if (write(sys_iop->poller_evfd, &sig, sizeof(uint64_t)) ==
        sizeof(uint64_t)) {
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
    const uint64_t sig = 1;

    tt_spinlock_acquire(&sys_iop->worker_lock);
    tt_dlist_push_tail(&sys_iop->worker_ev, &io_ev->node);
    tt_spinlock_release(&sys_iop->worker_lock);

again:
    if (write(sys_iop->worker_evfd, &sig, sizeof(uint64_t)) ==
        sizeof(uint64_t)) {
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
    const uint64_t sig = 1;

    tt_spinlock_acquire(&sys_iop->poller_lock);
    tt_dlist_push_tail(&sys_iop->poller_ev, &io_ev->node);
    tt_spinlock_release(&sys_iop->poller_lock);

again:
    if (write(sys_iop->poller_evfd, &sig, sizeof(uint64_t)) ==
        sizeof(uint64_t)) {
        return TT_SUCCESS;
    } else if (errno == EINTR) {
        goto again;
    } else {
        TT_ERROR_NTV("fail to send to poller");
        return TT_FAIL;
    }
}

tt_bool_t __worker_io(IN tt_io_ev_t *dummy, IN tt_io_poller_ntv_t *sys_iop)
{
    uint64_t num;
    tt_dlist_t dl;
    tt_dnode_t *node;

    read(sys_iop->worker_evfd, &num, sizeof(uint64_t));

    tt_dlist_init(&dl);
    tt_spinlock_acquire(&sys_iop->worker_lock);
    tt_dlist_move(&dl, &sys_iop->worker_ev);
    tt_spinlock_release(&sys_iop->worker_lock);

    while ((node = tt_dlist_pop_head(&dl)) != NULL) {
        tt_io_ev_t *io_ev = TT_CONTAINER(node, tt_io_ev_t, node);

        TT_ASSERT(io_ev->src != NULL);
        tt_fiber_resume(io_ev->src, TT_TRUE);
    }

    return TT_TRUE;
}

tt_bool_t __poller_io(IN tt_io_ev_t *dummy, IN tt_io_poller_ntv_t *sys_iop)
{
    uint64_t num;
    tt_dlist_t dl;
    tt_dnode_t *node;

    read(sys_iop->poller_evfd, &num, sizeof(uint64_t));

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

tt_bool_t __skt_io(IN tt_io_ev_t *io_ev, IN tt_io_poller_ntv_t *sys_iop)
{
    if (tt_skt_poller_io(io_ev)) {
        TT_ASSERT(io_ev->src != NULL);
        tt_fiber_resume(io_ev->src, TT_TRUE);
    }

    return TT_TRUE;
}

tt_bool_t __ipc_io(IN tt_io_ev_t *io_ev, IN tt_io_poller_ntv_t *sys_iop)
{
    if (tt_ipc_poller_io(io_ev)) {
        TT_ASSERT(io_ev->src != NULL);
        tt_fiber_resume(io_ev->src, TT_TRUE);
    }

    return TT_TRUE;
}
