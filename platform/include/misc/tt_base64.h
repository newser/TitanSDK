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
@file tt_base64.h
@brief base64 encode/decode

this file defines base64 encode/decode api
*/

#ifndef __TT_BASE64__
#define __TT_BASE64__

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

typedef struct
{
    const tt_char_t *start_boundary;
#define TT_BASE64_START_LF "-----\n"
#define TT_BASE64_START_CRLF "-----\r\n"

    const tt_char_t *end_boundary;
#define TT_BASE64_END_LF "\n-----"
#define TT_BASE64_END_CRLF "\r\n-----"

} tt_base64_decode_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// - if decoded is NULL, decoded_len returns length required
//   to store decoded data
// - efficient way to use this api is get a buffer for decoded
//   data with length not less than data_len, then decoded_len
//   will return length decoded data which is always less than
//   data_len
tt_export tt_result_t tt_base64_decode(IN tt_u8_t *data,
                                       IN tt_u32_t data_len,
                                       IN OPT tt_base64_decode_attr_t *attr,
                                       OUT tt_u8_t *decoded,
                                       IN OUT tt_u32_t *decoded_len);

tt_export tt_result_t
tt_base64_decode_alloc(IN tt_u8_t *data,
                       IN tt_u32_t data_len,
                       IN OPT tt_base64_decode_attr_t *attr,
                       OUT tt_u8_t **decoded,
                       IN OUT tt_u32_t *decoded_len);

tt_export void tt_base64_decode_attr_default(IN tt_base64_decode_attr_t *attr);

#endif /* __TT_BASE64__ */
