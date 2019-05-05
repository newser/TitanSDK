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

#include <network/ssh/context/tt_ssh_pubkey.h>

#include <crypto/tt_rsa.h>
#include <misc/tt_assert.h>

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

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_sshpubk_init(IN tt_sshpubk_t *pubk)
{
    pubk->alg = TT_SSH_PUBKEY_ALG_NUM;
    pubk->alg_u.key = NULL;

    tt_buf_init(&pubk->signature, NULL);
}

void tt_sshpubk_destroy(IN tt_sshpubk_t *pubk)
{
    tt_buf_destroy(&pubk->signature);
}

void tt_sshpubk_setalg(IN tt_sshpubk_t *pubk, IN tt_ssh_pubkey_alg_t alg,
                       IN void *key)
{
    TT_ASSERT(TT_SSH_PUBKEY_ALG_VALID(alg));
    pubk->alg = alg;
    pubk->alg_u.key = key;
}

tt_result_t tt_sshpubk_sign(IN tt_sshpubk_t *pubk, IN tt_u8_t *data,
                            IN tt_u32_t data_len)
{
    switch (pubk->alg) {
    case TT_SSH_PUBKEY_ALG_RSA: {
        TT_ASSERT(pubk->alg_u.key != NULL);

        tt_buf_reset_rwp(&pubk->signature);
        return tt_rsa_sign_buf(pubk->alg_u.rsa, data, data_len,
                               &pubk->signature);
    } break;

    default: {
        return TT_FAIL;
    } break;
    }
}
