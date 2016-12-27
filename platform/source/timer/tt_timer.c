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

#include <timer/tt_timer.h>

#include <algorithm/ptr/tt_ptr_heap.h>
#include <algorithm/tt_algorithm_def.h>
#include <event/tt_event_center.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <timer/tt_time_reference.h>
#include <timer/tt_timer_manager.h>

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

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_tmr_t *tt_tmr_create(IN OPT IN struct tt_tmr_mgr_s *mgr,
                        IN tt_s64_t delay_ms,
                        IN OPT void *opaque,
                        IN tt_tmr_cb_t cb,
                        IN OPT void *cb_param,
                        IN tt_u32_t flag)
{
    tt_tmr_t *tmr = NULL;

    TT_ASSERT(cb != NULL);

    if (mgr == NULL) {
        tt_evcenter_t *evc = tt_evc_current();
        if (evc == NULL) {
            TT_ERROR("not in evcenter");
            return NULL;
        }

        mgr = &evc->tmr_mgr;
    }

    // alloc timer
    tmr = (tt_tmr_t *)tt_malloc(sizeof(tt_tmr_t));
    if (tmr == NULL) {
        TT_ERROR("no mem for creating timer");
        return NULL;
    }

    // init timer
    tt_memset(tmr, 0, sizeof(tt_tmr_t));

    tmr->delay_ms = delay_ms;
    tmr->opaque = opaque;
    tmr->cb = cb;
    tmr->cb_param = cb_param;

    if (flag & TT_TMR_NOTIFY_DESTROYED) {
        tmr->notify_destroyed = TT_TRUE;
    }

    tmr->status = __TMR_INACTIVE;

    tmr->heap_idx = TT_POS_NULL;
    tmr->mgr = mgr;
    tmr->absolute_expire_time = 0;

    return tmr;
}

void tt_tmr_destroy(IN tt_tmr_t *tmr)
{
    TT_ASSERT(tmr != NULL);

    if (tmr->notify_destroyed) {
        tmr->cb(tmr, tmr->cb_param, TT_TMR_CB_DESTROYED);
    }

    if (tmr->heap_idx == TT_POS_NULL) {
        // not in heap
        tt_free(tmr);
    } else {
#if 1
        // keep it in heap could avoid rebuilding heap
        tmr->status = __TMR_ORPHAN;
#else
        // or free it, which could make more memory available
        tt_ptrheap_remove(&tmr->mgr->tmr_heap, tmr->heap_idx);
        tt_free(tmr);
#endif
    }
}

tt_result_t tt_tmr_start(IN tt_tmr_t *tmr)
{
    tt_tmr_mgr_t *mgr;

    TT_ASSERT(tmr != NULL);
    TT_ASSERT(tmr->mgr != NULL);

    mgr = tmr->mgr;

    // recalculate new expiration time
    do {
        tt_s64_t __absolute_expire_time;

        tmr->absolute_expire_time = tt_time_ref();

        __absolute_expire_time = tmr->absolute_expire_time;
        __absolute_expire_time += tt_time_ms2ref(tmr->delay_ms);
        if ((tmr->delay_ms != 0) &&
            (__absolute_expire_time <= tmr->absolute_expire_time)) {
            TT_ERROR("too large expiration, overflowed");
            return TT_FAIL;
        }

        tmr->absolute_expire_time = __absolute_expire_time;
    } while (0);

    tmr->status = __TMR_ACTIVE;

    if (tmr->heap_idx == TT_POS_NULL) {
        return tt_ptrheap_add(&mgr->tmr_heap, tmr, &tmr->heap_idx);
    } else {
        tt_ptrheap_fix(&mgr->tmr_heap, tmr->heap_idx);
        return TT_SUCCESS;
    }
}

void tt_tmr_stop(IN tt_tmr_t *tmr)
{
    TT_ASSERT(tmr != NULL);

    tmr->status = __TMR_INACTIVE;

    // - no need to remove tmr out from heap so as to avoid adjusting heap
    // - when the timer manager sees an inactive timer, it would pop it
    //   from heap and then the timer becomes wild(not in heap)
}
