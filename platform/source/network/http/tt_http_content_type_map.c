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

#include <network/http/tt_http_content_type_map.h>

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

tt_http_contype_map_t tt_g_http_contype_map;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __cte_init(IN tt_http_contype_entry_t *e,
                       IN tt_http_contype_t type);

static void __cte_destroy(IN tt_http_contype_entry_t *e);

static void __cte_clear(IN tt_http_contype_entry_t *e);

static tt_result_t __cte_set(IN tt_http_contype_entry_t *e,
                             IN const tt_char_t *name,
                             IN tt_u32_t name_len,
                             IN const tt_char_t *ext,
                             IN tt_u32_t ext_len);

static tt_result_t __cte_add_dynamic(IN tt_http_contype_entry_t *e,
                                     IN tt_ptrhmap_t *name_map,
                                     IN tt_ptrhmap_t *ext_map);

static tt_result_t __cte_add_static(IN tt_http_contype_entry_t *e,
                                    IN tt_ptrhmap_t *name_map,
                                    IN tt_ptrhmap_t *ext_map);

static void __cte_remove(IN tt_http_contype_entry_t *e,
                         IN tt_ptrhmap_t *name_map,
                         IN tt_ptrhmap_t *ext_map);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_http_contype_map_component_init(IN struct tt_component_s *comp,
                                               IN struct tt_profile_s *profile)
{
    tt_http_contype_map_attr_t attr;

    tt_http_contype_map_attr_default(&attr);

    if (!TT_OK(tt_http_contype_map_create(&tt_g_http_contype_map, &attr))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_http_contype_map_set_static(&tt_g_http_contype_map,
                                              tt_g_http_contype_static))) {
        return TT_FAIL;
    }

#if 0
    // confusing...
    tt_http_contype_map_set_default(&tt_g_http_contype_map,
                                    TT_HTTP_CONTYPE_APP_OCTET);
#endif

    return TT_SUCCESS;
}

void tt_http_contype_map_component_exit(IN struct tt_component_s *comp)
{
    tt_http_contype_map_destroy(&tt_g_http_contype_map);
}

tt_result_t tt_http_contype_map_create(IN tt_http_contype_map_t *cm,
                                       IN OPT tt_http_contype_map_attr_t *attr)
{
    tt_http_contype_map_attr_t __attr;
    tt_http_contype_t t;

    tt_u32_t __done = 0;
#define __CMC_NAME (1 << 0)
#define __CMC_SUFFIX (1 << 1)

    TT_ASSERT(cm != NULL);

    if (attr == NULL) {
        tt_http_contype_map_attr_default(&__attr);
        attr = &__attr;
    }

    cm->static_entry = NULL;

    if (!TT_OK(tt_ptrhmap_create(&cm->name_map,
                                 attr->name_slot_num,
                                 &attr->name_map_attr))) {
        goto fail;
    }
    __done |= __CMC_NAME;

    if (!TT_OK(tt_ptrhmap_create(&cm->ext_map,
                                 attr->ext_slot_num,
                                 &attr->ext_map_attr))) {
        goto fail;
    }
    __done |= __CMC_SUFFIX;

    for (t = 0; t < TT_HTTP_CONTYPE_NUM; ++t) {
        __cte_init(&cm->dynamic_entry[t], t);
    }

    cm->default_type = TT_HTTP_CONTYPE_NUM;

    return TT_SUCCESS;

fail:

    if (__done & __CMC_NAME) {
        tt_ptrhmap_destroy(&cm->name_map);
    }

    if (__done & __CMC_SUFFIX) {
        tt_ptrhmap_destroy(&cm->ext_map);
    }

    return TT_FAIL;
}

void tt_http_contype_map_destroy(IN tt_http_contype_map_t *cm)
{
    tt_http_contype_t t;

    TT_ASSERT(cm != NULL);

    tt_ptrhmap_destroy(&cm->name_map);

    tt_ptrhmap_destroy(&cm->ext_map);

    // we directly destroy cte without removing it from name/ext maps
    for (t = 0; t < TT_HTTP_CONTYPE_NUM; ++t) {
        __cte_destroy(&cm->dynamic_entry[t]);
    }
}

