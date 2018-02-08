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

#include <algorithm/tt_hashmap.h>
#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <os/tt_mutex.h>

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

static tt_mutex_t __malloc_lock;

static tt_hashmap_t __malloc_map;

static tt_oom_handler_t __s_oom_handler;

static void *__s_oom_param;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __malloc_component_init(IN tt_component_t *comp,
                                           IN tt_profile_t *profile);

static void __malloc_component_exit(IN tt_component_t *comp);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_memory_alloc_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __malloc_component_init, __malloc_component_exit,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_MEMORY_ALLOC,
                      "Memory Alloc",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

void tt_set_oom_handler(IN tt_oom_handler_t handler, IN void *param)
{
    __s_oom_handler = handler;
    __s_oom_param = param;
}

void *tt_malloc_tag(IN size_t size,
#ifdef TT_MEMORY_TAG_ENABLE
                    IN const tt_char_t *file,
                    IN const tt_char_t *function,
                    IN const tt_u32_t line
#endif
                    )
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

tt_result_t __malloc_component_init(IN tt_component_t *comp,
                                    IN tt_profile_t *profile)
{
#ifdef TT_MEMORY_TAG_ENABLE
    if (tt_mutex_create(&__malloc_lock, NULL)) {
        TT_ERROR("fail to create malloc lock");
        return TT_FAIL;
    }

    if (!TT_OK(tt_hmap_create(&__malloc_map, 127, NULL))) {
        TT_ERROR("fail to create malloc map");
        tt_mutex_destroy(&__malloc_lock);
        return TT_FAIL;
    }
#endif

    return TT_SUCCESS;
}

void __malloc_component_exit(IN tt_component_t *comp)
{
}
