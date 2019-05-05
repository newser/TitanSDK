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
@file tt_syslog_def.h
@brief syslog definitions

this file includes syslog definitions
*/

#ifndef __TT_SYSLOG_DEF__
#define __TT_SYSLOG_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_SYSLOG_PRIORITY(facility, level)                                    \
    tt_syslog_facility_map[(facility)] | tt_syslog_level_map[(level)]

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum
{
    TT_SYSLOG_KERN,
    TT_SYSLOG_USER,
    TT_SYSLOG_MAIL,
    TT_SYSLOG_DAEMON,
    TT_SYSLOG_AUTH,
    TT_SYSLOG_SYSLOG,
    TT_SYSLOG_LPR,
    TT_SYSLOG_NEWS,
    TT_SYSLOG_UUCP,
    TT_SYSLOG_CRON,
    TT_SYSLOG_AUTHPRIV,
    TT_SYSLOG_FTP,
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

typedef enum
{
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

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_s32_t tt_syslog_facility_map[TT_SYSLOG_FACILITY_NUM];

tt_export tt_s32_t tt_syslog_level_map[TT_SYSLOG_LEVEL_NUM];

#endif /* __TT_SYSLOG_DEF__ */
