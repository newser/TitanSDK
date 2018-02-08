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

#define __MTAG_OK(m) ((m)->addr == TT_PTR_INC(void, m, sizeof(__mtag_t)))

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    void *addr;
    tt_size_t size;
    const tt_char_t *file;
    const tt_char_t *function;
    tt_hnode_t hnode;
    tt_u32_t line;
} __mtag_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_mutex_t __mtag_lock;

static tt_hashmap_t __mtag_map;

static tt_bool_t __mtag_initialized;

static tt_oom_handler_t __s_oom_handler;

static void *__s_oom_param;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __mtag_component_init(IN tt_component_t *comp,
                                         IN tt_profile_t *profile);

static void __mtag_component_exit(IN tt_component_t *comp);

static void __mtag_destroy(IN tt_hnode_t *hnode);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_memory_tag_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __mtag_component_init, __mtag_component_exit,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_MEMORY_TAG,
                      "Memory Alloc",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

void tt_memory_tag_dump(IN tt_u32_t flag)
{
#ifdef TT_MEMORY_TAG_ENABLE
    tt_hmap_iter_t iter;
    tt_hnode_t *hnode;
    tt_size_t size = 0;

    if (!__mtag_initialized) {
        return;
    }

    tt_mutex_acquire(&__mtag_lock);

    tt_hmap_iter(&__mtag_map, &iter);
    while ((hnode = tt_hmap_iter_next(&iter)) != NULL) {
        __mtag_t *mtag = TT_CONTAINER(hnode, __mtag_t, hnode);
        TT_ASSERT(__MTAG_OK(mtag));

        if (flag & TT_MEMORY_TAG_EACH) {
            tt_printf("[%p][%dbytes] from [%s:%d]\n",
                      mtag->addr,
                      (tt_s32_t)mtag->size,
                      // mtag->file,
                      mtag->function,
                      mtag->line);
        }

        size += mtag->size;
    }

    tt_mutex_release(&__mtag_lock);

    if (flag & TT_MEMORY_TAG_EACH) {
        tt_printf("[%dbytes] allocated\n", (tt_s32_t)size);
    }
#endif
}

void tt_set_oom_handler(IN tt_oom_handler_t handler, IN void *param)
{
    __s_oom_handler = handler;
    __s_oom_param = param;
}

#ifdef TT_MEMORY_TAG_ENABLE

void *tt_malloc_tag(IN size_t size,
                    IN const tt_char_t *file,
                    IN const tt_char_t *function,
                    IN const tt_u32_t line)
{
    void *p = tt_c_malloc(sizeof(__mtag_t) + size);
    if (p != NULL) {
        __mtag_t *mtag = (__mtag_t *)p;

        mtag->addr = TT_PTR_INC(void, p, sizeof(__mtag_t));
        mtag->size = size;
        mtag->file = file;
        mtag->function = function;
        tt_hnode_init(&mtag->hnode);
        mtag->line = line;

        if (__mtag_initialized) {
            tt_mutex_acquire(&__mtag_lock);
            tt_hmap_add(&__mtag_map,
                        (tt_u8_t *)&mtag->addr,
                        sizeof(void *),
                        &mtag->hnode);
            tt_mutex_release(&__mtag_lock);
        }

        p = mtag->addr;
    } else if (__s_oom_handler != NULL) {
        __s_oom_handler(__s_oom_param);
    }
    return p;
}

void tt_free_tag(IN void *p)
{
    if (p != NULL) {
        __mtag_t *mtag = TT_PTR_DEC(__mtag_t, p, sizeof(__mtag_t));
        TT_ASSERT(__MTAG_OK(mtag));

        if (__mtag_initialized) {
            tt_mutex_acquire(&__mtag_lock);
            tt_hmap_remove(&__mtag_map, &mtag->hnode);
            tt_mutex_release(&__mtag_lock);
        }

        tt_c_free(mtag);
    }
}

#else

void *tt_malloc_tag(IN size_t size)
{
    void *p = tt_c_malloc(sizeof(__mtag_t) + size);
    if ((p == NULL) && (__s_oom_handler != NULL)) {
        __s_oom_handler(__s_oom_param);
    }
    return p;
}

#endif

tt_result_t __mtag_component_init(IN tt_component_t *comp,
                                  IN tt_profile_t *profile)
{
#ifdef TT_MEMORY_TAG_ENABLE
    if (!TT_OK(tt_mutex_create(&__mtag_lock, NULL))) {
        TT_ERROR("fail to create malloc lock");
        return TT_FAIL;
    }

    if (!TT_OK(tt_hmap_create(&__mtag_map, 127, NULL))) {
        TT_ERROR("fail to create malloc map");
        tt_mutex_destroy(&__mtag_lock);
        return TT_FAIL;
    }

    __mtag_initialized = TT_TRUE;
#endif

    return TT_SUCCESS;
}

void __mtag_component_exit(IN tt_component_t *comp)
{
#ifdef TT_MEMORY_TAG_ENABLE
    tt_memory_tag_dump(TT_MEMORY_TAG_EACH | TT_MEMORY_TAG_TOTAL);
    __mtag_initialized = TT_FALSE;

    tt_hmap_destroy(&__mtag_map, __mtag_destroy);

    tt_mutex_destroy(&__mtag_lock);
#endif
}

void __mtag_destroy(IN tt_hnode_t *hnode)
{
    __mtag_t *mtag = TT_CONTAINER(hnode, __mtag_t, hnode);
    TT_ASSERT(__MTAG_OK(mtag));
    tt_c_free(mtag);
}
