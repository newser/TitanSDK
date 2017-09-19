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

#include <network/ssh/message/tt_ssh_msg_keyinit.h>

#include <network/ssh/message/tt_ssh_message.h>
#include <network/ssh/message/tt_ssh_parse.h>
#include <network/ssh/message/tt_ssh_render.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __MAX_ALG_NUM 10

#define __check_alg_num(an)                                                    \
    do {                                                                       \
        if (ms_keyinit->an == 0) {                                             \
            TT_ERROR("no " #an);                                               \
            return TT_FAIL;                                                    \
        }                                                                      \
        if (ms_keyinit->an > __MAX_ALG_NUM) {                                  \
            TT_ERROR(#an "[%d] > __MAX_ALG_NUM[%d]",                           \
                     ms_keyinit->an,                                           \
                     __MAX_ALG_NUM);                                           \
            return TT_FAIL;                                                    \
        }                                                                      \
    } while (0)

#define __fill_alg_name(name, id, id_num, name_table)                          \
    do {                                                                       \
        tt_u32_t i;                                                            \
        for (i = 0; i < id_num; ++i) {                                         \
            name[i] = name_table[id[i]];                                       \
        }                                                                      \
    } while (0)

#define __find_alg_idx(name, name_len, table, table_num, i)                    \
    do {                                                                       \
        for (i = 0; i < table_num; ++i) {                                      \
            if (tt_strncmp(name, table[i], name_len) == 0) {                   \
                break;                                                         \
            }                                                                  \
        }                                                                      \
    } while (0)

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_result_t __keyinit_create(IN struct tt_sshmsg_s *msg);

static tt_result_t __keyinit_render_prepare(IN struct tt_sshmsg_s *msg,
                                            OUT tt_u32_t *len,
                                            OUT tt_ssh_render_mode_t *mode);
static tt_result_t __keyinit_render(IN struct tt_sshmsg_s *msg,
                                    IN OUT tt_buf_t *buf);

static tt_result_t __keyinit_parse(IN struct tt_sshmsg_s *msg,
                                   IN tt_buf_t *data);

static tt_sshmsg_itf_t __keyinit_op = {
    __keyinit_create,
    NULL,
    NULL,

    __keyinit_render_prepare,
    __keyinit_render,

    __keyinit_parse,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __keyinit_addalg_kex(IN tt_sshms_keyinit_t *msg,
                                        IN tt_ssh_kex_alg_t kex);
static tt_result_t __keyinit_addalg_pubkey(IN tt_sshms_keyinit_t *msg,
                                           IN tt_ssh_pubkey_alg_t pubkey);
static tt_result_t __keyinit_addalg_enc_c2s(IN tt_sshms_keyinit_t *msg,
                                            IN tt_ssh_enc_alg_t enc);
static tt_result_t __keyinit_addalg_enc_s2c(IN tt_sshms_keyinit_t *msg,
                                            IN tt_ssh_enc_alg_t enc);
static tt_result_t __keyinit_addalg_mac_c2s(IN tt_sshms_keyinit_t *msg,
                                            IN tt_ssh_mac_alg_t mac);
static tt_result_t __keyinit_addalg_mac_s2c(IN tt_sshms_keyinit_t *msg,
                                            IN tt_ssh_mac_alg_t mac);
static tt_result_t __keyinit_addalg_comp_c2s(IN tt_sshms_keyinit_t *msg,
                                             IN tt_ssh_cmprs_alg_t mac);
static tt_result_t __keyinit_addalg_comp_s2c(IN tt_sshms_keyinit_t *msg,
                                             IN tt_ssh_cmprs_alg_t mac);

static tt_result_t __keyinit_kex_parse(IN tt_char_t *name,
                                       IN tt_u32_t name_len,
                                       IN void *param);
static tt_result_t __keyinit_pubkey_parse(IN tt_char_t *name,
                                          IN tt_u32_t name_len,
                                          IN void *param);
static tt_result_t __keyinit_enc_c2s_parse(IN tt_char_t *name,
                                           IN tt_u32_t name_len,
                                           IN void *param);
static tt_result_t __keyinit_enc_s2c_parse(IN tt_char_t *name,
                                           IN tt_u32_t name_len,
                                           IN void *param);
static tt_result_t __keyinit_mac_c2s_parse(IN tt_char_t *name,
                                           IN tt_u32_t name_len,
                                           IN void *param);
static tt_result_t __keyinit_mac_s2c_parse(IN tt_char_t *name,
                                           IN tt_u32_t name_len,
                                           IN void *param);
static tt_result_t __keyinit_comp_c2s_parse(IN tt_char_t *name,
                                            IN tt_u32_t name_len,
                                            IN void *param);
static tt_result_t __keyinit_comp_s2c_parse(IN tt_char_t *name,
                                            IN tt_u32_t name_len,
                                            IN void *param);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshmsg_t *tt_sshms_keyinit_create()
{
    return tt_sshmsg_create(TT_SSH_MSGID_KEXINIT,
                            sizeof(tt_sshms_keyinit_t),
                            &__keyinit_op);
}

tt_result_t tt_sshms_keyinit_addalg_kex(IN tt_sshmsg_t *msg,
                                        IN tt_ssh_kex_alg_t kex)
{
    tt_sshms_keyinit_t *ms_keyinit;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_KEXINIT);

    ms_keyinit = TT_SSHMSG_CAST(msg, tt_sshms_keyinit_t);

    return __keyinit_addalg_kex(ms_keyinit, kex);
}

tt_result_t tt_sshms_keyinit_addalg_pubkey(IN tt_sshmsg_t *msg,
                                           IN tt_ssh_pubkey_alg_t pubkey)

{
    tt_sshms_keyinit_t *ms_keyinit;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_KEXINIT);

    ms_keyinit = TT_SSHMSG_CAST(msg, tt_sshms_keyinit_t);

    return __keyinit_addalg_pubkey(ms_keyinit, pubkey);
}

