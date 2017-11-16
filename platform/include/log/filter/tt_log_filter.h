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
@file tt_log_layout.h
@brief log filter

this file defines log filter
*/

#ifndef __TT_LOG_FILTER__
#define __TT_LOG_FILTER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/ptr/tt_ptr_queue.h>
#include <log/tt_log_def.h>
#include <misc/tt_reference_counter.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_LOGFLTR_CAST(lf, type) TT_PTR_INC(type, lf, sizeof(tt_logfltr_t))

#define tt_logfltr_ref(lf) TT_REF_ADD(tt_logfltr_t, lf, ref)

#define tt_logfltr_release(lf)                                                 \
    TT_REF_RELEASE(tt_logfltr_t, lf, ref, __logfltr_destroy)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_logfltr_s;
struct tt_buf_s;
struct tt_logio_s;

typedef void (*tt_logfltr_destroy_t)(IN struct tt_logfltr_s *lf);

typedef tt_u32_t (*tt_logfltr_input_t)(IN struct tt_logfltr_s *lf,
                                       IN tt_log_entry_t *entry,
                                       IN OUT struct tt_buf_s *buf);
#define TT_LOGFLTR_PASS (1 << 0)
#define TT_LOGFLTR_SELF (1 << 1)

typedef struct
{
    tt_logfltr_destroy_t destroy;
    tt_logfltr_input_t input;
} tt_logfltr_itf_t;

typedef struct tt_logfltr_s
{
    tt_logfltr_itf_t *itf;
    tt_ptrq_t io_q;
    tt_atomic_s32_t ref;
} tt_logfltr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_logfltr_t *tt_logfltr_create(IN tt_u32_t size,
                                          IN tt_logfltr_itf_t *itf);

tt_export void __logfltr_destroy(IN tt_logfltr_t *lf);

tt_export tt_result_t tt_logfltr_append_io(IN tt_logfltr_t *lf,
                                           IN TO struct tt_logio_s *lio);

tt_export tt_u32_t tt_logfltr_input(IN struct tt_logfltr_s *lf,
                                    IN tt_log_entry_t *entry,
                                    IN OUT struct tt_buf_s *buf);

#endif /* __TT_LOG_FILTER__ */
