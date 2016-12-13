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
@file tt_log_field.h
@brief a log field

this file defines log field
*/

#ifndef __TT_LOGFLD__
#define __TT_LOGFLD__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_LOGFLD_SEQ_NO_KEY "seq_no"
#define TT_LOGFLD_TIME_KEY "time"
#define TT_LOGFLD_LOGGER_KEY "logger"
#define TT_LOGFLD_LEVEL_KEY "level"
#define TT_LOGFLD_CONTENT_KEY "content"
#define TT_LOGFLD_FUNC_KEY "function"
#define TT_LOGFLD_LINE_KEY "line"

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_logfmt_s;

typedef enum {
    TT_LOGFLD_SEQ_NO,
    TT_LOGFLD_TIME,
    TT_LOGFLD_LOGGER,
    TT_LOGFLD_LEVEL,
    TT_LOGFLD_CONTENT,
    TT_LOGFLD_FUNC,
    TT_LOGFLD_LINE,

    TT_LOGFLD_TYPE_NUM,
} tt_logfld_type_t;
#define TT_LOGFLD_TYPE_VALID(t) ((t) < TT_LOGFLD_TYPE_NUM)

typedef struct tt_logfld_s
{
    tt_lnode_t node;
    tt_logfld_type_t type;
    const tt_char_t *format;
} tt_logfld_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_logfld_t *tt_logfld_create(IN struct tt_logfmt_s *lfmt,
                                     IN const tt_char_t *start,
                                     IN const tt_char_t *end);

extern void tt_logfld_destroy(IN tt_logfld_t *lfld);

extern tt_result_t tt_logfld_validate(IN struct tt_logfmt_s *lfmt,
                                      IN const tt_char_t *start,
                                      IN const tt_char_t *end);

extern tt_u32_t tt_logfld_output(IN tt_logfld_t *lfld,
                                 IN OUT tt_char_t *pos,
                                 IN OUT tt_u32_t left_len,
                                 IN struct tt_logfmt_s *lfmt);

#endif /* __TT_LOGFLD__ */