void tt_http_contype_map_attr_default(IN tt_http_contype_map_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->name_slot_num = 16;
    attr->ext_slot_num = 16;
    tt_ptrhmap_attr_default(&attr->name_map_attr);
    tt_ptrhmap_attr_default(&attr->ext_map_attr);
}

void tt_http_contype_map_clear_dynamic(IN tt_http_contype_map_t *cm)
{
    tt_http_contype_t t;

    TT_ASSERT(cm != NULL);

    for (t = 0; t < TT_HTTP_CONTYPE_NUM; ++t) {
        __cte_remove(&cm->dynamic_entry[t], &cm->name_map, &cm->ext_map);
        __cte_clear(&cm->dynamic_entry[t]);
    }

    cm->static_entry = NULL;
}

void tt_http_contype_map_clear_static(IN tt_http_contype_map_t *cm)
{
    TT_ASSERT(cm != NULL);

    if (cm->static_entry != NULL) {
        tt_http_contype_t t;
        for (t = 0; t < TT_HTTP_CONTYPE_NUM; ++t) {
            __cte_remove(&cm->static_entry[t], &cm->name_map, &cm->ext_map);
        }
        cm->static_entry = NULL;
    }
}

void tt_http_contype_map_clear(IN tt_http_contype_map_t *cm)
{
    tt_http_contype_t t;

    TT_ASSERT(cm != NULL);

    cm->static_entry = NULL;

    tt_ptrhmap_clear(&cm->name_map);

    tt_ptrhmap_clear(&cm->ext_map);

    // we directly clear cte without removing it from name/ext maps
    for (t = 0; t < TT_HTTP_CONTYPE_NUM; ++t) {
        __cte_clear(&cm->dynamic_entry[t]);
    }

    cm->default_type = TT_HTTP_CONTYPE_NUM;
}

tt_result_t tt_http_contype_map_set_static(IN tt_http_contype_map_t *cm,
                                           IN tt_http_contype_entry_t *entry)
{
    TT_ASSERT(cm != NULL);

    tt_http_contype_map_clear_static(cm);

    if (entry != NULL) {
        tt_http_contype_t t;

        // must set static before the for loop
        cm->static_entry = entry;

        for (t = 0; t < TT_HTTP_CONTYPE_NUM; ++t) {
            if (!TT_OK(
                    __cte_add_static(&entry[t], &cm->name_map, &cm->ext_map))) {
                tt_http_contype_map_clear_static(cm);
                return TT_FAIL;
            }
        }
    }

    return TT_SUCCESS;
}

