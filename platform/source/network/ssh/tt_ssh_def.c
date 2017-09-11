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

#include <network/ssh/tt_ssh_def.h>

#include <crypto/tt_aes.h>
#include <crypto/tt_hmac.h>

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

// ========================================
// algorithm name
// ========================================

const tt_char_t *tt_g_ssh_kex_alg_name[TT_SSH_KEX_ALG_NUM] = {
    "diffie-hellman-group1-sha1", // TT_SSH_KEYXCHG_DH_G1_SHA1
    "diffie-hellman-group14-sha1", // TT_SSH_KEYXCHG_DH_G14_SHA1

    "rsa1024-sha1", // TT_SSH_KEYXCHG_RSA1024_SHA1
    "rsa2048-sha256", // TT_SSH_KEYXCHG_RSA2048_SHA256
};

const tt_char_t *tt_g_ssh_pubkey_alg_name[TT_SSH_PUBKEY_ALG_NUM] = {
    "ssh-dss", // TT_SSH_PUBKEY_ALG_DSS
    "ssh-rsa", // TT_SSH_PUBKEY_ALG_RSA
};

const tt_char_t *tt_g_ssh_enc_alg_name[TT_SSH_ENC_ALG_NUM] = {
    "aes128-cbc", // TT_SSH_ENC_ALG_AES128_CBC,
    "aes256-cbc", // TT_SSH_ENC_ALG_AES256_CBC,
};

const tt_char_t *tt_g_ssh_mac_alg_name[TT_SSH_MAC_ALG_NUM] = {
    "hmac-sha1", // TT_SSH_MAC_ALG_HMAC_SHA1,
};

const tt_char_t *tt_g_ssh_cmprs_alg_name[TT_SSH_CMPRS_ALG_NUM] = {
    "none", // TT_SSH_CMPRS_ALG_NONE,
    "zlib", // TT_SSH_CMPRS_ALG_ZLIB,
};

// ========================================
// Service
// ========================================

const tt_char_t *tt_g_ssh_serv_name[TT_SSH_SERVICE_NUM] = {
    "ssh-userauth", "ssh-connection",
};

// ========================================
// Auth Method
// ========================================

const tt_char_t *tt_g_ssh_auth_name[TT_SSH_AUTH_NUM] = {"publickey",
                                                        "password",
                                                        "hostbased",
                                                        "none"};

// ========================================
// Channel
// ========================================

const tt_char_t *tt_g_ssh_chtype_name[TT_SSH_CHTYPE_NUM] = {
    "session", "x11", "forwarded-tcpip", "direct-tcpip",
};

const tt_char_t *tt_g_ssh_chreqtype_name[TT_SSH_CHREQTYPE_NUM] =
    {"pty-req",
     "x11-req",
     "env",
     "shell",
     "exec",
     "subsystem",
     "window-change",
     "xon-xoff",
     "signal",
     "exit-status",
     "exit-signal"};

// ========================================
// algorithm preference
// ========================================

tt_ssh_kex_alg_t tt_g_ssh_kex_pref[] = {
    TT_SSH_KEX_ALG_DH_G14_SHA1, TT_SSH_KEX_ALG_DH_G1_SHA1,
};
tt_u32_t tt_g_ssh_kex_pref_num =
    sizeof(tt_g_ssh_kex_pref) / sizeof(tt_g_ssh_kex_pref[0]);

tt_ssh_pubkey_alg_t tt_g_ssh_pubkey_pref[] = {
    TT_SSH_PUBKEY_ALG_RSA,
};
tt_u32_t tt_g_ssh_pubkey_pref_num =
    sizeof(tt_g_ssh_pubkey_pref) / sizeof(tt_g_ssh_pubkey_pref[0]);

tt_ssh_enc_alg_t tt_g_ssh_enc_c2s_pref[] = {TT_SSH_ENC_ALG_AES256_CBC,
                                            TT_SSH_ENC_ALG_AES128_CBC};
tt_u32_t tt_g_ssh_enc_c2s_pref_num =
    sizeof(tt_g_ssh_enc_c2s_pref) / sizeof(tt_g_ssh_enc_c2s_pref[0]);

tt_ssh_enc_alg_t tt_g_ssh_enc_s2c_pref[] = {TT_SSH_ENC_ALG_AES256_CBC,
                                            TT_SSH_ENC_ALG_AES128_CBC};
tt_u32_t tt_g_ssh_enc_s2c_pref_num =
    sizeof(tt_g_ssh_enc_s2c_pref) / sizeof(tt_g_ssh_enc_s2c_pref[0]);

tt_ssh_mac_alg_t tt_g_ssh_mac_c2s_pref[] = {
    TT_SSH_MAC_ALG_HMAC_SHA1,
};
tt_u32_t tt_g_ssh_mac_c2s_pref_num =
    sizeof(tt_g_ssh_mac_c2s_pref) / sizeof(tt_g_ssh_mac_c2s_pref[0]);

tt_ssh_mac_alg_t tt_g_ssh_mac_s2c_pref[] = {
    TT_SSH_MAC_ALG_HMAC_SHA1,
};
tt_u32_t tt_g_ssh_mac_s2c_pref_num =
    sizeof(tt_g_ssh_mac_s2c_pref) / sizeof(tt_g_ssh_mac_s2c_pref[0]);

