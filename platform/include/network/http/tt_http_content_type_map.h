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

/**
@file tt_http_content_type_map.h
@brief http content type map

this file defines http content type
*/

#ifndef __TT_HTTP_CONTENT_TYPE_MAP__
#define __TT_HTTP_CONTENT_TYPE_MAP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/ptr/tt_ptr_hashmap.h>
#include <network/http/def/tt_http_content_type_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_component_s;
struct tt_profile_s;

typedef struct tt_http_contype_map_s
{
    tt_http_contype_entry_t *static_entry;
    tt_ptrhmap_t name_map;
    tt_ptrhmap_t ext_map;
    tt_http_contype_entry_t dynamic_entry[TT_HTTP_CONTYPE_NUM];
    tt_http_contype_t default_type;
} tt_http_contype_map_t;

typedef struct
{
    tt_u32_t name_slot_num;
    tt_u32_t ext_slot_num;
    tt_ptrhmap_attr_t name_map_attr;
    tt_ptrhmap_attr_t ext_map_attr;
} tt_http_contype_map_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export tt_http_contype_map_t tt_g_http_contype_map;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_http_contype_map_component_init(
    IN struct tt_component_s *comp, IN struct tt_profile_s *profile);

tt_export void tt_http_contype_map_component_exit(
    IN struct tt_component_s *comp);

tt_export tt_result_t tt_http_contype_map_create(
    IN tt_http_contype_map_t *cm, IN OPT tt_http_contype_map_attr_t *attr);

tt_export void tt_http_contype_map_destroy(IN tt_http_contype_map_t *cm);

tt_export void tt_http_contype_map_attr_default(
    IN tt_http_contype_map_attr_t *attr);

tt_export void tt_http_contype_map_clear_dynamic(IN tt_http_contype_map_t *cm);

tt_export void tt_http_contype_map_clear_static(IN tt_http_contype_map_t *cm);

tt_export void tt_http_contype_map_clear(IN tt_http_contype_map_t *cm);

tt_export tt_result_t tt_http_contype_map_set_static(
    IN tt_http_contype_map_t *cm, IN tt_http_contype_entry_t *entry);

tt_export tt_result_t tt_http_contype_map_add_n(IN tt_http_contype_map_t *cm,
                                                IN tt_http_contype_t type,
                                                IN const tt_char_t *name,
                                                IN tt_u32_t name_len,
                                                IN OPT const tt_char_t *ext,
                                                IN tt_u32_t ext_len);

tt_inline tt_result_t tt_http_contype_map_add(IN tt_http_contype_map_t *cm,
                                              IN tt_http_contype_t type,
                                              IN const tt_char_t *name,
                                              IN OPT const tt_char_t *ext)
{
    return tt_http_contype_map_add_n(cm, type, name, (tt_u32_t)tt_strlen(name),
                                     ext,
                                     TT_COND(ext != NULL,
                                             (tt_u32_t)tt_strlen(ext), 0));
}

tt_export void tt_http_contype_map_remove(IN tt_http_contype_map_t *cm,
                                          IN tt_http_contype_t type);

tt_inline void tt_http_contype_map_set_default(IN tt_http_contype_map_t *cm,
                                               IN tt_http_contype_t type)
{
    TT_ASSERT(TT_HTTP_CONTYPE_VALID(type));
    cm->default_type = type;
}

tt_export tt_http_contype_entry_t *tt_http_contype_map_find_type(
    IN tt_http_contype_map_t *cm, IN tt_http_contype_t type);

tt_export tt_http_contype_entry_t *tt_http_contype_map_find_name_n(
    IN tt_http_contype_map_t *cm, IN const tt_char_t *name,
    IN tt_u32_t name_len);

tt_inline tt_http_contype_entry_t *tt_http_contype_map_find_name(
    IN tt_http_contype_map_t *cm, IN const tt_char_t *name)
{
    return tt_http_contype_map_find_name_n(cm, name, (tt_u32_t)tt_strlen(name));
}

tt_export tt_http_contype_entry_t *tt_http_contype_map_find_ext_n(
    IN tt_http_contype_map_t *cm, IN const tt_char_t *ext, IN tt_u32_t ext_len);

tt_inline tt_http_contype_entry_t *tt_http_contype_map_find_ext(
    IN tt_http_contype_map_t *cm, IN const tt_char_t *ext)
{
    return tt_http_contype_map_find_ext_n(cm, ext, (tt_u32_t)tt_strlen(ext));
}

#endif /* __TT_HTTP_CONTENT_TYPE_MAP__ */
