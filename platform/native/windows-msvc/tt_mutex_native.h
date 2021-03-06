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
@file tt_mutex_native.h
@brief system mutex APIs

this file implements mutex apis in system level
*/

#ifndef __TT_MUTEX_NATIVE__
#define __TT_MUTEX_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_assert_native.h>
#include <tt_sys_error.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_mutex_attr_s;

typedef struct
{
    HANDLE h_mutex;
} tt_mutex_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn tt_result_t tt_mutex_create_ntv(IN tt_mutex_ntv_t *sys_mutex,
                                    IN tt_mutex_attr_t *attr)
create a system mutex

@param [inout] sys_mutex system mutex to be created
@param [in] attr system mutex attribute

@return
- TT_SUCCESS, if creating system mutex succeeds
- TT_FAIL, otherwise
*/
extern tt_result_t tt_mutex_create_ntv(IN tt_mutex_ntv_t *sys_mutex,
                                       IN struct tt_mutex_attr_s *attr);

/**
@fn void tt_mutex_destroy_ntv(IN tt_mutex_ntv_t *sys_mutex)
destroy a system mutex

@param [in] sys_mutex mutex to be destroyed

@return
- TT_SUCCESS, destroying mutex done
- TT_FAIL, some error occur

@note
although the result may not affect caller's flow, it still print an error
message
*/
extern void tt_mutex_destroy_ntv(IN tt_mutex_ntv_t *sys_mutex);

/**
@fn tt_result_t tt_mutex_acquire_ntv(tt_mutex_ntv_t *sys_mutex)
acquire a system mutex

@param [in] sys_mutex mutex to be acquired

@return
- TT_SUCCESS, if locking done
- TT_FAIL, otherwise

@note
return value MUST be checked, lock operation is implemented based on
system call which is out of ts control
*/
tt_inline void tt_mutex_acquire_ntv(tt_mutex_ntv_t *sys_mutex)
{
    if (WaitForSingleObject(sys_mutex->h_mutex, INFINITE) != WAIT_OBJECT_0) {
        TT_ERROR_NTV("fail to lock system mutex");
        tt_throw_exception_ntv(NULL);
    }
}

/**
@fn tt_result_t tt_mutex_try_acquire_ntv(tt_mutex_ntv_t *sys_mutex)
try to acquire a system mutex

@param [in] sys_mutex mutex to be acquired

@return
- TT_SUCCESS, if locking done
- TT_E_TIMEOUT, if can not lock the mutex this time
- TT_FAIL, otherwise

@note
return value MUST be checked, lock operation is implemented based on
system call which is out of ts control
*/
tt_inline tt_bool_t tt_mutex_try_acquire_ntv(IN tt_mutex_ntv_t *sys_mutex)
{
    DWORD ret = WaitForSingleObject(sys_mutex->h_mutex, 0);
    if (ret == WAIT_OBJECT_0) {
        return TT_TRUE;
    } else if (ret == WAIT_TIMEOUT) {
        return TT_FALSE;
    } else {
        TT_ERROR_NTV("fail to lock system mutex");
        tt_throw_exception_ntv(NULL);
        return TT_FALSE;
    }
}

/**
@fn tt_result_t tt_mutex_release_ntv(IN tt_mutex_ntv_t *sys_mutex)
release a system mutex

@param [in] sys_mutex mutex to be released

@return
- TT_SUCCESS, if unlocking done
- TT_FAIL, otherwise
*/
tt_inline void tt_mutex_release_ntv(IN tt_mutex_ntv_t *sys_mutex)
{
    if (!ReleaseMutex(sys_mutex->h_mutex)) {
        TT_ERROR_NTV("fail to unlock system mutex");
        tt_throw_exception_ntv(NULL);
    }
}

#endif /* __TT_MUTEX_NATIVE__ */