tt_result_t tt_sshms_keyinit_addalg_enc(IN tt_sshmsg_t *msg,
                                        IN tt_ssh_enc_alg_t enc,
                                        IN tt_bool_t client2server)
{
    tt_sshms_keyinit_t *ms_keyinit;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_KEXINIT);

    ms_keyinit = TT_SSHMSG_CAST(msg, tt_sshms_keyinit_t);

    if (client2server) {
        return __keyinit_addalg_enc_c2s(ms_keyinit, enc);
    } else {
        return __keyinit_addalg_enc_s2c(ms_keyinit, enc);
    }
}

tt_result_t tt_sshms_keyinit_addalg_mac(IN tt_sshmsg_t *msg,
                                        IN tt_ssh_mac_alg_t mac,
                                        IN tt_bool_t client2server)
{
    tt_sshms_keyinit_t *ms_keyinit;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_KEXINIT);

    ms_keyinit = TT_SSHMSG_CAST(msg, tt_sshms_keyinit_t);

    if (client2server) {
        return __keyinit_addalg_mac_c2s(ms_keyinit, mac);
    } else {
        return __keyinit_addalg_mac_s2c(ms_keyinit, mac);
    }
}

tt_result_t tt_sshms_keyinit_addalg_comp(IN tt_sshmsg_t *msg,
                                         IN tt_ssh_cmprs_alg_t comp,
                                         IN tt_bool_t client2server)
{
    tt_sshms_keyinit_t *ms_keyinit;

    TT_ASSERT(msg != NULL);
    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_KEXINIT);

    ms_keyinit = TT_SSHMSG_CAST(msg, tt_sshms_keyinit_t);

    if (client2server) {
        return __keyinit_addalg_comp_c2s(ms_keyinit, comp);
    } else {
        return __keyinit_addalg_comp_s2c(ms_keyinit, comp);
    }
}

tt_result_t __keyinit_create(IN struct tt_sshmsg_s *msg)
{
    // need record is/ic
    msg->save = TT_TRUE;

    return TT_SUCCESS;
}

