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

#include <tt_event_center_native.h>

#include <event/tt_event_center.h>
#include <init/tt_component.h>
#include <misc/tt_util.h>

#include <tt_sys_error.h>
#include <tt_util_native.h>

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_evc_create_ntv(IN tt_evcenter_ntv_t *sys_evc,
                              IN struct tt_evc_attr_s *attr)
{
    int ep_fd = -1;
    int ep_ev_fd = -1;
    struct epoll_event ev = {0};

    TT_ASSERT(sys_evc != NULL);
    TT_ASSERT(attr != NULL);

    // create epoll read fd
    ep_fd = epoll_create(10000);
    if (ep_fd < 0) {
        TT_ERROR_NTV("fail to create epoll fd");
        goto evcc_fail;
    }
    sys_evc->ep_fd = ep_fd;

    sys_evc->ev_mark_thread_ev = TT_EVP_MARK_THREAD_EV;

    // create event fd
    // ep_ev_fd = eventfd(0, EFD_NONBLOCK| EFD_SEMAPHORE);
    ep_ev_fd = eventfd(0, EFD_SEMAPHORE);
    if (ep_ev_fd < 0) {
        TT_ERROR_NTV("fail to create epoll event fd");
        goto evcc_fail;
    }
    sys_evc->ep_ev_fd = ep_ev_fd;

    // add ev fd to epoll
    ev.events = EPOLLIN;
    ev.data.ptr = &sys_evc->ev_mark_thread_ev;
    if (epoll_ctl(sys_evc->ep_fd, EPOLL_CTL_ADD, ep_ev_fd, &ev) != 0) {
        TT_ERROR_NTV("fail to add epoll event fd");
        goto evcc_fail;
    }

    return TT_SUCCESS;

evcc_fail:

    if (ep_ev_fd >= 0) {
        __RETRY_IF_EINTR(close(ep_ev_fd) != 0);
    }

    if (ep_fd >= 0) {
        __RETRY_IF_EINTR(close(ep_fd) != 0);
    }

    return TT_FAIL;
}

tt_result_t tt_evc_destroy_ntv(IN tt_evcenter_ntv_t *sys_evc)
{
    TT_ASSERT(sys_evc != NULL);

    __RETRY_IF_EINTR(close(sys_evc->ep_ev_fd) != 0);

    __RETRY_IF_EINTR(close(sys_evc->ep_fd) != 0);

    return TT_SUCCESS;
}

tt_result_t tt_evc_sendto_thread_ntv(IN struct tt_evcenter_s *dst_evc,
                                     IN tt_ev_t *ev)
{
    tt_evcenter_ntv_t *sys_evc = &dst_evc->sys_evc;
    tt_thread_ev_t *tev = TT_EV_HDR(ev, tt_thread_ev_t);
    tt_result_t result = TT_SUCCESS;
    const uint64_t sig = 1;

    TT_ASSERT(dst_evc != NULL);
    TT_ASSERT(ev != NULL);

    __EVC_LOCK_EVQ(dst_evc);

    // add to q
    tt_list_push_tail(&dst_evc->ev_q, &tev->node);

// notify evcenter
__retry_wr:
    if (write(sys_evc->ep_ev_fd, &sig, sizeof(uint64_t)) != sizeof(uint64_t)) {
        if (errno == EINTR) {
            goto __retry_wr;
        } else {
            TT_ERROR_NTV("fail to post thread ev");
            result = TT_FAIL;
        }
    }

    // check if evcenter has been notified
    if (!TT_OK(result)) {
        tt_list_remove(&tev->node);
    }

    __EVC_UNLOCK_EVQ(dst_evc);

    return result;
}

tt_ev_t *tt_evc_recvfrom_thread_ntv(IN struct tt_evcenter_s *evc)
{
    tt_lnode_t *node;
    tt_thread_ev_t *tev = NULL;

    __EVC_LOCK_EVQ(evc);
    node = tt_list_pop_head(&evc->ev_q);
    if (node != NULL) {
        tev = TT_CONTAINER(node, tt_thread_ev_t, node);
    }
    __EVC_UNLOCK_EVQ(evc);

    if (tev != NULL) {
        return TT_EV_OF(tev);
    } else {
        return NULL;
    }
}
