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
@file tt_semaphore_native.h
@brief system semaphore APIs

this file implements semaphore apis in system level
*/

#ifndef __TT_SEMAPHORE_NATIVE__
#define __TT_SEMAPHORE_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <pthread.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_sem_attr_s;

typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    tt_s32_t count;
} tt_sem_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn tt_result_t tt_sem_create_ntv(IN tt_sem_ntv_t *sys_sem,
                                  IN tt_u32_t count,
                                  IN tt_sem_attr_portlayer_t *attr)
create a system semaphore

@param [inout] sys_sem system semaphore to be created
@param [in] attr system semaphore attribute

@return
- TT_SUCCESS, if creating system semaphore succeeds
- TT_FAIL, otherwise
*/
extern tt_result_t tt_sem_create_ntv(IN tt_sem_ntv_t *sys_sem,
                                     IN tt_u32_t count,
                                     IN struct tt_sem_attr_s *attr);

/**
@fn void tt_sem_destroy_ntv(IN tt_sem_ntv_t *sys_sem)
destroy a system semaphore

@param [in] sys_sem semaphore to be destroyed

@return
- TT_TRUE, if semaphore specified by param is destroyed
- TT_FALSE, otherwise
*/
extern void tt_sem_destroy_ntv(IN tt_sem_ntv_t *sys_sem);

/**
@fn tt_result_t tt_sem_acquire_ntv(IN tt_sem_ntv_t *sys_sem,
                                   IN tt_u32_t wait_ms)
wait a system semaphore

@param [in] sem semaphore to be waited
@param [in] wait_ms maximum time to wait on the semaphore
- TT_SEM_WAIT_INFINITE to wait for ever

@return
- TT_SUCCESS, if the semaphore is waited
- TT_TIME_OUT, if the time specified expires
- TT_FAIL, otherwise
*/
extern tt_bool_t tt_sem_acquire_ntv(IN tt_sem_ntv_t *sys_sem,
                                    IN tt_s64_t wait_ms);

/**
@fn tt_result_t tt_sem_try_acquire_ntv(IN tt_sem_ntv_t *sys_sem)
wait a system semaphore

@param [in] sem semaphore to be waited
@param [in] wait_ms maximum time to wait on the semaphore
- TT_SEM_WAIT_INFINITE to wait for ever

@return
- TT_SUCCESS, if the semaphore is waited
- TT_TIME_OUT, if sema can not be waited now
- TT_FAIL, otherwise
*/
extern tt_bool_t tt_sem_try_acquire_ntv(IN tt_sem_ntv_t *sys_sem);

/**
@fn tt_result_t tt_sem_release_ntv(IN tt_sem_ntv_t *sys_sem)
post a system semaphore

@param [in] sem semaphore to be posted

@return
- TT_SUCCESS, if the semaphore is posted
- TT_FAIL, otherwise
*/
extern void tt_sem_release_ntv(IN tt_sem_ntv_t *sys_sem);

#endif /* __TT_SEMAPHORE_NATIVE__ */
