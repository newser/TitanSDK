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
@file tt_log_io_oslog.h
@brief log io os log

this file defines log io os log
*/

#ifndef __TT_LOG_IO_OSLOG__
#define __TT_LOG_IO_OSLOG__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#ifdef TT_HAVE_OSLOG
#include <os/log.h>
#endif

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_logio_s;

typedef enum {
    TT_OSLOG_DEBUG,
    TT_OSLOG_DEFAULT,
    TT_OSLOG_ERROR,
    TT_OSLOG_FAULT,
    TT_OSLOG_INFO,

    TT_OSLOG_TYPE_NUM
} tt_oslog_type_t;
#define TT_OSLOG_TYPE_VALID ((t) < TT_OSLOG_TYPE_NUM)

typedef struct
{
#ifdef TT_HAVE_OSLOG
    os_log_t l;
#endif
    tt_oslog_type_t type;
} tt_logio_oslog_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export struct tt_logio_s *tt_logio_oslog_create(
    IN const tt_char_t *subsystem,
    IN const tt_char_t *category,
    IN tt_oslog_type_t type);

#endif /* __TT_LOG_IO_OSLOG__ */
