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
@file tt_ipc_aio.h
@brief async inter process communication

this file defines async IPC APIs
*/

#ifndef __TT_IPC_AIO__
#define __TT_IPC_AIO__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <io/tt_ipc_aio_cb.h>

#include <tt_ipc_aio_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_ipc_attr_s
{
    tt_u32_t recv_buf_size;

    tt_bool_t from_alloc : 1;
} tt_ipc_attr_t;

typedef struct tt_ipc_s
{
    tt_ipc_ntv_t sys_ipc;

    tt_ipc_attr_t attr;

    tt_bool_t listening : 1;
} tt_ipc_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// - for server ipc, local_addr must be set to a path. and for client,
//   local_addr must be null
extern tt_result_t tt_async_ipc_create(IN tt_ipc_t *ipc,
                                       IN const tt_char_t *local_addr,
                                       IN OPT tt_ipc_attr_t *attr,
                                       IN tt_ipc_exit_t *ipc_exit);

/**
@note
- all callbacks would be called either set @ref immediate to TT_TRUE or
  TT_FALSE. setting @ref immediate to TT_TRUE will calls all callback
  before this function return. setting to TT_FALSE will execute the
  callbacks asynchronously
- setting @ref immediate to TT_TRUE can be called immediately after
  tt_ipc_connect_async and tt_ipc_accept_async, in other cases the
  program may crash
 */
extern void tt_async_ipc_destroy(IN tt_ipc_t *ipc, IN tt_bool_t immediate);

extern void tt_ipc_attr_default(IN tt_ipc_attr_t *attr);

// caller can not assume that this function could return fail when
// passing an invalid remote_addr, the behavior depends on os
extern tt_result_t tt_ipc_connect_async(IN tt_ipc_t *ipc,
                                        IN const tt_char_t *remote_addr,
                                        IN tt_ipc_on_connect_t on_connect,
                                        IN OPT void *cb_param);

// on windows, new_ipc should be destroyed by tt_async_ipc_destroy
// once this function returns success
extern tt_result_t tt_ipc_accept_async(IN tt_ipc_t *listening_ipc,
                                       IN tt_ipc_t *new_ipc,
                                       IN OPT tt_ipc_attr_t *new_ipc_attr,
                                       IN tt_ipc_exit_t *new_ipc_exit,
                                       IN tt_ipc_on_accept_t on_accept,
                                       IN OPT void *cb_param);

extern tt_result_t tt_ipc_send_async(IN tt_ipc_t *ipc,
                                     IN tt_ev_t *pev,
                                     IN tt_ipc_on_send_t on_send,
                                     IN OPT void *cb_param);

extern tt_result_t tt_ipc_recv_async(IN tt_ipc_t *ipc,
                                     IN tt_ipc_on_recv_t on_recv,
                                     IN OPT void *cb_param);

#endif /* __TT_IPC_AIO__ */
