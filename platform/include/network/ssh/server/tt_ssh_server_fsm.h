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
@file tt_ssh_server_fsm.h
@brief ts ssh server fsm
*/

#ifndef __TT_SSH_SERVER_FSM__
#define __TT_SSH_SERVER_FSM__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

//#include <io/tt_socket_aio.h>
#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_SSHSVR_CIPHER_STATE(state) ((state) >= TT_SSHSVRST_KEX_DONE)

#define TT_SSH_EV_IGNORED(state, event)                                        \
    do {                                                                       \
        TT_INFO("event[%s] is ignored at state[%s]",                           \
                tt_sshsvr_event_name[(event)],                                 \
                tt_sshsvr_state_name[(state)]);                                \
    } while (0)

#define TT_SSH_MSGID_IGNORED(state, msg_id)                                    \
    do {                                                                       \
        TT_INFO("message[%d] is ignored at state[%s]",                         \
                (msg_id),                                                      \
                tt_sshsvr_state_name[(state)]);                                \
    } while (0)
#define TT_SSH_MSG_FAILURE(state, msg_id)                                      \
    do {                                                                       \
        TT_INFO("can not handle message[%d] at state[%s]",                     \
                (msg_id),                                                      \
                tt_sshsvr_state_name[(state)]);                                \
    } while (0)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_sshsvrconn_s;

typedef enum {
    // initial states
    TT_SSHSVRST_VERXCHG,

    // key exchange states
    TT_SSHSVRST_KEYXCHG,
    TT_SSHSVRST_KEXDH,
    TT_SSHSVRST_KEX_DONE,

    // authentication states
    TT_SSHSVRST_AUTH,
    TT_SSHSVRST_AUTH_DONE,

    TT_SSHSVRST_DISCONNECTED,

    TT_SSHSVRST_NUM
} tt_sshsvr_state_t;
#define TT_SSHSVR_STATE_VALID(state) ((state) < TT_SSHSVRST_NUM)

typedef enum {
    TT_SSHSVREV_PACKET,

    // the proper way to send ssh disconnect is to post a
    // TT_SSHSVREV_DISCONNECT event during fsm, but note
    // do not set ssh state to TT_SSHSVRST_DISCONNECTED
    // at same time, otherwise TT_SSHSVREV_DISCONNECT
    // would be ignored
    TT_SSHSVREV_DISCONNECT,

    TT_SSHSVREV_VERXCHG,

    TT_SSHSVREV_KEYINIT,
    TT_SSHSVREV_NEWKEYS,

    TT_SSHSVREV_KEXDH_REPLY,

    TT_SSHSVREV_SERVICE_REQUEST,
    TT_SSHSVREV_SERVICE_ACCEPT,

    TT_SSHSVREV_NUM
} tt_sshsvr_event_t;
#define TT_SSHSVR_EVENT_VALID(event) ((event) < TT_SSHSVREV_NUM)

typedef struct tt_sshsvr_action_s
{
    tt_sshsvr_state_t new_state;
    tt_sshsvr_event_t new_event;
    union
    {
        void *reserved;
    } param;
} tt_sshsvr_action_t;

typedef void (*tt_sshsvr_state_evhdlr_t)(IN struct tt_sshsvrconn_s *svrconn,
                                         IN tt_sshsvr_event_t event,
                                         IN void *param,
                                         OUT tt_sshsvr_action_t *svract);

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export const tt_char_t *tt_sshsvr_state_name[TT_SSHSVRST_NUM];

tt_export const tt_char_t *tt_sshsvr_event_name[TT_SSHSVREV_NUM];

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// return
//  - TT_SUCCESS, event has been processed
//  - TT_E_END, app should shutdown ssh conn and then destroy
//  - TT_FAIL, error occurred, app should destroy ssh conn
tt_export tt_result_t tt_sshsvr_fsm(IN struct tt_sshsvrconn_s *svrconn,
                                    IN tt_sshsvr_event_t event,
                                    IN void *param);

tt_export void tt_sshsvr_new_state(IN struct tt_sshsvrconn_s *svrconn,
                                   IN tt_sshsvr_state_t new_state,
                                   IN tt_sshsvr_event_t by_event);

tt_export void tt_sshsvr_disconnect(IN struct tt_sshsvrconn_s *svrconn,
                                    IN tt_u32_t reason_code,
                                    IN OPT const tt_char_t *description);

#endif /* __TT_SSH_SERVER_FSM__ */