tt_result_t tt_http_contype_map_add_n(IN tt_http_contype_map_t *cm,
                                      IN tt_http_contype_t type,
                                      IN const tt_char_t *name,
                                      IN tt_u32_t name_len,
                                      IN const tt_char_t *ext,
                                      IN tt_u32_t ext_len)
{
    tt_http_contype_entry_t *e;

    TT_ASSERT(cm != NULL);
    TT_ASSERT(TT_HTTP_CONTYPE_VALID(type));
    // name can not be null or empty
    TT_ASSERT((name != NULL) && (name[0] != 0) && (name_len != 0));
    // ext can be null but not empty
    TT_ASSERT((ext == NULL) || ((ext[0] != 0) && (ext_len != 0)));

    e = &cm->dynamic_entry[type];

    __cte_remove(e, &cm->name_map, &cm->ext_map);
    if (!TT_OK(__cte_set(e, name, name_len, ext, ext_len))) {
        return TT_FAIL;
    }
    if (!TT_OK(__cte_add_dynamic(e, &cm->name_map, &cm->ext_map))) {
        __cte_clear(e);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_http_contype_map_remove(IN tt_http_contype_map_t *cm,
                                IN tt_http_contype_t type)
{
    TT_ASSERT(cm != NULL);
    TT_ASSERT(TT_HTTP_CONTYPE_VALID(type));

    __cte_remove(&cm->dynamic_entry[type], &cm->name_map, &cm->ext_map);
    __cte_clear(&cm->dynamic_entry[type]);
}

tt_http_contype_entry_t *tt_http_contype_map_find_type(
    IN tt_http_contype_map_t *cm, IN tt_http_contype_t type)
{
    tt_http_contype_entry_t *e;

    TT_ASSERT(cm != NULL);
    TT_ASSERT(TT_HTTP_CONTYPE_VALID(type));

    e = &cm->dynamic_entry[type];
    if (e->name != NULL) {
        return e;
    }

    if (cm->static_entry != NULL) {
        e = &cm->static_entry[type];
        if (e->name != NULL) {
            return e;
        }
    }

    // try default if set
    if (TT_HTTP_CONTYPE_VALID(cm->default_type) && (type != cm->default_type)) {
        return tt_http_contype_map_find_type(cm, cm->default_type);
    }

    return NULL;
}

tt_http_contype_entry_t *tt_http_contype_map_find_name_n(
    IN tt_http_contype_map_t *cm,
    IN const tt_char_t *name,
    IN tt_u32_t name_len)
{
    tt_http_contype_entry_t *e;

    TT_ASSERT(cm != NULL);

    if ((name == NULL) || (name[0] == 0) || (name_len == 0)) {
        return NULL;
    }

    e = tt_ptrhmap_find(&cm->name_map, (tt_u8_t *)name, name_len);
    if (e != NULL) {
        return e;
    }

    if (TT_HTTP_CONTYPE_VALID(cm->default_type)) {
        return tt_http_contype_map_find_type(cm, cm->default_type);
    }

    return NULL;
}

tt_http_contype_entry_t *tt_http_contype_map_find_ext_n(
    IN tt_http_contype_map_t *cm, IN const tt_char_t *ext, IN tt_u32_t ext_len)
{
    tt_http_contype_entry_t *e;

    TT_ASSERT(cm != NULL);

    if ((ext == NULL) || (ext[0] == 0) || (ext_len == 0)) {
        return NULL;
    }

    e = tt_ptrhmap_find(&cm->ext_map, (tt_u8_t *)ext, ext_len);
    if (e != NULL) {
        return e;
    }

    if (TT_HTTP_CONTYPE_VALID(cm->default_type)) {
        return tt_http_contype_map_find_type(cm, cm->default_type);
    }

    return NULL;
}

void __cte_init(IN tt_http_contype_entry_t *e, IN tt_http_contype_t type)
{
    e->type = type;
    e->name = NULL;
    e->ext = NULL;
    e->name_len = 0;
    e->ext_len = 0;
}

void __cte_destroy(IN tt_http_contype_entry_t *e)
{
    __cte_clear(e);
}

void __cte_clear(IN tt_http_contype_entry_t *e)
{
    if (e->name != NULL) {
        tt_free((tt_char_t *)e->name);
        e->name = NULL;
        e->name_len = 0;
    }

    if (e->ext != NULL) {
        tt_free((tt_char_t *)e->ext);
        e->ext = NULL;
        e->ext_len = 0;
    }
}

tt_result_t __cte_set(IN tt_http_contype_entry_t *e,
                      IN const tt_char_t *name,
                      IN tt_u32_t name_len,
                      IN const tt_char_t *ext,
                      IN tt_u32_t ext_len)
{
    tt_char_t *new_name, *new_ext;

    new_name = tt_malloc(name_len + 1);
    if (new_name == NULL) {
        TT_ERROR("no mem for cte name");
        return TT_E_NOMEM;
    }
    tt_memcpy(new_name, name, name_len);
    new_name[name_len] = 0;

    if (ext != NULL) {
        new_ext = tt_malloc(ext_len + 1);
        if (new_ext == NULL) {
            TT_ERROR("no mem for cte ext");
            tt_free(new_name);
            return TT_E_NOMEM;
        }
        tt_memcpy(new_ext, ext, ext_len);
        new_ext[ext_len] = 0;
    } else {
        new_ext = NULL;
    }

    __cte_destroy(e);
    e->name = new_name;
    e->name_len = name_len;
    e->ext = new_ext;
    e->ext_len = ext_len;

    return TT_SUCCESS;
}

tt_result_t __cte_add_dynamic(IN tt_http_contype_entry_t *e,
                              IN tt_ptrhmap_t *name_map,
                              IN tt_ptrhmap_t *ext_map)
{
    tt_u32_t len;

    TT_ASSERT((e->name != NULL) && (e->name[0] != 0));

    // adding dynamic entry should remove existing static entry first
    tt_ptrhmap_remove_key(name_map, (tt_u8_t *)e->name, e->name_len);
    if (!TT_OK(tt_ptrhmap_add(name_map, (tt_u8_t *)e->name, e->name_len, e))) {
        return TT_FAIL;
    }

    if (e->ext != NULL) {
        const tt_char_t *ext, *end, *pos;

        ext = e->ext;
        end = ext + e->ext_len;
        while ((pos = tt_strchr(ext, ';')) != NULL) {
            if (ext < pos) {
                len = (tt_u32_t)(pos - ext);
                tt_ptrhmap_remove_key(ext_map, (tt_u8_t *)ext, len);
                if (!TT_OK(tt_ptrhmap_add(ext_map, (tt_u8_t *)ext, len, e))) {
                    // must remove whole from both name & ext maps
                    __cte_remove(e, name_map, ext_map);
                    return TT_FAIL;
                }
            }

            ext = pos + 1;
        }
        if (ext < end) {
            len = (tt_u32_t)(end - ext);
            tt_ptrhmap_remove_key(ext_map, (tt_u8_t *)ext, len);
            if (!TT_OK(tt_ptrhmap_add(ext_map, (tt_u8_t *)ext, len, e))) {
                __cte_remove(e, name_map, ext_map);
                return TT_FAIL;
            }
        }
    }

    return TT_SUCCESS;
}

tt_result_t __cte_add_static(IN tt_http_contype_entry_t *e,
                             IN tt_ptrhmap_t *name_map,
                             IN tt_ptrhmap_t *ext_map)
{
    tt_u32_t len;

    TT_ASSERT((e->name != NULL) && (e->name[0] != 0));

    // adding static entry should check if there are existing entry first
    if ((tt_ptrhmap_find(name_map, (tt_u8_t *)e->name, e->name_len) == NULL) &&
        !TT_OK(tt_ptrhmap_add(name_map, (tt_u8_t *)e->name, e->name_len, e))) {
        return TT_FAIL;
    }

    if (e->ext != NULL) {
        const tt_char_t *ext, *end, *pos;

        ext = e->ext;
        end = ext + e->ext_len;
        while ((pos = tt_strchr(ext, ';')) != NULL) {
            if (ext < pos) {
                len = (tt_u32_t)(pos - ext);
                if ((tt_ptrhmap_find(ext_map, (tt_u8_t *)ext, len) == NULL) &&
                    !TT_OK(tt_ptrhmap_add(ext_map, (tt_u8_t *)ext, len, e))) {
                    // must remove whole from both name & ext maps
                    __cte_remove(e, name_map, ext_map);
                    return TT_FAIL;
                }
            }

            ext = pos + 1;
        }
        if (ext < end) {
            len = (tt_u32_t)(end - ext);
            if ((tt_ptrhmap_find(ext_map, (tt_u8_t *)ext, len) == NULL) &&
                !TT_OK(tt_ptrhmap_add(ext_map, (tt_u8_t *)ext, len, e))) {
                __cte_remove(e, name_map, ext_map);
                return TT_FAIL;
            }
        }
    }

    return TT_SUCCESS;
}

void __cte_remove(IN tt_http_contype_entry_t *e,
                  IN tt_ptrhmap_t *name_map,
                  IN tt_ptrhmap_t *ext_map)
{
    // must use remove_pair

    if (e->name != NULL) {
        tt_ptrhmap_remove_pair(name_map, (tt_u8_t *)e->name, e->name_len, e);
    }

    if (e->ext != NULL) {
        const tt_char_t *ext, *end, *pos;

        ext = e->ext;
        end = ext + e->ext_len;
        while ((pos = tt_strchr(ext, ';')) != NULL) {
            if (ext < pos) {
                tt_ptrhmap_remove_pair(ext_map,
                                       (tt_u8_t *)ext,
                                       (tt_u32_t)(pos - ext),
                                       e);
            }

            ext = pos + 1;
        }
        if (ext < end) {
            tt_ptrhmap_remove_pair(ext_map,
                                   (tt_u8_t *)ext,
                                   (tt_u32_t)(end - ext),
                                   e);
        }
    }
}
