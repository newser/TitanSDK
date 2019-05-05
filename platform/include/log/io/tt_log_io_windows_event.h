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
@file tt_log_io_windows_event.h
@brief log io windows event

this file defines log io windows event output
*/

#ifndef __TT_LOG_IO_WINDOWS_EVENT__
#define __TT_LOG_IO_WINDOWS_EVENT__

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

struct tt_logio_s;

typedef enum
{
    TT_WINEV_SUCCESS,
    TT_WINEV_ERROR,
    TT_WINEV_WARNING,
    TT_WINEV_INFORMATION,
    TT_WINEV_AUDIT_SUCCESS,
    TT_WINEV_AUDIT_FAILURE,

    TT_WINEV_TYPE_NUM
} tt_winev_type_t;
#define TT_WINEV_TYPE_VALID ((t) < TT_WINEV_TYPE_NUM)

typedef struct
{
    const tt_char_t *server;
} tt_logio_winev_attr_t;

typedef struct
{
#ifdef TT_HAVE_WINDOWS_EVENT_LOG
    HANDLE source;
#endif
    tt_winev_type_t type;
    tt_u32_t category;
    tt_u32_t ev_id;
} tt_logio_winev_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export struct tt_logio_s *tt_logio_winev_create(
    IN const tt_char_t *source, IN tt_winev_type_t type, IN tt_u32_t category,
    IN tt_u32_t ev_id, IN OPT tt_logio_winev_attr_t *attr);

tt_export void tt_logio_winev_attr_default(IN tt_logio_winev_attr_t *attr);

#endif /* __TT_LOG_IO_WINDOWS_EVENT__ */
