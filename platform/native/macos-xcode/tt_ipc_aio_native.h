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
@file tt_ipc_aio_native.h
@brief ipc portlayer

this file defines ipc native
*/

#ifndef __TT_IPC_AIO_NATIVE__
#define __TT_IPC_AIO_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>
#include <algorithm/tt_list.h>
#include <event/tt_event_base.h>
#include <io/tt_ipc_aio_cb.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_ipc_s;
struct tt_ipc_attr_s;
struct tt_evpoller_s;

// ipc event ranges
enum
{
    TT_IPC_RANGE_KQ_MARK,
    TT_IPC_RANGE_AIO,
};

enum
{
    TT_IPC_EV_START =
        TT_EV_MKID_SUB(TT_EV_RANGE_INTERNAL_IPC, TT_IPC_RANGE_KQ_MARK, 0),

    TT_IPC_MARK_KQ_RD,
    TT_IPC_MARK_KQ_WR,

    TT_IPC_EV_END,
};

typedef struct
{
    int s;

    // ========================================
    // below for async aio
    // ========================================

    struct tt_evcenter_s *evc;
    tt_u32_t ev_mark_rd;
    tt_u32_t ev_mark_wr;

    void *on_destroy;
    void *on_destroy_param;

    tt_list_t read_q;
    tt_list_t write_q;
    tt_buf_t recv_buf;

    tt_bool_t rd_closing : 1;
    tt_bool_t wr_closing : 1;
    tt_bool_t connecting : 1;
    tt_bool_t connected : 1;
} tt_ipc_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_async_ipc_create_ntv(IN tt_ipc_ntv_t *sys_ipc,
                                           IN OPT const tt_char_t *local_addr,
                                           IN OPT struct tt_ipc_attr_s *attr,
                                           IN tt_ipc_exit_t *ipc_exit);

extern void tt_async_ipc_destroy_ntv(IN tt_ipc_ntv_t *sys_ipc,
                                     IN tt_bool_t immediate);

extern tt_result_t tt_ipc_connect_async_ntv(IN tt_ipc_ntv_t *sys_ipc,
                                            IN const tt_char_t *remote_addr,
                                            IN tt_ipc_on_connect_t on_connect,
                                            IN OPT void *cb_param);

extern tt_result_t tt_ipc_accept_async_ntv(
    IN tt_ipc_ntv_t *listening_ipc,
    IN struct tt_ipc_s *new_ipc,
    IN OPT struct tt_ipc_attr_s *new_ipc_attr,
    IN tt_ipc_exit_t *new_ipc_exit,
    IN tt_ipc_on_accept_t on_accept,
    IN OPT void *cb_param);

extern tt_result_t tt_ipc_send_async_ntv(IN tt_ipc_ntv_t *sys_ipc,
                                         IN tt_ev_t *ev,
                                         IN tt_ipc_on_send_t on_send,
                                         IN OPT void *cb_param);

extern tt_result_t tt_ipc_recv_async_ntv(IN tt_ipc_ntv_t *sys_ipc,
                                         IN tt_ipc_on_recv_t on_recv,
                                         IN OPT void *cb_param);

extern tt_result_t tt_ipc_tev_handler(IN struct tt_evpoller_s *evp,
                                      IN tt_ev_t *ev);

extern tt_result_t tt_ipc_kq_rd_handler(IN struct tt_ipc_s *ipc,
                                        IN tt_u32_t flags,
                                        IN tt_u32_t fflags,
                                        IN tt_uintptr_t data);

extern tt_result_t tt_ipc_kq_wr_handler(IN struct tt_ipc_s *ipc,
                                        IN tt_u32_t flags,
                                        IN tt_u32_t fflags,
                                        IN tt_uintptr_t data);

#endif /* __TT_IPC_AIO_NATIVE__ */
