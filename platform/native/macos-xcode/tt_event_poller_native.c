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

#include <sys/event.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

// - larger num could save time for calling kevent, but may descrease
//   timer accuracy
// - less num could increase overhead for calling kevent, but inscreases
//   timer accuracy
#define __KQ_EV_NUM (1)

#define __KEY2SKT_RD(key)                                                      \
    TT_CONTAINER(TT_CONTAINER(key, tt_skt_ntv_t, ev_mark_rd), tt_skt_t, sys_skt)
#define __KEY2SKT_WR(key)                                                      \
    TT_CONTAINER(TT_CONTAINER(key, tt_skt_ntv_t, ev_mark_wr), tt_skt_t, sys_skt)

#define __KEY2IPC_RD(key)                                                      \
    TT_CONTAINER(TT_CONTAINER(key, tt_ipc_ntv_t, ev_mark_rd), tt_ipc_t, sys_ipc)
#define __KEY2IPC_WR(key)                                                      \
    TT_CONTAINER(TT_CONTAINER(key, tt_ipc_ntv_t, ev_mark_wr), tt_ipc_t, sys_ipc)

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
    tt_result_t result;

    int kq_fd = sys_evc->kq_fd;
    struct kevent kq_ev[__KQ_EV_NUM];
    struct timespec __timeout, *timeout;
    int nfds;

    if (wait_ms != TT_TIME_INFINITE) {
        __timeout.tv_sec = wait_ms / 1000;
        __timeout.tv_nsec = (wait_ms % 1000) * 1000000;
        timeout = &__timeout;
    } else {
        timeout = NULL;
    }

    // poll
    nfds = kevent(kq_fd, NULL, 0, kq_ev, __KQ_EV_NUM, timeout);
    if (nfds > 0) {
        tt_u32_t i;
        for (i = 0; i < nfds; ++i) {
            switch (*((tt_u32_t *)kq_ev[i].udata)) {
                case TT_EVP_MARK_THREAD_EV: {
                    tt_ev_t *ev;

                    TT_ASSERT_EVPP(kq_ev[i].ident == TT_EVP_MARK_THREAD_EV);
                    TT_ASSERT_EVPP(kq_ev[i].filter == EVFILT_USER);

                    while ((ev = tt_evc_recvfrom_thread_ntv(evc)) != NULL) {
                        result = __evp_tev_dispatch(evp, ev);
                        if (!TT_OK(result)) {
                            return TT_FAIL;
                        }

                        // to keep timer accuracy, check if there is any timer
                        // expired when one event is processed
                        tt_tmr_mgr_run(&evc->tmr_mgr);
                    }
                } break;
                /*
            case TT_SKT_MARK_KQ_RD: {
                result = tt_skt_kq_rd_handler(__KEY2SKT_RD(kq_ev[i].udata),
                                              kq_ev[i].flags,
                                              kq_ev[i].fflags,
                                              kq_ev[i].data);
                if (!TT_OK(result)) {
                    return TT_FAIL;
                }
            } break;
            case TT_SKT_MARK_KQ_WR: {
                result = tt_skt_kq_wr_handler(__KEY2SKT_WR(kq_ev[i].udata),
                                              kq_ev[i].flags,
                                              kq_ev[i].fflags,
                                              kq_ev[i].data);
                if (!TT_OK(result)) {
                    return TT_FAIL;
                }
            } break;

                case TT_IPC_MARK_KQ_RD: {
                    result = tt_ipc_kq_rd_handler(__KEY2IPC_RD(kq_ev[i].udata),
                                                  kq_ev[i].flags,
                                                  kq_ev[i].fflags,
                                                  kq_ev[i].data);
                    if (!TT_OK(result)) {
                        return TT_FAIL;
                    }
                } break;
                case TT_IPC_MARK_KQ_WR: {
                    result = tt_ipc_kq_wr_handler(__KEY2IPC_WR(kq_ev[i].udata),
                                                  kq_ev[i].flags,
                                                  kq_ev[i].fflags,
                                                  kq_ev[i].data);
                    if (!TT_OK(result)) {
                        return TT_FAIL;
                    }
                } break;*/

                default: {
                    TT_FATAL("unknown event range: %d",
                             *((tt_u32_t *)kq_ev[i].udata));
                } break;
            }
        }
    } else if ((nfds != 0) && (errno != EINTR)) {
        TT_ERROR_NTV("kevent fail");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
