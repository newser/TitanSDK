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
@def TT_DEBUG
print common detail log
*/
#define TT_DEBUG(...) tt_log_debug(__FUNCTION__, __LINE__, __VA_ARGS__)

/**
@def TT_INFO
print common informational log
*/
#define TT_INFO(...) tt_log_info(__FUNCTION__, __LINE__, __VA_ARGS__)

/**
@def TT_WARN
print warnings
*/
#define TT_WARN(...) tt_log_warn(__FUNCTION__, __LINE__, __VA_ARGS__)

/**
@def TT_ERROR
print common error log
*/
#define TT_ERROR(...) tt_log_error(__FUNCTION__, __LINE__, __VA_ARGS__)

/**
@def TT_FATAL
print common fatal error log
*/
#define TT_FATAL(...) tt_log_fatal(__FUNCTION__, __LINE__, __VA_ARGS__)

#else

#define TT_DEBUG(...)
#define TT_INFO(...)
#define TT_WARN(...)
#define TT_ERROR(...)
#define TT_FATAL(...)

#endif

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_log_debug(IN const tt_char_t *func,
                            IN tt_u32_t line,
                            IN const tt_char_t *format,
                            ...);

tt_export void tt_log_info(IN const tt_char_t *func,
                           IN tt_u32_t line,
                           IN const tt_char_t *format,
                           ...);

tt_export void tt_log_warn(IN const tt_char_t *func,
                           IN tt_u32_t line,
                           IN const tt_char_t *format,
                           ...);

tt_export void tt_log_error(IN const tt_char_t *func,
                            IN tt_u32_t line,
                            IN const tt_char_t *format,
                            ...);

tt_export void tt_log_fatal(IN const tt_char_t *func,
                            IN tt_u32_t line,
                            IN const tt_char_t *format,
                            ...);

#endif /* __TT_LOG__ */
