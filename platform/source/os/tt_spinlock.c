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

#include <os/tt_spinlock.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
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

static tt_result_t __spinlock_component_init(IN tt_component_t *comp,
                                             IN tt_profile_t *profile);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_spinlock_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __spinlock_component_init,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_SPINLOCK, "Spinlock", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t __spinlock_component_init(IN tt_component_t *comp,
                                      IN tt_profile_t *profile)
{
    if (!TT_OK(tt_spinlock_component_init_ntv(profile))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_spinlock_create(IN tt_spinlock_t *slock,
                               IN tt_spinlock_attr_t *attr)
{
    TT_ASSERT(slock != NULL);

    if (attr != NULL) {
        tt_memcpy(&slock->attr, attr, sizeof(tt_spinlock_attr_t));
    } else {
        tt_spinlock_attr_default(&slock->attr);
    }

    if (!TT_OK(tt_spinlock_create_ntv(&slock->sys_spinlock, &slock->attr))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_spinlock_destroy(IN tt_spinlock_t *slock)
{
    TT_ASSERT(slock != NULL);

    tt_spinlock_destroy_ntv(&slock->sys_spinlock);
}

void tt_spinlock_attr_default(IN tt_spinlock_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->reserved = 0;
}

void __spinlock_acquire(IN tt_spinlock_t *slock
#if (TT_SPINLOCK_DEBUG_OPT & TT_SPINLOCK_LOCKER_DEBUG)
                        ,
                        IN const tt_char_t *function,
                        IN tt_u32_t line
#endif
                        )
{
    tt_result_t result;

    TT_ASSERT(slock != NULL);

    result = tt_spinlock_acquire_ntv(&slock->sys_spinlock);
    TT_ASSERT_ALWAYS(TT_OK(result));
}

tt_result_t __spinlock_try_acquire(IN tt_spinlock_t *slock
#if (TT_SPINLOCK_DEBUG_OPT & TT_SPINLOCK_LOCKER_DEBUG)
                                   ,
                                   IN const tt_char_t *function,
                                   IN tt_u32_t line
#endif
                                   )
{
    tt_result_t result;

    TT_ASSERT(slock != NULL);

    result = tt_spinlock_try_acquire_ntv(&slock->sys_spinlock);
    if (!TT_OK(result)) {
        TT_ASSERT_ALWAYS(result == TT_TIME_OUT);
        return TT_TIME_OUT;
    }

    return TT_SUCCESS;
}

void tt_spinlock_release(IN tt_spinlock_t *slock)
{
    TT_ASSERT(slock != NULL);

    tt_spinlock_release_ntv(&slock->sys_spinlock);
}
