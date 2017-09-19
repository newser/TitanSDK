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
@file tt_rwlock_native.h
@brief system read write lock APIs

this file implements read write lock apis in system level
*/

#ifndef __TT_RWLOCK_NATIVE__
#define __TT_RWLOCK_NATIVE__

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

struct tt_rwlock_attr_s;

typedef struct
{
    SRWLOCK lock;
} tt_rwlock_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn tt_result_t tt_rwlock_create_ntv(IN tt_rwlock_ntv_t *sys_rwlock,
                                         IN tt_rwlock_attr_t *attr)
create a system rwlock

@param [inout] sys_rwlock system rwlock to be created
@param [in] attr system rwlock attribute

@return
- TT_SUCCESS, if creating system rwlock succeeds
- TT_FAIL, otherwise
*/
extern tt_result_t tt_rwlock_create_ntv(IN tt_rwlock_ntv_t *sys_rwlock,
                                        IN struct tt_rwlock_attr_s *attr);

/**
@fn void tt_rwlock_destroy_ntv(IN tt_rwlock_ntv_t *sys_rwlock)
destroy a system rwlock

@param [in] sys_rwlock rwlock to be destroyed

@return
- TT_SUCCESS, destroying rwlock done
- TT_FAIL, some error occur

@note
although the result may not affect caller's flow, it still print an error
message
*/
extern void tt_rwlock_destroy_ntv(IN tt_rwlock_ntv_t *sys_rwlock);

/**
@fn void tt_rwlock_acquire_r_ntv(tt_rwlock_ntv_t *sys_rwlock)
acquire a system rwlock to read

@param [in] sys_rwlock rwlock to be acquired

@return
- TT_SUCCESS, if locking done
- TT_FAIL, otherwise

@note
return value MUST be checked, lock operation is implemented based on
system call which is out of ts control
*/
tt_inline void tt_rwlock_acquire_r_ntv(IN tt_rwlock_ntv_t *sys_rwlock)
{
    AcquireSRWLockShared(&sys_rwlock->lock);
}

/**
@fn void tt_rwlock_try_acquire_r_ntv(tt_rwlock_ntv_t *sys_rwlock)
try to acquire a system rwlock to read

@param [in] sys_rwlock rwlock to be acquired

@return
- TT_SUCCESS, if locking done
- TT_FAIL, otherwise

@note
return value MUST be checked, lock operation is implemented based on
system call which is out of ts control
*/
tt_inline tt_bool_t tt_rwlock_try_acquire_r_ntv(IN tt_rwlock_ntv_t *sys_rwlock)
{
    return TT_BOOL(TryAcquireSRWLockShared(&sys_rwlock->lock));
}

/**
@fn void tt_rwlock_release_r_ntv(IN tt_rwlock_ntv_t *sys_rwlock)
release a system rwlock after reading

@param [in] sys_rwlock rwlock to be released

@return
- TT_SUCCESS, if unlocking done
- TT_FAIL, otherwise
*/
tt_inline void tt_rwlock_release_r_ntv(IN tt_rwlock_ntv_t *sys_rwlock)
{
    ReleaseSRWLockShared(&sys_rwlock->lock);
}

/**
@fn void tt_rwlock_acquire_w_ntv(tt_rwlock_ntv_t *sys_rwlock)
acquire a system rwlock to write

@param [in] sys_rwlock rwlock to be acquired

@return
- TT_SUCCESS, if locking done
- TT_FAIL, otherwise

@note
return value MUST be checked, lock operation is implemented based on
system call which is out of ts control
*/
tt_inline void tt_rwlock_acquire_w_ntv(IN tt_rwlock_ntv_t *sys_rwlock)
{
    AcquireSRWLockExclusive(&sys_rwlock->lock);
}

/**
@fn void tt_rwlock_try_acquire_w_ntv(tt_rwlock_ntv_t *sys_rwlock)
try to acquire a system rwlock to write

@param [in] sys_rwlock rwlock to be acquired

@return
- TT_SUCCESS, if locking done
- TT_FAIL, otherwise

@note
return value MUST be checked, lock operation is implemented based on
system call which is out of ts control
*/
tt_inline tt_bool_t tt_rwlock_try_acquire_w_ntv(IN tt_rwlock_ntv_t *sys_rwlock)
{
    return TT_BOOL(TryAcquireSRWLockExclusive(&sys_rwlock->lock));
}

/**
@fn void tt_rwlock_release_w_ntv(IN tt_rwlock_ntv_t *sys_rwlock)
release a system rwlock after writing

@param [in] sys_rwlock rwlock to be released

@return
- TT_SUCCESS, if unlocking done
- TT_FAIL, otherwise
*/
tt_inline void tt_rwlock_release_w_ntv(IN tt_rwlock_ntv_t *sys_rwlock)
{
    ReleaseSRWLockExclusive(&sys_rwlock->lock);
}

#endif /* __TT_RWLOCK_NATIVE__ */
