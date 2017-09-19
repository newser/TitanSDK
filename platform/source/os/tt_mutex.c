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

#include <os/tt_mutex.h>

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

void tt_mutex_component_register()
{
}

tt_result_t tt_mutex_create(IN tt_mutex_t *mutex, IN OPT tt_mutex_attr_t *attr)
{
    tt_mutex_attr_t __attr;

    TT_ASSERT(mutex != NULL);

    if (attr == NULL) {
        tt_mutex_attr_default(&__attr);
        attr = &__attr;
    }

    return tt_mutex_create_ntv(&mutex->sys_mutex, attr);
}

void tt_mutex_destroy(IN tt_mutex_t *mutex)
{
    TT_ASSERT(mutex != NULL);

    tt_mutex_destroy_ntv(&mutex->sys_mutex);
}

void tt_mutex_attr_default(IN tt_mutex_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->config_recursive = TT_FALSE;
    attr->recursive = TT_FALSE;
}
