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
@file tt_log_pattern_field.h
@brief field of log pattern

this file defines log pattern field
*/

#ifndef __TT_LOG_PATTERN_FIELD__
#define __TT_LOG_PATTERN_FIELD__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_LPFLD_SEQ_NUM_KEY "seq_num"
#define TT_LPFLD_TIME_KEY "time"
#define TT_LPFLD_LOGGER_KEY "logger"
#define TT_LPFLD_LEVEL_KEY "level"
#define TT_LPFLD_CONTENT_KEY "content"
#define TT_LPFLD_FUNC_KEY "function"
#define TT_LPFLD_LINE_KEY "line"

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_buf_s;

typedef enum {
    TT_LPFLD_SEQ_NUM,
    TT_LPFLD_TIME,
    TT_LPFLD_LOGGER,
    TT_LPFLD_LEVEL,
    TT_LPFLD_CONTENT,
    TT_LPFLD_FUNC,
    TT_LPFLD_LINE,

    TT_LPFLD_TYPE_NUM,
} tt_lpfld_type_t;
#define TT_LPFLD_TYPE_VALID(t) ((t) < TT_LPFLD_TYPE_NUM)

typedef struct tt_lpfld_s
{
    tt_lnode_t node;
    tt_lpfld_type_t type;
    tt_char_t *format;
} tt_lpfld_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// a log field must be reentrant
extern tt_lpfld_t *tt_lpfld_create(IN const tt_char_t *start,
                                   IN const tt_char_t *end);

extern void tt_lpfld_destroy(IN tt_lpfld_t *lpf);

extern tt_result_t tt_lpfld_check(IN const tt_char_t *start,
                                  IN const tt_char_t *end);

extern tt_result_t tt_lpfld_output(IN tt_lpfld_t *lpf,
                                   OUT struct tt_buf_s *outbuf);

extern tt_result_t tt_lpfld_output_cstr(IN tt_lpfld_t *lpf,
                                        IN const tt_char_t *cstr_val,
                                        OUT struct tt_buf_s *outbuf);

extern tt_result_t tt_lpfld_output_s32(IN tt_lpfld_t *lpf,
                                       IN tt_s32_t s32_val,
                                       OUT struct tt_buf_s *outbuf);

#endif /* __TT_LOG_PATTERN_FIELD__ */
