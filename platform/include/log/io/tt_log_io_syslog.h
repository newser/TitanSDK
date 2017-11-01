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
@file tt_log_io_standard.h
@brief log io syslog

this file defines log io syslog output
*/

#ifndef __TT_LOG_IO_SYSLOG__
#define __TT_LOG_IO_SYSLOG__

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

typedef enum {
    TT_SYSLOG_USER,
    TT_SYSLOG_LOCAL0,
    TT_SYSLOG_LOCAL1,
    TT_SYSLOG_LOCAL2,
    TT_SYSLOG_LOCAL3,
    TT_SYSLOG_LOCAL4,
    TT_SYSLOG_LOCAL5,
    TT_SYSLOG_LOCAL6,
    TT_SYSLOG_LOCAL7,

    TT_SYSLOG_FACILITY_NUM
} tt_syslog_facility_t;
#define TT_SYSLOG_FACILITY_VALID(f) ((f) < TT_SYSLOG_FACILITY_NUM)

typedef enum {
    TT_SYSLOG_EMERG,
    TT_SYSLOG_ALERT,
    TT_SYSLOG_CRIT,
    TT_SYSLOG_ERR,
    TT_SYSLOG_WARNING,
    TT_SYSLOG_NOTICE,
    TT_SYSLOG_INFO,
    TT_SYSLOG_DEBUG,

    TT_SYSLOG_LEVEL_NUM
} tt_syslog_level_t;
#define TT_SYSLOG_LEVEL_VALID(l) ((f) < TT_SYSLOG_FACILITY_NUM)

typedef struct
{
    tt_syslog_facility_t facility;
    tt_syslog_level_t level;
} tt_logio_syslog_attr_t;

typedef struct
{
    tt_syslog_facility_t facility;
    tt_syslog_level_t level;
} tt_logio_syslog_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export struct tt_logio_s *tt_logio_syslog_create(
    IN OPT tt_logio_syslog_attr_t *attr);

tt_export void tt_logio_syslog_attr_default(IN tt_logio_syslog_attr_t *attr);

#endif /* __TT_LOG_IO_SYSLOG__ */
