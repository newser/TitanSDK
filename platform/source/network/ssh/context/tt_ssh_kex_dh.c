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

#include <network/ssh/context/tt_ssh_kex_dh.h>

#include <crypto/tt_crypto_def.h>
#include <misc/tt_assert.h>
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

static tt_u8_t tt_s_dh_prime_oakley2[] =
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2,
     0x21, 0x68, 0xC2, 0x34, 0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1,
     0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74, 0x02, 0x0B, 0xBE, 0xA6,
     0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
     0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 0x2B, 0x0A, 0x6D,
     0xF2, 0x5F, 0x14, 0x37, 0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45,
     0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6, 0xF4, 0x4C, 0x42, 0xE9,
     0xA6, 0x37, 0xED, 0x6B, 0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
     0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 0x9F, 0x24, 0x11,
     0x7C, 0x4B, 0x1F, 0xE6, 0x49, 0x28, 0x66, 0x51, 0xEC, 0xE6, 0x53, 0x81,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static tt_u8_t tt_s_dh_prime_oakley14[] =
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2,
     0x21, 0x68, 0xC2, 0x34, 0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1,
     0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74, 0x02, 0x0B, 0xBE, 0xA6,
     0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
     0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 0x2B, 0x0A, 0x6D,
     0xF2, 0x5F, 0x14, 0x37, 0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45,
     0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6, 0xF4, 0x4C, 0x42, 0xE9,
     0xA6, 0x37, 0xED, 0x6B, 0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
     0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 0x9F, 0x24, 0x11,
     0x7C, 0x4B, 0x1F, 0xE6, 0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D,
     0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05, 0x98, 0xDA, 0x48, 0x36,
     0x1C, 0x55, 0xD3, 0x9A, 0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
     0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96, 0x1C, 0x62, 0xF3, 0x56,
     0x20, 0x85, 0x52, 0xBB, 0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D,
     0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04, 0xF1, 0x74, 0x6C, 0x08,
     0xCA, 0x18, 0x21, 0x7C, 0x32, 0x90, 0x5E, 0x46, 0x2E, 0x36, 0xCE, 0x3B,
     0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03, 0x9B, 0x27, 0x83, 0xA2,
     0xEC, 0x07, 0xA2, 0x8F, 0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9,
     0xDE, 0x2B, 0xCB, 0xF6, 0x95, 0x58, 0x17, 0x18, 0x39, 0x95, 0x49, 0x7C,
     0xEA, 0x95, 0x6A, 0xE5, 0x15, 0xD2, 0x26, 0x18, 0x98, 0xFA, 0x05, 0x10,
     0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAC, 0xAA, 0x68, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_sshkexdh_create(IN tt_sshkexdh_t *kexdh, IN tt_ssh_kex_alg_t alg)
{
    tt_dh_keydata_t keydata;

    kexdh->alg = alg;

    tt_memset(&keydata, 0, sizeof(tt_dh_keydata_t));
    if (alg == TT_SSH_KEX_ALG_DH_G1_SHA1) {
        keydata.param.prime.addr = tt_s_dh_prime_oakley2;
        keydata.param.prime.len = sizeof(tt_s_dh_prime_oakley2);
        keydata.param.generator.addr = TT_DH_GENERATOR_2;
    } else {
        TT_ASSERT(alg == TT_SSH_KEX_ALG_DH_G14_SHA1);

        keydata.param.prime.addr = tt_s_dh_prime_oakley14;
        keydata.param.prime.len = sizeof(tt_s_dh_prime_oakley14);
        keydata.param.generator.addr = TT_DH_GENERATOR_2;
    }
    if (!TT_OK(tt_dh_create(&kexdh->dh, TT_DH_FORMAT_PARAM, &keydata, NULL))) {
        return TT_FAIL;
    }

    tt_buf_init(&kexdh->e, NULL);
    tt_buf_init(&kexdh->f, NULL);

    return TT_SUCCESS;
}

void tt_sshkexdh_destroy(IN tt_sshkexdh_t *kexdh)
{
    tt_dh_destroy(&kexdh->dh);

    tt_buf_destroy(&kexdh->e);
    tt_buf_destroy(&kexdh->f);
}

tt_result_t tt_sshkexdh_compute(IN tt_sshkexdh_t *kexdh,
                                IN tt_u8_t *peerpub,
                                IN tt_u32_t peerpub_len)
{
    return tt_dh_compute(&kexdh->dh, peerpub, peerpub_len);
}

tt_result_t tt_sshkexdh_set_e(IN tt_sshkexdh_t *kexdh,
                              IN tt_u8_t *e,
                              IN tt_u32_t e_len,
                              IN tt_bool_t format)
{
    tt_buf_reset_rwp(&kexdh->e);
    if (format) {
        return tt_ssh_mpint_render(&kexdh->e, e, e_len, TT_FALSE);
    } else {
        return tt_buf_put(&kexdh->e, e, e_len);
    }
}

tt_result_t tt_sshkexdh_load_e(IN tt_sshkexdh_t *kexdh)
{
    tt_buf_reset_rwp(&kexdh->e);
    return tt_dh_get_pubkey_buf(&kexdh->dh, &kexdh->e, TT_CRYPTO_FMT_SSH_MPINT);
}

tt_result_t tt_sshkexdh_set_f(IN tt_sshkexdh_t *kexdh,
                              IN tt_u8_t *f,
                              IN tt_u32_t f_len,
                              IN tt_bool_t format)
{
    tt_buf_reset_rwp(&kexdh->f);
    if (format) {
        return tt_ssh_mpint_render(&kexdh->f, f, f_len, TT_FALSE);
    } else {
        return tt_buf_put(&kexdh->f, f, f_len);
    }
}

tt_result_t tt_sshkexdh_load_f(IN tt_sshkexdh_t *kexdh)
{
    tt_buf_reset_rwp(&kexdh->f);
    return tt_dh_get_pubkey_buf(&kexdh->dh, &kexdh->f, TT_CRYPTO_FMT_SSH_MPINT);
}

tt_result_t tt_sshkexdh_get_k(IN tt_sshkexdh_t *kexdh, OUT tt_buf_t *k)
{
    tt_buf_reset_rwp(k);
    return tt_dh_get_secret_buf(&kexdh->dh, k, TT_CRYPTO_FMT_SSH_MPINT);
}
