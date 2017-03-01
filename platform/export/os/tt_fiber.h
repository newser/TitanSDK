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
@file tt_fiber.h
@brief fiber
*/

#ifndef __TT_FIBER__
#define __TT_FIBER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef void (*tt_fiber_routine_t)(IN void *param);

// ========================================
// fiber scheduler
// ========================================

typedef struct
{
    tt_u32_t reserved;
} tt_fiber_sched_t;

typedef struct
{
    tt_u32_t reserved;
} tt_fiber_sched_attr_t;

// ========================================
// fiber
// ========================================

typedef struct
{
    tt_u32_t reserved;
} tt_fiber_t;

typedef struct
{
    tt_u32_t reserved;
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

extern tt_fiber_sched_t *tt_fiber_sched_create(
    IN OPT tt_fiber_sched_attr_t *attr);

extern void tt_fiber_sched_destroy(IN tt_fiber_sched_t *fs);

extern void tt_fiber_sched_attr_default(IN tt_fiber_sched_attr_t *attr);

// ========================================
// fiber
// ========================================

extern tt_fiber_t *tt_fiber_create(IN tt_fiber_routine_t routine,
                                   IN void *param,
                                   IN OPT tt_fiber_attr_t *attr);

extern void tt_fiber_destroy(IN tt_fiber_t *fiber);

extern void tt_fiber_attr_default(IN tt_fiber_attr_t *attr);

extern void tt_fiber_yield();

extern void tt_fiber_resume(IN tt_fiber_t *fiber);

#endif /* __TT_FIBER__ */
