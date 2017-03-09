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
                               IN OPT tt_spinlock_attr_t *attr)
{
    tt_spinlock_attr_t __attr;

    TT_ASSERT(slock != NULL);

    if (attr != NULL) {
        tt_spinlock_attr_default(&__attr);
        attr = &__attr;
    }

    return tt_spinlock_create_ntv(&slock->sys_lock, attr);
}

void tt_spinlock_destroy(IN tt_spinlock_t *slock)
{
    TT_ASSERT(slock != NULL);

    tt_spinlock_destroy_ntv(&slock->sys_lock);
}

void tt_spinlock_attr_default(IN tt_spinlock_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->reserved = 0;
}
