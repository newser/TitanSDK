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
@file tt_spin_lock_native.h
@brief spin lk implemented by windows critical section

this files defines system APIs of spin lk implemented by windows critical
section
*/

#ifndef __TT_SPIN_LOCK_NATIVE__
#define __TT_SPIN_LOCK_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <misc/tt_util.h>

#include <errno.h>
#include <pthread.h>
#include <string.h>

#include <Availability.h>

#ifdef __MAC_10_12 // 10.12 and later
#include <os/lock.h>
#else
#include <libkern/OSAtomic.h>
#endif

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
#ifdef __MAC_10_12
    os_unfair_lock lk;
#else
    OSSpinLock lk;
#endif
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

tt_inline tt_result_t tt_spinlock_create_ntv(IN tt_spinlock_ntv_t *slock,
                                             IN struct tt_spinlock_attr_s *attr)
{
#ifdef __MAC_10_12
    slock->lk = OS_UNFAIR_LOCK_INIT;
#else
    slock->lk = OS_SPINLOCK_INIT;
#endif

    return TT_SUCCESS;
}

tt_inline void tt_spinlock_destroy_ntv(IN tt_spinlock_ntv_t *slock)
{
}

tt_inline void tt_spinlock_acquire_ntv(IN tt_spinlock_ntv_t *slock)
{
#ifdef __MAC_10_12
    os_unfair_lock_lock(&slock->lk);
#else
    OSSpinLockLock(&slock->lk);
#endif
}

tt_inline tt_bool_t tt_spinlock_try_acquire_ntv(IN tt_spinlock_ntv_t *slock)
{
#ifdef __MAC_10_12
    return TT_BOOL(os_unfair_lock_trylock(&slock->lk));
#else
    return TT_BOOL(OSSpinLockTry(&slock->lk));
#endif
}

tt_inline void tt_spinlock_release_ntv(IN tt_spinlock_ntv_t *slock)
{
#ifdef __MAC_10_12
    os_unfair_lock_unlock(&slock->lk);
#else
    OSSpinLockUnlock(&slock->lk);
#endif
}

#endif // __TT_SPIN_LOCK_NATIVE__
