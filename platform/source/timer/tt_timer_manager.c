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

#include <timer/tt_timer_manager.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <log/tt_log.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <timer/tt_time_reference.h>
#include <timer/tt_timer.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
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

static tt_s32_t __tmr_cmp(IN void *l, IN void *r);

static tt_result_t __tmr_mgr_component_init(IN tt_component_t *comp,
                                            IN tt_profile_t *profile);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_tmr_mgr_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __tmr_mgr_component_init,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_TIMER_MGR,
                      "Timer Manager",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t __tmr_mgr_component_init(IN tt_component_t *comp,
                                     IN tt_profile_t *profile)
{
    return TT_SUCCESS;
}

tt_result_t tt_tmr_mgr_create(IN tt_tmr_mgr_t *mgr, IN tt_tmr_mgr_attr_t *attr)
{
    TT_ASSERT(mgr != NULL);

    if (attr != NULL) {
        tt_memcpy(&mgr->attr, attr, sizeof(tt_tmr_mgr_attr_t));
    } else {
        tt_tmr_mgr_attr_default(&mgr->attr);
    }

    // timer heap
    tt_ptrheap_init(&mgr->tmr_heap, __tmr_cmp, &mgr->attr.tmr_heap_attr);

    return TT_SUCCESS;
}

void tt_tmr_mgr_destroy(IN tt_tmr_mgr_t *mgr)
{
    TT_ASSERT(mgr != NULL);

    // the heap would check whether there are still some timer running
    tt_ptrheap_destroy(&mgr->tmr_heap);
}

void tt_tmr_mgr_attr_default(IN tt_tmr_mgr_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_ptrheap_attr_default(&attr->tmr_heap_attr);
}

tt_s64_t tt_tmr_mgr_run(IN tt_tmr_mgr_t *mgr)
{
    tt_s64_t now;
    tt_tmr_t *head;
    tt_bool_t some_tmr_expired;

    TT_ASSERT(mgr != NULL);

    do {
        some_tmr_expired = TT_FALSE;
        now = tt_time_ref();

    recheck:
        head = (tt_tmr_t *)tt_ptrheap_head(&mgr->tmr_heap);
        // to process a timer if
        //  - timer is not active: remove it or release it
        //  - timer is active and expired now
        if ((head != NULL) && ((head->status != __TMR_ACTIVE) ||
                               (head->absolute_expire_time <= now))) {
            some_tmr_expired = TT_TRUE;
            head = (tt_tmr_t *)tt_ptrheap_pop(&mgr->tmr_heap);

            // process expired timer
            switch (head->status) {
                case __TMR_INACTIVE: {
                    // ignore inactive timer
                } break;
                case __TMR_ACTIVE: {
                    // process active timer
                    head->cb(head, head->cb_param, TT_TMR_CB_EXPIRED);
                } break;
                case __TMR_ORPHAN: {
                    // return unused timer to slab
                    tt_free(head);
                } break;
                default: {
                    TT_ERROR("invalid timer status: %d", head->status);
                } break;
            }

            // no need to update current time ref, continue checking
            // heap head. such behavior may save time for calling
            // tt_time_ref()
            goto recheck;
        }

        // if it has spent time on processing expired timer, then "now" would
        // have difference with real time
    } while (some_tmr_expired);

    if (head == NULL) {
        // no more timer in manager
        return TT_TIME_INFINITE;
    } else {
        // return how long next timer would expire
        TT_ASSERT(head->absolute_expire_time > now);
        return tt_time_ref2ms(head->absolute_expire_time - now);
    }
}

tt_s32_t __tmr_cmp(IN void *l, IN void *r)
{
    tt_s64_t diff = ((tt_tmr_t *)l)->absolute_expire_time -
                    ((tt_tmr_t *)r)->absolute_expire_time;
    // absolute_expire_time is tt_s64_t, the difference may overflow from
    // tt_s32_t, so it can not simply return (tt_s32_t)diff

    // note timer heap is a min heap, but array heap is max heap
    if (diff < 0) {
        return 1;
    } else if (diff == 0) {
        return 0;
    } else {
        return -1;
    }
}
