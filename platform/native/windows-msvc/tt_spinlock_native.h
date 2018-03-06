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
@file tt_spinlock_native.h
@brief spin lock implemented by windows critical section

this files defines system APIs of spin lock implemented by windows spin
critical section
*/

#ifndef __TT_SPINLOCK_NATIVE__
#define __TT_SPINLOCK_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <misc/tt_util.h>

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
    __declspec(align(4)) LONG v;
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

tt_inline void tt_spinlock_component_exit_ntv()
{
}

extern tt_result_t tt_spinlock_create_ntv(IN tt_spinlock_ntv_t *slock,
                                          IN struct tt_spinlock_attr_s *attr);

extern void tt_spinlock_destroy_ntv(IN tt_spinlock_ntv_t *slock);

tt_inline void tt_spinlock_acquire_ntv(IN tt_spinlock_ntv_t *slock)
{
    while (InterlockedCompareExchange(&slock->v, 1, 0) != 0)
        ;
}

tt_inline tt_bool_t tt_spinlock_try_acquire_ntv(IN tt_spinlock_ntv_t *slock)
{
    return TT_BOOL(InterlockedCompareExchange(&slock->v, 1, 0) == 0);
}

tt_inline void tt_spinlock_release_ntv(IN tt_spinlock_ntv_t *slock)
{
    TT_ASSERT_ALWAYS(InterlockedCompareExchange(&slock->v, 0, 1) == 1);
}

#endif /* __TT_SPINLOCK_NATIVE__ */
