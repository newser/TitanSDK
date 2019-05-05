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
@file tt_charset_convert.h
@brief charset conversion

define charset conversion apis
*/

#ifndef __TT_CHARSET_CONVERT__
#define __TT_CHARSET_CONVERT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>
#include <misc/tt_charset_def.h>

#include <tt_charset_convert_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_CHARSET_MAX_MBCHAR_LEN 8

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    void *reserved;
} tt_chsetconv_attr_t;

typedef struct tt_chsetconv_s
{
    tt_chsetconv_ntv_t sys_csconv;

    tt_charset_t from;
    tt_charset_t to;
    tt_chsetconv_attr_t attr;

    tt_u8_t head[TT_CHARSET_MAX_MBCHAR_LEN];
    tt_u8_t tail[TT_CHARSET_MAX_MBCHAR_LEN];
    tt_u32_t head_len;
    tt_u32_t tail_len;
    tt_bool_t head_complete : 1;

    tt_buf_t converted;
} tt_chsetconv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_chsetconv_create(IN tt_chsetconv_t *csconv,
                                          IN tt_charset_t from,
                                          IN tt_charset_t to,
                                          IN OPT tt_chsetconv_attr_t *attr);

tt_export void tt_chsetconv_destroy(IN tt_chsetconv_t *csconv);

tt_export void tt_chsetconv_attr_default(IN tt_chsetconv_attr_t *attr);

tt_export tt_result_t tt_chsetconv_input(IN tt_chsetconv_t *csconv,
                                         IN void *input, IN tt_u32_t input_len);

// - set output to NULL to get output_len
// - output stored in csconv is discarded when it returned
tt_export void tt_chsetconv_output(IN tt_chsetconv_t *csconv,
                                   OUT OPT void *output,
                                   IN OUT tt_u32_t *output_len);

// - caller can directly access converted data stored in csconv
// - if there is no converted data, output is set to NULL and
//   output_len is set to 0
tt_export void tt_chsetconv_output_ptr(IN tt_chsetconv_t *csconv,
                                       OUT OPT tt_u8_t **output,
                                       OUT tt_u32_t *output_len);

tt_export void tt_chsetconv_reset(IN tt_chsetconv_t *csconv);

#endif /* __TT_CHARSET_CONVERT__ */
