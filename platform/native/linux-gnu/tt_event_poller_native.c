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

#include <tt_event_poller_native.h>

#include <event/tt_event_center.h>
#include <event/tt_event_poller.h>
//#include <io/tt_file_system_aio.h>
//#include <io/tt_ipc_aio.h>
//#include <io/tt_socket_aio.h>
#include <misc/tt_error.h>

#include <sys/epoll.h>
#include <unistd.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

// - larger num could save time for calling kevent, but may descrease
//   timer accuracy
// - less num could increase overhead for calling kevent, but inscreases
//   timer accuracy
#define __EP_EV_NUM (1)

#if 1
#define TT_ASSERT_EVPP TT_ASSERT
#else
#define TT_ASSERT_EVPP(...)
#endif

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern tt_result_t __evp_tev_dispatch(IN tt_evpoller_t *evp, IN tt_ev_t *ev);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_evp_create_ntv(IN tt_evpoller_ntv_t *sys_evp,
                              IN struct tt_evc_attr_s *evc_attr)
{
    return TT_SUCCESS;
}

tt_result_t tt_evp_destroy_ntv(IN tt_evpoller_ntv_t *sys_evp)
{
    return TT_SUCCESS;
}

tt_result_t tt_evp_poll_ntv(IN struct tt_evpoller_s *evp, IN tt_s64_t wait_ms)
{
    tt_evpoller_ntv_t *sys_evp = &evp->sys_evp;
    tt_evcenter_t *evc = evp->evc;
    tt_evcenter_ntv_t *sys_evc = &evc->sys_evc;

    int ep_fd = sys_evc->ep_fd;
    struct epoll_event ep_ev[__EP_EV_NUM];
    int nfds;
    int timeout;

    if (wait_ms == TT_TIME_INFINITE) {
        timeout = -1;
    } else if (wait_ms >= 0x7FFFFFFF) {
        timeout = -1;
    } else {
        timeout = (int)wait_ms;
    }

    // wait
    nfds = epoll_wait(ep_fd, ep_ev, __EP_EV_NUM, timeout);
    if (nfds > 0) {
        tt_u32_t i;
        for (i = 0; i < nfds; ++i) {
            switch (*((tt_u32_t *)ep_ev[i].data.ptr)) {
                case TT_EVP_MARK_THREAD_EV: {
                    uint64_t sig;
                    tt_ev_t *ev;

                    TT_ASSERT(read(sys_evc->ep_ev_fd, &sig, sizeof(uint64_t)) ==
                              sizeof(uint64_t));
                    TT_ASSERT(sig == 1);

                    ev = tt_evc_recvfrom_thread_ntv(evc);
                    TT_ASSERT(ev != NULL);
                    if (!TT_OK(__evp_tev_dispatch(evp, ev))) {
                        return TT_FAIL;
                    }
                } break;
                /*case TT_EVP_MARK_EPOLL_SOCKET: {
                    tt_skt_ntv_t *sys_skt =
                        TT_CONTAINER(ep_ev[i].data.ptr, tt_skt_ntv_t, ev_mark);
                    tt_skt_t *skt = TT_CONTAINER(sys_skt, tt_skt_t, sys_skt);

                    if (!TT_OK(tt_skt_ep_handler(skt, ep_ev[i].events))) {
                        return TT_FAIL;
                    }
                } break;
                case TT_EVP_MARK_EPOLL_IPC: {
                    tt_ipc_ntv_t *sys_ipc =
                        TT_CONTAINER(ep_ev[i].data.ptr, tt_ipc_ntv_t, ev_mark);
                    tt_ipc_t *ipc = TT_CONTAINER(sys_ipc, tt_ipc_t, sys_ipc);

                    if (!TT_OK(tt_ipc_ep_handler(ipc, ep_ev[i].events))) {
                        return TT_FAIL;
                    }
                } break;*/

                default: {
                    TT_FATAL("unknown event range: %x",
                             *((tt_u32_t *)ep_ev[i].data.ptr));
                } break;
            }

            // new timers may be created during processing events, so check
            // timers when one event is processed
            tt_tmr_mgr_run(&evc->tmr_mgr);
        }
    } else if ((nfds != 0) && (errno != EINTR)) {
        TT_ERROR_NTV("epoll_wait fail");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
