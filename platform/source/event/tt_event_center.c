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

#include <event/tt_event_center.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <misc/tt_assert.h>
#include <network/adns/tt_adns_domain_manager.h>

#include <tt_cstd_api.h>

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

static tt_result_t __evc_component_init(IN tt_component_t *comp,
                                        IN tt_profile_t *profile);

static tt_result_t __evc_destroy(IN tt_evcenter_t *evc);

tt_result_t __evc_on_init(IN tt_evcenter_t *evc);

void __evc_on_exit(IN tt_evcenter_t *evc);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_evcenter_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __evc_component_init,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_EVENT_CENTER,
                      "Event Center",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t tt_evc_create(IN tt_evcenter_t *evc,
                          IN tt_bool_t local_run,
                          IN OPT tt_evc_attr_t *attr)
{
    tt_tmr_mgr_attr_t tmr_mgr_attr;

    tt_u32_t __done = 0;
#define __EVCC_NATIVE (1 << 0)
#define __EVCC_TMR_MGR (1 << 2)
#define __EVCC_POLLER (1 << 3)

    TT_ASSERT(evc != NULL);

    // attr
    if (attr != NULL) {
        tt_memcpy(&evc->attr, attr, sizeof(tt_evc_attr_t));
    } else {
        tt_evc_attr_default(&evc->attr);
    }

    if (attr->adns_ev_attr.enable) {
        if ((attr->adns_ev_attr.name_server == NULL) ||
            (attr->adns_ev_attr.name_server_num == 0)) {
            TT_ERROR("no name server for asyn dns manager");
            return TT_FAIL;
        }
    }

    // native evc
    if (!TT_OK(tt_evc_create_ntv(&evc->sys_evc, &evc->attr))) {
        goto __evcc_fail;
    }
    __done |= __EVCC_NATIVE;

    // timer
    tt_tmr_mgr_attr_default(&tmr_mgr_attr);

    if (!TT_OK(tt_tmr_mgr_create(&evc->tmr_mgr, &tmr_mgr_attr))) {
        TT_ERROR("fail to create evc timer manager");
        goto __evcc_fail;
    }
    __done |= __EVCC_TMR_MGR;

    // thread event
    tt_list_init(&evc->ev_q);
    tt_atomic_s32_set(&evc->ev_q_lock, 0);

    // adns would be created in poller thread
    evc->adns_dmgr = NULL;

    // - evpoller must be created at final step
    // - all other members of the evc structure must have been initialized
    // - on_init will be called by evpoller
    if (!TT_OK(tt_evp_create(&evc->poller, local_run))) {
        goto __evcc_fail;
    }
    __done |= __EVCC_POLLER;

    return TT_SUCCESS;

__evcc_fail:

    if (__done & __EVCC_POLLER) {
        tt_evp_exit(&evc->poller);
        tt_evp_wait(&evc->poller);
    }

    if (__done & __EVCC_TMR_MGR) {
        tt_tmr_mgr_destroy(&evc->tmr_mgr);
    }

    if (__done & __EVCC_NATIVE) {
        tt_evc_destroy_ntv(&evc->sys_evc);
    }

    return TT_FAIL;
}

void tt_evc_attr_default(IN tt_evc_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->evc_name = "evcenter";
    tt_thread_attr_default(&attr->evp_thread_attr);

    attr->on_init = NULL;
    attr->on_init_param = NULL;

    attr->on_exit = NULL;
    attr->on_exit_param = NULL;

    tt_tmr_ev_attr_default(&attr->tmr_ev_attr);
    tt_thread_ev_attr_default(&attr->thread_ev_attr);
    tt_adns_ev_attr_default(&attr->adns_ev_attr);
    tt_process_ev_attr_default(&attr->process_ev_attr);
    tt_site_ev_attr_default(&attr->site_ev_attr);
}

void tt_evc_exit(IN tt_evcenter_t *evc)
{
    if (evc == TT_LOCAL_EVC) {
        evc = tt_evc_current();
    }
    if (evc == NULL) {
        TT_FATAL("null evc");
        return;
    }

    tt_evp_exit(&evc->poller);
}

tt_result_t tt_evc_wait(IN tt_evcenter_t *evc)
{
    TT_ASSERT(evc != NULL);

    if ((evc == TT_LOCAL_EVC) || (tt_evc_current() == evc)) {
        TT_ERROR("can not wait evc in its poller");
        return TT_BAD_PARAM;
    }

    if (!TT_OK(tt_evp_wait(&evc->poller))) {
        TT_FATAL("fail to wait poller");
        return TT_FAIL;
    }

    return __evc_destroy(evc);
}

tt_result_t tt_evc_sendto_thread_evid(IN tt_evcenter_t *dst_evc,
                                      IN tt_u32_t ev_id)
{
    tt_ev_t *ev = tt_thread_ev_create(ev_id, 0, NULL);
    if (ev == NULL) {
        TT_ERROR("fail to alloc thread ev id[%x]", ev_id);
        return TT_FAIL;
    }

    return tt_evc_sendto_thread(dst_evc, ev);
}

tt_result_t __evc_component_init(IN tt_component_t *comp,
                                 IN tt_profile_t *profile)
{
    return TT_SUCCESS;
}

tt_result_t __evc_destroy(IN tt_evcenter_t *evc)
{
#if 0
    // - the poller should have been stopped
    // - on_uninit is already called by the poller
    
    // evc does not know how to release events in saq
    if (!tt_list_empty(&evc->ev_q)) {
        TT_FATAL("%d events in ev q of evc", tt_list_count(&evc->ev_q));
    }
    
    if (evc->adns_dmgr != NULL) {
        tt_adns_dmgr_destroy(evc->adns_dmgr);
        evc->adns_dmgr = NULL;
    }
    
    tt_tmr_mgr_destroy(&evc->tmr_mgr);
    
    tt_evc_destroy_ntv(&evc->sys_evc);
#endif

    return TT_SUCCESS;
}

tt_result_t __evc_on_init(IN tt_evcenter_t *evc)
{
    tt_evc_attr_t *attr = &evc->attr;

    tt_u32_t __done = 0;
#define __EOI_ADNS (1 << 0)

// this function is called in evp thread

#if 0
    if (evc->attr.adns_ev_attr.enable) {
        TT_ASSERT(evc->adns_dmgr == NULL);
        evc->adns_dmgr = tt_adns_dmgr_create(evc,
                                             attr->adns_ev_attr.name_server,
                                             attr->adns_ev_attr.name_server_num,
                                             NULL);
        if (evc->adns_dmgr == NULL) {
            TT_ERROR("fail to create evc adns cache");
            goto __eoi_fail;
        }
        __done |= __EOI_ADNS;
    }
#endif

    return TT_SUCCESS;

__eoi_fail:

#if 0
    if (__done & __EOI_ADNS) {
        tt_adns_dmgr_destroy(evc->adns_dmgr);
        evc->adns_dmgr = NULL;
    }
#endif

    return TT_FAIL;
}

void __evc_on_exit(IN tt_evcenter_t *evc)
{
// this function is called in evp thread

#if 0
    if (evc->attr.adns_ev_attr.enable) {
        TT_ASSERT(evc->adns_dmgr != NULL);
        tt_adns_dmgr_destroy(evc->adns_dmgr);
        evc->adns_dmgr = NULL;
    } else {
        TT_ASSERT(evc->adns_dmgr == NULL);
    }
#endif
}
