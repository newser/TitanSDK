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

tt_result_t tt_sem_create(IN tt_sem_t *sem, IN tt_u32_t count,
                          IN OPT tt_sem_attr_t *attr)
{
    tt_sem_attr_t __attr;

    TT_ASSERT(sem != NULL);
    TT_ASSERT(count <= TT_SEM_MAX_COUNT);

    if (attr != NULL) {
        tt_sem_attr_default(&__attr);
        attr = &__attr;
    }

    return tt_sem_create_ntv(&sem->sys_sem, count, attr);
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
