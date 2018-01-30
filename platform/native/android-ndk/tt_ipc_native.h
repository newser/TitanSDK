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
@file tt_ipc_native.h
@brief ipc native

this file defines ipc native
*/

#ifndef __TT_IPC_NATIVE__
#define __TT_IPC_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <os/tt_fiber_event.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;
struct tt_ipc_attr_s;
struct tt_tmr_s;
struct tt_io_ev_s;
struct tt_ipc_s;
struct tt_ipc_attr_s;
struct tt_skt_s;

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

extern tt_result_t tt_ipc_component_init_ntv(IN struct tt_profile_s *profile);

extern tt_result_t tt_ipc_create_ntv(IN tt_ipc_ntv_t *ipc,
                                     IN OPT const tt_char_t *addr,
                                     IN OPT struct tt_ipc_attr_s *attr);

extern void tt_ipc_destroy_ntv(IN tt_ipc_ntv_t *ipc);

extern tt_result_t tt_ipc_connect_ntv(IN tt_ipc_ntv_t *ipc,
                                      IN const tt_char_t *addr);

extern struct tt_ipc_s *tt_ipc_accept_ntv(IN tt_ipc_ntv_t *ipc,
                                          IN struct tt_ipc_attr_s *new_attr,
                                          OUT tt_fiber_ev_t **p_fev,
                                          OUT struct tt_tmr_s **p_tmr);

extern tt_result_t tt_ipc_send_ntv(IN tt_ipc_ntv_t *ipc,
                                   IN tt_u8_t *buf,
                                   IN tt_u32_t len,
                                   OUT OPT tt_u32_t *sent);

extern tt_result_t tt_ipc_recv_ntv(IN tt_ipc_ntv_t *ipc,
                                   OUT tt_u8_t *buf,
                                   IN tt_u32_t len,
                                   OUT tt_u32_t *recvd,
                                   OUT tt_fiber_ev_t **p_fev,
                                   OUT struct tt_tmr_s **p_tmr);

extern tt_result_t tt_ipc_sendskt_ntv(IN tt_ipc_ntv_t *ipc,
                                      IN TO struct tt_skt_s *skt);

extern tt_result_t tt_ipc_recvskt_ntv(IN tt_ipc_ntv_t *ipc,
                                      OUT tt_fiber_ev_t **p_fev,
                                      OUT struct tt_tmr_s **p_tmr,
                                      OUT struct tt_skt_s **p_skt);

extern void tt_ipc_worker_io(IN struct tt_io_ev_s *io_ev);

extern tt_bool_t tt_ipc_poller_io(IN struct tt_io_ev_s *io_ev);

extern tt_result_t tt_ipc_local_addr_ntv(IN tt_ipc_ntv_t *ipc,
                                         OUT tt_char_t *addr,
                                         IN tt_u32_t size,
                                         OUT OPT tt_u32_t *len);

extern tt_result_t tt_ipc_remote_addr_ntv(IN tt_ipc_ntv_t *ipc,
                                          OUT tt_char_t *addr,
                                          IN tt_u32_t size,
                                          OUT OPT tt_u32_t *len);

#endif /* __TT_IPC_NATIVE__ */