tt_ssh_cmprs_alg_t tt_g_ssh_cmprs_c2s_pref[] = {
    TT_SSH_CMPRS_ALG_NONE,
};
tt_u32_t tt_g_ssh_cmprs_c2s_pref_num =
    sizeof(tt_g_ssh_cmprs_c2s_pref) / sizeof(tt_g_ssh_cmprs_c2s_pref[0]);

tt_ssh_cmprs_alg_t tt_g_ssh_cmprs_s2c_pref[] = {
    TT_SSH_CMPRS_ALG_NONE,
};
tt_u32_t tt_g_ssh_cmprs_s2c_pref_num =
    sizeof(tt_g_ssh_cmprs_s2c_pref) / sizeof(tt_g_ssh_cmprs_s2c_pref[0]);

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_ssh_pubkey_alg_t tt_ssh_pubkey_alg_match(IN tt_u8_t *name,
                                            IN tt_u32_t name_len)
{
    tt_ssh_pubkey_alg_t i;
    for (i = 0; i < TT_SSH_PUBKEY_ALG_NUM; ++i) {
        if (tt_strncmp(tt_g_ssh_pubkey_alg_name[i],
                       (tt_char_t *)name,
                       name_len) == 0) {
            break;
        }
    }
    return i;
}

tt_ssh_enc_alg_t tt_ssh_enc_alg_match(IN tt_u8_t *name, IN tt_u32_t name_len)
{
    tt_ssh_enc_alg_t i;
    for (i = 0; i < TT_SSH_ENC_ALG_NUM; ++i) {
        if (tt_strncmp(tt_g_ssh_enc_alg_name[i], (tt_char_t *)name, name_len) ==
            0) {
            break;
        }
    }
    return i;
}

tt_ssh_mac_alg_t tt_ssh_mac_alg_match(IN tt_u8_t *name, IN tt_u32_t name_len)
{
    tt_ssh_mac_alg_t i;
    for (i = 0; i < TT_SSH_MAC_ALG_NUM; ++i) {
        if (tt_strncmp(tt_g_ssh_mac_alg_name[i], (tt_char_t *)name, name_len) ==
            0) {
            break;
        }
    }
    return i;
}

tt_ssh_cmprs_alg_t tt_ssh_cmprs_alg_match(IN tt_u8_t *name,
                                          IN tt_u32_t name_len)
{
    tt_ssh_cmprs_alg_t i;
    for (i = 0; i < TT_SSH_CMPRS_ALG_NUM; ++i) {
        if (tt_strncmp(tt_g_ssh_cmprs_alg_name[i],
                       (tt_char_t *)name,
                       name_len) == 0) {
            break;
        }
    }
    return i;
}

tt_ssh_service_t tt_ssh_serv_match(IN tt_u8_t *name, IN tt_u32_t name_len)
{
    tt_ssh_service_t i;
    for (i = 0; i < TT_SSH_SERVICE_NUM; ++i) {
        if (tt_strncmp(tt_g_ssh_serv_name[i], (tt_char_t *)name, name_len) ==
            0) {
            break;
        }
    }
    return i;
}

tt_ssh_auth_t tt_ssh_auth_match(IN tt_u8_t *name, IN tt_u32_t name_len)
{
    tt_ssh_auth_t i;
    for (i = 0; i < TT_SSH_AUTH_NUM; ++i) {
        if (tt_strncmp(tt_g_ssh_auth_name[i], (tt_char_t *)name, name_len) ==
            0) {
            break;
        }
    }
    return i;
}

tt_ssh_chtype_t tt_ssh_chtype_match(IN tt_u8_t *name, IN tt_u32_t name_len)
{
    tt_ssh_chtype_t i;
    for (i = 0; i < TT_SSH_CHTYPE_NUM; ++i) {
        if (tt_strncmp(tt_g_ssh_chtype_name[i], (tt_char_t *)name, name_len) ==
            0) {
            break;
        }
    }
    return i;
}

tt_ssh_chreqtype_t tt_ssh_chreqtype_match(IN tt_u8_t *name,
                                          IN tt_u32_t name_len)
{
    tt_ssh_chreqtype_t i;
    for (i = 0; i < TT_SSH_CHREQTYPE_NUM; ++i) {
        if (tt_strncmp(tt_g_ssh_chreqtype_name[i],
                       (tt_char_t *)name,
                       name_len) == 0) {
            break;
        }
    }
    return i;
}

tt_u32_t tt_ssh_enc_iv_len(IN tt_ssh_enc_alg_t enc_alg)
{
    switch (enc_alg) {
        case TT_SSH_ENC_ALG_AES128_CBC:
            return TT_AES_IV_SIZE;
        case TT_SSH_ENC_ALG_AES256_CBC:
            return TT_AES_IV_SIZE;
        default:
            return 0;
    }
}

tt_u32_t tt_ssh_enc_key_len(IN tt_ssh_enc_alg_t enc_alg)
{
    switch (enc_alg) {
        case TT_SSH_ENC_ALG_AES128_CBC:
            return TT_AES128_KEY_SIZE;
        case TT_SSH_ENC_ALG_AES256_CBC:
            return TT_AES256_KEY_SIZE;
        default:
            return 0;
    }
}

tt_u32_t tt_ssh_mac_key_len(IN tt_ssh_mac_alg_t mac_alg)
{
    switch (mac_alg) {
        case TT_SSH_MAC_ALG_HMAC_SHA1:
            return TT_HMAC_SHA1_DIGEST_LENGTH;
        default:
            return 0;
    }
}