tt_result_t __keyinit_render_prepare(IN struct tt_sshmsg_s *msg,
                                     OUT tt_u32_t *len,
                                     OUT tt_ssh_render_mode_t *mode)
{
    tt_sshms_keyinit_t *ms_keyinit;
    tt_u32_t msg_len;
    const tt_char_t *alg_name[__MAX_ALG_NUM];
    tt_u32_t alg_name_num;

    /*
     uint32 packet_length
     byte padding_length
     byte[n1] payload; n1 = packet_length - padding_length - 1
     byte[n2] random padding; n2 = padding_length
     byte[m] mac (Message Authentication Code - MAC); m = mac_length
     */

    /*
     byte SSH_MSG_KEXINIT
     byte[16] cookie (random bytes)
     name-list kex_algorithms
     name-list server_host_key_algorithms
     name-list encryption_algorithms_client_to_server
     name-list encryption_algorithms_server_to_client
     name-list mac_algorithms_client_to_server
     name-list mac_algorithms_server_to_client
     name-list compression_algorithms_client_to_server
     name-list compression_algorithms_server_to_client
     name-list languages_client_to_server
     name-list languages_server_to_client
     boolean first_kex_packet_follows
     uint32 0 (reserved for future extension)
     */

    ms_keyinit = TT_SSHMSG_CAST(msg, tt_sshms_keyinit_t);

    __check_alg_num(kex_alg_num);
    __check_alg_num(pubkey_alg_num);
    __check_alg_num(enc_c2s_num);
    __check_alg_num(enc_s2c_num);
    __check_alg_num(mac_c2s_num);
    __check_alg_num(mac_s2c_num);
    __check_alg_num(comp_c2s_num);
    __check_alg_num(comp_s2c_num);

    // byte & byte[16]
    msg_len = 17;

    // key ex
    alg_name_num = ms_keyinit->kex_alg_num;
    __fill_alg_name(alg_name,
                    ms_keyinit->kex_alg,
                    alg_name_num,
                    tt_g_ssh_kex_alg_name);
    msg_len += tt_ssh_namelist_render_prepare(alg_name, alg_name_num);

    // pubkey
    alg_name_num = ms_keyinit->pubkey_alg_num;
    __fill_alg_name(alg_name,
                    ms_keyinit->pubkey_alg,
                    ms_keyinit->pubkey_alg_num,
                    tt_g_ssh_pubkey_alg_name);
    msg_len += tt_ssh_namelist_render_prepare(alg_name, alg_name_num);

    // enc c2s
    alg_name_num = ms_keyinit->enc_c2s_num;
    __fill_alg_name(alg_name,
                    ms_keyinit->enc_c2s,
                    ms_keyinit->enc_c2s_num,
                    tt_g_ssh_enc_alg_name);
    msg_len += tt_ssh_namelist_render_prepare(alg_name, alg_name_num);

    // enc s2c
    alg_name_num = ms_keyinit->enc_s2c_num;
    __fill_alg_name(alg_name,
                    ms_keyinit->enc_s2c,
                    ms_keyinit->enc_s2c_num,
                    tt_g_ssh_enc_alg_name);
    msg_len += tt_ssh_namelist_render_prepare(alg_name, alg_name_num);

    // mac c2s
    alg_name_num = ms_keyinit->mac_c2s_num;
    __fill_alg_name(alg_name,
                    ms_keyinit->mac_c2s,
                    ms_keyinit->mac_c2s_num,
                    tt_g_ssh_mac_alg_name);
    msg_len += tt_ssh_namelist_render_prepare(alg_name, alg_name_num);

    // mac s2c
    alg_name_num = ms_keyinit->mac_s2c_num;
    __fill_alg_name(alg_name,
                    ms_keyinit->mac_s2c,
                    ms_keyinit->mac_s2c_num,
                    tt_g_ssh_mac_alg_name);
    msg_len += tt_ssh_namelist_render_prepare(alg_name, alg_name_num);

    // comp c2s
    alg_name_num = ms_keyinit->comp_c2s_num;
    __fill_alg_name(alg_name,
                    ms_keyinit->comp_c2s,
                    ms_keyinit->comp_c2s_num,
                    tt_g_ssh_cmprs_alg_name);
    msg_len += tt_ssh_namelist_render_prepare(alg_name, alg_name_num);

    // mac s2c
    alg_name_num = ms_keyinit->comp_s2c_num;
    __fill_alg_name(alg_name,
                    ms_keyinit->comp_s2c,
                    ms_keyinit->comp_s2c_num,
                    tt_g_ssh_cmprs_alg_name);
    msg_len += tt_ssh_namelist_render_prepare(alg_name, alg_name_num);

    // language c2s
    msg_len += tt_ssh_namelist_render_prepare(NULL, 0);

    // language s2c
    msg_len += tt_ssh_namelist_render_prepare(NULL, 0);

#if 0
    // byte
    msg_len += tt_ssh_byte_render_prepare();

    // uint32
    msg_len += tt_ssh_uint32_render_prepare();
#else
    msg_len += 5;
#endif

    *len = msg_len;
    *mode = TT_SSH_RENDER_MODE_BINARY;
    return TT_SUCCESS;
}

