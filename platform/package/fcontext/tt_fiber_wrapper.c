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

#if TT_ENV_OS_IS_WINDOWS
#else
#include <sys/mman.h>
#endif

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

static tt_result_t __create_stack(IN tt_fiber_wrap_t *wrap_fb,
                                  IN tt_u32_t stack_size);

static void __destroy_stack();

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

    if (!TT_OK(__create_stack(wrap_fb, stack_size))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_fiber_destroy_wrap(IN tt_fiber_wrap_t *wrap_fb)
{
    __destroy_stack();
}

void tt_fiber_switch_wrap(IN tt_fiber_sched_t *fs,
                          IN tt_fiber_t *from,
                          IN tt_fiber_t *to)
{
    tt_fiber_wrap_t *wrap_to = &to->wrap_fb;
    tt_transfer_t t;
    tt_fiber_t *prev;

    // from: current fiber
    // to: the fiber to be switched to
    // prev: the previous fiber when tt_jump_fcontext() returns

    // an example is: f1 -> f2 -> f3 -> f1, when f1 returns
    // from tt_jump_fcontext():
    //  from: f1, as f1 ever switched to f2
    //  to: f2, as f1 ever switched to f2
    //  prev: f3, as f1 returns from f3

    if (wrap_to->fctx == NULL) {
        wrap_to->fctx = tt_make_fcontext(wrap_to->stack,
                                         wrap_to->stack_size,
                                         __fiber_routine_wrapper);
    }

    wrap_to->from = from;
    t = tt_jump_fcontext(wrap_to->fctx, to);

    prev = from->wrap_fb.from;
    if (prev->end) {
        tt_list_remove(&prev->node);
        tt_fiber_destroy(prev);
    } else {
        prev->wrap_fb.fctx = t.fctx;
    }
}

void __fiber_routine_wrapper(IN tt_transfer_t t)
{
    tt_fiber_t *cfb = t.data;
    tt_fiber_sched_t *cfs = cfb->fs;
    tt_fiber_wrap_t *wrap_main = &cfb->fs->__main->wrap_fb;

    // must save from's context
    cfb->wrap_fb.from->wrap_fb.fctx = t.fctx;

    cfb->routine(cfb->param);

    // must not use t.fctx, see a case:
    //  f1 -> f2
    //  f2 -> f3 ; now f2's t.fctx is stored in f1
    //  f3 -> f1
    //  f1 -> main ; f1 termiantes
    //  main -> f2
    //  f2 -> ? ; f2 terminates now, but t.fctx is invalid
    //
    // the simplest solution is jumping to main fiber as it's
    // always valid

    cfb->end = TT_TRUE;
    wrap_main->from = cfb;
    tt_jump_fcontext(wrap_main->fctx, cfs->__main);
}

tt_result_t __create_stack(IN tt_fiber_wrap_t *wrap_fb, IN tt_u32_t stack_size)
{
    void *stack;
    tt_u32_t size;

    TT_U32_ALIGN_INC_PAGE(stack_size);
    size = stack_size + (tt_g_page_size << 1);

#if TT_ENV_OS_IS_WINDOWS
#else
    stack = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (stack == NULL) {
        TT_ERROR_NTV("fail to reserve stack");
        return TT_FAIL;
    }

    if (mprotect(TT_PTR_INC(void, stack, tt_g_page_size),
                 stack_size,
                 PROT_READ | PROT_WRITE) != 0) {
        TT_ERROR_NTV("fail to change pages to read/write");
        munmap(stack, size);
        return TT_FAIL;
    }
#endif

    wrap_fb->stack = TT_PTR_INC(void, stack, stack_size + tt_g_page_size);
    wrap_fb->stack_size = stack_size;
    return TT_SUCCESS;
}

void __destroy_stack(IN tt_fiber_wrap_t *wrap_fb)
{
#if TT_ENV_OS_IS_WINDOWS
#else
    munmap(TT_PTR_DEC(void,
                      wrap_fb->stack,
                      wrap_fb->stack_size + tt_g_page_size),
           wrap_fb->stack_size + (tt_g_page_size << 1));
#endif
}
