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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <log/tt_log.h>

#include <log/tt_log_init.h>

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

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_log_debug(IN const tt_char_t *func,
                  IN tt_u32_t line,
                  IN const tt_char_t *format,
                  ...)
{
    va_list ap;

    va_start(ap, format);
    if (tt_g_logmgr_ok) {
        tt_logmgr_inputv(&tt_g_logmgr, TT_LOG_DEBUG, func, line, format, ap);
    } else {
        TT_VPRINTF(format, ap);
        TT_PRINTF("\n");
    }
    va_end(ap);
}

void tt_log_info(IN const tt_char_t *func,
                 IN tt_u32_t line,
                 IN const tt_char_t *format,
                 ...)
{
    va_list ap;

    va_start(ap, format);
    if (tt_g_logmgr_ok) {
        tt_logmgr_inputv(&tt_g_logmgr, TT_LOG_INFO, func, line, format, ap);
    } else {
        TT_VPRINTF(format, ap);
        TT_PRINTF("\n");
    }
    va_end(ap);
}

void tt_log_warn(IN const tt_char_t *func,
                 IN tt_u32_t line,
                 IN const tt_char_t *format,
                 ...)
{
    va_list ap;

    va_start(ap, format);
    if (tt_g_logmgr_ok) {
        tt_logmgr_inputv(&tt_g_logmgr, TT_LOG_WARN, func, line, format, ap);
    } else {
        TT_VPRINTF(format, ap);
        TT_PRINTF("\n");
    }
    va_end(ap);
}

void tt_log_error(IN const tt_char_t *func,
                  IN tt_u32_t line,
                  IN const tt_char_t *format,
                  ...)
{
    va_list ap;

    va_start(ap, format);
    if (tt_g_logmgr_ok) {
        tt_logmgr_inputv(&tt_g_logmgr, TT_LOG_ERROR, func, line, format, ap);
    } else {
        TT_VPRINTF(format, ap);
        TT_PRINTF("\n");
    }
    va_end(ap);
}

void tt_log_fatal(IN const tt_char_t *func,
                  IN tt_u32_t line,
                  IN const tt_char_t *format,
                  ...)
{
    va_list ap;

    va_start(ap, format);
    if (tt_g_logmgr_ok) {
        tt_logmgr_inputv(&tt_g_logmgr, TT_LOG_FATAL, func, line, format, ap);
    } else {
        TT_VPRINTF(format, ap);
        TT_PRINTF("\n");
    }
    va_end(ap);
}
