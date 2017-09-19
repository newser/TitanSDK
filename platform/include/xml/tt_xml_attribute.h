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
@file tt_xml_attribute.h
@brief xml attribute

this file specifies xml attribute APIs
*/

#ifndef __TT_XML_ATTRIBUTE__
#define __TT_XML_ATTRIBUTE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef tt_ptr_t tt_xattr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_xattr_component_register();

tt_export const tt_char_t *tt_xattr_get_name(IN tt_xattr_t xa);

tt_export tt_result_t tt_xattr_set_name(IN tt_xattr_t xa,
                                        IN const tt_char_t *name);

tt_export tt_xattr_t tt_xattr_next(IN tt_xattr_t xa);

tt_export tt_xattr_t tt_xattr_prev(IN tt_xattr_t xa);

// ========================================
// xml attribute value
// ========================================

tt_export const tt_char_t *tt_xattr_get_value(IN tt_xattr_t xa,
                                              IN const tt_char_t *def);

tt_export tt_result_t tt_xattr_set_value(IN tt_xattr_t xa,
                                         IN const tt_char_t *value);

tt_export tt_bool_t tt_xattr_get_bool(IN tt_xattr_t xa, IN tt_bool_t def);

tt_export tt_result_t tt_xattr_set_bool(IN tt_xattr_t xa, IN tt_bool_t value);

tt_export tt_s32_t tt_xattr_get_s32(IN tt_xattr_t xa, IN tt_s32_t def);

tt_export tt_result_t tt_xattr_set_s32(IN tt_xattr_t xa, IN tt_s32_t value);

tt_export tt_u32_t tt_xattr_get_u32(IN tt_xattr_t xa, IN tt_u32_t def);

tt_export tt_result_t tt_xattr_set_u32(IN tt_xattr_t xa, IN tt_u32_t value);

tt_export tt_s64_t tt_xattr_get_s64(IN tt_xattr_t xa, IN tt_s64_t def);

tt_export tt_result_t tt_xattr_set_s64(IN tt_xattr_t xa, IN tt_s64_t value);

tt_export tt_u64_t tt_xattr_get_u64(IN tt_xattr_t xa, IN tt_u64_t def);

tt_export tt_result_t tt_xattr_set_u64(IN tt_xattr_t xa, IN tt_u64_t value);

tt_export tt_float_t tt_xattr_get_float(IN tt_xattr_t xa, IN tt_float_t def);

tt_export tt_result_t tt_xattr_set_float(IN tt_xattr_t xa, IN tt_float_t value);

tt_export tt_double_t tt_xattr_get_double(IN tt_xattr_t xa, IN tt_double_t def);

tt_export tt_result_t tt_xattr_set_double(IN tt_xattr_t xa,
                                          IN tt_double_t value);

#endif /* __TT_XML_ATTRIBUTE__ */
