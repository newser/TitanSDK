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
@file tt_event_poller.h
@brief event poller

this file defines event poller APIs
*/

#ifndef __TT_EVENT_POLLER__
#define __TT_EVENT_POLLER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <event/tt_adns_event.h>
#include <event/tt_evp_event.h>
#include <event/tt_process_event.h>
#include <event/tt_site_event.h>
#include <event/tt_thread_event.h>
#include <event/tt_timer_event.h>
#include <os/tt_thread.h>

#include <tt_event_poller_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_evpoller_s
{
    tt_evpoller_ntv_t sys_evp;
    struct tt_evcenter_s *evc;
    tt_bool_t local_run;
    tt_thread_t *thread;

    // thread event
    tt_evc_on_thread_ev_t on_thread_ev;

    // process event
    tt_evc_on_process_ev_t on_process_ev;

    // site event
    tt_evc_on_site_ev_t on_site_ev;
} tt_evpoller_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_evpoller_component_register()
register ts event poller component
*/
extern void tt_evpoller_component_register();

extern tt_result_t tt_evp_create(IN tt_evpoller_t *evp, IN tt_bool_t local_run);

// - can be called in or out of poller thread
extern void tt_evp_exit(IN tt_evpoller_t *evp);

// - can not be called in poller thread. if @evp is local, this function must
//   be called locally, otherwise, caller must be another thread
// - wait until the poller thread exit
// - will destroy evp
extern tt_result_t tt_evp_wait(IN tt_evpoller_t *evp);

tt_inline tt_evpoller_t *tt_evp_current()
{
    tt_thread_t *current = tt_current_thread();
    return TT_COND(current != NULL, current->evp, NULL);
}

#endif /* __TT_EVENT_POLLER__ */
