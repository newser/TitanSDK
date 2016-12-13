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

#include <os/tt_semaphore.h>

#include <misc/tt_assert.h>

#include <tt_cstd_api.h>

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

void tt_sem_component_register()
{
}

tt_result_t tt_sem_create(IN tt_sem_t *sem,
                          IN tt_u32_t init_count,
                          IN tt_sem_attr_t *attr)
{
    TT_ASSERT(sem != NULL);

    if (attr != NULL) {
        tt_memcpy(&sem->attr, attr, sizeof(tt_sem_attr_t));
    } else {
        tt_sem_attr_default(&sem->attr);
    }

    if (!TT_OK(tt_sem_create_ntv(&sem->sys_sem, init_count, &sem->attr))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_sem_destroy(IN tt_sem_t *sem)
{
    TT_ASSERT(sem != NULL);

    tt_sem_destroy_ntv(&sem->sys_sem);
}

void tt_sem_attr_default(IN tt_sem_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->reserved = 0;
}

tt_result_t __sem_acquire(IN tt_sem_t *sem,
                          IN tt_u32_t wait_ms
#if (TT_SEM_DEBUG_OPT & TT_SEM_LOCKER_DEBUG)
                          ,
                          IN const tt_char_t *function,
                          IN tt_u32_t line
#endif
                          )
{
    tt_result_t result;

    TT_ASSERT(sem != NULL);

    result = tt_sem_acquire_ntv(&sem->sys_sem, wait_ms);
    if (!TT_OK(result)) {
        TT_ASSERT_ALWAYS(result == TT_TIME_OUT);
        return TT_TIME_OUT;
    }

    return TT_SUCCESS;
}

tt_result_t __sem_try_acquire(IN tt_sem_t *sem
#if (TT_SEM_DEBUG_OPT & TT_SEM_LOCKER_DEBUG)
                              ,
                              IN const tt_char_t *function,
                              IN tt_u32_t line
#endif
                              )
{
    tt_result_t result;

    TT_ASSERT(sem != NULL);

    result = tt_sem_try_acquire_ntv(&sem->sys_sem);
    if (!TT_OK(result)) {
        TT_ASSERT_ALWAYS(result == TT_TIME_OUT);
        return TT_TIME_OUT;
    }

    return TT_SUCCESS;
}

void tt_sem_release(IN tt_sem_t *sem)
{
    tt_result_t result;

    TT_ASSERT(sem != NULL);

    result = tt_sem_release_ntv(&sem->sys_sem);
    TT_ASSERT_ALWAYS(TT_OK(result));
}
