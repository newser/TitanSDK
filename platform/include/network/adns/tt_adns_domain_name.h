/* Licensed to the Apache Software Foundation (ASF) under one or more
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
@file tt_adns_domain_name.h
@brief adns domain name

this file defines adns domain name APIs
*/

#ifndef __TT_ADNS_DOMAIN_NAME__
#define __TT_ADNS_DOMAIN_NAME__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_MAX_DOMAIN_NAME_LEN 256

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_buf_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// name_len must count the ending0
tt_export tt_bool_t tt_adns_name_verify(IN const tt_char_t *name,
                                     IN tt_u32_t name_len);

// ========================================
// render & parse
// ========================================

/*
 domain name compression:
 - first encode [name[0], name[name_len - 1]], if name_len != 0
 - then put compress_ptr if compress_ptr != 0
 */

// name_len should count ending 0
tt_export tt_result_t tt_adns_name_render_prepare(IN const tt_char_t *name,
                                               IN tt_u32_t name_len,
                                               IN tt_u16_t compress_ptr,
                                               OUT tt_u32_t *len);

tt_export tt_result_t tt_adns_name_render(IN struct tt_buf_s *buf,
                                       IN const tt_char_t *name,
                                       IN tt_u32_t name_len,
                                       IN tt_u16_t compress_ptr);

tt_export tt_result_t tt_adns_name_parse_prepare(IN struct tt_buf_s *buf);

// - if name is NULL, it return new allocated buffer, and name_len returns
//   length of the new buffer
// - if name is not NULL, it just fill the buffer with parsed value. if
//   name_len is less than required size, it returns NULL
// - returned name_len does not count ending 0
tt_export tt_char_t *tt_adns_name_parse(IN struct tt_buf_s *buf,
                                     OUT tt_char_t *name,
                                     IN OUT tt_u32_t *name_len,
                                     IN tt_u8_t *data,
                                     IN tt_u32_t data_len);

// - returning 0 means parsing failed
// - returned value does not include ending 0
tt_export tt_u32_t tt_adns_name_parse_len(IN struct tt_buf_s *buf,
                                       IN tt_u8_t *pkt,
                                       IN tt_u32_t pkt_len);

#endif /* __TT_ADNS_DOMAIN_NAME__ */
