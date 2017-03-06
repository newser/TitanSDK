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

#include <fcontext/source/latest/fcontext.h>
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

static void __fiber_routine_wrapper(IN transfer_t t);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_fiber_create_wrap(IN tt_fiber_wrap_t *wrap_f,
                                 IN tt_u32_t stack_size)
{
    wrap_f->fctx = NULL;
    wrap_f->from = NULL;

    wrap_f->stack = tt_malloc(stack_size);
    if (wrap_f->stack == NULL) {
        TT_ERROR("no mem for fiber stack");
        return TT_FAIL;
    }
    wrap_f->stack = TT_PTR_INC(void, wrap_f->stack, stack_size);
    wrap_f->stack_size = stack_size;

    return TT_SUCCESS;
}

void tt_fiber_destroy_wrap(IN tt_fiber_wrap_t *wrap_f)
{
    tt_free(TT_PTR_DEC(void, wrap_f->stack, wrap_f->stack_size));
}

void tt_fiber_switch_wrap(IN tt_fiber_sched_t *fs,
                          IN tt_fiber_t *from,
                          IN tt_fiber_t *to)
{
    tt_fiber_wrap_t *w_to = &to->wrap_f;
    transfer_t t;

    if (w_to->fctx == NULL) {
        w_to->fctx = make_fcontext(w_to->stack,
                                   w_to->stack_size,
                                   __fiber_routine_wrapper);
    }

    w_to->from = from;
    t = jump_fcontext(w_to->fctx, to);
    if (t.data != NULL) {
        w_to->fctx = t.fctx;
    } else {
        tt_dlist_remove(&fs->fiber_list, &to->node);
        tt_fiber_destroy(to);
    }
}

void __fiber_routine_wrapper(IN transfer_t t)
{
    tt_fiber_t *cf = t.data;

    // save from's context
    cf->wrap_f.from->wrap_f.fctx = t.fctx;

    cf->routine(cf->param);

    // note return NULL indicating the fiber is terminated
    jump_fcontext(t.fctx, NULL);
}