tt_result_t __keyinit_render(IN struct tt_sshmsg_s *msg, IN OUT tt_buf_t *buf)
{
    tt_sshms_keyinit_t *ms_keyinit;
    const tt_char_t *alg_name[__MAX_ALG_NUM];
    tt_u32_t alg_name_num;

    ms_keyinit = TT_SSHMSG_CAST(msg, tt_sshms_keyinit_t);

    // byte SSH_MSG_KEXINIT
    TT_DO(tt_buf_put_u8(buf, TT_SSH_MSGID_KEXINIT));

    // byte[16] cookie (random bytes)
    TT_DO(tt_buf_put_rand(buf, 16));

    // key ex
    alg_name_num = ms_keyinit->kex_alg_num;
    __fill_alg_name(alg_name,
                    ms_keyinit->kex_alg,
                    alg_name_num,
                    tt_g_ssh_kex_alg_name);
    TT_DO(tt_ssh_namelist_render(buf, alg_name, alg_name_num));

    // pubkey
    alg_name_num = ms_keyinit->pubkey_alg_num;
    __fill_alg_name(alg_name,
                    ms_keyinit->pubkey_alg,
                    ms_keyinit->pubkey_alg_num,
                    tt_g_ssh_pubkey_alg_name);
    TT_DO(tt_ssh_namelist_render(buf, alg_name, alg_name_num));

    // enc c2s
    alg_name_num = ms_keyinit->enc_c2s_num;
    __fill_alg_name(alg_name,
                    ms_keyinit->enc_c2s,
                    ms_keyinit->enc_c2s_num,
                    tt_g_ssh_enc_alg_name);
    TT_DO(tt_ssh_namelist_render(buf, alg_name, alg_name_num));

    // enc s2c
    alg_name_num = ms_keyinit->enc_s2c_num;
    __fill_alg_name(alg_name,
                    ms_keyinit->enc_s2c,
                    ms_keyinit->enc_s2c_num,
                    tt_g_ssh_enc_alg_name);
    TT_DO(tt_ssh_namelist_render(buf, alg_name, alg_name_num));

    // mac c2s
    alg_name_num = ms_keyinit->mac_c2s_num;
    __fill_alg_name(alg_name,
                    ms_keyinit->mac_c2s,
                    ms_keyinit->mac_c2s_num,
                    tt_g_ssh_mac_alg_name);
    TT_DO(tt_ssh_namelist_render(buf, alg_name, alg_name_num));

    // mac s2c
    alg_name_num = ms_keyinit->mac_s2c_num;
    __fill_alg_name(alg_name,
                    ms_keyinit->mac_s2c,
                    ms_keyinit->mac_s2c_num,
                    tt_g_ssh_mac_alg_name);
    TT_DO(tt_ssh_namelist_render(buf, alg_name, alg_name_num));

    // comp c2s
    alg_name_num = ms_keyinit->comp_c2s_num;
    __fill_alg_name(alg_name,
                    ms_keyinit->comp_c2s,
                    ms_keyinit->comp_c2s_num,
                    tt_g_ssh_cmprs_alg_name);
    TT_DO(tt_ssh_namelist_render(buf, alg_name, alg_name_num));

    // mac s2c
    alg_name_num = ms_keyinit->comp_s2c_num;
    __fill_alg_name(alg_name,
                    ms_keyinit->comp_s2c,
                    ms_keyinit->comp_s2c_num,
                    tt_g_ssh_cmprs_alg_name);
    TT_DO(tt_ssh_namelist_render(buf, alg_name, alg_name_num));

    // language c2s
    TT_DO(tt_ssh_namelist_render(buf, NULL, 0));

    // language s2c
    TT_DO(tt_ssh_namelist_render(buf, NULL, 0));

    // byte
    TT_DO(tt_ssh_byte_render(buf, 0));

    // uint32
    TT_DO(tt_ssh_uint32_render(buf, 0));

    return TT_SUCCESS;
}

