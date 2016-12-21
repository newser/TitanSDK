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
@file tt_log.h
@brief basic log APIs

this file declare APIs for outputing log
*/

#ifndef __TT_LOG__
#define __TT_LOG__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#ifdef TT_LOG_ENABLE

/**
@def TT_DETAIL
print common detail log
*/
#define TT_DETAIL(...)                                                         \
    do {                                                                       \
        tt_logfmt_input_flf(tt_g_logmgr.lfmt[TT_LOG_LEVEL_DETAIL],             \
                            __FUNCTION__,                                      \
                            __LINE__,                                          \
                            __VA_ARGS__);                                      \
        tt_logfmt_output(tt_g_logmgr.lfmt[TT_LOG_LEVEL_DETAIL]);               \
    } while (0)

/**
@def TT_INFO
print common informational log
*/
#define TT_INFO(...)                                                           \
    do {                                                                       \
        tt_logfmt_input_flf(tt_g_logmgr.lfmt[TT_LOG_LEVEL_INFO],               \
                            __FUNCTION__,                                      \
                            __LINE__,                                          \
                            __VA_ARGS__);                                      \
        tt_logfmt_output(tt_g_logmgr.lfmt[TT_LOG_LEVEL_INFO]);                 \
    } while (0)

/**
@def TT_WARN
print warnings
*/
#define TT_WARN(...)                                                           \
    do {                                                                       \
        tt_logfmt_input_flf(tt_g_logmgr.lfmt[TT_LOG_LEVEL_WARN],               \
                            __FUNCTION__,                                      \
                            __LINE__,                                          \
                            __VA_ARGS__);                                      \
        tt_logfmt_output(tt_g_logmgr.lfmt[TT_LOG_LEVEL_WARN]);                 \
    } while (0)

/**
@def TT_ERROR
print common error log
*/
#define TT_ERROR(...)                                                          \
    do {                                                                       \
        tt_logfmt_input_flf(tt_g_logmgr.lfmt[TT_LOG_LEVEL_ERROR],              \
                            __FUNCTION__,                                      \
                            __LINE__,                                          \
                            __VA_ARGS__);                                      \
        tt_logfmt_output(tt_g_logmgr.lfmt[TT_LOG_LEVEL_ERROR]);                \
    } while (0)

/**
@def TT_FATAL
print common fatal error log
*/
#define TT_FATAL(...)                                                          \
    do {                                                                       \
        tt_logfmt_input_flf(tt_g_logmgr.lfmt[TT_LOG_LEVEL_FATAL],              \
                            __FUNCTION__,                                      \
                            __LINE__,                                          \
                            __VA_ARGS__);                                      \
        tt_logfmt_output(tt_g_logmgr.lfmt[TT_LOG_LEVEL_FATAL]);                \
    } while (0)

#else

#define TT_DETAIL(...)
#define TT_INFO(...)
#define TT_WARN(...)
#define TT_ERROR(...)
#define TT_FATAL(...)

#endif

/**
@def TT_PRINTF
print raw log

@note
this macro can be used when log system is not initialized
*/
#define TT_PRINTF printf

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

// extern tt_logmgr_t tt_g_logmgr;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_log_component_register()
register ts log component
*/
extern void tt_log_component_register();

/**
@fn void tt_sync_log_component_register()
synchronous log component
*/
extern void tt_sync_log_component_register();

extern void tt_log_config_component_register();

#endif /* __TT_LOG__ */
