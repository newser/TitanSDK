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
@file tt_spinlock_native_cs.h
@brief spin lock implemented by windows critical section

this files defines system APIs of spin lock implemented by windows spin
critical section
*/

#ifndef __TT_SPINLOCK_NATIVE_CS__
#define __TT_SPINLOCK_NATIVE_CS__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_sys_error.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;
struct tt_spinlock_attr_s;

typedef struct
{
    CRITICAL_SECTION cs;
} tt_spinlock_ntv_cs_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_result_t
tt_spinlock_component_init_ntv_cs(IN struct tt_profile_s *profile)
{
    return TT_SUCCESS;
}

extern tt_result_t tt_spinlock_create_ntv_cs(
    IN tt_spinlock_ntv_cs_t *lock, IN struct tt_spinlock_attr_s *attr);

extern void tt_spinlock_destroy_ntv_cs(IN tt_spinlock_ntv_cs_t *lock);

tt_inline tt_result_t tt_spinlock_acquire_ntv_cs(IN tt_spinlock_ntv_cs_t *lock)
{
    EnterCriticalSection(&lock->cs);
    return TT_SUCCESS;
}

tt_inline tt_result_t
tt_spinlock_try_acquire_ntv_cs(IN tt_spinlock_ntv_cs_t *lock)
{
    if (TryEnterCriticalSection(&lock->cs)) {
        return TT_SUCCESS;
    } else {
        return TT_TIME_OUT;
    }
}

tt_inline void tt_spinlock_release_ntv_cs(IN tt_spinlock_ntv_cs_t *lock)
{
    LeaveCriticalSection(&lock->cs);
}

#endif /* __TT_SPINLOCK_NATIVE_CS__ */
