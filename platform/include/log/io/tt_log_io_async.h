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
@file tt_log_io_async.h
@brief log io async

this file defines log io async output
*/

#ifndef __TT_LOG_IO_ASYNC__
#define __TT_LOG_IO_ASYNC__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/ptr/tt_ptr_queue.h>
#include <algorithm/tt_buffer.h>
#include <os/tt_task.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_logio_s;

typedef struct
{
    tt_ptrq_attr_t io_q_attr;
    tt_buf_attr_t ibuf_attr;
    tt_buf_attr_t obuf_attr;
    tt_spinlock_attr_t lock_attr;
    tt_task_attr_t task_attr;
} tt_logio_async_attr_t;

typedef struct
{
    tt_fiber_t *io_f;
    tt_ptrq_t io_q;
    tt_buf_t ibuf;
    tt_buf_t obuf;
    tt_spinlock_t lock;
    tt_task_t task;
} tt_logio_async_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export struct tt_logio_s *tt_logio_async_create(
    IN OPT tt_logio_async_attr_t *attr);

tt_export void tt_logio_async_attr_default(IN tt_logio_async_attr_t *attr);

// can only be called just after tt_logio_async_create()
tt_export tt_result_t tt_logio_async_append(IN struct tt_logio_s *lio_async,
                                            IN TO struct tt_logio_s *lio);

#endif /* __TT_LOG_IO_ASYNC__ */
