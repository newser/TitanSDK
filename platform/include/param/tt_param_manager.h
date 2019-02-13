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
@file tt_param_manager.h
@brief parameter manager

this file defines parameter manager APIs
*/

#ifndef __TT_PARAM_MANAGER__
#define __TT_PARAM_MANAGER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_param_s;
struct tt_fiber_s;
struct tt_string_s;

typedef struct
{
    tt_u32_t reserved;
} tt_param_mgr_attr_t;

typedef struct tt_param_mgr_s
{
    struct tt_param_s *root;
    struct tt_fiber_s *fb;
} tt_param_mgr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export tt_param_mgr_t tt_g_param_mgr;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_param_mgr_component_register();

tt_export tt_result_t tt_param_mgr_create(IN tt_param_mgr_t *pm,
                                          IN struct tt_param_s *root,
                                          IN OPT tt_param_mgr_attr_t *attr);

tt_export void tt_param_mgr_destroy(IN tt_param_mgr_t *pm);

tt_export void tt_param_mgr_attr_default(IN tt_param_mgr_attr_t *attr);

tt_export struct tt_param_s *tt_param_mgr_find_n(IN tt_param_mgr_t *pm,
                                                 IN const tt_char_t *path,
                                                 IN tt_u32_t len);

tt_inline struct tt_param_s *tt_param_mgr_find(IN tt_param_mgr_t *pm,
                                               IN const tt_char_t *path)
{
    return tt_param_mgr_find_n(pm,
                               (tt_char_t *)path,
                               (tt_u32_t)tt_strlen(path));
}

tt_export tt_result_t tt_param_mgr_fiber_routine(IN void *param);

// ========================================
// param mgr get/set
// ========================================

tt_export tt_result_t tt_param_mgr_get_bool(IN tt_param_mgr_t *pm,
                                            IN const tt_char_t *path,
                                            IN tt_u32_t len,
                                            OUT tt_bool_t *val);

tt_export tt_result_t tt_param_mgr_set_bool(IN tt_param_mgr_t *pm,
                                            IN const tt_char_t *path,
                                            IN tt_u32_t len,
                                            IN tt_bool_t val);

tt_export tt_result_t tt_param_mgr_get_u32(IN tt_param_mgr_t *pm,
                                           IN const tt_char_t *path,
                                           IN tt_u32_t len,
                                           OUT tt_u32_t *val);

tt_export tt_result_t tt_param_mgr_set_u32(IN tt_param_mgr_t *pm,
                                           IN const tt_char_t *path,
                                           IN tt_u32_t len,
                                           IN tt_u32_t val);

tt_export tt_result_t tt_param_mgr_get_s32(IN tt_param_mgr_t *pm,
                                           IN const tt_char_t *path,
                                           IN tt_u32_t len,
                                           OUT tt_s32_t *val);

tt_export tt_result_t tt_param_mgr_set_s32(IN tt_param_mgr_t *pm,
                                           IN const tt_char_t *path,
                                           IN tt_u32_t len,
                                           IN tt_s32_t val);

tt_export tt_result_t tt_param_mgr_get_str(IN tt_param_mgr_t *pm,
                                           IN const tt_char_t *path,
                                           IN tt_u32_t len,
                                           OUT struct tt_string_s *val);

tt_export tt_result_t tt_param_mgr_set_str(IN tt_param_mgr_t *pm,
                                           IN const tt_char_t *path,
                                           IN tt_u32_t len,
                                           IN const tt_char_t *val,
                                           IN tt_u32_t val_len);

// ========================================
// param mgr get/set by any fiber
// ========================================

tt_export tt_result_t tt_get_param_bool(IN const tt_char_t *path,
                                        IN tt_u32_t len,
                                        OUT tt_bool_t *val);

tt_export tt_result_t tt_set_param_bool(IN const tt_char_t *path,
                                        IN tt_u32_t len,
                                        IN tt_bool_t val);

tt_export tt_result_t tt_get_param_u32(IN const tt_char_t *path,
                                       IN tt_u32_t len,
                                       OUT tt_u32_t *val);

tt_export tt_result_t tt_set_param_u32(IN const tt_char_t *path,
                                       IN tt_u32_t len,
                                       IN tt_u32_t val);

tt_export tt_result_t tt_get_param_s32(IN const tt_char_t *path,
                                       IN tt_u32_t len,
                                       OUT tt_s32_t *val);

tt_export tt_result_t tt_set_param_s32(IN const tt_char_t *path,
                                       IN tt_u32_t len,
                                       IN tt_s32_t val);

tt_export tt_result_t tt_get_param_str(IN const tt_char_t *path,
                                       IN tt_u32_t len,
                                       OUT struct tt_string_s *val);

tt_export tt_result_t tt_set_param_str(IN const tt_char_t *path,
                                       IN tt_u32_t len,
                                       IN const tt_char_t *val,
                                       IN tt_u32_t val_len);

#endif /* __TT_PARAM_MANAGER__ */
