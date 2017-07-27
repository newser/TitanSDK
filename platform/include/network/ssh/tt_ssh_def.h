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
@file tt_ssh_def.h
@brief ssh definitions
*/

#ifndef __TT_SSH_DEF__
#define __TT_SSH_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_ASSERT_SSH TT_ASSERT

// Message Numbers
// SSH-TRANS
#define TT_SSH_MSGID_DISCONNECT 1
#define TT_SSH_MSGID_IGNORE 2
#define TT_SSH_MSGID_UNIMPLEMENTED 3
#define TT_SSH_MSGID_DEBUG 4
#define TT_SSH_MSGID_SERVICE_REQUEST 5
#define TT_SSH_MSGID_SERVICE_ACCEPT 6
#define TT_SSH_MSGID_KEXINIT 20
#define TT_SSH_MSGID_NEWKEYS 21
#define TT_SSH_MSGID_KEXDH_INIT 30
#define TT_SSH_MSGID_KEXDH_REPLY 31
#define TT_SSH_MSGID_KEXRSA_PUBKEY 30
#define TT_SSH_MSGID_KEXRSA_SECRET 31
#define TT_SSH_MSGID_KEXRSA_DONE 32
// SSH-USERAUTH
#define TT_SSH_MSGID_USERAUTH_REQUEST 50
#define TT_SSH_MSGID_USERAUTH_FAILURE 51
#define TT_SSH_MSGID_USERAUTH_SUCCESS 52
#define TT_SSH_MSGID_USERAUTH_BANNER 53
#define TT_SSH_MSGID_USERAUTH_PK_OK 60
#define TT_SSH_MSGID_USERAUTH_PASSWD_CHANGEREQ 60
// SSH-CONNECT
#define TT_SSH_MSGID_GLOBAL_REQUEST 80
#define TT_SSH_MSGID_REQUEST_SUCCESS 81
#define TT_SSH_MSGID_REQUEST_FAILURE 82
#define TT_SSH_MSGID_CHANNEL_OPEN 90
#define TT_SSH_MSGID_CHANNEL_OPEN_CONFIRMATION 91
#define TT_SSH_MSGID_CHANNEL_OPEN_FAILURE 92
#define TT_SSH_MSGID_CHANNEL_WINDOW_ADJUST 93
#define TT_SSH_MSGID_CHANNEL_DATA 94
#define TT_SSH_MSGID_CHANNEL_EXTENDED_DATA 95
#define TT_SSH_MSGID_CHANNEL_EOF 96
#define TT_SSH_MSGID_CHANNEL_CLOSE 97
#define TT_SSH_MSGID_CHANNEL_REQUEST 98
#define TT_SSH_MSGID_CHANNEL_SUCCESS 99
#define TT_SSH_MSGID_CHANNEL_FAILURE 100
// The IANA will not control the message numbers in the range
// of 192 through 255
#define TT_SSH_MSGID_VERXCHG 192

// Disconnection Messages Reason Codes and Descriptions
#define TT_SSH_DMRC_HOST_NOT_ALLOWED_TO_CONNECT 1
#define TT_SSH_DMRC_PROTOCOL_ERROR 2
#define TT_SSH_DMRC_KEY_EXCHANGE_FAILED 3
#define TT_SSH_DMRC_RESERVED 4
#define TT_SSH_DMRC_MAC_ERROR 5
#define TT_SSH_DMRC_COMPRESSION_ERROR 6
#define TT_SSH_DMRC_SERVICE_NOT_AVAILABLE 7
#define TT_SSH_DMRC_PROTOCOL_VERSION_NOT_SUPPORTED 8
#define TT_SSH_DMRC_HOST_KEY_NOT_VERIFIABLE 9
#define TT_SSH_DMRC_CONNECTION_LOST 10
#define TT_SSH_DMRC_BY_APPLICATION 11
#define TT_SSH_DMRC_TOO_MANY_CONNECTIONS 12
#define TT_SSH_DMRC_AUTH_CANCELLED_BY_USER 13
#define TT_SSH_DMRC_NO_MORE_AUTH_METHODS_AVAILABLE 14
#define TT_SSH_DMRC_ILLEGAL_USER_NAME 15

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_SSH_VER_1_0,
    TT_SSH_VER_1_99,
    TT_SSH_VER_2_0,

    TT_SSH_VER_NUM
} tt_ssh_ver_t;
#define TT_SSH_VER_VALID(v) ((v) < TT_SSH_VER_NUM)

// ========================================
// Key Exchange Method
// ========================================

