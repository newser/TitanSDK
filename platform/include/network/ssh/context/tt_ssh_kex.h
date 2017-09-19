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
@file tt_ssh_kex.h
@brief ts ssh context: key exchange
*/

#ifndef __TT_SSH_KEX__
#define __TT_SSH_KEX__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>
#include <network/ssh/context/tt_ssh_kex_dh.h>
#include <network/ssh/tt_ssh_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_sshkex_s
{
    tt_ssh_kex_alg_t alg;
    union
    {
        tt_sshkexdh_t kexdh;
    } alg_u;

    // the client’s identification string (CR and LF excluded)
    tt_buf_t v_c;
    // the server’s identification string (CR and LF excluded)
    tt_buf_t v_s;
    // the payload of the client’s SSH_MSG_KEXINIT
    tt_buf_t i_c;
    // the payload of the server’s SSH_MSG_KEXINIT
    tt_buf_t i_s;
    // the host key
    tt_buf_t k_s;
    // the shared secret
    tt_buf_t k;
    // the exchange hash
    tt_buf_t h;
} tt_sshkex_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_sshkex_create(IN tt_sshkex_t *kex);

tt_export void tt_sshkex_destroy(IN tt_sshkex_t *kex);

tt_export tt_result_t tt_sshkex_setalg(IN tt_sshkex_t *kex,
                                       IN tt_ssh_kex_alg_t alg);

tt_export tt_result_t tt_sshkex_setvc(IN tt_sshkex_t *kex,
                                      IN tt_u8_t *vc,
                                      IN tt_u32_t vc_len,
                                      IN tt_bool_t format);
tt_export tt_result_t tt_sshkex_setvs(IN tt_sshkex_t *kex,
                                      IN tt_u8_t *vs,
                                      IN tt_u32_t vs_len,
                                      IN tt_bool_t format);
tt_export tt_result_t tt_sshkex_setic(IN tt_sshkex_t *kex,
                                      IN tt_u8_t *ic,
                                      IN tt_u32_t ic_len,
                                      IN tt_bool_t format);
tt_export tt_result_t tt_sshkex_setis(IN tt_sshkex_t *kex,
                                      IN tt_u8_t *is,
                                      IN tt_u32_t is_len,
                                      IN tt_bool_t format);
tt_export tt_result_t tt_sshkex_setks(IN tt_sshkex_t *kex,
                                      IN tt_u8_t *ks,
                                      IN tt_u32_t ks_len,
                                      IN tt_bool_t format);
tt_export tt_result_t tt_sshkex_setks_rsa(IN tt_sshkex_t *kex,
                                          IN tt_blob_t *e,
                                          IN tt_blob_t *n);

tt_export tt_result_t tt_sshkex_calc_h(IN tt_sshkex_t *kex);

#endif /* __TT_SSH_KEX__ */
