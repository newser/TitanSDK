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
@file tt_log_context.h
@brief log context

this file declare log context
*/

#ifndef __TT_LOG_CONTEXT__
#define __TT_LOG_CONTEXT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>
#include <algorithm/tt_pointer_list.h>
#include <log/tt_log_def.h>

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
    tt_buf_attr_t buf_attr;
} tt_logctx_attr_t;

typedef struct
{
    tt_log_level_t level;
    struct tt_loglyt_s *lyt;

    tt_buf_t buf;
    tt_ptrlist_t filter_list;
    tt_ptrlist_t io_list;
} tt_logctx_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_logctx_create(IN tt_logctx_t *lctx,
                                    IN tt_log_level_t level,
                                    IN OPT struct tt_loglyt_s *lyt,
                                    IN OPT tt_logctx_attr_t *attr);

extern void tt_logctx_destroy(IN tt_logctx_t *lctx);

extern void tt_logctx_attr_default(IN tt_logctx_attr_t *attr);

tt_inline void tt_logctx_set_layout(IN tt_logctx_t *lctx,
                                    IN struct tt_loglyt_s *lyt)
{
    lctx->lyt = lyt;
}

extern tt_result_t tt_logctx_append_filter(IN tt_logctx_t *lctx,
                                           IN tt_log_filter_t filter);

extern tt_result_t tt_logctx_append_io(IN tt_logctx_t *lctx,
                                       IN struct tt_logio_s *lio);

extern tt_result_t tt_logctx_input(IN tt_logctx_t *lctx,
                                   IN tt_log_entry_t *entry);

#endif /* __TT_LOG_CONTEXT__ */
