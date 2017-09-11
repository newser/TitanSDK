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

tt_result_t tt_rwlock_create(IN tt_rwlock_t *rwlock,
                             IN OPT tt_rwlock_attr_t *attr)
{
    tt_rwlock_attr_t __attr;

    TT_ASSERT(rwlock != NULL);

    if (attr != NULL) {
        tt_rwlock_attr_default(&__attr);
        attr = &__attr;
    }

    return tt_rwlock_create_ntv(&rwlock->sys_rwlock, attr);
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
