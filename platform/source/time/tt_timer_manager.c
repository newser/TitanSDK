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

#include <time/tt_timer_manager.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <misc/tt_assert.h>
#include <os/tt_fiber_event.h>
#include <time/tt_time_reference.h>
#include <time/tt_timer.h>

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

static tt_result_t __tmr_mgr_component_init(IN tt_component_t *comp,
                                            IN tt_profile_t *profile);

static tt_s32_t __tmr_cmp(IN void *l, IN void *r);

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

void tt_tmr_mgr_init(IN tt_tmr_mgr_t *mgr, IN tt_tmr_mgr_attr_t *attr)
{
    tt_tmr_mgr_attr_t __attr;

    TT_ASSERT(mgr != NULL);

    if (attr == NULL) {
        tt_tmr_mgr_attr_default(&__attr);
        attr = &__attr;
    }

    tt_ptrheap_init(&mgr->tmr_heap, __tmr_cmp, &attr->tmr_heap_attr);
}

void tt_tmr_mgr_destroy(IN tt_tmr_mgr_t *mgr)
{
    TT_ASSERT(mgr != NULL);

    if (!tt_ptrheap_empty(&mgr->tmr_heap)) {
        TT_WARN("still timer in tmr mgr");
    }

    tt_ptrheap_destroy(&mgr->tmr_heap);
}

void tt_tmr_mgr_attr_default(IN tt_tmr_mgr_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_ptrheap_attr_default(&attr->tmr_heap_attr);
}

tt_s64_t tt_tmr_mgr_run(IN tt_tmr_mgr_t *mgr)
{
    tt_ptrheap_t *tmr_heap = &mgr->tmr_heap;
    tt_tmr_t *head;
    tt_s64_t now = 0;
    tt_bool_t cb_exec = TT_FALSE;

    while ((head = (tt_tmr_t *)tt_ptrheap_head(tmr_heap)) != NULL) {
        if (head->status == TT_TMR_ACTIVE) {
            if (now == 0) {
                now = tt_time_ref();
            }
        again:
            if (head->absolute_ref <= now) {
                tt_ptrheap_pop(tmr_heap);
                tt_fiber_send_timer(head->owner, head);
                cb_exec = TT_TRUE;
            } else if (cb_exec) {
                // executing last callback may take some time, so update now
                // value
                now = tt_time_ref();
                cb_exec = TT_FALSE;
                goto again;
            } else {
                return tt_time_ref2ms(head->absolute_ref - now);
            }
        } else if (head->status == TT_TMR_INACTIVE) {
            tt_ptrheap_pop(tmr_heap);
        } else {
            TT_ASSERT(head->status == TT_TMR_ORPHAN);
            tt_ptrheap_pop(tmr_heap);
            tt_tmr_destroy(head);
        }
    }

    return TT_TIME_INFINITE;
}

tt_result_t __tmr_mgr_component_init(IN tt_component_t *comp,
                                     IN tt_profile_t *profile)
{
    return TT_SUCCESS;
}

tt_s32_t __tmr_cmp(IN void *l, IN void *r)
{
    tt_s64_t diff;

    // - absolute_ref is tt_s64_t, the difference may overflow from
    //   tt_s32_t, so it can not simply return (tt_s32_t)diff
    // - note timer heap is a min heap, but array heap is max heap

    diff = ((tt_tmr_t *)l)->absolute_ref - ((tt_tmr_t *)r)->absolute_ref;
    if (diff < 0) {
        return 1;
    } else if (diff == 0) {
        return 0;
    } else {
        return -1;
    }
}
