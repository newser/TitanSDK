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
@file tt_log_layout.h
@brief log layout

this file defines log layout
*/

#ifndef __TT_LOG_LAYOUT__
#define __TT_LOG_LAYOUT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_LOGLYT_CAST(lyt, type) TT_PTR_INC(type, lyt, sizeof(tt_loglyt_t))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_loglyt_s;
struct tt_buf_s;

typedef tt_result_t (*tt_loglyt_create_t)(IN struct tt_loglyt_s *lyt);

typedef void (*tt_loglyt_destroy_t)(IN struct tt_loglyt_s *lyt);

typedef tt_result_t (*tt_loglyt_format_t)(IN struct tt_loglyt_s *lyt,
                                          OUT struct tt_buf_s *outbuf,
                                          IN const tt_char_t *func,
                                          IN tt_u32_t line,
                                          IN const tt_char_t *format,
                                          IN va_list ap);

typedef struct
{
    tt_loglyt_create_t create;
    tt_loglyt_destroy_t destroy;
    tt_loglyt_format_t format;
} tt_loglyt_itf_t;

typedef struct tt_loglyt_s
{
    const tt_char_t *logger;
    tt_loglyt_itf_t *itf;
} tt_loglyt_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_loglyt_t *tt_loglyt_create(IN tt_u32_t size,
                                     IN OPT const tt_char_t *logger,
                                     IN tt_loglyt_itf_t *itf);

extern void tt_loglyt_destroy(IN tt_loglyt_t *lyt);

tt_inline tt_result_t tt_loglyt_format(IN tt_loglyt_t *lyt,
                                       OUT struct tt_buf_s *outbuf,
                                       IN const tt_char_t *func,
                                       IN tt_u32_t line,
                                       IN const tt_char_t *format,
                                       IN va_list ap)
{
    return lyt->itf->format(lyt, outbuf, func, line, format, ap);
}

#endif /* __TT_LOG_LAYOUT__ */
