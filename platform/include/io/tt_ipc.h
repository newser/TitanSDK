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
@file tt_ipc.h
@brief inter process communication

this file defines IPC APIs
*/

#ifndef __TT_IPC__
#define __TT_IPC__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>

#include <tt_ipc_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_fiber_ev_s;

typedef struct tt_ipc_attr_s
{
    tt_buf_attr_t recv_buf_attr;
} tt_ipc_attr_t;

typedef struct tt_ipc_s
{
    tt_buf_t buf;
    tt_ipc_ntv_t sys_ipc;
} tt_ipc_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_ipc_component_register()
register socket system
*/
extern void tt_ipc_component_register();

// - for server ipc, addr must be set to a path. and for client,
//   addr must be null
extern tt_ipc_t *tt_ipc_create(IN OPT const tt_char_t *addr,
                               IN OPT tt_ipc_attr_t *attr);

extern void tt_ipc_destroy(IN tt_ipc_t *ipc);

extern void tt_ipc_attr_default(IN tt_ipc_attr_t *attr);

// client can only connect when server has called tt_ipc_accept(),
// an example: ipc is established when server calls accept() and
// then client calls connect(), now if client create a new ipc and
// tries to connect to same address, the connect would fail as server
// does not issued new accept()
extern tt_result_t tt_ipc_connect(IN tt_ipc_t *ipc, IN const tt_char_t *addr);

// will block calling thread during retrying
extern tt_result_t tt_ipc_connect_retry(IN tt_ipc_t *ipc,
                                        IN const tt_char_t *addr,
                                        IN tt_u32_t interval_ms,
                                        IN tt_u32_t retry_count);

extern tt_ipc_t *tt_ipc_accept(IN tt_ipc_t *ipc,
                               IN OPT tt_ipc_attr_t *new_attr);

tt_inline tt_result_t tt_ipc_send(IN tt_ipc_t *ipc,
                                  IN tt_u8_t *buf,
                                  IN tt_u32_t len,
                                  OUT OPT tt_u32_t *sent)
{
    if (len != 0) {
        return tt_ipc_send_ntv(&ipc->sys_ipc, buf, len, sent);
    } else {
        *sent = 0;
        return TT_SUCCESS;
    }
}

tt_inline tt_result_t tt_ipc_recv(IN tt_ipc_t *ipc,
                                  OUT tt_u8_t *buf,
                                  IN tt_u32_t len,
                                  OUT OPT tt_u32_t *recvd,
                                  OUT OPT struct tt_fiber_ev_s **fev)
{
    if (len != 0) {
        return tt_ipc_recv_ntv(&ipc->sys_ipc, buf, len, recvd, fev);
    } else {
        TT_ERROR("recv buf len can not be 0");
        return TT_FAIL;
    }
}

#endif /* __TT_IPC__ */
