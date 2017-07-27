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
@file tt_ssh_channel.h
@brief ts ssh channel
*/

#ifndef __TT_SSH_CHANNEL__
#define __TT_SSH_CHANNEL__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>
#include <network/ssh/tt_ssh_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_sshch_cb_s;
struct tt_sshchmgr_s;
struct tt_sshmsg_s;
struct tt_sshsvr_action_s;

typedef struct tt_sshch_s
{
    tt_ssh_chtype_t type;
    tt_u32_t chnum;
    tt_u32_t peer_chnum;
    tt_u32_t win_size;
    tt_u32_t pkt_size;
    struct tt_sshch_cb_s *cb;
    void *opaque;
    tt_buf_t send_buf;

    struct tt_sshchmgr_s *chmgr;
    tt_u32_t head_mac_size;
    tt_u32_t block_size;

    tt_bool_t eof_in : 1;
    tt_bool_t eof_out : 1;
    tt_bool_t close_in : 1;
    tt_bool_t close_out : 1;
} tt_sshch_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_sshch_t *tt_sshch_create(IN tt_ssh_chtype_t type,
                                      IN tt_u32_t chnum,
                                      IN struct tt_sshch_cb_s *cb,
                                      IN struct tt_sshchmgr_s *chmgr);

// destroy ssh channel
tt_export void tt_sshch_destroy(IN tt_sshch_t *ch);

// send eof and close
tt_export void tt_sshch_shutdown(IN tt_sshch_t *ch);

tt_inline void tt_sshch_set_peerchnum(IN tt_sshch_t *ch, IN tt_u32_t peer_chnum)
{
    ch->peer_chnum = peer_chnum;
}

tt_inline void tt_sshch_set_winsize(IN tt_sshch_t *ch, IN tt_u32_t win_size)
{
    ch->win_size = win_size;
}

tt_inline void tt_sshch_set_pktsize(IN tt_sshch_t *ch, IN tt_u32_t pkt_size)
{
    ch->pkt_size = pkt_size;
}

tt_inline void tt_sshch_set_opaque(IN tt_sshch_t *ch, IN void *opaque)
{
    ch->opaque = opaque;
}

tt_export tt_result_t tt_sshch_send(IN tt_sshch_t *ch,
                                    IN tt_u8_t *data,
                                    IN tt_u32_t data_len,
                                    IN tt_u32_t flag);

tt_export void tt_sshch_pkt_handler(IN tt_sshch_t *ch,
                                    IN struct tt_sshmsg_s *msg,
                                    OUT struct tt_sshsvr_action_s *svract);

#endif /* __TT_SSH_CHANNEL__ */
