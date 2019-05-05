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
@file tt_ssh_kdf.h
@brief ts ssh key derivation function
*/

#ifndef __TT_SSH_KDF__
#define __TT_SSH_KDF__

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

typedef struct
{
    tt_ssh_hash_alg_t hash_alg;

    tt_buf_t iv_c2s;
    tt_buf_t iv_s2c;

    tt_buf_t enc_c2s;
    tt_buf_t enc_s2c;

    tt_buf_t mac_c2s;
    tt_buf_t mac_s2c;
} tt_sshkdf_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_sshkdf_init(IN tt_sshkdf_t *kdf,
                              IN tt_ssh_hash_alg_t hash_alg);

tt_export void tt_sshkdf_destroy(IN tt_sshkdf_t *kdf);

// - k: mpint formatted
// - h: raw
// - session_id: raw
tt_export tt_result_t tt_sshkdf_run(
    IN tt_sshkdf_t *kdf, IN tt_blob_t *k, IN tt_blob_t *h,
    IN tt_blob_t *session_id, IN tt_u32_t iv_c2s_len, IN tt_u32_t iv_s2c_len,
    IN tt_u32_t enc_c2s_len, IN tt_u32_t enc_s2c_len, IN tt_u32_t mac_c2s_len,
    IN tt_u32_t mac_s2c_len);

#endif /* __TT_SSH_KDF__ */
