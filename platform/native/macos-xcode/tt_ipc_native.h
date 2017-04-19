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
@file tt_ipc_native.h
@brief ipc native

this file defines ipc native
*/

#ifndef __TT_IPC_NATIVE__
#define __TT_IPC_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_ipc_attr_s;
struct tt_fiber_ev_s;
struct tt_io_ev_s;

typedef struct
{
    int s;
} tt_ipc_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_ipc_create_ntv(IN tt_ipc_ntv_t *ipc,
                                     IN OPT const tt_char_t *addr,
                                     IN OPT struct tt_ipc_attr_s *attr);

extern void tt_ipc_destroy_ntv(IN tt_ipc_ntv_t *ipc);

extern tt_result_t tt_ipc_connect_ntv(IN tt_ipc_ntv_t *ipc,
                                      IN const tt_char_t *addr);

extern tt_result_t tt_ipc_accept_ntv(IN tt_ipc_ntv_t *ipc,
                                     IN tt_ipc_ntv_t *new_ipc);

extern tt_result_t tt_ipc_send_ntv(IN tt_ipc_ntv_t *ipc,
                                   IN tt_u8_t *buf,
                                   IN tt_u32_t len,
                                   OUT OPT tt_u32_t *sent);

extern tt_result_t tt_ipc_recv_ntv(IN tt_ipc_ntv_t *ipc,
                                   OUT tt_u8_t *buf,
                                   IN tt_u32_t len,
                                   OUT OPT tt_u32_t *recvd,
                                   OUT OPT struct tt_fiber_ev_s **fev);

extern void tt_ipc_worker_io(IN struct tt_io_ev_s *io_ev);

extern tt_bool_t tt_ipc_poller_io(IN struct tt_io_ev_s *io_ev);

#endif /* __TT_IPC_NATIVE__ */