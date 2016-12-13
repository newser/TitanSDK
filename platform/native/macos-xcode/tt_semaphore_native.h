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

#include <tt_sys_error.h>

#include <errno.h>
#include <pthread.h>
#include <string.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define __INFINITE_SEM_NUM (0x7FFFFFFF)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_sem_attr_s;

typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    tt_s32_t avail_num;
} tt_sem_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn tt_result_t tt_sem_create_ntv(IN tt_sem_ntv_t *sys_sem,
                                  IN tt_u32_t init_num,
                                  IN tt_sem_attr_portlayer_t *attr)
create a system semaphore

@param [inout] sys_sem system semaphore to be created
@param [in] attr system semaphore attribute

@return
- TT_SUCCESS, if creating system semaphore succeeds
- TT_FAIL, otherwise
*/
extern tt_result_t tt_sem_create_ntv(IN tt_sem_ntv_t *sys_sem,
                                     IN tt_u32_t init_num,
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
tt_inline tt_result_t tt_sem_acquire_ntv(IN tt_sem_ntv_t *sys_sem,
                                         IN tt_u32_t wait_ms)
{
    struct timespec wait_ts;
    tt_result_t result = TT_SUCCESS;

    int ret = pthread_mutex_lock(&sys_sem->mutex);
    if (ret != 0) {
        TT_ERROR("fail to lock sem-mutex: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
    }

    --sys_sem->avail_num;
    if (sys_sem->avail_num < 0) {
        if (wait_ms == TT_TIME_INFINITE) {
            ret = pthread_cond_wait(&sys_sem->cond, &sys_sem->mutex);
            if (ret != 0) {
                TT_ERROR("fail to wait sem-cond: %d[%s]", ret, strerror(ret));
                result = TT_FAIL;
            }
        } else {
            wait_ts.tv_sec = wait_ms / 1000;
            wait_ts.tv_nsec = (wait_ms % 1000) * 1000000;
            ret = pthread_cond_timedwait_relative_np(&sys_sem->cond,
                                                     &sys_sem->mutex,
                                                     &wait_ts);
            if (ret == ETIMEDOUT) {
                ++sys_sem->avail_num;
                result = TT_TIME_OUT;
            } else if (ret != 0) {
                TT_ERROR("fail to wait sem-cond: %d[%s]", ret, strerror(ret));
                result = TT_FAIL;
            }
        }
    }

    ret = pthread_mutex_unlock(&sys_sem->mutex);
    if (ret != 0) {
        TT_ERROR("fail to unlock sem-mutex: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
    }

    return result;
}

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
tt_inline tt_result_t tt_sem_try_acquire_ntv(IN tt_sem_ntv_t *sys_sem)
{
    tt_bool_t acquired = TT_FALSE;

    int ret = pthread_mutex_trylock(&sys_sem->mutex);
    if (ret == EBUSY) {
        return TT_TIME_OUT;
    } else if (ret != 0) {
        TT_ERROR("fail to trylock sem-mutex: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
    }

    if (sys_sem->avail_num > 0) {
        --sys_sem->avail_num;
        acquired = TT_TRUE;
    }

    ret = pthread_mutex_unlock(&sys_sem->mutex);
    if (ret != 0) {
        TT_ERROR("fail to unlock sem-mutex: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
    }

    if (acquired) {
        return TT_SUCCESS;
    } else {
        return TT_TIME_OUT;
    }
}

/**
@fn tt_result_t tt_sem_release_ntv(IN tt_sem_ntv_t *sys_sem)
post a system semaphore

@param [in] sem semaphore to be posted

@return
- TT_SUCCESS, if the semaphore is posted
- TT_FAIL, otherwise
*/
tt_inline tt_result_t tt_sem_release_ntv(IN tt_sem_ntv_t *sys_sem)
{
    int ret = pthread_mutex_lock(&sys_sem->mutex);
    if (ret != 0) {
        TT_ERROR("fail to lock sem-mutex: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
    }

    ++sys_sem->avail_num;
    if (sys_sem->avail_num <= 0) {
        ret = pthread_cond_signal(&sys_sem->cond);
        if (ret != 0) {
            TT_ERROR("fail to signal sem-cond: %d[%s]", ret, strerror(ret));
            return TT_FAIL;
        }
    }

    ret = pthread_mutex_unlock(&sys_sem->mutex);
    if (ret != 0) {
        TT_ERROR("fail to unlock sem-mutex: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

#endif /* __TT_SEMAPHORE_NATIVE__ */