typedef enum {
    TT_SSH_KEX_ALG_DH_G1_SHA1,
    TT_SSH_KEX_ALG_DH_G14_SHA1,

    TT_SSH_KEX_ALG_RSA1024_SHA1,
    TT_SSH_KEX_ALG_RSA2048_SHA256,

    TT_SSH_KEX_ALG_NUM
} tt_ssh_kex_alg_t;
#define TT_SSH_KEX_ALG_DH(a)                                                   \
    (((a) >= TT_SSH_KEX_ALG_DH_G1_SHA1) && ((a) <= TT_SSH_KEX_ALG_DH_G14_SHA1))
#define TT_SSH_KEX_ALG_RSA(a)                                                  \
    (((a) >= TT_SSH_KEX_ALG_RSA1024_SHA1) &&                                   \
     ((a) <= TT_SSH_KEX_ALG_RSA2048_SHA256))

tt_export const tt_char_t *tt_g_ssh_kex_alg_name[TT_SSH_KEX_ALG_NUM];

// HASH
typedef enum {
    TT_SSH_HASH_ALG_SHA1,
    TT_SSH_HASH_ALG_SHA256,

    TT_SSH_HASH_ALG_NUM
} tt_ssh_hash_alg_t;
#define TT_SSH_HASH_ALG_VALID(a) ((a) < TT_SSH_HASH_ALG_NUM)

// ========================================
// Public Key Algorithms
// ========================================

typedef enum {
    TT_SSH_PUBKEY_ALG_DSS,
    TT_SSH_PUBKEY_ALG_RSA,

    TT_SSH_PUBKEY_ALG_NUM
} tt_ssh_pubkey_alg_t;
#define TT_SSH_PUBKEY_ALG_VALID(a) ((a) < TT_SSH_PUBKEY_ALG_NUM)

tt_export const tt_char_t *tt_g_ssh_pubkey_alg_name[TT_SSH_PUBKEY_ALG_NUM];

tt_export tt_ssh_pubkey_alg_t tt_ssh_pubkey_alg_match(IN tt_u8_t *name,
                                                      IN tt_u32_t name_len);

// ========================================
// Encryption
// ========================================

typedef enum {
    TT_SSH_ENC_ALG_AES128_CBC,
    TT_SSH_ENC_ALG_AES256_CBC,

    TT_SSH_ENC_ALG_NUM
} tt_ssh_enc_alg_t;
#define TT_SSH_ENC_ALG_VALID(a) ((a) < TT_SSH_ENC_ALG_NUM)

tt_export const tt_char_t *tt_g_ssh_enc_alg_name[TT_SSH_ENC_ALG_NUM];

tt_export tt_ssh_enc_alg_t tt_ssh_enc_alg_match(IN tt_u8_t *name,
                                                IN tt_u32_t name_len);

// ========================================
// Data Integrity
// ========================================

typedef enum {
    TT_SSH_MAC_ALG_HMAC_SHA1,

    TT_SSH_MAC_ALG_NUM
} tt_ssh_mac_alg_t;
#define TT_SSH_MAC_ALG_VALID(a) ((a) < TT_SSH_MAC_ALG_NUM)

tt_export const tt_char_t *tt_g_ssh_mac_alg_name[TT_SSH_MAC_ALG_NUM];

tt_export tt_ssh_mac_alg_t tt_ssh_mac_alg_match(IN tt_u8_t *name,
                                                IN tt_u32_t name_len);

// ========================================
// Compression
// ========================================

typedef enum {
    TT_SSH_CMPRS_ALG_NONE,
    TT_SSH_CMPRS_ALG_ZLIB,

    TT_SSH_CMPRS_ALG_NUM
} tt_ssh_cmprs_alg_t;
#define TT_SSH_CMPRS_ALG_VALID(a) ((a) < TT_SSH_MAC_ALG_NUM)

tt_export const tt_char_t *tt_g_ssh_cmprs_alg_name[TT_SSH_CMPRS_ALG_NUM];

tt_export tt_ssh_cmprs_alg_t tt_ssh_cmprs_alg_match(IN tt_u8_t *name,
                                                    IN tt_u32_t name_len);

// ========================================
// Service
// ========================================

typedef enum {
    TT_SSH_SERVICE_USERAUTH,
    TT_SSH_SERVICE_CONNECTION,

    TT_SSH_SERVICE_NUM
} tt_ssh_service_t;
#define TT_SSH_SERVICE_VALID(s) ((s) < TT_SSH_SERVICE_NUM)

tt_export const tt_char_t *tt_g_ssh_serv_name[TT_SSH_SERVICE_NUM];

tt_export tt_ssh_service_t tt_ssh_serv_match(IN tt_u8_t *name,
                                             IN tt_u32_t name_len);

// ========================================
// Auth Method
// ========================================

typedef enum {
    TT_SSH_AUTH_PUBLICKEY,
    TT_SSH_AUTH_PASSWORD,
    TT_SSH_AUTH_HOSTBASED,
    TT_SSH_AUTH_NONE,

    TT_SSH_AUTH_NUM
} tt_ssh_auth_t;
#define TT_SSH_AUTH_VALID(s) ((s) < TT_SSH_AUTH_NUM)

