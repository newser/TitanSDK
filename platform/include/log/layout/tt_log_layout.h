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
@file tt_log_layout.h
@brief log layout

this file defines log layout
*/

#ifndef __TT_LOG_LAYOUT__
#define __TT_LOG_LAYOUT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <log/tt_log_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_LOGLYT_CAST(ll, type) TT_PTR_INC(type, ll, sizeof(tt_loglyt_t))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_loglyt_s;
struct tt_buf_s;

typedef tt_result_t (*tt_loglyt_create_t)(IN struct tt_loglyt_s *ll);

typedef void (*tt_loglyt_destroy_t)(IN struct tt_loglyt_s *ll);

typedef tt_result_t (*tt_loglyt_format_t)(IN struct tt_loglyt_s *ll,
                                          IN tt_log_entry_t *entry,
                                          OUT struct tt_buf_s *outbuf);

typedef struct
{
    tt_loglyt_create_t create;
    tt_loglyt_destroy_t destroy;
    tt_loglyt_format_t format;
} tt_loglyt_itf_t;

typedef struct tt_loglyt_s
{
    tt_loglyt_itf_t *itf;
} tt_loglyt_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_loglyt_t *tt_loglyt_create(IN tt_u32_t size,
                                        IN tt_loglyt_itf_t *itf);

tt_export void tt_loglyt_destroy(IN tt_loglyt_t *ll);

tt_export tt_result_t tt_loglyt_format(IN tt_loglyt_t *ll,
                                       IN tt_log_entry_t *entry,
                                       OUT struct tt_buf_s *outbuf);

#endif /* __TT_LOG_LAYOUT__ */
