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
@file tt_spin_lock_native_ul.h
@brief spin lock implemented by user level spinning

this files defines system APIs of spin lock implemented user level spinning
*/

#ifndef __TT_SPIN_LOCK_NATIVE_UL__
#define __TT_SPIN_LOCK_NATIVE_UL__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_sys_error.h>

#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define __SPINLOCK_RELEASED 0
#define __SPINLOCK_ACQUIRED 1

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;
struct tt_spinlock_attr_s;

typedef struct
{
    volatile LONG status;
} tt_spinlock_ntv_ul_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_result_t
tt_spinlock_component_init_ntv_ul(IN struct tt_profile_s *profile)
{
    return TT_SUCCESS;
}

extern tt_result_t tt_spinlock_create_ntv_ul(
    IN tt_spinlock_ntv_ul_t *lock, IN struct tt_spinlock_attr_s *attr);

extern void tt_spinlock_destroy_ntv_ul(IN tt_spinlock_ntv_ul_t *lock);

tt_inline tt_result_t tt_spinlock_acquire_ntv_ul(IN tt_spinlock_ntv_ul_t *lock)
{
acq_again:

    // spin utill the lock is released
    while (lock->status == __SPINLOCK_ACQUIRED)
        ;

    // do stable check and insert necessary barrier
    if (InterlockedCompareExchange(&lock->status,
                                   __SPINLOCK_ACQUIRED,
                                   __SPINLOCK_RELEASED) !=
        __SPINLOCK_RELEASED) {
        // someone else acquire it
        goto acq_again;
    }
    // locked

    return TT_SUCCESS;
}

tt_inline tt_result_t
tt_spinlock_try_acquire_ntv_ul(IN tt_spinlock_ntv_ul_t *lock)
{
    // do stable check and insert necessary barrier
    if (InterlockedCompareExchange(&lock->status,
                                   __SPINLOCK_ACQUIRED,
                                   __SPINLOCK_RELEASED) ==
        __SPINLOCK_RELEASED) {
        // locked
        return TT_SUCCESS;
    } else {
        return TT_TIME_OUT;
    }
}

tt_inline void tt_spinlock_release_ntv_ul(IN tt_spinlock_ntv_ul_t *lock)
{
    TT_ASSERT_ALWAYS(InterlockedCompareExchange(&lock->status,
                                                __SPINLOCK_RELEASED,
                                                __SPINLOCK_ACQUIRED) ==
                     __SPINLOCK_ACQUIRED);
}

#endif /* __TT_SPIN_LOCK_NATIVE_UL__ */
