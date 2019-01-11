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
@file tt_json_value.h
@brief json value

this file specifies json value api
*/

#ifndef __TT_JSON_VALUE__
#define __TT_JSON_VALUE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <json/tt_json_value_def.h>
#include <misc/tt_util.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_component_s;
struct tt_profile_s;
struct tt_jdoc_s;

typedef struct
{
    tt_ptr_t v;
    tt_ptr_t i;
} tt_jarray_iter_t;

typedef struct
{
    tt_ptr_t v;
    tt_ptr_t i;
} tt_jobj_iter_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_json_value_component_init(
    IN struct tt_component_s *comp, IN struct tt_profile_s *profile);

tt_export void tt_json_value_component_exit(IN struct tt_component_s *comp);

tt_export void tt_jval_init(IN tt_jval_t *jv);

tt_export void tt_jval_destroy(IN tt_jval_t *jv);

tt_export tt_bool_t tt_jval_cmp(IN tt_jval_t *a, IN tt_jval_t *b);

tt_export tt_jval_type_t tt_jval_get_type(IN tt_jval_t *jv);

tt_export void tt_jval_swap(IN tt_jval_t *a, IN tt_jval_t *b);

// ========================================
// null
// ========================================

tt_export tt_bool_t tt_jval_is_null(IN tt_jval_t *jv);

tt_export void tt_jval_set_null(IN tt_jval_t *jv);

// ========================================
// bool
// ========================================

tt_export void tt_jval_init_bool(IN tt_jval_t *jv, IN tt_bool_t val);

tt_export tt_bool_t tt_jval_is_bool(IN tt_jval_t *jv);

tt_export tt_bool_t tt_jval_get_bool(IN tt_jval_t *jv);

tt_export void tt_jval_set_bool(IN tt_jval_t *jv, IN tt_bool_t val);

// ========================================
// number
// ========================================

tt_export tt_bool_t tt_jval_is_number(IN tt_jval_t *jv);

tt_export void tt_jval_init_u32(IN tt_jval_t *jv, IN tt_u32_t val);

tt_export tt_bool_t tt_jval_is_u32(IN tt_jval_t *jv);

tt_export tt_u32_t tt_jval_get_u32(IN tt_jval_t *jv);

tt_export void tt_jval_set_u32(IN tt_jval_t *jv, IN tt_u32_t val);

tt_export void tt_jval_init_s32(IN tt_jval_t *jv, IN tt_s32_t val);

tt_export tt_bool_t tt_jval_is_s32(IN tt_jval_t *jv);

tt_export tt_s32_t tt_jval_get_s32(IN tt_jval_t *jv);

tt_export void tt_jval_set_s32(IN tt_jval_t *jv, IN tt_s32_t val);

tt_export void tt_jval_init_u64(IN tt_jval_t *jv, IN tt_u64_t val);

tt_export tt_bool_t tt_jval_is_u64(IN tt_jval_t *jv);

tt_export tt_u64_t tt_jval_get_u64(IN tt_jval_t *jv);

tt_export void tt_jval_set_u64(IN tt_jval_t *jv, IN tt_u64_t val);

tt_export void tt_jval_init_s64(IN tt_jval_t *jv, IN tt_s64_t val);

tt_export tt_bool_t tt_jval_is_s64(IN tt_jval_t *jv);

tt_export tt_s64_t tt_jval_get_s64(IN tt_jval_t *jv);

tt_export void tt_jval_set_s64(IN tt_jval_t *jv, IN tt_s64_t val);

tt_export void tt_jval_init_double(IN tt_jval_t *jv, IN tt_double_t val);

tt_export tt_bool_t tt_jval_is_double(IN tt_jval_t *jv);

tt_export tt_double_t tt_jval_get_double(IN tt_jval_t *jv);

tt_export void tt_jval_set_double(IN tt_jval_t *jv, IN tt_double_t val);

// ========================================
// string
// ========================================

tt_export void tt_jval_init_strn(IN tt_jval_t *jv,
                                 IN const tt_char_t *val,
                                 IN tt_u32_t len,
                                 IN struct tt_jdoc_s *jd);

