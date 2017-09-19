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

/**
@file tt_fiber.h
@brief fiber
*/

#ifndef __TT_FIBER__
#define __TT_FIBER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_double_linked_list.h>
#include <algorithm/tt_list.h>
#include <os/tt_spinlock.h>
#include <os/tt_thread.h>

#if TT_ENV_OS_IS_WINDOWS
#include <tt_fiber_windows_wrapper.h>
#else
#include <tt_fiber_wrapper.h>
#endif

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_fiber_s;
struct tt_thread_s;

typedef tt_result_t (*tt_fiber_routine_t)(IN void *param);

// ========================================
// fiber scheduler
// ========================================

typedef struct tt_fiber_sched_s
{
    tt_list_t active;
    tt_list_t pending;
    struct tt_fiber_s *__main;
    struct tt_fiber_s *current;
    struct tt_thread_s *thread;
} tt_fiber_sched_t;

typedef struct
{
    tt_u32_t reserved;
} tt_fiber_sched_attr_t;

// ========================================
// fiber
// ========================================

typedef struct tt_fiber_s
{
    tt_lnode_t node;
    const tt_char_t *name;
    tt_fiber_routine_t routine;
    void *param;
    tt_fiber_sched_t *fs;
    tt_dlist_t ev;
    tt_list_t unexpired_tmr;
    tt_list_t expired_tmr;
    tt_fiber_wrap_t wrap_fb;
    tt_bool_t can_yield : 1;
    tt_bool_t end : 1;
    tt_bool_t recving : 1;
} tt_fiber_t;

typedef struct tt_fiber_attr_s
{
    tt_u32_t stack_size;
} tt_fiber_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// ========================================
// fiber scheduler
// ========================================

tt_export tt_fiber_sched_t *tt_fiber_sched_create(
    IN OPT tt_fiber_sched_attr_t *attr);

tt_export void tt_fiber_sched_destroy(IN tt_fiber_sched_t *fs);

tt_export void tt_fiber_sched_attr_default(IN tt_fiber_sched_attr_t *attr);

tt_inline tt_fiber_sched_t *tt_current_fiber_sched()
{
    tt_thread_t *t = tt_current_thread();
    return TT_COND(t != NULL, t->fiber_sched, NULL);
}

tt_inline tt_bool_t tt_fiber_sched_empty(IN tt_fiber_sched_t *fs)
{
    return tt_list_empty(&fs->active) && tt_list_empty(&fs->pending);
}

tt_export tt_fiber_t *tt_fiber_sched_find(IN tt_fiber_sched_t *fs,
                                          IN const tt_char_t *name);

tt_export tt_fiber_t *tt_fiber_sched_next(IN tt_fiber_sched_t *fs);

// ========================================
// fiber
// ========================================

tt_export tt_fiber_t *tt_fiber_create(IN OPT const tt_char_t *name,
                                      IN tt_fiber_routine_t routine,
                                      IN void *param,
                                      IN OPT tt_fiber_attr_t *attr);

tt_export void tt_fiber_destroy(IN tt_fiber_t *fb);

tt_export void tt_fiber_attr_default(IN tt_fiber_attr_t *attr);

tt_export void tt_fiber_suspend();

tt_export void tt_fiber_resume(IN tt_fiber_t *fb, IN tt_bool_t suspend);

tt_inline tt_fiber_t *tt_current_fiber()
{
    tt_fiber_sched_t *fs = tt_current_fiber_sched();
    return TT_COND(fs != NULL, fs->current, NULL);
}

tt_inline tt_bool_t tt_fiber_can_suspend()
{
    tt_fiber_t *f = tt_current_fiber();
    return TT_COND(f != NULL, f->can_yield, TT_FALSE);
}

tt_inline tt_fiber_t *tt_fiber_find(IN const tt_char_t *name)
{
    return tt_fiber_sched_find(tt_current_fiber_sched(), name);
}

#endif /* __TT_FIBER__ */
