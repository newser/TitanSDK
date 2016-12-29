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

#include <tt_sys_error.h>
#include <tt_util_native.h>

#include <sys/event.h>
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
    int kq_fd = -1;
    struct kevent kev;

    TT_ASSERT(sys_evc != NULL);
    TT_ASSERT(attr != NULL);

    // create kqueue read fd
    kq_fd = kqueue();
    if (kq_fd < 0) {
        TT_ERROR_NTV("fail to create kqueue read fd");
        goto evcc_fail;
    }

    // add thread ev to kqueue
    EV_SET(&kev,
           TT_EVP_MARK_THREAD_EV,
           EVFILT_USER,
           EV_ADD | EV_CLEAR,
           0,
           0,
           NULL);

__retry_kev:
    if (kevent(kq_fd, &kev, 1, NULL, 0, NULL) != 0) {
        if (errno == EINTR) {
            goto __retry_kev;
        } else {
            TT_ERROR_NTV("fail to add rd thread ev mark");
            goto evcc_fail;
        }
    }

    sys_evc->kq_fd = kq_fd;

    sys_evc->ev_mark_thread_ev = TT_EVP_MARK_THREAD_EV;

    return TT_SUCCESS;

evcc_fail:

    if (kq_fd >= 0) {
        __RETRY_IF_EINTR(close(kq_fd) != 0);
    }

    return TT_FAIL;
}

tt_result_t tt_evc_destroy_ntv(IN tt_evcenter_ntv_t *sys_evc)
{
    TT_ASSERT(sys_evc != NULL);

    __RETRY_IF_EINTR(close(sys_evc->kq_fd) != 0);

    return TT_SUCCESS;
}

tt_result_t tt_evc_sendto_thread_ntv(IN struct tt_evcenter_s *dst_evc,
                                     IN tt_ev_t *ev)
{
    tt_evcenter_ntv_t *sys_evc = &dst_evc->sys_evc;
    tt_thread_ev_t *tev = TT_EV_HDR(ev, tt_thread_ev_t);
    struct kevent kev;
    tt_result_t result = TT_SUCCESS;

    TT_ASSERT(dst_evc != NULL);
    TT_ASSERT(ev != NULL);

    __EVC_LOCK_EVQ(dst_evc);

    tt_list_push_tail(&dst_evc->ev_q, &tev->node);

    EV_SET(&kev,
           TT_EVP_MARK_THREAD_EV,
           EVFILT_USER,
           0,
           NOTE_TRIGGER,
           0,
           &sys_evc->ev_mark_thread_ev);

__retry_kev:
    if (kevent(sys_evc->kq_fd, &kev, 1, NULL, 0, NULL) != 0) {
        if (errno == EINTR) {
            goto __retry_kev;
        } else {
            TT_ERROR_NTV("fail to post thread ev");
            result = TT_FAIL;
        }
    }

    // remove the event if kevent failed
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
