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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <log/tt_syslog_def.h>

#ifdef TT_HAVE_SYSLOG
#include <syslog.h>
#endif

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

#ifdef TT_HAVE_SYSLOG

tt_s32_t tt_syslog_facility_map[TT_SYSLOG_FACILITY_NUM] = {
    LOG_KERN,     LOG_USER,   LOG_MAIL,   LOG_DAEMON, LOG_AUTH,
    LOG_SYSLOG,   LOG_LPR,    LOG_NEWS,   LOG_UUCP,   LOG_CRON,
    LOG_AUTHPRIV, LOG_FTP,    LOG_LOCAL0, LOG_LOCAL1, LOG_LOCAL2,
    LOG_LOCAL3,   LOG_LOCAL4, LOG_LOCAL5, LOG_LOCAL6, LOG_LOCAL7,
};

tt_s32_t tt_syslog_level_map[TT_SYSLOG_LEVEL_NUM] = {
    LOG_EMERG,
    LOG_ALERT,
    LOG_CRIT,
    LOG_ERR,
    LOG_WARNING,
    LOG_NOTICE,
    LOG_INFO,
    LOG_DEBUG,
};

#else

tt_s32_t tt_syslog_facility_map[TT_SYSLOG_FACILITY_NUM];

tt_s32_t tt_syslog_level_map[TT_SYSLOG_LEVEL_NUM];

#endif

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////
