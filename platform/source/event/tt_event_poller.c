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

#include <event/tt_event_poller.h>

#include <event/tt_event_center.h>
#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <log/tt_log.h>
#include <misc/tt_assert.h>
#include <network/adns/tt_adns_domain_manager.h>
#include <os/tt_atomic.h>
#include <timer/tt_timer_manager.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_EVP TT_ASSERT

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern tt_result_t __evc_on_init(IN tt_evcenter_t *evc);

extern void __evc_on_exit(IN tt_evcenter_t *evc);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __evp_component_init(IN tt_component_t *comp,
                                        IN tt_profile_t *profile);

static tt_result_t __evp_routine(IN void *param);

static void __evp_destroy(IN tt_evpoller_t *evp);

// ========================================
// thread event
// ========================================

tt_result_t __evp_tev_dispatch(IN tt_evpoller_t *evp, IN tt_ev_t *tev);

static tt_result_t __evp_internal_tev_handler(IN tt_evpoller_t *evp,
                                              IN tt_ev_t *ev);

static tt_result_t __evp_tev_handler(IN tt_evpoller_t *evp, IN tt_ev_t *ev);

static tt_result_t __evp_ctl_tev_handler(IN tt_evpoller_t *evp, IN tt_ev_t *ev);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

// |- thread tev:
//    |- on_thread_ev
//    |- __evp_internal_tev_handler
//       |- tt_file_tev_handler
//       |- tt_dir_tev_handler
//       |- tt_skt_tev_handler
//       |- __evp_tev_handler
//          |- tt_evp_timer_tev_handler
//          |- __evp_ctl_tev_handler
// |- process tev:
//    |- on_process_ev
// |- site tev:
//    |- on_site_ev

void tt_evpoller_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __evp_component_init,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_EVENT_POLLER,
                      "Event Poller",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t tt_evp_create(IN tt_evpoller_t *evp, IN tt_bool_t local_run)
{
    tt_evcenter_t *evc;
    tt_evc_attr_t *evc_attr;

    tt_u32_t __done = 0;
#define __EC_NATIVE (1 << 0)
#define __EC_THREAD (1 << 2)

    TT_ASSERT(evp != NULL);

    // per latest implementation, evp must be the member of evc
    evc = TT_CONTAINER(evp, tt_evcenter_t, poller);
    evc_attr = &evc->attr;

    if (!TT_OK(tt_evp_create_ntv(&evp->sys_evp, &evc->attr))) {
        goto __ec_fail;
    }
    __done |= __EC_NATIVE;

    evp->evc = evc;
    evp->local_run = local_run;
    evp->thread = NULL;

    // thread event
    evp->on_thread_ev = evc_attr->thread_ev_attr.on_thread_ev;

    // process event
    evp->on_process_ev = evc_attr->process_ev_attr.on_process_ev;

    // site event
    evp->on_site_ev = evc_attr->site_ev_attr.on_site_ev;

    // starting thread must be done as final step
    if (evp->local_run) {
        tt_thread_t *thread = tt_current_thread();

        if (thread == NULL) {
            TT_ERROR("not in ts thread");
            goto __ec_fail;
        }
        if (thread->evp != NULL) {
            TT_ERROR("already in evp");
            goto __ec_fail;
        }

        __evp_routine(evp);
    } else {
        evp->thread =
            tt_thread_create(__evp_routine, evp, &evc_attr->evp_thread_attr);
        if (evp->thread == NULL) {
            TT_ERROR("fail to create evp thread");
            goto __ec_fail;
        }
        __done |= __EC_THREAD;
    }

    return TT_SUCCESS;

__ec_fail:

    if (__done & __EC_THREAD) {
        // howto?
    }

    if (__done & __EC_NATIVE) {
        tt_evp_destroy_ntv(&evp->sys_evp);
    }

    return TT_FAIL;
}

void tt_evp_exit(IN tt_evpoller_t *evp)
{
    // this function could work even in a local_run evp
    if (!TT_OK(tt_evc_sendto_thread_evid(evp->evc, TT_EVP_CTL_EXIT))) {
        TT_FATAL("fail to post TT_EVP_CTL_EXIT");
    }
}

tt_result_t tt_evp_wait(IN tt_evpoller_t *evp)
{
    TT_ASSERT(evp != NULL);

    if (tt_evp_current() == evp) {
        TT_ERROR("can not wait evp in its routine");
        return TT_BAD_PARAM;
    }

    // only wait for non local_run poller thread
    if (!evp->local_run && !TT_OK(tt_thread_wait(evp->thread))) {
        TT_FATAL("fail to wait poller thread");
        return TT_FAIL;
    }

    __evp_destroy(evp);

    return TT_SUCCESS;
}

