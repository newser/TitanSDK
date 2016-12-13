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

#include <os/tt_rwlock.h>

#include <misc/tt_assert.h>
#include <os/tt_atomic.h>

// portlayer header files
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

void tt_rwlock_component_register()
{
}

tt_result_t tt_rwlock_create(IN tt_rwlock_t *rwlock, IN tt_rwlock_attr_t *attr)
{
    TT_ASSERT(rwlock != NULL);

    if (attr != NULL) {
        tt_memcpy(&rwlock->attr, attr, sizeof(tt_rwlock_attr_t));
    } else {
        tt_rwlock_attr_default(&rwlock->attr);
    }

    if (!TT_OK(tt_rwlock_create_ntv(&rwlock->sys_rwlock, &rwlock->attr))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_rwlock_destroy(IN tt_rwlock_t *rwlock)
{
    TT_ASSERT(rwlock != NULL);

    tt_rwlock_destroy_ntv(&rwlock->sys_rwlock);
}

void tt_rwlock_attr_default(IN tt_rwlock_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->reserved = 0;
}

void __rwlock_acquire_r(IN tt_rwlock_t *rwlock
#if (TT_RWLOCK_DEBUG_OPT & TT_RWLOCK_LOCKER_DEBUG)
                        ,
                        IN const tt_char_t *function,
                        IN tt_u32_t line
#endif
                        )
{
    tt_result_t result;

    TT_ASSERT(rwlock != NULL);

    result = tt_rwlock_acquire_r_ntv(&rwlock->sys_rwlock);
    TT_ASSERT_ALWAYS(TT_OK(result));
}

tt_result_t __rwlock_try_acquire_r(IN tt_rwlock_t *rwlock
#if (TT_RWLOCK_DEBUG_OPT & TT_RWLOCK_LOCKER_DEBUG)
                                   ,
                                   IN const tt_char_t *function,
                                   IN tt_u32_t line
#endif
                                   )
{
    tt_result_t result;

    TT_ASSERT(rwlock != NULL);

    result = tt_rwlock_try_acquire_r_ntv(&rwlock->sys_rwlock);
    if (!TT_OK(result)) {
        TT_ASSERT_ALWAYS(result == TT_TIME_OUT);
        return TT_TIME_OUT;
    }

    return TT_SUCCESS;
}

void tt_rwlock_release_r(IN tt_rwlock_t *rwlock)
{
    tt_result_t result;

    TT_ASSERT(rwlock != NULL);

    result = tt_rwlock_release_r_ntv(&rwlock->sys_rwlock);
    TT_ASSERT_ALWAYS(TT_OK(result));
}

void __rwlock_acquire_w(IN tt_rwlock_t *rwlock
#if (TT_RWLOCK_DEBUG_OPT & TT_RWLOCK_LOCKER_DEBUG)
                        ,
                        IN const tt_char_t *function,
                        IN tt_u32_t line
#endif
                        )
{
    tt_result_t result;

    TT_ASSERT(rwlock != NULL);

    result = tt_rwlock_acquire_w_ntv(&rwlock->sys_rwlock);
    TT_ASSERT_ALWAYS(TT_OK(result));
}

tt_result_t __rwlock_try_acquire_w(IN tt_rwlock_t *rwlock
#if (TT_RWLOCK_DEBUG_OPT & TT_RWLOCK_LOCKER_DEBUG)
                                   ,
                                   IN const tt_char_t *function,
                                   IN tt_u32_t line
#endif
                                   )
{
    tt_result_t result;

    TT_ASSERT(rwlock != NULL);

    result = tt_rwlock_try_acquire_w_ntv(&rwlock->sys_rwlock);
    if (!TT_OK(result)) {
        TT_ASSERT_ALWAYS(result == TT_TIME_OUT);
        return TT_TIME_OUT;
    }

    return TT_SUCCESS;
}

void tt_rwlock_release_w(IN tt_rwlock_t *rwlock)
{
    tt_result_t result;

    TT_ASSERT(rwlock != NULL);

    result = tt_rwlock_release_w_ntv(&rwlock->sys_rwlock);
    TT_ASSERT_ALWAYS(TT_OK(result));
}