tt_inline void tt_jval_init_str(IN tt_jval_t *jv,
                                IN const tt_char_t *val,
                                IN struct tt_jdoc_s *jd)
{
    tt_jval_init_strn(jv, val, tt_strlen(val), jd);
}

tt_export void tt_jval_create_strn(IN tt_jval_t *jv,
                                   IN const tt_char_t *val,
                                   IN tt_u32_t len,
                                   IN struct tt_jdoc_s *jd);

tt_inline void tt_jval_create_str(IN tt_jval_t *jv,
                                  IN const tt_char_t *val,
                                  IN struct tt_jdoc_s *jd)
{
    tt_jval_create_strn(jv, val, tt_strlen(val), jd);
}

tt_export tt_bool_t tt_jval_is_str(IN tt_jval_t *jv);

tt_export const tt_char_t *tt_jval_get_str(IN tt_jval_t *jv);

tt_export void tt_jval_set_strn(IN tt_jval_t *jv,
                                IN const tt_char_t *val,
                                IN tt_u32_t len);

tt_inline void tt_jval_set_str(IN tt_jval_t *jv, IN const tt_char_t *val)
{
    tt_jval_set_strn(jv, val, tt_strlen(val));
}

tt_export void tt_jval_copy_strn(IN tt_jval_t *jv,
                                 IN const tt_char_t *val,
                                 IN tt_u32_t len);

tt_inline void tt_jval_copy_str(IN tt_jval_t *jv, IN const tt_char_t *val)
{
    tt_jval_copy_strn(jv, val, tt_strlen(val));
}

// ========================================
// array
// ========================================

tt_export void tt_jval_init_array(IN tt_jval_t *jv, IN struct tt_jdoc_s *jd);

tt_export tt_bool_t tt_jval_is_array(IN tt_jval_t *jv);

tt_export void tt_jval_set_array(IN tt_jval_t *jv);

tt_export void tt_jarray_clear(IN tt_jval_t *jv);

tt_export tt_bool_t tt_jarray_empty(IN tt_jval_t *jv);

tt_export tt_u32_t tt_jarray_count(IN tt_jval_t *jv);

tt_export tt_u32_t tt_jarray_capacity(IN tt_jval_t *jv);

tt_export void tt_jarray_reserve(IN tt_jval_t *jv, IN tt_u32_t num);

tt_export void tt_jarray_push(IN tt_jval_t *jv, IN TO tt_jval_t *val);

tt_export void tt_jarray_pop(IN tt_jval_t *jv);

tt_export tt_jval_t *tt_jarray_get(IN tt_jval_t *jv, IN tt_u32_t idx);

tt_export void tt_jarray_iter(IN tt_jval_t *jv, OUT tt_jarray_iter_t *iter);

tt_export tt_jval_t *tt_jarray_iter_next(IN OUT tt_jarray_iter_t *iter);

/*
tt_export void tt_jarray_remove(IN tt_jval_t *jv,
                                    IN OUT tt_jarray_iter_t *iter);
*/

tt_export void tt_jarray_push_null(IN tt_jval_t *jv);

tt_export void tt_jarray_push_bool(IN tt_jval_t *jv, IN tt_bool_t val);

tt_export void tt_jarray_push_u32(IN tt_jval_t *jv, IN tt_u32_t val);

tt_export void tt_jarray_push_s32(IN tt_jval_t *jv, IN tt_s32_t val);

tt_export void tt_jarray_push_u64(IN tt_jval_t *jv, IN tt_u64_t val);

tt_export void tt_jarray_push_s64(IN tt_jval_t *jv, IN tt_s64_t val);

tt_export void tt_jarray_push_double(IN tt_jval_t *jv, IN tt_double_t val);

tt_export void tt_jarray_push_strn(IN tt_jval_t *jv,
                                   IN const tt_char_t *val,
                                   IN tt_u32_t len);

tt_inline void tt_jarray_push_str(IN tt_jval_t *jv, IN const tt_char_t *val)
{
    tt_jarray_push_strn(jv, val, tt_strlen(val));
}

