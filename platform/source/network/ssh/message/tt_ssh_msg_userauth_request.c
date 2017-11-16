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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/ssh/message/tt_ssh_msg_userauth_request.h>

#include <network/ssh/message/tt_ssh_message.h>
#include <network/ssh/message/tt_ssh_parse.h>
#include <network/ssh/message/tt_ssh_render.h>

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

static tt_result_t __uar_create(IN struct tt_sshmsg_s *msg);
static void __uar_destroy(IN struct tt_sshmsg_s *msg);

static tt_result_t __uar_render_prepare(IN struct tt_sshmsg_s *msg,
                                        OUT tt_u32_t *len,
                                        OUT tt_ssh_render_mode_t *mode);
static tt_result_t __uar_render(IN struct tt_sshmsg_s *msg,
                                IN OUT tt_buf_t *buf);

static tt_result_t __uar_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data);

static tt_sshmsg_itf_t __uar_op = {
    __uar_create,
    __uar_destroy,
    NULL,

    __uar_render_prepare,
    __uar_render,

    __uar_parse,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshmsg_t *tt_sshmsg_uar_create()
{
    return tt_sshmsg_create(TT_SSH_MSGID_USERAUTH_REQUEST,
                            sizeof(tt_sshmsg_uar_t),
                            &__uar_op);
}

tt_result_t tt_sshmsg_uar_set_user(IN tt_sshmsg_t *msg,
                                   IN const tt_char_t *user)
{
    tt_sshmsg_uar_t *uar;
    tt_u32_t user_len;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_USERAUTH_REQUEST);
    TT_ASSERT(user != NULL);

    uar = TT_SSHMSG_CAST(msg, tt_sshmsg_uar_t);

    tt_blob_destroy(&uar->user);

    user_len = (tt_u32_t)tt_strlen(user);
    if (user_len > 0) {
        return tt_blob_create(&uar->user, (tt_u8_t *)user, user_len);
    } else {
        tt_blob_init(&uar->user);
        return TT_SUCCESS;
    }
}

void tt_sshmsg_uar_set_service(IN tt_sshmsg_t *msg, IN tt_ssh_service_t service)
{
    tt_sshmsg_uar_t *uar;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_USERAUTH_REQUEST);
    TT_ASSERT(TT_SSH_SERVICE_VALID(service));

    uar = TT_SSHMSG_CAST(msg, tt_sshmsg_uar_t);

    uar->service = service;
}

void tt_sshmsg_uar_set_auth(IN tt_sshmsg_t *msg, IN tt_ssh_auth_t auth)
{
    tt_sshmsg_uar_t *uar;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_USERAUTH_REQUEST);
    TT_ASSERT(TT_SSH_AUTH_VALID(auth));

    uar = TT_SSHMSG_CAST(msg, tt_sshmsg_uar_t);

    uar->auth = auth;
}

tt_result_t tt_sshmsg_uar_set_pubkey_alg(IN tt_sshmsg_t *msg,
                                         IN tt_ssh_pubkey_alg_t alg)
{
    tt_sshmsg_uar_t *uar;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_USERAUTH_REQUEST);
    TT_ASSERT(TT_SSH_PUBKEY_ALG_VALID(alg));

    uar = TT_SSHMSG_CAST(msg, tt_sshmsg_uar_t);

    if (uar->auth != TT_SSH_AUTH_PUBLICKEY) {
        TT_ERROR("not pubkey auth");
        return TT_FAIL;
    }

    uar->auth_u.pubkey.pubkey_alg = alg;

    return TT_SUCCESS;
}

