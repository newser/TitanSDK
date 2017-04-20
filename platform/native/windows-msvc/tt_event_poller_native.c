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

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __KEY2FILE(key)                                                        \
    TT_CONTAINER(TT_CONTAINER(key, tt_file_ntv_t, ev_mark), tt_file_t, sys_file)
#define __KEY2DIR(key)                                                         \
    TT_CONTAINER(TT_CONTAINER(key, tt_dir_ntv_t, ev_mark), tt_dir_t, sys_dir)
#define __KEY2SKT(key)                                                         \
    TT_CONTAINER(TT_CONTAINER(key, tt_skt_ntv_t, ev_mark), tt_skt_t, sys_skt)
#define __KEY2IPC(key)                                                         \
    TT_CONTAINER(TT_CONTAINER(key, tt_ipc_ntv_t, ev_mark), tt_ipc_t, sys_ipc)

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

    DWORD NumberOfBytes;
    ULONG_PTR CompletionKey;
    OVERLAPPED *Overlapped;
    DWORD dwMilliseconds;
    BOOL ret;

    if ((wait_ms == TT_TIME_INFINITE) || (wait_ms > (tt_s64_t)0x7FFFFFFF)) {
        dwMilliseconds = INFINITE;
    } else {
        dwMilliseconds = (DWORD)wait_ms;
    }

    ret = GetQueuedCompletionStatus(sys_evc->iocp,
                                    &NumberOfBytes,
                                    &CompletionKey,
                                    &Overlapped,
                                    dwMilliseconds);
    // TT_INFO("GQCS: %d, %p", ret, Overlapped);
    if (ret || (Overlapped != NULL)) {
        tt_result_t iocp_result;
        tt_result_t result;

        if (ret) {
            iocp_result = TT_SUCCESS;
        } else {
            DWORD dwError = GetLastError();
            if ((dwError == ERROR_HANDLE_EOF) ||
                (dwError == ERROR_BROKEN_PIPE) ||
                (dwError == WSAECONNABORTED) || (dwError == WSAECONNRESET)) {
                iocp_result = TT_END;
            } else {
                iocp_result = TT_FAIL;
            }
        }

        switch (*((tt_u32_t *)CompletionKey)) {
            /*            case TT_EV_RANGE_INTERNAL_FILE: {
                            result =
               tt_file_iocp_handler(__KEY2FILE(CompletionKey),
                                                          NumberOfBytes,
                                                          Overlapped,
                                                          iocp_result);
                            if (!TT_OK(result)) {
                                return TT_FAIL;
                            }
                        } break;
                        case TT_EV_RANGE_INTERNAL_DIR: {
                            result =
               tt_dir_iocp_handler(__KEY2DIR(CompletionKey),
                                                         NumberOfBytes,
                                                         Overlapped,
                                                         iocp_result);
                            if (!TT_OK(result)) {
                                return TT_FAIL;
                            }
                        } break;
            case TT_EV_RANGE_INTERNAL_SOCKET: {
                result = tt_skt_iocp_handler(__KEY2SKT(CompletionKey),
                                             NumberOfBytes,
                                             Overlapped,
                                             iocp_result);
                if (!TT_OK(result)) {
                    return TT_FAIL;
                }
            } break;
            case TT_EV_RANGE_INTERNAL_IPC: {
                result = tt_ipc_iocp_handler(__KEY2IPC(CompletionKey),
                                             NumberOfBytes,
                                             Overlapped,
                                             iocp_result);
                if (!TT_OK(result)) {
                    return TT_FAIL;
                }
            } break;*/
            case TT_EVP_MARK_THREAD_EV: {
                tt_ev_t *ev;

                TT_ASSERT(TT_OK(iocp_result));
                TT_ASSERT(NumberOfBytes == 1);

                ev = tt_evc_recvfrom_thread_ntv(evc);
                TT_ASSERT(ev != NULL);
                result = __evp_tev_dispatch(evp, ev);
                if (!TT_OK(result)) {
                    return TT_FAIL;
                }
            } break;

            default: {
                TT_FATAL("unknown event range: 0x%x",
                         *((tt_u32_t *)CompletionKey));
            } break;
        }
    } else if (GetLastError() != WAIT_TIMEOUT) {
        TT_ERROR_NTV("GetQueuedCompletionStatus fail");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