tt_export void tt_jarray_copy_strn(IN tt_jval_t *jv,
                                   IN const tt_char_t *val,
                                   IN tt_u32_t len);

tt_inline void tt_jarray_copy_str(IN tt_jval_t *jv, IN const tt_char_t *val)
{
    tt_jarray_copy_strn(jv, val, tt_strlen(val));
}

// ========================================
// object
// ========================================

tt_export void tt_jval_init_obj(IN tt_jval_t *jv, IN struct tt_jdoc_s *jd);

tt_export tt_bool_t tt_jval_is_obj(IN tt_jval_t *jv);

tt_export void tt_jval_set_obj(IN tt_jval_t *jv);

tt_export void tt_jobj_clear(IN tt_jval_t *jv);

tt_export tt_bool_t tt_jobj_empty(IN tt_jval_t *jv);

tt_export tt_u32_t tt_jobj_member_count(IN tt_jval_t *jv);

tt_export tt_u32_t tt_jobj_capacity(IN tt_jval_t *jv);

tt_export void tt_jobj_reserve(IN tt_jval_t *jv, IN tt_u32_t num);

tt_export tt_jval_t *tt_jobj_find(IN tt_jval_t *jv, IN const tt_char_t *name);

tt_export tt_bool_t tt_jobj_contain(IN tt_jval_t *jv, IN const tt_char_t *name);

tt_export void tt_jobj_add(IN tt_jval_t *jv,
                           IN const tt_char_t *name,
                           IN TO tt_jval_t *val);

tt_export void tt_jobj_add_nv(IN tt_jval_t *jv,
                              IN TO tt_jval_t *name,
                              IN TO tt_jval_t *val);

tt_export void tt_jobj_add_null(IN tt_jval_t *jv, IN const tt_char_t *name);

tt_export void tt_jobj_add_bool(IN tt_jval_t *jv,
                                IN const tt_char_t *name,
                                IN tt_bool_t val);

tt_export void tt_jobj_add_u32(IN tt_jval_t *jv,
                               IN const tt_char_t *name,
                               IN tt_u32_t val);

tt_export void tt_jobj_add_s32(IN tt_jval_t *jv,
                               IN const tt_char_t *name,
                               IN tt_s32_t val);

tt_export void tt_jobj_add_u64(IN tt_jval_t *jv,
                               IN const tt_char_t *name,
                               IN tt_u64_t val);

tt_export void tt_jobj_add_s64(IN tt_jval_t *jv,
                               IN const tt_char_t *name,
                               IN tt_s64_t val);

tt_export void tt_jobj_add_double(IN tt_jval_t *jv,
                                  IN const tt_char_t *name,
                                  IN tt_double_t val);

tt_export void tt_jobj_add_strn(IN tt_jval_t *jv,
                                IN const tt_char_t *name,
                                IN const tt_char_t *val,
                                IN tt_u32_t len);

tt_inline void tt_jobj_add_str(IN tt_jval_t *jv,
                               IN const tt_char_t *name,
                               IN const tt_char_t *val)
{
    tt_jobj_add_strn(jv, name, val, tt_strlen(val));
}

tt_export void tt_jobj_copy_strn(IN tt_jval_t *jv,
                                 IN const tt_char_t *name,
                                 IN const tt_char_t *val,
                                 IN tt_u32_t len);

tt_inline void tt_jobj_copy_str(IN tt_jval_t *jv,
                                IN const tt_char_t *name,
                                IN const tt_char_t *val)
{
    tt_jobj_copy_strn(jv, name, val, tt_strlen(val));
}

tt_export void tt_jobj_remove(IN tt_jval_t *jv, IN const tt_char_t *name);

tt_export void tt_jobj_iter(IN tt_jval_t *jv, OUT tt_jobj_iter_t *iter);

tt_export tt_jval_t *tt_jobj_iter_next(IN OUT tt_jobj_iter_t *iter);

#endif /* __TT_JSON_VALUE__ */
