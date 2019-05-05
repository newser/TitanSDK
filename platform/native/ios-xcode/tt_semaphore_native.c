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

#include <mach/mach.h>
#include <mach/semaphore.h>

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

tt_result_t tt_sem_create_ntv(IN tt_sem_ntv_t *sys_sem, IN tt_u32_t count,
                              IN tt_sem_attr_t *attr)
{
    kern_return_t r = semaphore_create(mach_task_self(), &sys_sem->sem,
                                       SYNC_POLICY_FIFO, count);
    if (r == KERN_SUCCESS) {
        return TT_SUCCESS;
    } else {
        TT_FATAL("semaphore_create fail: %d[%s]", r, mach_error_string(r));
        return TT_FAIL;
    }
}

void tt_sem_destroy_ntv(IN tt_sem_ntv_t *sys_sem)
{
    semaphore_destroy(mach_task_self(), sys_sem->sem);
}

tt_bool_t tt_sem_acquire_ntv(IN tt_sem_ntv_t *sys_sem, IN tt_s64_t wait_ms)
{
    kern_return_t r;

    if (wait_ms == TT_TIME_INFINITE) {
        // again1:
        kern_return_t r = semaphore_wait(sys_sem->sem);
        if (r == KERN_SUCCESS) {
            return TT_TRUE;
        } /*else if (r == KERN_ABORTED) {
            goto again1;
        }*/
        else {
            TT_FATAL("semaphore_wait fail: %d[%s]", r, mach_error_string(r));
            tt_throw_exception_ntv(NULL);
            return TT_FALSE;
        }
    } else {
        mach_timespec_t t;
        kern_return_t r;

        t.tv_sec = (unsigned int)(wait_ms / 1000);
        t.tv_nsec = (wait_ms % 1000) * 1000000;

        // again2:
        r = semaphore_timedwait(sys_sem->sem, t);
        if (r == KERN_SUCCESS) {
            return TT_TRUE;
        } /*else if (r == KERN_ABORTED) {
           goto again2;
        }*/
        else if (r == KERN_OPERATION_TIMED_OUT) {
            return TT_FALSE;
        } else {
            TT_FATAL("semaphore_wait fail: %d[%s]", r, mach_error_string(r));
            tt_throw_exception_ntv(NULL);
            return TT_FALSE;
        }
    }
}

tt_bool_t tt_sem_try_acquire_ntv(IN tt_sem_ntv_t *sys_sem)
{
    mach_timespec_t t = {0};
    kern_return_t r;

    // again:
    r = semaphore_timedwait(sys_sem->sem, t);
    if (r == KERN_SUCCESS) {
        return TT_TRUE;
    } /*else if (r == KERN_ABORTED) {
       goto again;
       }*/
    else if (r == KERN_OPERATION_TIMED_OUT) {
        return TT_FALSE;
    } else {
        TT_FATAL("semaphore_wait fail: %d[%s]", r, mach_error_string(r));
        tt_throw_exception_ntv(NULL);
        return TT_FALSE;
    }
}

void tt_sem_release_ntv(IN tt_sem_ntv_t *sys_sem)
{
    kern_return_t r = semaphore_signal(sys_sem->sem);
    if (r != KERN_SUCCESS) {
        TT_FATAL("semaphore_signal fail: %d[%s]", r, mach_error_string(r));
        tt_throw_exception_ntv(NULL);
    }
}
