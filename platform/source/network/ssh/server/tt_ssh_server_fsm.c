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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/ssh/server/tt_ssh_server_fsm.h>

#include <network/ssh/message/tt_ssh_message.h>
#include <network/ssh/message/tt_ssh_msg_disconnect.h>
#include <network/ssh/message/tt_ssh_msg_verxchg.h>
#include <network/ssh/server/tt_ssh_server_conn.h>
#include <network/ssh/server/tt_ssh_server_state_auth.h>
#include <network/ssh/server/tt_ssh_server_state_authdone.h>
#include <network/ssh/server/tt_ssh_server_state_kexdh.h>
#include <network/ssh/server/tt_ssh_server_state_kexdone.h>
#include <network/ssh/server/tt_ssh_server_state_keyxchg.h>
#include <network/ssh/server/tt_ssh_server_state_verxchg.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_sshsvr_state_evhdlr_t __state_evhdlr_table[TT_SSHSVRST_NUM] = {
    tt_sshsvr_state_verxchg, // TT_SSHSVRST_VERXCHG
    tt_sshsvr_state_keyxchg, // TT_SSHSVRST_KEYXCHG
    tt_sshsvr_state_kexdh, // TT_SSHSVRST_KEXDH
    tt_sshsvr_state_kexdone, // TT_SSHSVRST_KEX_DONE
    tt_sshsvr_state_auth, // TT_SSHSVRST_AUTH
    tt_sshsvr_state_authdone, // TT_SSHSVRST_AUTH_DONE
};

const tt_char_t *tt_sshsvr_state_name[TT_SSHSVRST_NUM] = {
    "VERXCHG", "KEYXCHG", "KEXDH", "KEX_DONE", "AUTH", "AUTH_DONE",
};

const tt_char_t *tt_sshsvr_event_name[TT_SSHSVREV_NUM] = {"PACKET",
                                                          "DISCONNECT",
                                                          "VERXCHG",
                                                          "KEYINIT",
                                                          "NEWKEYS",
                                                          "KEXDH_REPLY",
                                                          "SERVICE_REQUEST",
                                                          "SERVICE_ACCEPT"};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __sshsvract_init(IN tt_sshsvr_action_t *svract);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_sshsvr_fsm(IN struct tt_sshsvrconn_s *svrconn,
                          IN tt_sshsvr_event_t event,
                          IN void *param)
{
    tt_sshsvr_state_t state;
    tt_sshsvr_action_t svract;

enter_fsm:

    state = svrconn->state;
    TT_DETAIL("ssh fsm state: [%s], event[%s]",
              tt_sshsvr_state_name[state],
              tt_sshsvr_event_name[event]);

    TT_ASSERT(TT_SSHSVR_STATE_VALID(state));
    TT_ASSERT(__state_evhdlr_table[state] != NULL);
    TT_ASSERT(TT_SSHSVR_EVENT_VALID(event));

    // ========================================
    // pre processing
    // ========================================

    if (state == TT_SSHSVRST_DISCONNECTED) {
        TT_SSH_EV_IGNORED(state, event);
        return TT_END;
    }

    if (event == TT_SSHSVREV_PACKET) {
        tt_sshmsg_t *msg = (tt_sshmsg_t *)param;
        switch (msg->msg_id) {
            case TT_SSH_MSGID_DISCONNECT: {
                return TT_END;
            } break;
            case TT_SSH_MSGID_IGNORE:
            case TT_SSH_MSGID_UNIMPLEMENTED:
            case TT_SSH_MSGID_DEBUG: {
                TT_SSH_EV_IGNORED(state, event);
                return TT_SUCCESS;
            } break;

            default: {
                // let fsm process it
            } break;
        }
    } else if (event == TT_SSHSVREV_DISCONNECT) {
        tt_sshsvr_disconnect(svrconn, TT_SSH_DMRC_BY_APPLICATION, param);
        return TT_END;
    }

    // ========================================
    // processing
    // ========================================

    __sshsvract_init(&svract);
    __state_evhdlr_table[state](svrconn, event, param, &svract);

    // ========================================
    // post processing
    // ========================================

    if (svract.new_state != TT_SSHSVRST_NUM) {
        tt_sshsvr_new_state(svrconn, svract.new_state, event);
    }

    if (svract.new_event != TT_SSHSVREV_NUM) {
        event = svract.new_event;
        param = &svract.param;
        goto enter_fsm;
    }

    return TT_SUCCESS;
}

void tt_sshsvr_new_state(IN struct tt_sshsvrconn_s *svrconn,
                         IN tt_sshsvr_state_t new_state,
                         IN tt_sshsvr_event_t by_event)
{
    svrconn->last_state = svrconn->state;
    svrconn->by_event = svrconn->by_event;

    svrconn->state = new_state;
}

void tt_sshsvr_disconnect(IN tt_sshsvrconn_t *svrconn,
                          IN tt_u32_t reason_code,
                          IN OPT const tt_char_t *description)
{
    tt_sshmsg_t *msg = NULL;

    if (svrconn->state == TT_SSHSVRST_DISCONNECTED) {
        return;
    }
    tt_sshsvr_new_state(svrconn,
                        TT_SSHSVRST_DISCONNECTED,
                        TT_SSHSVREV_DISCONNECT);

    // ssh msg: disconnect
    msg = tt_sshmsg_disconnect_create();
    if (msg == NULL) {
        TT_FATAL("fail to new ssh disconnect msg");
        return;
    }

    // set reason
    tt_sshmsg_disconnect_set_reason(msg, reason_code);

    // set description
    if (description != NULL) {
        tt_sshmsg_disconnect_set_desc(msg, description);
    }

    // send
    tt_sshsvrconn_send(svrconn, msg);
}

void __sshsvract_init(IN tt_sshsvr_action_t *svract)
{
    svract->new_state = TT_SSHSVRST_NUM;
    svract->new_event = TT_SSHSVREV_NUM;
    tt_memset(&svract->param, 0, sizeof(svract->param));
}
