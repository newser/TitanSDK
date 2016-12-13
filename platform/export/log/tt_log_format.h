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
@file tt_log_format.h
@brief log format

this file defines log format
*/

#ifndef __TT_LOG_FORMAT__
#define __TT_LOG_FORMAT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_spinlock_s;
struct tt_logio_s;

typedef enum {
    TT_LOG_LEVEL_DETAIL,
    TT_LOG_LEVEL_INFO,
    TT_LOG_LEVEL_WARN,
    TT_LOG_LEVEL_ERROR,
    TT_LOG_LEVEL_FATAL,

    TT_LOG_LEVEL_NUM
} tt_log_level_t;
#define TT_LOG_LEVEL_VALID(l) ((l) < TT_LOG_LEVEL_NUM)

typedef struct tt_logfmt_s
{
    tt_list_t lio_list;

    // buffer
    struct tt_spinlock_s *lock;
    tt_char_t *buf;
    tt_u32_t buf_size;
    tt_u32_t buf_pos;

    // pattern
    tt_char_t *pattern;
    tt_u32_t pattern_len;
    tt_u32_t pattern_content_len; // including ending 0
    tt_list_t lfld_list;

    // sequence number
    tt_u32_t seq_no;

    // logger name
    const tt_char_t *logger;

    // log level
    tt_log_level_t level;

    // status
    tt_bool_t enabled : 1;
    tt_bool_t can_expand : 1;
} tt_logfmt_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// - when buf_size is set to 0, the fmt would use some initial
//   initial size and increase the buf size when buf is full
//   until it reaches some predefined limitation
extern tt_result_t tt_logfmt_create(IN tt_logfmt_t *lfmt,
                                    IN tt_log_level_t level,
                                    IN const tt_char_t *pattern,
                                    IN OPT tt_u32_t buf_size,
                                    IN OPT const tt_char_t *logger);

extern void tt_logfmt_destroy(IN tt_logfmt_t *lfmt);

extern tt_result_t tt_logfmt_add_io(IN tt_logfmt_t *lfmt,
                                    IN struct tt_logio_s *lio);

extern void tt_logfmt_input_flf(IN tt_logfmt_t *lfmt,
                                IN const tt_char_t *func,
                                IN tt_u32_t line,
                                IN const tt_char_t *format,
                                ...);

extern void tt_logfmt_output(IN tt_logfmt_t *lfmt);

tt_inline void tt_logfmt_enable(IN tt_logfmt_t *lfmt)
{
    lfmt->enabled = TT_TRUE;
}

tt_inline void tt_logfmt_disable(IN tt_logfmt_t *lfmt)
{
    lfmt->enabled = TT_FALSE;
}

extern tt_result_t tt_logfmt_enable_lock(IN tt_logfmt_t *lfmt);

#endif /* __TT_LOG_FORMAT__ */