tt_result_t __keyinit_parse(IN struct tt_sshmsg_s *msg, IN tt_buf_t *data)
{
    tt_sshms_keyinit_t *ms_keyinit;

    TT_ASSERT(msg->msg_id == TT_SSH_MSGID_KEXINIT);
    ms_keyinit = TT_SSHMSG_CAST(msg, tt_sshms_keyinit_t);

    /*
     byte SSH_MSG_KEXINIT
     byte[16] cookie (random bytes)
     name-list kex_algorithms
     name-list server_host_key_algorithms
     name-list encryption_algorithms_client_to_server
     name-list encryption_algorithms_server_to_client
     name-list mac_algorithms_client_to_server
     name-list mac_algorithms_server_to_client
     name-list compression_algorithms_client_to_server
     name-list compression_algorithms_server_to_client
     name-list languages_client_to_server
     name-list languages_server_to_client
     boolean first_kex_packet_follows
     uint32 0 (reserved for future extension)
     */

    // note the data does not include beginning byte

    // byte[16]
    TT_DO(tt_buf_inc_rp(data, 16));

    // kex
    TT_DO(tt_ssh_namelist_parse(data, __keyinit_kex_parse, ms_keyinit));

    // pubkey
    TT_DO(tt_ssh_namelist_parse(data, __keyinit_pubkey_parse, ms_keyinit));

    // enc c2s
    TT_DO(tt_ssh_namelist_parse(data, __keyinit_enc_c2s_parse, ms_keyinit));

    // enc s2c
    TT_DO(tt_ssh_namelist_parse(data, __keyinit_enc_s2c_parse, ms_keyinit));

    // mac c2s
    TT_DO(tt_ssh_namelist_parse(data, __keyinit_mac_c2s_parse, ms_keyinit));

    // mac s2c
    TT_DO(tt_ssh_namelist_parse(data, __keyinit_mac_s2c_parse, ms_keyinit));

    // comp c2s
    TT_DO(tt_ssh_namelist_parse(data, __keyinit_comp_c2s_parse, ms_keyinit));

    // comp s2c
    TT_DO(tt_ssh_namelist_parse(data, __keyinit_comp_s2c_parse, ms_keyinit));

    return TT_SUCCESS;
}

tt_result_t __keyinit_addalg_kex(IN tt_sshms_keyinit_t *msg,
                                 IN tt_ssh_kex_alg_t kex)
{
    tt_u32_t i;
    for (i = 0; i < msg->kex_alg_num; ++i) {
        if (kex == msg->kex_alg[i]) {
            return TT_SUCCESS;
        }
    }

    if (msg->kex_alg_num < TT_SSH_KEX_ALG_NUM) {
        msg->kex_alg[msg->kex_alg_num] = kex;
        ++msg->kex_alg_num;
        return TT_SUCCESS;
    } else {
        TT_ERROR("kex alg full");
        return TT_FAIL;
    }
}

tt_result_t __keyinit_addalg_pubkey(IN tt_sshms_keyinit_t *msg,
                                    IN tt_ssh_pubkey_alg_t pubkey)
{
    tt_u32_t i;
    for (i = 0; i < msg->pubkey_alg_num; ++i) {
        if (pubkey == msg->pubkey_alg[i]) {
            return TT_SUCCESS;
        }
    }

    if (msg->pubkey_alg_num < TT_SSH_PUBKEY_ALG_NUM) {
        msg->pubkey_alg[msg->pubkey_alg_num] = pubkey;
        ++msg->pubkey_alg_num;
        return TT_SUCCESS;
    } else {
        TT_ERROR("pubkey alg full");
        return TT_FAIL;
    }
}

tt_result_t __keyinit_addalg_enc_c2s(IN tt_sshms_keyinit_t *msg,
                                     IN tt_ssh_enc_alg_t enc)
{
    tt_u32_t i;
    for (i = 0; i < msg->enc_c2s_num; ++i) {
        if (enc == msg->enc_c2s[i]) {
            return TT_SUCCESS;
        }
    }

    if (msg->enc_c2s_num < TT_SSH_ENC_ALG_NUM) {
        msg->enc_c2s[msg->enc_c2s_num] = enc;
        ++msg->enc_c2s_num;
        return TT_SUCCESS;
    } else {
        TT_ERROR("enc c2s alg full");
        return TT_FAIL;
    }
}

