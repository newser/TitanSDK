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
@file tt_log_def.h
@brief log definition

this file declare log definition
*/

#ifndef __TT_LOG_DEF__
#define __TT_LOG_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_LOGFLD_SEQ_NUM_KEY "seq_num"
#define TT_LOGFLD_TIME_KEY "time"
#define TT_LOGFLD_LOGGER_KEY "logger"
#define TT_LOGFLD_LEVEL_KEY "level"
#define TT_LOGFLD_CONTENT_KEY "content"
#define TT_LOGFLD_FUNC_KEY "function"
#define TT_LOGFLD_LINE_KEY "line"

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_LOG_DEBUG,
    TT_LOG_INFO,
    TT_LOG_WARN,
    TT_LOG_ERROR,
    TT_LOG_FATAL,

    TT_LOG_LEVEL_NUM
} tt_log_level_t;
#define TT_LOG_LEVEL_VALID(l) ((l) < TT_LOG_LEVEL_NUM)

typedef enum {
    TT_LOGIO_STANDARD,
    TT_LOGIO_LOGCAT, // for android
    TT_LOGIO_FILE,
    TT_LOGIO_SYSLOG,

    TT_LOGIO_NUM
} tt_logio_type_t;
#define TT_LOGIO_TYPE_VALID(t) ((t) < TT_LOGIO_NUM)

typedef enum {
    TT_LOGFLD_LOGGER,
    TT_LOGFLD_FUNC,
    TT_LOGFLD_CONTENT,
    TT_LOGFLD_SEQ_NUM,
    TT_LOGFLD_LINE,
    TT_LOGFLD_LEVEL,
    TT_LOGFLD_TIME,

    TT_LOGFLD_TYPE_NUM,
} tt_logfld_type_t;
#define TT_LOGFLD_TYPE_VALID(t) ((t) < TT_LOGFLD_TYPE_NUM)

typedef struct
{
    const tt_char_t *logger;
    const tt_char_t *function;
    const tt_char_t *content;
    tt_u32_t seq_num;
    tt_u32_t line;
    tt_log_level_t level;
} tt_log_entry_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export const tt_char_t *tt_g_log_level_name[TT_LOG_LEVEL_NUM];

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_LOG_DEF__ */