tt_result_t __evp_component_init(IN tt_component_t *comp,
                                 IN tt_profile_t *profile)
{
    // check event range
    if (TT_EV_RANGE_APP_END > TT_EV_RANGE_INTERNAL) {
        TT_ERROR("TT_EV_RANGE_APP_END[%d] > TT_EV_RANGE_INTERNAL[%d]",
                 TT_EV_RANGE_APP_END,
                 TT_EV_RANGE_INTERNAL);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __evp_routine(IN void *param)
{
    tt_evpoller_t *evp = (tt_evpoller_t *)param;
    tt_evcenter_t *evc = evp->evc;
    tt_result_t result = TT_SUCCESS;
    tt_thread_t *thread = tt_current_thread();

    TT_ASSERT(thread->evp == NULL);
    thread->evp = evp;

    // init
    if (!TT_OK(__evc_on_init(evc))) {
        return TT_FAIL;
    }
    if (evc->attr.on_init != NULL) {
        result = evc->attr.on_init(evc, evc->attr.on_init_param);
    }

    // poll events
    if (TT_OK(result)) {
        while (TT_OK(tt_evp_poll_ntv(evp, tt_tmr_mgr_run(&evc->tmr_mgr))))
            ;
    }

    // exit
    if (evc->attr.on_exit != NULL) {
        evc->attr.on_exit(evc, evc->attr.on_exit_param);
    }
    __evc_on_exit(evc);

    thread->evp = NULL;
    return TT_SUCCESS;
}

void __evp_destroy(IN tt_evpoller_t *evp)
{
    // evp thread should have been terminated

    tt_evp_destroy_ntv(&evp->sys_evp);
}

// ========================================
// thread event
// ========================================

// - return TT_END when it want evp exit
tt_result_t __evp_tev_dispatch(IN tt_evpoller_t *evp, IN tt_ev_t *ev)
{
    tt_u32_t ev_range;

    ev_range = TT_EV_RANGE(ev->ev_id);
    if (ev_range < TT_EV_RANGE_INTERNAL) {
        // application events
        if (evp->on_thread_ev != NULL) {
            evp->on_thread_ev(evp, ev);
        } else {
            TT_FATAL("can not handle incoming app event: %x", ev->ev_id);
        }

        // application events never affect evp running
        return TT_SUCCESS;
    } else {
        // internal events may affect evp running
        return __evp_internal_tev_handler(evp, ev);
    }
}

tt_result_t __evp_internal_tev_handler(IN tt_evpoller_t *evp, IN tt_ev_t *ev)
{
    switch (TT_EV_RANGE(ev->ev_id)) {
        /*
    case TT_EV_RANGE_INTERNAL_FILE: {
        return tt_file_tev_handler(evp, ev);
    } break;
    case TT_EV_RANGE_INTERNAL_DIR: {
        return tt_dir_tev_handler(evp, ev);
    } break;
         case TT_EV_RANGE_INTERNAL_ASYNC_DNS: {
         return tt_adns_dmgr_tev_handler(evp, ev);
         } break;

        case TT_EV_RANGE_INTERNAL_SOCKET:
        case TT_EV_RANGE_INTERNAL_SSL: {
            return tt_skt_tev_handler(evp, ev);
        } break;
        case TT_EV_RANGE_INTERNAL_IPC: {
            return tt_ipc_tev_handler(evp, ev);
        } break;*/
        case TT_EV_RANGE_INTERNAL_EVPOLLER: {
            return __evp_tev_handler(evp, ev);
        } break;

        default: {
            TT_FATAL("unknown event: %x", ev->ev_id);
            return TT_SUCCESS;
        } break;
    }
}

tt_result_t __evp_tev_handler(IN tt_evpoller_t *evp, IN tt_ev_t *ev)
{
    switch (TT_EV_RANGE_SUB(ev->ev_id)) {
        case TT_EVP_RANGE_CONTROL: {
            return __evp_ctl_tev_handler(evp, ev);
        } break;

        default: {
            TT_FATAL("unknown evp event: %x", ev->ev_id);
            return TT_SUCCESS;
        } break;
    }
}

tt_result_t __evp_ctl_tev_handler(IN tt_evpoller_t *evp, IN tt_ev_t *ev)
{
    switch (ev->ev_id) {
        case TT_EVP_CTL_EXIT: {
            tt_ev_destroy(ev);
            return TT_END;
        } break;

        default: {
            TT_FATAL("unknown evp event: %x", ev->ev_id);
            return TT_SUCCESS;
        } break;
    }
}