tt_result_t __keyinit_addalg_enc_s2c(IN tt_sshms_keyinit_t *msg,
                                     IN tt_ssh_enc_alg_t enc)
{
    tt_u32_t i;
    for (i = 0; i < msg->enc_s2c_num; ++i) {
        if (enc == msg->enc_s2c[i]) {
            return TT_SUCCESS;
        }
    }

    if (msg->enc_s2c_num < TT_SSH_ENC_ALG_NUM) {
        msg->enc_s2c[msg->enc_s2c_num] = enc;
        ++msg->enc_s2c_num;
        return TT_SUCCESS;
    } else {
        TT_ERROR("enc s2c alg full");
        return TT_FAIL;
    }
}

tt_result_t __keyinit_addalg_mac_c2s(IN tt_sshms_keyinit_t *msg,
                                     IN tt_ssh_mac_alg_t mac)
{
    tt_u32_t i;
    for (i = 0; i < msg->mac_c2s_num; ++i) {
        if (mac == msg->mac_c2s[i]) {
            return TT_SUCCESS;
        }
    }

    if (msg->mac_c2s_num < TT_SSH_MAC_ALG_NUM) {
        msg->mac_c2s[msg->mac_c2s_num] = mac;
        ++msg->mac_c2s_num;
        return TT_SUCCESS;
    } else {
        TT_ERROR("mac c2s alg full");
        return TT_FAIL;
    }
}

tt_result_t __keyinit_addalg_mac_s2c(IN tt_sshms_keyinit_t *msg,
                                     IN tt_ssh_mac_alg_t mac)
{
    tt_u32_t i;
    for (i = 0; i < msg->mac_s2c_num; ++i) {
        if (mac == msg->mac_s2c[i]) {
            return TT_SUCCESS;
        }
    }

    if (msg->mac_s2c_num < TT_SSH_MAC_ALG_NUM) {
        msg->mac_s2c[msg->mac_s2c_num] = mac;
        ++msg->mac_s2c_num;
        return TT_SUCCESS;
    } else {
        TT_ERROR("mac s2c alg full");
        return TT_FAIL;
    }
}

tt_result_t __keyinit_addalg_comp_c2s(IN tt_sshms_keyinit_t *msg,
                                      IN tt_ssh_cmprs_alg_t comp)
{
    tt_u32_t i;
    for (i = 0; i < msg->comp_c2s_num; ++i) {
        if (comp == msg->comp_c2s[i]) {
            return TT_SUCCESS;
        }
    }

    if (msg->comp_c2s_num < TT_SSH_CMPRS_ALG_NUM) {
        msg->comp_c2s[msg->comp_c2s_num] = comp;
        ++msg->comp_c2s_num;
        return TT_SUCCESS;
    } else {
        TT_ERROR("comp c2s alg full");
        return TT_FAIL;
    }
}

tt_result_t __keyinit_addalg_comp_s2c(IN tt_sshms_keyinit_t *msg,
                                      IN tt_ssh_cmprs_alg_t comp)
{
    tt_u32_t i;
    for (i = 0; i < msg->comp_s2c_num; ++i) {
        if (comp == msg->comp_s2c[i]) {
            return TT_SUCCESS;
        }
    }

    if (msg->comp_s2c_num < TT_SSH_CMPRS_ALG_NUM) {
        msg->comp_s2c[msg->comp_s2c_num] = comp;
        ++msg->comp_s2c_num;
        return TT_SUCCESS;
    } else {
        TT_ERROR("comp s2c alg full");
        return TT_FAIL;
    }
}

tt_result_t __keyinit_kex_parse(IN tt_char_t *name,
                                IN tt_u32_t name_len,
                                IN void *param)
{
    tt_u32_t i;

    __find_alg_idx(name,
                   name_len,
                   tt_g_ssh_kex_alg_name,
                   TT_SSH_KEX_ALG_NUM,
                   i);
    if (i < TT_SSH_KEX_ALG_NUM) {
        return __keyinit_addalg_kex((tt_sshms_keyinit_t *)param,
                                    (tt_ssh_kex_alg_t)i);
    } else {
        // ignore it
        return TT_SUCCESS;
    }
}

