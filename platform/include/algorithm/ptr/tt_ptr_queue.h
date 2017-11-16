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
@file tt_ptr_queue.h
@brief pqueue
 */

#ifndef __TT_PTR_QUEUE__
#define __TT_PTR_QUEUE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_double_linked_list.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_u32_t ptr_per_frame;
} tt_ptrq_attr_t;

typedef struct tt_ptrq_s
{
    tt_dlist_t frame;
    void *cached_frame;
    tt_u32_t count;
    tt_u32_t ptr_per_frame;
} tt_ptrq_t;

typedef struct
{
    tt_ptrq_t *pq;
    void *frame;
    tt_u32_t idx;
} tt_ptrq_iter_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_ptrq_init(IN tt_ptrq_t *pq, IN OPT tt_ptrq_attr_t *attr);

tt_export void tt_ptrq_destroy(IN tt_ptrq_t *pq);

tt_export void tt_ptrq_attr_default(IN tt_ptrq_attr_t *attr);

tt_inline tt_u32_t tt_ptrq_count(IN tt_ptrq_t *pq)
{
    return pq->count;
}

tt_inline tt_bool_t tt_ptrq_empty(IN tt_ptrq_t *pq)
{
    return pq->count == 0 ? TT_TRUE : TT_FALSE;
}

tt_export void tt_ptrq_clear(IN tt_ptrq_t *pq);

tt_export tt_result_t tt_ptrq_push_head(IN tt_ptrq_t *pq, IN tt_ptr_t p);

tt_export tt_result_t tt_ptrq_push_tail(IN tt_ptrq_t *pq, IN tt_ptr_t p);

tt_export tt_ptr_t tt_ptrq_pop_head(IN tt_ptrq_t *pq);

tt_export tt_ptr_t tt_ptrq_pop_tail(IN tt_ptrq_t *pq);

tt_export tt_ptr_t tt_ptrq_head(IN tt_ptrq_t *pq);

tt_export tt_ptr_t tt_ptrq_tail(IN tt_ptrq_t *pq);

tt_export void tt_ptrq_iter(IN tt_ptrq_t *pq, OUT tt_ptrq_iter_t *iter);

tt_export tt_ptr_t tt_ptrq_iter_next(IN OUT tt_ptrq_iter_t *iter);

tt_export tt_ptr_t tt_ptrq_get(IN tt_ptrq_t *pq, IN tt_u32_t idx);

tt_export tt_ptr_t tt_ptrq_set(IN tt_ptrq_t *pq,
                               IN tt_u32_t idx,
                               IN tt_ptr_t p);

#endif /* __TT_PTR_QUEUE__ */
