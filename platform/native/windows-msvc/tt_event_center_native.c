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

#include <tt_sys_error.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

// 0: default
// >0: number
// <0: cpu_num * ratio
#define __IOCP_WORKER_CPU_RATIO 0

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
    HANDLE iocp;

    TT_ASSERT(sys_evc != NULL);

    iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1); // 1 or 0?
    if (iocp == NULL) {
        TT_ERROR_NTV("fail to create iocp");
        goto evcc_fail;
    }
    sys_evc->iocp = iocp;

    sys_evc->ev_mark = TT_EVP_MARK_THREAD_EV;

    return TT_SUCCESS;

evcc_fail:

    if (iocp != NULL) {
        CloseHandle(iocp);
    }

    return TT_FAIL;
}

tt_result_t tt_evc_destroy_ntv(IN tt_evcenter_ntv_t *sys_evc)
{
    TT_ASSERT(sys_evc != NULL);

    CloseHandle(sys_evc->iocp);
    sys_evc->iocp = NULL;

    return TT_SUCCESS;
}

tt_result_t tt_evc_sendto_thread_ntv(IN struct tt_evcenter_s *dst_evc,
                                     IN tt_ev_t *ev)
{
    tt_evcenter_ntv_t *sys_evc = &dst_evc->sys_evc;
    tt_thread_ev_t *tev = TT_EV_HDR(ev, tt_thread_ev_t);
    tt_result_t result = TT_SUCCESS;

    TT_ASSERT(dst_evc != NULL);
    TT_ASSERT(ev != NULL);

    __EVC_LOCK_EVQ(dst_evc);

    tt_list_addtail(&dst_evc->ev_q, &tev->node);

    if (!PostQueuedCompletionStatus(sys_evc->iocp,
                                    1,
                                    (ULONG_PTR)&sys_evc->ev_mark,
                                    NULL)) {
        TT_ERROR_NTV("fail to post thread ev");
        result = TT_FAIL;
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
    node = tt_list_pophead(&evc->ev_q);
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
