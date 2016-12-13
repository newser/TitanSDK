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
@file tt_thread_event.h
@brief inter thread event

this file defines inter thread event APIs
*/

#ifndef __TT_THREAD_EVENT__
#define __TT_THREAD_EVENT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>
#include <event/tt_event_base.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_TEV_OF(data) TT_PTR_DEC(tt_thread_ev_t, data, sizeof(tt_thread_ev_t))
#define TT_TEV_DATA(tev, type) TT_PTR_INC(type, tev, sizeof(tt_thread_ev_t))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_evpoller_s;

typedef struct tt_thread_ev_s
{
    tt_lnode_t node;
} tt_thread_ev_t;

// callback when received inter thread event
typedef tt_result_t (*tt_evc_on_thread_ev_t)(IN struct tt_evpoller_s *evp,
                                             IN tt_ev_t *tev);

typedef struct
{
    tt_evc_on_thread_ev_t on_thread_ev;
} tt_thread_ev_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_thread_ev_attr_default(OUT tt_thread_ev_attr_t *attr);

// here length is the length of data of thread ev, does not count the thread ev
// structure size
extern tt_ev_t *tt_thread_ev_create(IN tt_u32_t ev_id,
                                    IN tt_u32_t data_size,
                                    IN tt_ev_itf_t *itf);

#endif /* __TT_THREAD_EVENT__ */
