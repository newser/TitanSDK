/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
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

#include <time/tt_timer.h>

#include <algorithm/tt_algorithm_def.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <os/tt_task.h>
#include <time/tt_time_reference.h>
#include <time/tt_timer_manager.h>

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

tt_tmr_t *tt_tmr_create(IN tt_s64_t delay_ms, IN tt_u32_t ev,
                        IN OPT void *param)
{
    tt_tmr_t *tmr;
    tt_fiber_t *cfb;

    TT_ASSERT(delay_ms >= 0);

    tmr = tt_malloc(sizeof(tt_tmr_t));
    if (tmr == NULL) {
        TT_ERROR("no mem for timer");
        return NULL;
    }

    tmr->delay_ms = delay_ms;
    tmr->absolute_ref = 0;

    cfb = tt_current_fiber();
    TT_ASSERT(cfb != NULL);
    tmr->owner = cfb;
    tt_lnode_init(&tmr->node);
    tt_list_push_tail(&cfb->unexpired_tmr, &tmr->node);

    tmr->param = param;
    tmr->mgr = &tmr->owner->fs->thread->task->tmr_mgr;
    tmr->ev = ev;
    tmr->heap_pos = TT_POS_NULL;
    tmr->status = TT_TMR_INACTIVE;

    return tmr;
}

void tt_tmr_destroy(IN tt_tmr_t *tmr)
{
    TT_ASSERT(tmr != NULL);

    // does not matter whether it's in expired or unexpired timer list
    tt_list_remove(&tmr->node);

    if (tmr->heap_pos == TT_POS_NULL) {
        // not in heap
        tt_free(tmr);
    } else {
#if 1
        // keep it in heap could avoid rebuilding heap
        tmr->status = TT_TMR_ORPHAN;
#else
        // or free it, which could make more memory available
        tt_ptrheap_remove(&tmr->mgr->tmr_heap, tmr->heap_pos);
        tt_free(tmr);
#endif
    }
}

tt_result_t tt_tmr_start(IN tt_tmr_t *tmr)
{
    tt_ptrheap_t *tmr_heap = &tmr->mgr->tmr_heap;
    tt_s64_t delay_ref;

    tmr->absolute_ref = tt_time_ref();
    delay_ref = tt_time_ms2ref(tmr->delay_ms);
    if ((tmr->absolute_ref + delay_ref) <= delay_ref) {
        // both absolute_ref and delay_ref are positive
        TT_ERROR("too large expiration, overflowed");
        return TT_FAIL;
    }
    tmr->absolute_ref += delay_ref;

    tmr->status = TT_TMR_ACTIVE;

    if (tmr->heap_pos == TT_POS_NULL) {
        return tt_ptrheap_add(tmr_heap, tmr, &tmr->heap_pos);
    } else {
        tt_ptrheap_fix(tmr_heap, tmr->heap_pos);
        return TT_SUCCESS;
    }
}
