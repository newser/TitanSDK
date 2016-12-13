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
@file tt_mutex_native.h
@brief system mutex APIs

this file implements mutex apis in system level
*/

#ifndef __TT_MUTEX_NATIVE__
#define __TT_MUTEX_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <log/tt_log.h>

#include <errno.h>
#include <pthread.h>
#include <string.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_mutex_attr_s;

typedef struct
{
    pthread_mutex_t mutex;
} tt_mutex_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn tt_result_t tt_mutex_create_ntv(IN tt_mutex_ntv_t *sys_mutex,
                                    IN tt_mutex_attr_portlayer_t *attr)
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
@fn tt_result_t tt_mutex_acquire_portlayer(tt_mutex_ntv_t *sys_mutex)
acquire a system mutex

@param [in] sys_mutex mutex to be acquired

@return
- TT_SUCCESS, if locking done
- TT_FAIL, otherwise

@note
return value MUST be checked, lock operation is implemented based on
system call which is out of ts control
*/
tt_inline tt_result_t tt_mutex_acquire_ntv(tt_mutex_ntv_t *sys_mutex)
{
    int ret = pthread_mutex_lock(&sys_mutex->mutex);
    if (ret == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to lock system mutex: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
    }
}

/**
@fn tt_result_t tt_mutex_try_acquire_ntv(tt_mutex_ntv_t *sys_mutex)
try to acquire a system mutex

@param [in] sys_mutex mutex to be acquired

@return
- TT_SUCCESS, if locking done
- TT_TIME_OUT, if can not lock the mutex this time
- TT_FAIL, otherwise

@note
return value MUST be checked, lock operation is implemented based on
system call which is out of ts control
*/
tt_inline tt_result_t tt_mutex_try_acquire_ntv(IN tt_mutex_ntv_t *sys_mutex)
{
    int ret = pthread_mutex_trylock(&sys_mutex->mutex);
    if (ret == 0) {
        return TT_SUCCESS;
    } else if (ret == EBUSY) {
        return TT_TIME_OUT;
    } else {
        TT_ERROR("fail to try lock system mutex: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
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
tt_inline tt_result_t tt_mutex_release_ntv(IN tt_mutex_ntv_t *sys_mutex)
{
    int ret = pthread_mutex_unlock(&sys_mutex->mutex);
    if (ret == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to unlock system mutex: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
    }
}

#endif /* __TT_MUTEX_NATIVE__ */
