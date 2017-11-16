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

#include <memory/tt_memory_alloc.h>

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

static tt_oom_handler_t __s_oom_handler;

static void *__s_oom_param;

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_set_oom_handler(IN tt_oom_handler_t handler, IN void *param)
{
    __s_oom_handler = handler;
    __s_oom_param = param;
}

void *tt_malloc(IN size_t size)
{
    void *p = tt_c_malloc(size);
    if ((p == NULL) && (__s_oom_handler != NULL)) {
        __s_oom_handler(__s_oom_param);
    }
    return p;
}

void *tt_realloc(IN void *ptr, IN size_t size)
{
    void *p = tt_c_realloc(ptr, size);
    if ((p == NULL) && (__s_oom_handler != NULL)) {
        __s_oom_handler(__s_oom_param);
    }
    return p;
}