tt_result_t __keyinit_pubkey_parse(IN tt_char_t *name,
                                   IN tt_u32_t name_len,
                                   IN void *param)
{
    tt_u32_t i;

    __find_alg_idx(name,
                   name_len,
                   tt_g_ssh_pubkey_alg_name,
                   TT_SSH_PUBKEY_ALG_NUM,
                   i);
    if (i < TT_SSH_PUBKEY_ALG_NUM) {
        return __keyinit_addalg_pubkey((tt_sshms_keyinit_t *)param,
                                       (tt_ssh_pubkey_alg_t)i);
    } else {
        // ignore it
        return TT_SUCCESS;
    }
}

tt_result_t __keyinit_enc_c2s_parse(IN tt_char_t *name,
                                    IN tt_u32_t name_len,
                                    IN void *param)
{
    tt_u32_t i;

    __find_alg_idx(name,
                   name_len,
                   tt_g_ssh_enc_alg_name,
                   TT_SSH_ENC_ALG_NUM,
                   i);
    if (i < TT_SSH_ENC_ALG_NUM) {
        return __keyinit_addalg_enc_c2s((tt_sshms_keyinit_t *)param,
                                        (tt_ssh_enc_alg_t)i);
    } else {
        // ignore it
        return TT_SUCCESS;
    }
}

tt_result_t __keyinit_enc_s2c_parse(IN tt_char_t *name,
                                    IN tt_u32_t name_len,
                                    IN void *param)
{
    tt_u32_t i;

    __find_alg_idx(name,
                   name_len,
                   tt_g_ssh_enc_alg_name,
                   TT_SSH_ENC_ALG_NUM,
                   i);
    if (i < TT_SSH_ENC_ALG_NUM) {
        return __keyinit_addalg_enc_s2c((tt_sshms_keyinit_t *)param,
                                        (tt_ssh_enc_alg_t)i);
    } else {
        // ignore it
        return TT_SUCCESS;
    }
}

tt_result_t __keyinit_mac_c2s_parse(IN tt_char_t *name,
                                    IN tt_u32_t name_len,
                                    IN void *param)
{
    tt_u32_t i;

    __find_alg_idx(name,
                   name_len,
                   tt_g_ssh_mac_alg_name,
                   TT_SSH_MAC_ALG_NUM,
                   i);
    if (i < TT_SSH_MAC_ALG_NUM) {
        return __keyinit_addalg_mac_c2s((tt_sshms_keyinit_t *)param,
                                        (tt_ssh_mac_alg_t)i);
    } else {
        // ignore it
        return TT_SUCCESS;
    }
}

tt_result_t __keyinit_mac_s2c_parse(IN tt_char_t *name,
                                    IN tt_u32_t name_len,
                                    IN void *param)
{
    tt_u32_t i;

    __find_alg_idx(name,
                   name_len,
                   tt_g_ssh_mac_alg_name,
                   TT_SSH_MAC_ALG_NUM,
                   i);
    if (i < TT_SSH_MAC_ALG_NUM) {
        return __keyinit_addalg_mac_s2c((tt_sshms_keyinit_t *)param,
                                        (tt_ssh_mac_alg_t)i);
    } else {
        // ignore it
        return TT_SUCCESS;
    }
}

tt_result_t __keyinit_comp_c2s_parse(IN tt_char_t *name,
                                     IN tt_u32_t name_len,
                                     IN void *param)
{
    tt_u32_t i;

    __find_alg_idx(name,
                   name_len,
                   tt_g_ssh_cmprs_alg_name,
                   TT_SSH_CMPRS_ALG_NUM,
                   i);
    if (i < TT_SSH_CMPRS_ALG_NUM) {
        return __keyinit_addalg_comp_c2s((tt_sshms_keyinit_t *)param,
                                         (tt_ssh_cmprs_alg_t)i);
    } else {
        // ignore it
        return TT_SUCCESS;
    }
}

tt_result_t __keyinit_comp_s2c_parse(IN tt_char_t *name,
                                     IN tt_u32_t name_len,
                                     IN void *param)
{
    tt_u32_t i;

    __find_alg_idx(name,
                   name_len,
                   tt_g_ssh_cmprs_alg_name,
                   TT_SSH_CMPRS_ALG_NUM,
                   i);
    if (i < TT_SSH_CMPRS_ALG_NUM) {
        return __keyinit_addalg_comp_s2c((tt_sshms_keyinit_t *)param,
                                         (tt_ssh_cmprs_alg_t)i);
    } else {
        // ignore it
        return TT_SUCCESS;
    }
}