tt_export const tt_char_t *tt_g_ssh_auth_name[TT_SSH_AUTH_NUM];

tt_export tt_ssh_auth_t tt_ssh_auth_match(IN tt_u8_t *name,
                                          IN tt_u32_t name_len);

// ========================================
// Channel
// ========================================

typedef enum {
    TT_SSH_CHTYPE_SESSION,
    TT_SSH_CHTYPE_X11,
    TT_SSH_CHTYPE_FORWARD_TCPIP,
    TT_SSH_CHTYPE_DIRECT_TCPIP,

    TT_SSH_CHTYPE_NUM,
} tt_ssh_chtype_t;
#define TT_SSH_CHTYPE_VALID(s) ((s) < TT_SSH_CHTYPE_NUM)

tt_export const tt_char_t *tt_g_ssh_chtype_name[TT_SSH_CHTYPE_NUM];

tt_export tt_ssh_chtype_t tt_ssh_chtype_match(IN tt_u8_t *name,
                                              IN tt_u32_t name_len);

typedef enum {
    TT_SSH_CHREQTYPE_PTYREQ,
    TT_SSH_CHREQTYPE_X11REQ,
    TT_SSH_CHREQTYPE_ENV,
    TT_SSH_CHREQTYPE_SHELL,
    TT_SSH_CHREQTYPE_EXEC,
    TT_SSH_CHREQTYPE_SUBSYSTEM,
    TT_SSH_CHREQTYPE_WINCHANGE,
    TT_SSH_CHREQTYPE_XONXOFF,
    TT_SSH_CHREQTYPE_SIGNAL,
    TT_SSH_CHREQTYPE_EXITSTATUS,
    TT_SSH_CHREQTYPE_EXITSIGNAL,

    TT_SSH_CHREQTYPE_NUM,
} tt_ssh_chreqtype_t;
#define TT_SSH_CHREQTYPE_VALID(s) ((s) < TT_SSH_CHREQTYPE_NUM)

tt_export const tt_char_t *tt_g_ssh_chreqtype_name[TT_SSH_CHREQTYPE_NUM];

tt_export tt_ssh_chreqtype_t tt_ssh_chreqtype_match(IN tt_u8_t *name,
                                                    IN tt_u32_t name_len);

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export tt_ssh_kex_alg_t tt_g_ssh_kex_pref[TT_SSH_KEX_ALG_NUM];
tt_export tt_u32_t tt_g_ssh_kex_pref_num;

tt_export tt_ssh_pubkey_alg_t tt_g_ssh_pubkey_pref[TT_SSH_PUBKEY_ALG_NUM];
tt_export tt_u32_t tt_g_ssh_pubkey_pref_num;

tt_export tt_ssh_enc_alg_t tt_g_ssh_enc_c2s_pref[TT_SSH_ENC_ALG_NUM];
tt_export tt_u32_t tt_g_ssh_enc_c2s_pref_num;

tt_export tt_ssh_enc_alg_t tt_g_ssh_enc_s2c_pref[TT_SSH_ENC_ALG_NUM];
tt_export tt_u32_t tt_g_ssh_enc_s2c_pref_num;

tt_export tt_ssh_mac_alg_t tt_g_ssh_mac_c2s_pref[TT_SSH_MAC_ALG_NUM];
tt_export tt_u32_t tt_g_ssh_mac_c2s_pref_num;

tt_export tt_ssh_mac_alg_t tt_g_ssh_mac_s2c_pref[TT_SSH_MAC_ALG_NUM];
tt_export tt_u32_t tt_g_ssh_mac_s2c_pref_num;

tt_export tt_ssh_cmprs_alg_t tt_g_ssh_cmprs_c2s_pref[TT_SSH_CMPRS_ALG_NUM];
tt_export tt_u32_t tt_g_ssh_cmprs_c2s_pref_num;

tt_export tt_ssh_cmprs_alg_t tt_g_ssh_cmprs_s2c_pref[TT_SSH_CMPRS_ALG_NUM];
tt_export tt_u32_t tt_g_ssh_cmprs_s2c_pref_num;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// return 0 for unknown enc_alg
tt_export tt_u32_t tt_ssh_enc_iv_len(IN tt_ssh_enc_alg_t enc_alg);

// return 0 for unknown enc_alg
tt_export tt_u32_t tt_ssh_enc_key_len(IN tt_ssh_enc_alg_t enc_alg);

// return 0 for unknown mac_alg
tt_export tt_u32_t tt_ssh_mac_key_len(IN tt_ssh_mac_alg_t mac_alg);

#endif /* __TT_SSH_DEF__ */
