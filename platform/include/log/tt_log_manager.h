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
@file tt_log_manager.h
@brief log manager

this file declare log manager
*/

#ifndef __TT_LOG_MANAGER__
#define __TT_LOG_MANAGER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>
#include <log/tt_log_context.h>
#include <os/tt_spinlock.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_loglyt_s;
struct tt_logio_s;

typedef struct
{
    tt_spinlock_attr_t lock_attr;
    tt_buf_attr_t buf_attr;
    tt_logctx_attr_t ctx_attr[TT_LOG_LEVEL_NUM];
} tt_logmgr_attr_t;

typedef struct tt_logmgr_s
{
    const tt_char_t *logger;
    tt_log_level_t level;
    tt_spinlock_t lock;
    tt_u32_t seq_num;
    tt_buf_t buf;
    tt_logctx_t ctx[TT_LOG_LEVEL_NUM];
} tt_logmgr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_logmgr_create(IN tt_logmgr_t *lmgr,
                                    IN const tt_char_t *logger,
                                    IN OPT tt_logmgr_attr_t *attr);

extern void tt_logmgr_destroy(IN tt_logmgr_t *lmgr);

extern void tt_logmgr_attr_default(IN tt_logmgr_attr_t *attr);

// set level to TT_LOG_LEVEL_NUM to disable all
tt_inline tt_result_t tt_logmgr_set_level(IN tt_logmgr_t *lmgr,
                                          IN tt_log_level_t level)
{
    if (TT_LOG_LEVEL_VALID(level) || (level == TT_LOG_LEVEL_NUM)) {
        lmgr->level = level;
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

// set level to TT_LOG_LEVEL_NUM to set all
extern tt_result_t tt_logmgr_set_layout(IN tt_logmgr_t *lmgr,
                                        IN tt_log_level_t level,
                                        IN struct tt_loglyt_s *lyt);

// set level to TT_LOG_LEVEL_NUM to set all
extern tt_result_t tt_logmgr_append_filter(IN tt_logmgr_t *lmgr,
                                           IN tt_log_level_t level,
                                           IN tt_log_filter_t filter);

// set level to TT_LOG_LEVEL_NUM to set all
extern tt_result_t tt_logmgr_append_io(IN tt_logmgr_t *lmgr,
                                       IN tt_log_level_t level,
                                       IN struct tt_logio_s *lio);

extern tt_result_t tt_logmgr_inputv(IN tt_logmgr_t *lmgr,
                                    IN tt_log_level_t level,
                                    IN const tt_char_t *func,
                                    IN tt_u32_t line,
                                    IN const tt_char_t *format,
                                    IN va_list ap);

tt_inline tt_result_t tt_logmgr_inputf(IN tt_logmgr_t *lmgr,
                                       IN tt_log_level_t level,
                                       IN const tt_char_t *func,
                                       IN tt_u32_t line,
                                       IN const tt_char_t *format,
                                       ...)
{
    va_list ap;
    tt_result_t result;

    va_start(ap, format);
    result = tt_logmgr_inputv(lmgr, level, func, line, format, ap);
    va_end(ap);

    return result;
}

#endif /* __TT_LOG_MANAGER__ */
