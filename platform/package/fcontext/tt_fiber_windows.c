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

#include <fcontext/tt_fiber_windows.h>

#include <os/tt_fiber.h>

#include <tt_sys_error.h>

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

static VOID CALLBACK __fiber_routine_wrapper(PVOID lpParameter);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_fiber_create_wrap(IN tt_fiber_wrap_t *wrap_fb,
                                 IN tt_u32_t stack_size)
{
    tt_u32_t reserve, commit;

    TT_U32_ALIGN_INC_PAGE(stack_size);

    // The system commits additional pages from the reserved
    // stack memory as they are needed, until either the stack
    // reaches the reserved size minus one page (which is used
    // as a guard page to prevent stack overflow)
    reserve = stack_size + tt_g_page_size;

    // 4 pages as initial committed size
    commit = TT_MIN(stack_size, tt_g_page_size << 2);

    wrap_fb->fb = CreateFiberEx(commit,
                                reserve,
                                FIBER_FLAG_FLOAT_SWITCH,
                                __fiber_routine_wrapper,
                                TT_CONTAINER(wrap_fb, tt_fiber_t, wrap_fb));
    if (wrap_fb->fb == NULL) {
        TT_ERROR_NTV("fail to create fiber");
        return TT_FAIL;
    }

    wrap_fb->from = NULL;

    return TT_SUCCESS;
}

void tt_fiber_destroy_wrap(IN tt_fiber_wrap_t *wrap_fb)
{
    DeleteFiber(wrap_fb->fb);
}

tt_result_t tt_fiber_create_local_wrap(IN tt_fiber_wrap_t *wrap_fb)
{
    wrap_fb->fb =
        ConvertThreadToFiberEx(NULL, FIBER_FLAG_FLOAT_SWITCH);
    if (wrap_fb->fb == NULL) {
        TT_ERROR_NTV("fail to convert thread to fiber");
        return TT_FAIL;
    }

    wrap_fb->from = NULL;

    return TT_SUCCESS;
}

void tt_fiber_destroy_local_wrap(IN tt_fiber_wrap_t *wrap_fb)
{
    if (!ConvertFiberToThread()) {
        TT_ERROR_NTV("fail to convert fiber to thread");
    }
}

void tt_fiber_switch_wrap(IN tt_fiber_sched_t *fs,
                          IN tt_fiber_t *from,
                          IN tt_fiber_t *to)
{
    tt_fiber_wrap_t *wrap_to = &to->wrap_fb;
    tt_fiber_t *prev;

    // from: current fiber
    // to: the fiber to be switched to
    // prev: the previous fiber when tt_jump_fcontext() returns

    // an example is: f1 -> f2 -> f3 -> f1, when f1 returns
    // from tt_jump_fcontext():
    //  from: f1, as f1 ever switched to f2
    //  to: f2, as f1 ever switched to f2
    //  prev: f3, as f1 returns from f3

    wrap_to->from = from;
    SwitchToFiber(to->wrap_fb.fb);

    prev = from->wrap_fb.from;
    if (prev->end) {
        tt_list_remove(&prev->node);
        tt_fiber_destroy(prev);
    }
}

VOID CALLBACK __fiber_routine_wrapper(PVOID lpParameter)
{
    tt_fiber_t *cfb = (tt_fiber_t *)lpParameter;
    tt_fiber_sched_t *cfs = cfb->fs;
    tt_fiber_wrap_t *wrap_main = &cfb->fs->__main->wrap_fb;

    if (!FlsSetValue(tt_g_thread_fls_index, (LPVOID)cfs->thread)) {
        TT_ERROR_NTV("fail to set fls");
        goto out;
    }

    cfb->routine(cfb->param);

out:
    if (!FlsSetValue(tt_g_thread_fls_index, NULL)) {
        TT_ERROR_NTV("fail to clear fls");
    }
    
    cfb->end = TT_TRUE;
    wrap_main->from = cfb;
    SwitchToFiber(wrap_main->fb);
}

