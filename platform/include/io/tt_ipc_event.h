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
@file tt_ipc_event.h
@brief ipc event
*/

#ifndef __TT_IPC_EVENT__
#define __TT_IPC_EVENT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_IPC_EV_CAST(pev, type) TT_PTR_INC(type, pev, sizeof(tt_ipc_ev_t))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_ipc_s;
struct tt_fiber_ev_s;
struct tt_tmr_s;

typedef struct tt_ipc_ev_s
{
    tt_u32_t ev;
    tt_u32_t size;
} tt_ipc_ev_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_ipc_ev_init(IN tt_ipc_ev_t *pev,
                              IN tt_u32_t ev,
                              IN tt_u32_t size)
{
    pev->ev = ev;
    pev->size = size;
}

extern tt_ipc_ev_t *tt_ipc_ev_create(IN tt_u32_t ev, IN tt_u32_t size);

extern void tt_ipc_ev_destroy(IN tt_ipc_ev_t *pev);

extern tt_result_t tt_ipc_send_ev(IN struct tt_ipc_s *dst, IN tt_ipc_ev_t *pev);

extern tt_result_t tt_ipc_recv_ev(IN struct tt_ipc_s *ipc,
                                  OUT tt_ipc_ev_t **p_pev,
                                  OUT struct tt_fiber_ev_s **p_fev,
                                  OUT struct tt_tmr_s **p_tmr);

#endif // __TT_IPC_EVENT__
