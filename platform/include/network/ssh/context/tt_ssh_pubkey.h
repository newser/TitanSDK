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
@file tt_ssh_pubkey.h
@brief ts ssh context: public key
*/

#ifndef __TT_SSH_PUBKEY__
#define __TT_SSH_PUBKEY__

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

struct tt_rsa_s;

typedef struct
{
    tt_ssh_pubkey_alg_t alg;
    union
    {
        void *key;
        struct tt_rsa_s *rsa;
    } alg_u;

    tt_buf_t signature;
} tt_sshpubk_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_sshpubk_init(IN tt_sshpubk_t *pubk);

tt_export void tt_sshpubk_destroy(IN tt_sshpubk_t *pubk);

tt_export void tt_sshpubk_setalg(IN tt_sshpubk_t *pubk,
                                 IN tt_ssh_pubkey_alg_t alg, IN void *key);

tt_export tt_result_t tt_sshpubk_sign(IN tt_sshpubk_t *pubk, IN tt_u8_t *data,
                                      IN tt_u32_t data_len);

#endif /* __TT_SSH_PUBKEY__ */
