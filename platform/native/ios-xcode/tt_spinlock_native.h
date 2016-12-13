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
@file tt_spin_lock_native.h
@brief spin lock implemented by windows critical section

this files defines system APIs of spin lock implemented by windows critical
section
*/

#ifndef __TT_SPIN_LOCK_NATIVE__
#define __TT_SPIN_LOCK_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <errno.h>
#include <libkern/OSAtomic.h>
#include <pthread.h>
#include <string.h>

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
    OSSpinLock spinlock;
} tt_spinlock_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_result_t
tt_spinlock_component_init_ntv(IN struct tt_profile_s *profile)
{
    return TT_SUCCESS;
}

tt_inline tt_result_t tt_spinlock_create_ntv(IN tt_spinlock_ntv_t *lock,
                                             IN struct tt_spinlock_attr_s *attr)
{
    lock->spinlock = OS_SPINLOCK_INIT;
    return TT_SUCCESS;
}

tt_inline void tt_spinlock_destroy_ntv(IN tt_spinlock_ntv_t *lock)
{
}

tt_inline tt_result_t tt_spinlock_acquire_ntv(IN tt_spinlock_ntv_t *lock)
{
    OSSpinLockLock(&lock->spinlock);
    return TT_SUCCESS;
}

tt_inline tt_result_t tt_spinlock_try_acquire_ntv(IN tt_spinlock_ntv_t *lock)
{
    if (OSSpinLockTry(&lock->spinlock)) {
        return TT_SUCCESS;
    } else {
        return TT_TIME_OUT;
    }
}

tt_inline void tt_spinlock_release_ntv(IN tt_spinlock_ntv_t *lock)
{
    OSSpinLockUnlock(&lock->spinlock);
}

#endif // __TT_SPIN_LOCK_NATIVE__
