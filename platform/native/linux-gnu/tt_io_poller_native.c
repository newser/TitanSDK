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

#include <os/tt_fiber.h>
#include <io/tt_io_event.h>

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

enum
{
    __POLLER_YIELD,
    __POLLER_EXIT,

    __POLLER_EV_NUM,
};

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_io_handler_t __io_handler[TT_IO_NUM] = {
    NULL, NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_io_poller_create_ntv(IN tt_io_poller_ntv_t *sys_iop)
{
    tt_queue_attr_t evq_attr;
    int ep, evfd;
    struct epoll_event ev = {0};

    tt_queue_attr_default(&evq_attr);
    evq_attr.obj_per_frame = 128;
    tt_queue_init(&sys_iop->evq, sizeof(tt_u8_t), &evq_attr);
    
    ep = epoll_create(10000);
    if (ep < 0) {
        TT_ERROR_NTV("fail to create epoll fd");
        return TT_FAIL;
    }    
    sys_iop->ep = ep;

    evfd = eventfd(0, EFD_SEMAPHORE);
    if (evfd < 0) {
        TT_ERROR_NTV("fail to create event fd");
        __RETRY_IF_EINTR(close(ep) != 0);
        return TT_FAIL;
    }
    sys_iop->evfd = evfd;

    ev.events = EPOLLIN;
    ev.data.ptr = &sys_iop->io;
    if (epoll_ctl(ep, EPOLL_CTL_ADD, evfd, &ev) != 0) {
        TT_ERROR_NTV("fail to add event fd");
        __RETRY_IF_EINTR(close(evfd) != 0);
        __RETRY_IF_EINTR(close(ep) != 0);
        return TT_FAIL;
    }

    sys_iop->io = TT_IO_POLLER;

    if (!TT_OK(tt_spinlock_create(&sys_iop->lock, NULL))) {
        TT_ERROR("fail to create io poller lock");
        __RETRY_IF_EINTR(close(evfd) != 0);
        __RETRY_IF_EINTR(close(ep) != 0);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_io_poller_destroy_ntv(IN tt_io_poller_ntv_t *sys_iop)
{
    tt_queue_destroy(&sys_iop->evq);
    
    __RETRY_IF_EINTR(close(sys_iop->ep) != 0);
    
    __RETRY_IF_EINTR(close(sys_iop->evfd) != 0);

    tt_spinlock_destroy(&sys_iop->lock);
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
            tt_u32_t *io = (tt_u32_t*)ev[i].data.ptr;
            if (*io == TT_IO_POLLER) {            
                uint64_t sig;
                tt_u8_t ev = __POLLER_EV_NUM;
                
                read(sys_iop->evfd, &sig, sizeof(uint64_t));
                TT_ASSERT(sig == 1);

                tt_spinlock_acquire(&sys_iop->lock);
                tt_queue_pop(&sys_iop->evq, &ev);
                tt_spinlock_release(&sys_iop->lock);
                if (ev == __POLLER_YIELD) {
                    tt_fiber_yield();
                }else if (ev == __POLLER_EXIT) {
                    return TT_FALSE;
                }
            } else {
                TT_ASSERT(TT_IO_VALID(*io));
                __io_handler[*io](TT_CONTAINER(io, tt_io_ev_t, io));
            }
        }
    } else if ((nev != 0) && (errno != EINTR)) {
        TT_ERROR_NTV("epoll fail");
        return TT_FALSE;
    }

    return TT_TRUE;
}

void tt_io_poller_yield_ntv(IN tt_io_poller_ntv_t *sys_iop)
{
    const uint64_t sig = 1;
    tt_u8_t ev = __POLLER_YIELD;
    tt_bool_t pushed;
    
    tt_spinlock_acquire(&sys_iop->lock);
    pushed = TT_OK(tt_queue_push(&sys_iop->evq, &ev));
    tt_spinlock_release(&sys_iop->lock);
    if (!pushed) {
        TT_FATAL("fail to push poller yield event");
        return;
    }
    
again:
    if (write(sys_iop->evfd, &sig, sizeof(uint64_t)) != sizeof(uint64_t)) {
        TT_ERROR_NTV("ep write failed");
        TT_ASSERT_ALWAYS(errno == EINTR);
        goto again;
    }
}

void tt_io_poller_exit_ntv(IN tt_io_poller_ntv_t *sys_iop)
{
    const uint64_t sig = 1;
    tt_u8_t ev = __POLLER_EXIT;
    tt_bool_t pushed;
    
    tt_spinlock_acquire(&sys_iop->lock);
    pushed = TT_OK(tt_queue_push(&sys_iop->evq, &ev));
    tt_spinlock_release(&sys_iop->lock);
    if (!pushed) {
        TT_FATAL("fail to push poller exit event");
        return;
    }
    
again:
    if (write(sys_iop->evfd, &sig, sizeof(uint64_t)) != sizeof(uint64_t)) {
        TT_ERROR_NTV("ep write failed");
        TT_ASSERT_ALWAYS(errno == EINTR);
        goto again;
    }
}