tt_result_t tt_sshmsg_uar_set_pubkey(IN tt_sshmsg_t *msg,
                                     IN tt_u8_t *pubkey,
                                     IN tt_u32_t pubkey_len)
{
    tt_sshmsg_uar_t *uar;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_USERAUTH_REQUEST);
    TT_ASSERT((pubkey != NULL) && (pubkey_len != 0));

    uar = TT_SSHMSG_CAST(msg, tt_sshmsg_uar_t);

    if (uar->auth != TT_SSH_AUTH_PUBLICKEY) {
        TT_ERROR("not pubkey auth");
        return TT_FAIL;
    }

    tt_blob_destroy(&uar->auth_u.pubkey.pubkey);
    if (!TT_OK(
            tt_blob_create(&uar->auth_u.pubkey.pubkey, pubkey, pubkey_len))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_sshmsg_uar_set_signature(IN tt_sshmsg_t *msg,
                                        IN tt_u8_t *sig,
                                        IN tt_u32_t sig_len)
{
    tt_sshmsg_uar_t *uar;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_USERAUTH_REQUEST);
    TT_ASSERT((sig != NULL) && (sig_len != 0));

    uar = TT_SSHMSG_CAST(msg, tt_sshmsg_uar_t);

    if (uar->auth != TT_SSH_AUTH_PUBLICKEY) {
        TT_ERROR("not pubkey auth");
        return TT_FAIL;
    }

    tt_blob_destroy(&uar->auth_u.pubkey.signature);
    if (!TT_OK(tt_blob_create(&uar->auth_u.pubkey.signature, sig, sig_len))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_sshmsg_uar_set_pwd(IN tt_sshmsg_t *msg, IN const tt_char_t *pwd)
{
    tt_sshmsg_uar_t *uar;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_USERAUTH_REQUEST);
    TT_ASSERT(pwd != NULL);

    uar = TT_SSHMSG_CAST(msg, tt_sshmsg_uar_t);

    if (uar->auth != TT_SSH_AUTH_PASSWORD) {
        TT_ERROR("not password auth");
        return TT_FAIL;
    }

    tt_blob_destroy(&uar->auth_u.pwd.pwd);
    if (!TT_OK(tt_blob_create(&uar->auth_u.pwd.pwd,
                              (tt_u8_t *)pwd,
                              (tt_u32_t)tt_strlen(pwd)))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_sshmsg_uar_set_newpwd(IN tt_sshmsg_t *msg,
                                     IN const tt_char_t *pwd)
{
    tt_sshmsg_uar_t *uar;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_USERAUTH_REQUEST);
    TT_ASSERT(pwd != NULL);

    uar = TT_SSHMSG_CAST(msg, tt_sshmsg_uar_t);

    if (uar->auth != TT_SSH_AUTH_PASSWORD) {
        TT_ERROR("not password auth");
        return TT_FAIL;
    }

    tt_blob_destroy(&uar->auth_u.pwd.new_pwd);
    if (!TT_OK(tt_blob_create(&uar->auth_u.pwd.new_pwd,
                              (tt_u8_t *)pwd,
                              (tt_u32_t)tt_strlen(pwd)))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __uar_create(IN struct tt_sshmsg_s *msg)
{
    tt_sshmsg_uar_t *uar;

    uar = TT_SSHMSG_CAST(msg, tt_sshmsg_uar_t);

    tt_blob_init(&uar->user);
    uar->service = TT_SSH_SERVICE_NUM;
    uar->auth = TT_SSH_AUTH_NUM;
    tt_memset(&uar->auth_u, 0, sizeof(uar->auth_u));

    return TT_SUCCESS;
}

void __uar_destroy(IN struct tt_sshmsg_s *msg)
{
    tt_sshmsg_uar_t *uar;

    uar = TT_SSHMSG_CAST(msg, tt_sshmsg_uar_t);

    tt_blob_destroy(&uar->user);

    if (uar->auth == TT_SSH_AUTH_PUBLICKEY) {
        tt_ssh_auth_pubkey_t *pubkey = &uar->auth_u.pubkey;

        tt_blob_destroy(&pubkey->pubkey);
        tt_blob_destroy(&pubkey->signature);
    } else if (uar->auth == TT_SSH_AUTH_PASSWORD) {
        tt_ssh_auth_pwd_t *pwd = &uar->auth_u.pwd;

        tt_blob_destroy(&pwd->pwd);
        tt_blob_destroy(&pwd->new_pwd);
    }
}

tt_result_t __uar_render_prepare(IN struct tt_sshmsg_s *msg,
                                 OUT tt_u32_t *len,
                                 OUT tt_ssh_render_mode_t *mode)
{
    tt_sshmsg_uar_t *uar;
    tt_ssh_service_t sshserv;
    tt_ssh_auth_t sshauth;
    tt_u32_t msg_len = 0;

    /*
     byte SSH_MSG_USERAUTH_REQUEST
     string user name in ISO-10646 UTF-8 encoding [RFC3629]
     string service name in US-ASCII
     string method name in US-ASCII
     .... method specific fields
     */

    uar = TT_SSHMSG_CAST(msg, tt_sshmsg_uar_t);

    // check
    sshserv = uar->service;
    if (!TT_SSH_SERVICE_VALID(sshserv)) {
        TT_ERROR("no service set");
        return TT_FAIL;
    }

    sshauth = uar->auth;
    if (!TT_SSH_AUTH_VALID(sshauth)) {
        TT_ERROR("no auth method set");
        return TT_FAIL;
    }

    // byte SSH_MSG_USERAUTH_REQUEST
    msg_len += tt_ssh_byte_render_prepare();

    // string user name
    msg_len += tt_ssh_string_render_prepare(NULL, uar->user.len);

    // string service name
    msg_len += tt_ssh_string_render_prepare(NULL,
                                            (tt_u32_t)tt_strlen(
                                                tt_g_ssh_serv_name[sshserv]));

    // string method name
    msg_len += tt_ssh_string_render_prepare(NULL,
                                            (tt_u32_t)tt_strlen(
                                                tt_g_ssh_auth_name[sshauth]));
    if (uar->auth == TT_SSH_AUTH_PUBLICKEY) {
        tt_ssh_auth_pubkey_t *pubkey = &uar->auth_u.pubkey;
        tt_ssh_pubkey_alg_t pubkey_alg = pubkey->pubkey_alg;

        /*
         byte SSH_MSG_USERAUTH_REQUEST
         string user name in ISO-10646 UTF-8 encoding [RFC3629]
         string service name in US-ASCII
         string "publickey"
         boolean FALSE
         string public key algorithm name
         string public key blob

         byte SSH_MSG_USERAUTH_REQUEST
         string user name
         string service name
         string "publickey"
         boolean TRUE
         string public key algorithm name
         string public key to be used for authentication
         string signaturenature
         */

        if (!TT_SSH_PUBKEY_ALG_VALID(pubkey_alg)) {
            TT_ERROR("no pub key alg set");
            return TT_FAIL;
        }
        if ((pubkey->pubkey.addr == NULL) || (pubkey->pubkey.len == 0)) {
            TT_ERROR("no pub key set");
            return TT_FAIL;
        }

        // boolean FALSE
        msg_len += tt_ssh_boolean_render_prepare();

        // string public key algorithm name
        msg_len += tt_ssh_string_render_prepare(NULL,
                                                (tt_u32_t)tt_strlen(
                                                    tt_g_ssh_pubkey_alg_name
                                                        [pubkey_alg]));

        // string public key blob
        msg_len += tt_ssh_string_render_prepare(NULL, pubkey->pubkey.len);

        // string signaturenature
        if (pubkey->signature.addr != NULL) {
            msg_len +=
                tt_ssh_string_render_prepare(NULL, pubkey->signature.len);
        }
    } else {
        tt_ssh_auth_pwd_t *pwd = &uar->auth_u.pwd;

        /*
         byte SSH_MSG_USERAUTH_REQUEST
         string user name
         string service name
         string "password"
         boolean FALSE
         string plaintext password in ISO-10646 UTF-8 encoding

         byte SSH_MSG_USERAUTH_REQUEST
         string user name
         string service name
         string "password"
         boolean TRUE
         string plaintext old password in ISO-10646 UTF-8 encoding
         string plaintext new password in ISO-10646 UTF-8 encoding
         */

        TT_ASSERT(uar->auth == TT_SSH_AUTH_PASSWORD);

        if ((pwd->pwd.addr == NULL) || (pwd->pwd.len == 0)) {
            TT_ERROR("no password set");
            return TT_FAIL;
        }

        // boolean FALSE
        msg_len += tt_ssh_boolean_render_prepare();

        // string plaintext password
        msg_len += tt_ssh_string_render_prepare(NULL, pwd->pwd.len);

        // string plaintext new password
        if (pwd->new_pwd.addr != NULL) {
            msg_len += tt_ssh_string_render_prepare(NULL, pwd->new_pwd.len);
        }
    }

    *len = msg_len;
    *mode = TT_SSH_RENDER_MODE_BINARY;
    return TT_SUCCESS;
}

tt_result_t __uar_render(IN struct tt_sshmsg_s *msg, IN OUT tt_buf_t *buf)
{
    tt_sshmsg_uar_t *uar;
    tt_ssh_service_t sshserv;
    tt_ssh_auth_t sshauth;

    /*
     byte SSH_MSG_USERAUTH_REQUEST
     string user name in ISO-10646 UTF-8 encoding [RFC3629]
     string service name in US-ASCII
     string method name in US-ASCII
     .... method specific fields
     */

    uar = TT_SSHMSG_CAST(msg, tt_sshmsg_uar_t);

    sshserv = uar->service;
    sshauth = uar->auth;

    // byte SSH_MSG_USERAUTH_REQUEST
    TT_DO(tt_ssh_byte_render(buf, TT_SSH_MSGID_USERAUTH_REQUEST));

    // string user name
    TT_DO(tt_ssh_string_render(buf, uar->user.addr, uar->user.len));

    // string service name
    TT_DO(
        tt_ssh_string_render(buf,
                             (tt_u8_t *)tt_g_ssh_serv_name[sshserv],
                             (tt_u32_t)tt_strlen(tt_g_ssh_serv_name[sshserv])));

    // string method name
    TT_DO(
        tt_ssh_string_render(buf,
                             (tt_u8_t *)tt_g_ssh_auth_name[sshauth],
                             (tt_u32_t)tt_strlen(tt_g_ssh_auth_name[sshauth])));
    if (uar->auth == TT_SSH_AUTH_PUBLICKEY) {
        tt_ssh_auth_pubkey_t *pubkey = &uar->auth_u.pubkey;
        tt_ssh_pubkey_alg_t pubkey_alg = pubkey->pubkey_alg;

        // boolean FALSE
        TT_DO(tt_ssh_boolean_render(buf,
                                    TT_BOOL(pubkey->signature.addr != NULL)));

        // string public key algorithm name
        TT_DO(tt_ssh_string_render(buf,
                                   (tt_u8_t *)
                                       tt_g_ssh_pubkey_alg_name[pubkey_alg],
                                   (tt_u32_t)tt_strlen(
                                       tt_g_ssh_pubkey_alg_name[pubkey_alg])));

        // string public key blob
        TT_DO(
            tt_ssh_string_render(buf, pubkey->pubkey.addr, pubkey->pubkey.len));

        // string signaturenature
        if (pubkey->signature.addr != NULL) {
            TT_DO(tt_ssh_string_render(buf,
                                       pubkey->signature.addr,
                                       pubkey->signature.len));
        }
    } else {
        tt_ssh_auth_pwd_t *pwd = &uar->auth_u.pwd;

        /*
         byte SSH_MSG_USERAUTH_REQUEST
         string user name
         string service name
         string "password"
         boolean FALSE
         string plaintext password in ISO-10646 UTF-8 encoding

         byte SSH_MSG_USERAUTH_REQUEST
         string user name
         string service name
         string "password"
         boolean TRUE
         string plaintext old password in ISO-10646 UTF-8 encoding
         string plaintext new password in ISO-10646 UTF-8 encoding
         */

        TT_ASSERT(uar->auth == TT_SSH_AUTH_PASSWORD);

        // boolean FALSE
        TT_DO(tt_ssh_boolean_render(buf, TT_BOOL(pwd->new_pwd.addr != NULL)));

        // string plaintext password
        TT_DO(tt_ssh_string_render(buf, pwd->pwd.addr, pwd->pwd.len));

        // string plaintext new password
        if (pwd->new_pwd.addr != NULL) {
            TT_DO(
                tt_ssh_string_render(buf, pwd->new_pwd.addr, pwd->new_pwd.len));
        }
    }

    return TT_SUCCESS;
}

tt_result_t __uar_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data)
{
    tt_sshmsg_uar_t *uar;
    tt_u8_t *addr;
    tt_u32_t len;
    tt_ssh_service_t sshserv;
    tt_ssh_auth_t sshauth;

    uar = TT_SSHMSG_CAST(msg, tt_sshmsg_uar_t);

    /*
     byte SSH_MSG_USERAUTH_REQUEST
     string user name in ISO-10646 UTF-8 encoding [RFC3629]
     string service name in US-ASCII
     string method name in US-ASCII
     .... method specific fields
     */

    // string user name
    TT_DO(tt_ssh_string_parse(data, &addr, &len));
    if ((len != 0) && !TT_OK(tt_blob_create(&uar->user, addr, len))) {
        TT_ERROR("fail to create uar user");
        return TT_FAIL;
    }

    // string service name
    TT_DO(tt_ssh_string_parse(data, &addr, &len));
    sshserv = tt_ssh_serv_match(addr, len);
    if (sshserv == TT_SSH_SERVICE_NUM) {
        TT_ERROR("not supported ssh service");
        return TT_FAIL;
    }
    uar->service = sshserv;

    // string method name
    TT_DO(tt_ssh_string_parse(data, &addr, &len));
    sshauth = tt_ssh_auth_match(addr, len);
    if (sshauth == TT_SSH_AUTH_NUM) {
        TT_ERROR("not supported ssh auth");
        return TT_FAIL;
    }
    uar->auth = sshauth;

    if (uar->auth == TT_SSH_AUTH_PUBLICKEY) {
        tt_ssh_auth_pubkey_t *pubkey = &uar->auth_u.pubkey;
        tt_bool_t has_signature;
        tt_ssh_pubkey_alg_t pubkey_alg;

        TT_DO(tt_ssh_boolean_parse(data, &has_signature));

        // string public key algorithm name
        TT_DO(tt_ssh_string_parse(data, &addr, &len));
        pubkey_alg = tt_ssh_pubkey_alg_match(addr, len);
        if (pubkey_alg == TT_SSH_PUBKEY_ALG_NUM) {
            TT_ERROR("unsupported pubkey alg");
            return TT_FAIL;
        }

        // string public blob
        TT_DO(tt_ssh_string_parse(data, &addr, &len));
        if ((len == 0) || !TT_OK(tt_blob_create(&pubkey->pubkey, addr, len))) {
            TT_ERROR("fail to create ssh auth pub key");
            return TT_FAIL;
        }

        // string signaturenature
        if (has_signature) {
            TT_DO(tt_ssh_string_parse(data, &addr, &len));
            if ((len == 0) ||
                !TT_OK(tt_blob_create(&pubkey->signature, addr, len))) {
                TT_ERROR("fail to create ssh auth signaturenature");
                return TT_FAIL;
            }
        }
    } else if (uar->auth == TT_SSH_AUTH_PASSWORD) {
        tt_ssh_auth_pwd_t *pwd = &uar->auth_u.pwd;
        tt_bool_t has_newpwd;

        /*
         byte SSH_MSG_USERAUTH_REQUEST
         string user name
         string service name
         string "password"
         boolean FALSE
         string plaintext password in ISO-10646 UTF-8 encoding

         byte SSH_MSG_USERAUTH_REQUEST
         string user name
         string service name
         string "password"
         boolean TRUE
         string plaintext old password in ISO-10646 UTF-8 encoding
         string plaintext new password in ISO-10646 UTF-8 encoding
         */

        TT_DO(tt_ssh_boolean_parse(data, &has_newpwd));

        // string plaintext password
        TT_DO(tt_ssh_string_parse(data, &addr, &len));
        if ((len == 0) || !TT_OK(tt_blob_create(&pwd->pwd, addr, len))) {
            TT_ERROR("fail to create ssh auth password");
            return TT_FAIL;
        }

        // string signaturenature
        if (has_newpwd) {
            TT_DO(tt_ssh_string_parse(data, &addr, &len));
            if ((len == 0) ||
                !TT_OK(tt_blob_create(&pwd->new_pwd, addr, len))) {
                TT_ERROR("fail to create ssh auth new password");
                return TT_FAIL;
            }
        }
    }
#if 0
    else
    {
        TT_ERROR("not supported auth[%s]",
                 tt_g_ssh_auth_name[uar->auth]);
        return TT_FAIL;
    }
#endif

    return TT_SUCCESS;
}
