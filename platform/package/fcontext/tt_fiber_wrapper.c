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

#include <fcontext/tt_fiber_wrapper.h>

#include <memory/tt_memory_alloc.h>
#include <os/tt_fiber.h>

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

static void __fiber_routine_wrapper(IN tt_transfer_t t);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_fiber_create_wrap(IN tt_fiber_wrap_t *wrap_fb,
                                 IN tt_u32_t stack_size)
{
    void *stack;

    TT_U32_ALIGN_INC_PAGE(stack_size);

    wrap_fb->fctx = NULL;
    wrap_fb->from = NULL;

    stack = tt_malloc(stack_size);
    if (stack == NULL) {
        TT_ERROR("no mem for fiber stack");
        return TT_FAIL;
    }
    wrap_fb->stack = TT_PTR_INC(void, stack, stack_size);
    wrap_fb->stack_size = stack_size;

    return TT_SUCCESS;
}

void tt_fiber_destroy_wrap(IN tt_fiber_wrap_t *wrap_fb)
{
    tt_free(TT_PTR_DEC(void, wrap_fb->stack, wrap_fb->stack_size));
}

void tt_fiber_switch_wrap(IN tt_fiber_sched_t *fs,
                          IN tt_fiber_t *from,
                          IN tt_fiber_t *to)
{
    tt_fiber_wrap_t *wrap_to = &to->wrap_fb;
    tt_transfer_t t;

    if (wrap_to->fctx == NULL) {
        wrap_to->fctx = tt_make_fcontext(wrap_to->stack,
                                         wrap_to->stack_size,
                                         __fiber_routine_wrapper);
    }

    wrap_to->from = from;
    t = tt_jump_fcontext(wrap_to->fctx, to);
    if (t.data != NULL) {
        // must save to's context
        wrap_to->fctx = t.fctx;
    } else {
        tt_dlist_remove(&fs->fiber_list, &to->node);
        tt_fiber_destroy(to);
    }
}

void __fiber_routine_wrapper(IN tt_transfer_t t)
{
    tt_fiber_t *cfb = t.data;

    // must save from's context
    cfb->wrap_fb.from->wrap_fb.fctx = t.fctx;

    cfb->routine(cfb->param);

    // note return NULL indicating the fiber is terminated
    tt_jump_fcontext(t.fctx, NULL);
}
