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
@file tt_log_io.h
@brief log io

this file defines log io
*/

#ifndef __TT_LOG_IO__
#define __TT_LOG_IO__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>
#include <log/tt_log_def.h>
#include <misc/tt_reference_counter.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_LOGIO_CAST(lio, type) TT_PTR_INC(type, lio, sizeof(tt_logio_t))

#define tt_logio_ref(lio) TT_REF_ADD(tt_logio_t, lio, ref)

#define tt_logio_release(lio)                                                  \
    TT_REF_RELEASE(tt_logio_t, lio, ref, __logio_destroy)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_logio_s;

typedef tt_result_t (*tt_logio_create_t)(IN struct tt_logio_s *lio);

typedef void (*tt_logio_destroy_t)(IN struct tt_logio_s *lio);

// - data must be null terminated string, len equals tt_strlen(data)
typedef void (*tt_logio_output_t)(IN struct tt_logio_s *lio,
                                  IN const tt_char_t *data,
                                  IN tt_u32_t len);

typedef struct
{
    tt_logio_type_t type;

    tt_logio_create_t create;
    tt_logio_destroy_t destroy;
    tt_logio_output_t output;
} tt_logio_itf_t;

typedef struct tt_logio_s
{
    tt_logio_itf_t *itf;
    tt_atomic_s32_t ref;
} tt_logio_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// size does not count sizeof(tt_logio_t)
tt_export tt_logio_t *tt_logio_create(IN tt_u32_t size, IN tt_logio_itf_t *itf);

tt_export void __logio_destroy(IN tt_logio_t *lio);

tt_export void tt_logio_output(IN tt_logio_t *lio,
                               IN const tt_char_t *data,
                               IN tt_u32_t len);

#endif /* __TT_LOG_IO__ */
