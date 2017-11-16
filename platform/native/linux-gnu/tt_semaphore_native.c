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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_semaphore_native.h>

#include <misc/tt_assert.h>
#include <os/tt_semaphore.h>

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
                              IN tt_sem_attr_t *attr)
{
    if (sem_init(&sys_sem->sem, 0, count) == 0) {
        return TT_SUCCESS;
    } else {
        TT_FATAL("sem_init fail: %d[%s]", errno, strerror(errno));
        return TT_FAIL;
    }
}

void tt_sem_destroy_ntv(IN tt_sem_ntv_t *sys_sem)
{
    sem_destroy(&sys_sem->sem);
}

tt_bool_t tt_sem_acquire_ntv(IN tt_sem_ntv_t *sys_sem, IN tt_s64_t wait_ms)
{
    if (wait_ms == TT_TIME_INFINITE) {
    again1:
        if (sem_wait(&sys_sem->sem) == 0) {
            return TT_TRUE;
        } else if (errno == EINTR) {
            goto again1;
        } else {
            TT_FATAL("sem_wait fail: %d[%s]", errno, strerror(errno));
            tt_throw_exception_ntv(NULL);
            return TT_FALSE;
        }
    } else {
        struct timespec t;

        t.tv_sec = wait_ms / 1000;
        t.tv_nsec = (wait_ms % 1000) * 1000000;
    again2:
        if (sem_timedwait(&sys_sem->sem, &t) == 0) {
            return TT_TRUE;
        } else if (errno == EINTR) {
            goto again2;
        } else if (errno == ETIMEDOUT) {
            return TT_FALSE;
        } else {
            TT_FATAL("sem_wait fail: %d[%s]", errno, strerror(errno));
            tt_throw_exception_ntv(NULL);
            return TT_FALSE;
        }
    }
}

tt_bool_t tt_sem_try_acquire_ntv(IN tt_sem_ntv_t *sys_sem)
{
again:
    if (sem_trywait(&sys_sem->sem) == 0) {
        return TT_TRUE;
    } else if (errno == EINTR) {
        goto again;
    } else if (errno == EAGAIN) {
        return TT_FALSE;
    } else {
        TT_FATAL("sem_trywait fail: %d[%s]", errno, strerror(errno));
        tt_throw_exception_ntv(NULL);
        return TT_FALSE;
    }
}

void tt_sem_release_ntv(IN tt_sem_ntv_t *sys_sem)
{
    if (sem_post(&sys_sem->sem) != 0) {
        TT_FATAL("sem_post fail: %d[%s]", errno, strerror(errno));
        tt_throw_exception_ntv(NULL);
    }
}
