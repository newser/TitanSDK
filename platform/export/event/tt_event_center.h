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

/**
@file tt_event_center.h
@brief event center

this file defines event center APIs
*/

#ifndef __TT_EVENT_CENTER__
#define __TT_EVENT_CENTER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <event/tt_event_poller.h>
#include <os/tt_atomic.h>
#include <timer/tt_timer_manager.h>

#include <tt_event_center_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// this is to be the arg passed to api that need a specified
// event center, so it must be a special value rather than
// 0(NULL)
#define TT_LOCAL_EVC ((tt_evcenter_t *)1)

#define __EVC_LOCK_EVQ(evc)                                                    \
    while (!TT_OK(tt_atomic_s32_cas(&(evc)->ev_q_lock, 0, 1)))
#define __EVC_UNLOCK_EVQ(evc)                                                  \
    TT_ASSERT_ALWAYS(TT_OK(tt_atomic_s32_cas(&(evc)->ev_q_lock, 1, 0)))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_evcenter_s;
struct tt_adns_dmgr_s;

typedef tt_result_t (*tt_evc_on_init_t)(IN struct tt_evcenter_s *evc,
                                        IN void *on_init_param);

typedef tt_result_t (*tt_evc_on_exit_t)(IN struct tt_evcenter_s *evc,
                                        IN void *on_exit_param);

typedef struct tt_evc_attr_s
{
    const tt_char_t *evc_name;
    tt_thread_attr_t evp_thread_attr;

    tt_evc_on_init_t on_init;
    void *on_init_param;

    tt_evc_on_exit_t on_exit;
    void *on_exit_param;

    tt_tmr_ev_attr_t tmr_ev_attr;
    tt_thread_ev_attr_t thread_ev_attr;
    tt_adns_ev_attr_t adns_ev_attr;
    tt_process_ev_attr_t process_ev_attr;
    tt_site_ev_attr_t site_ev_attr;
} tt_evc_attr_t;

typedef struct tt_evcenter_s
{
    tt_evc_attr_t attr;
    tt_evcenter_ntv_t sys_evc;

    // timer
    tt_tmr_mgr_t tmr_mgr;

    // thread event
    tt_list_t ev_q;
    tt_atomic_s32_t ev_q_lock;

    // async dns
    struct tt_adns_dmgr_s *adns_dmgr;

    // evp
    tt_evpoller_t poller;
} tt_evcenter_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_evcenter_component_register()
register event set system
*/
extern void tt_evcenter_component_register();

// - create -> exit -> wait
extern tt_result_t tt_evc_create(IN tt_evcenter_t *evc,
                                 IN tt_bool_t local_run,
                                 IN OPT tt_evc_attr_t *attr);

extern void tt_evc_attr_default(IN tt_evc_attr_t *attr);

// - can be called in or out of poller thread
extern void tt_evc_exit(IN tt_evcenter_t *evc);

// - can not be called in poller thread
// - wait until the poller thread exit
// - will destroy evc
extern tt_result_t tt_evc_wait(IN tt_evcenter_t *evc);

tt_inline tt_evcenter_t *tt_evc_current()
{
    tt_evpoller_t *evp = tt_evp_current();
    return TT_COND(evp != NULL, evp->evc, NULL);
}

// ========================================
// thread event api
// ========================================

tt_inline tt_result_t tt_evc_sendto_thread(IN tt_evcenter_t *dst_evc,
                                           IN tt_ev_t *ev)
{
    TT_ASSERT(ev->inter_thread);
    TT_ASSERT(TT_EV_HDR(ev, tt_thread_ev_t)->node.lst == NULL);

    if (dst_evc == TT_LOCAL_EVC) {
        dst_evc = tt_evc_current();
    }
    return tt_evc_sendto_thread_ntv(dst_evc, ev);
}

tt_inline tt_ev_t *tt_evc_recvfrom_thread(IN tt_evcenter_t *evc)
{
    return tt_evc_recvfrom_thread_ntv(evc);
}

extern tt_result_t tt_evc_sendto_thread_evid(IN tt_evcenter_t *dst_evc,
                                             IN tt_u32_t ev_id);

#endif /* __TT_EVENT_CENTER__ */
