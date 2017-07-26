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
@file tt_ssh_server_conn.h
@brief ts ssh server connection
*/

#ifndef __TT_SSH_SERVER_CONN__
#define __TT_SSH_SERVER_CONN__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>
#include <io/tt_socket.h>
#include <network/ssh/context/tt_ssh_context.h>
#include <network/ssh/server/tt_ssh_server_fsm.h>
#include <network/ssh/tt_ssh_channel_manager.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_sshsvr_s;
struct tt_sshch_s;
struct tt_sshmsg_s;

typedef struct tt_sshsvrconn_s
{
    struct tt_sshsvr_s *server;
    tt_sshchmgr_t chmgr;

    tt_skt_t skt;
    tt_buf_t recv_buf;
    tt_u32_t plaintext_len;
    tt_lnode_t node;

    tt_sshsvr_state_t state;
    tt_sshctx_t ctx;
    struct tt_sshch_s **ch;
    tt_u32_t ch_num;

    tt_u32_t flag;

    // message status bits
    tt_bool_t ms_verxchg_in : 1;
    tt_bool_t ms_verxchg_out : 1;
    tt_bool_t ms_keyinit_in : 1;
    tt_bool_t ms_keyinit_out : 1;
    tt_bool_t ms_newkeys_in : 1;
    tt_bool_t ms_newkeys_out : 1;

    // debug
    tt_sshsvr_state_t last_state;
    tt_sshsvr_event_t by_event;
} tt_sshsvrconn_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_sshsvrconn_t *tt_sshsvrconn_create(IN struct tt_sshsvr_s *server);

tt_export void tt_sshsvrconn_destroy(IN tt_sshsvrconn_t *svrconn,
                                  IN tt_bool_t immediate);

tt_export void tt_sshsvrconn_shutdown(IN tt_sshsvrconn_t *svrconn);

tt_export tt_result_t tt_sshsvrconn_recv(IN tt_sshsvrconn_t *svrconn);

tt_export tt_result_t tt_sshsvrconn_send(IN tt_sshsvrconn_t *svrconn,
                                      IN TO struct tt_sshmsg_s *sshmsg);

// ========================================
// ssh server channel
// ========================================

tt_export struct tt_sshch_s *tt_sshsvrconn_ch_create(IN tt_sshsvrconn_t *svrconn,
                                                  IN tt_ssh_chtype_t type);

tt_export void tt_sshsvrconn_ch_pkt_handler(IN tt_sshsvrconn_t *svrconn,
                                         IN struct tt_sshmsg_s *msg,
                                         OUT tt_sshsvr_action_t *svract);

#endif /* __TT_SSH_SERVER_CONN__ */
