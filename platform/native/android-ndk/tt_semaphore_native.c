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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_semaphore_native.h>

#include <misc/tt_assert.h>

#include <errno.h>
#include <string.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_sem_create_ntv(IN tt_sem_ntv_t *sys_sem,
                              IN tt_u32_t count,
                              IN struct tt_sem_attr_s *attr)
{
    int ret;
    pthread_condattr_t cond_attr;

    ret = pthread_mutex_init(&sys_sem->mutex, NULL);
    if (ret != 0) {
        TT_ERROR("fail to create system mutex: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
    }

    ret = pthread_condattr_init(&cond_attr);
    if (ret != 0) {
        TT_ERROR("fail to init system cond attr: %d[%s]", ret, strerror(ret));
        pthread_mutex_destroy(&sys_sem->mutex);
        return TT_FAIL;
    }

#if __ANDROID_API__ >= 21
    ret = pthread_condattr_setclock(&cond_attr, CLOCK_MONOTONIC);
    if (ret != 0) {
        TT_ERROR("fail to set system cond clock: %d[%s]", ret, strerror(ret));
        pthread_condattr_destroy(&cond_attr);
        pthread_mutex_destroy(&sys_sem->mutex);
        return TT_FAIL;
    }
#endif

    ret = pthread_cond_init(&sys_sem->cond, &cond_attr);
    pthread_condattr_destroy(&cond_attr);
    if (ret != 0) {
        TT_ERROR("fail to create system cond: %d[%s]", ret, strerror(ret));
        pthread_mutex_destroy(&sys_sem->mutex);
        return TT_FAIL;
    }

    if (count > 0x7FFFFFFF) {
        sys_sem->count = 0x7FFFFFFF;
    } else {
        sys_sem->count = (tt_s32_t)count;
    }

    return TT_SUCCESS;
}

void tt_sem_destroy_ntv(IN tt_sem_ntv_t *sys_sem)
{
    int ret;

    ret = pthread_cond_destroy(&sys_sem->cond);
    if (ret != 0) {
        TT_ERROR("fail to destroy system cond: %d[%s]", ret, strerror(ret));
    }

    ret = pthread_mutex_destroy(&sys_sem->mutex);
    if (ret != 0) {
        TT_ERROR("fail to destroy system mutex: %d[%s]", ret, strerror(ret));
    }
}

tt_bool_t tt_sem_acquire_ntv(IN tt_sem_ntv_t *sys_sem, IN tt_u32_t wait_ms)
{
    int ret;
    tt_bool_t result = TT_TRUE;

    ret = pthread_mutex_lock(&sys_sem->mutex);
    if (ret != 0) {
        TT_FATAL("fail to lock sem-mutex: %d[%s]", ret, strerror(ret));
        tt_throw_exception_ntv(NULL);
        return TT_FALSE;
    }

    --sys_sem->count;
    if (sys_sem->count < 0) {
        if (wait_ms == TT_TIME_INFINITE) {
            ret = pthread_cond_wait(&sys_sem->cond, &sys_sem->mutex);
            if (ret != 0) {
                TT_FATAL("fail to wait sem-cond: %d[%s]", ret, strerror(ret));
                tt_throw_exception_ntv(NULL);
                return TT_FALSE;
            }
        } else {
            struct timespec abstime;

            clock_gettime(CLOCK_MONOTONIC, &abstime);
            abstime.tv_sec += wait_ms / 1000;
            abstime.tv_nsec += (wait_ms % 1000) * 1000000;
            if (abstime.tv_nsec >= 1000000000) {
                abstime.tv_sec += 1;
                abstime.tv_nsec -= 1000000000;
            }

        rewait:
            ret = pthread_cond_timedwait(&sys_sem->cond,
                                         &sys_sem->mutex,
                                         &abstime);
            if (ret == ETIMEDOUT) {
                ++sys_sem->count;
                result = TT_FALSE;
            } else if (ret == EINTR) {
                goto rewait;
            } else if (ret != 0) {
                TT_FATAL("fail to wait sem-cond: %d[%s]", ret, strerror(ret));
                tt_throw_exception_ntv(NULL);
                return TT_FALSE;
            }
        }
    }

    ret = pthread_mutex_unlock(&sys_sem->mutex);
    if (ret != 0) {
        TT_FATAL("fail to unlock sem-mutex: %d[%s]", ret, strerror(ret));
        tt_throw_exception_ntv(NULL);
        return TT_FALSE;
    }

    return result;
}

tt_bool_t tt_sem_try_acquire_ntv(IN tt_sem_ntv_t *sys_sem)
{
    int ret;
    tt_bool_t result = TT_FALSE;

    ret = pthread_mutex_trylock(&sys_sem->mutex);
    if (ret == EBUSY) {
        return TT_FALSE;
    } else if (ret != 0) {
        TT_FATAL("fail to trylock sem-mutex: %d[%s]", ret, strerror(ret));
        tt_throw_exception_ntv(NULL);
        return TT_FALSE;
    }

    if (sys_sem->count > 0) {
        --sys_sem->count;
        result = TT_TRUE;
    }

    ret = pthread_mutex_unlock(&sys_sem->mutex);
    if (ret != 0) {
        TT_FATAL("fail to unlock sem-mutex: %d[%s]", ret, strerror(ret));
        tt_throw_exception_ntv(NULL);
        return TT_FALSE;
    }

    return result;
}

void tt_sem_release_ntv(IN tt_sem_ntv_t *sys_sem)
{
    int ret;

    ret = pthread_mutex_lock(&sys_sem->mutex);
    if (ret != 0) {
        TT_FATAL("fail to lock sem-mutex: %d[%s]", ret, strerror(ret));
        tt_throw_exception_ntv(NULL);
        return;
    }

    ++sys_sem->count;
    if (sys_sem->count <= 0) {
        ret = pthread_cond_signal(&sys_sem->cond);
        if (ret != 0) {
            TT_FATAL("fail to signal sem-cond: %d[%s]", ret, strerror(ret));
            tt_throw_exception_ntv(NULL);
            return;
        }
    }

    ret = pthread_mutex_unlock(&sys_sem->mutex);
    if (ret != 0) {
        TT_FATAL("fail to unlock sem-mutex: %d[%s]", ret, strerror(ret));
        tt_throw_exception_ntv(NULL);
        return;
    }
}
