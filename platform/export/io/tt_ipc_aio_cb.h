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
@file tt_ipc_aio_cb.h
@brief ipc callbacks

this file defines ipc callback type
*/

#ifndef __TT_IPC_AIO_CB__
#define __TT_IPC_AIO_CB__

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

struct tt_ipc_s;
struct tt_ev_s;

typedef struct
{
    tt_result_t result;
    void *cb_param;
} tt_ipc_aioctx_t;

typedef void (*tt_ipc_on_accept_t)(IN struct tt_ipc_s *listening_ipc,
                                   IN struct tt_ipc_s *new_ipc,
                                   IN tt_ipc_aioctx_t *aioctx);

typedef void (*tt_ipc_on_connect_t)(IN struct tt_ipc_s *ipc,
                                    IN const tt_char_t *remote_addr,
                                    IN tt_ipc_aioctx_t *aioctx);

typedef void (*tt_ipc_on_send_t)(IN struct tt_ipc_s *ipc,
                                 IN struct tt_ev_s *pev,
                                 IN tt_ipc_aioctx_t *aioctx);

typedef void (*tt_ipc_on_recv_t)(IN struct tt_ipc_s *ipc,
                                 IN tt_ipc_aioctx_t *aioctx,
                                 IN TO struct tt_ev_s **pev,
                                 IN tt_u32_t pev_num);

typedef void (*tt_ipc_on_destroy_t)(IN struct tt_ipc_s *ipc, IN void *cb_param);

typedef struct
{
    tt_ipc_on_destroy_t on_destroy;
    void *cb_param;
} tt_ipc_exit_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_IPC_AIO_CB__ */
