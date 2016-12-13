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

#include <crypto/tt_dh_mpn.h>

#include <crypto/tt_crypto_def.h>
#include <crypto/tt_dh.h>
#include <math/mp/tt_mpn.h>
#include <math/mp/tt_mpn_exp.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>

#include <tt_cstd_api.h>

#ifdef TT_DH_MPN_IMPLEMENTATION

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

tt_result_t tt_dh_create_param_ntv(IN tt_dh_ntv_t *sys_dh,
                                   IN tt_blob_t *prime,
                                   IN tt_blob_t *generator,
                                   IN struct tt_dh_attr_s *attr)
{
    tt_u32_t __done = 0;
#define __DCP_P (1 << 0)
#define __DCP_G (1 << 1)
#define __DCP_PUB (1 << 2)
#define __DCP_PRIV (1 << 3)
#define __DCP_MC (1 << 4)

    // p
    TT_DO_G(dcp_fail,
            tt_mpn_create(&sys_dh->p, prime->addr, prime->len, TT_FALSE, 0));
    __done |= __DCP_P;

    // g
    if (generator->addr == TT_DH_GENERATOR_2) {
        tt_mpn_init(&sys_dh->g);
        tt_mpn_set_u(&sys_dh->g, 2, TT_FALSE, 0);
    } else if (generator->addr == TT_DH_GENERATOR_5) {
        tt_mpn_init(&sys_dh->g);
        tt_mpn_set_u(&sys_dh->g, 5, TT_FALSE, 0);
    } else {
        TT_DO_G(dcp_fail,
                tt_mpn_create(&sys_dh->g,
                              prime->addr,
                              prime->len,
                              TT_FALSE,
                              0));
    }
    __done |= __DCP_G;

    // priv
    tt_mpn_init(&sys_dh->priv);
    TT_DO_G(dcp_fail,
            tt_mpn_rand_lessthan(&sys_dh->priv, &sys_dh->p, TT_MPN_RAND_FULL));
    __done |= __DCP_PRIV;

    // mpn cache
    TT_DO_G(dcp_fail, tt_mpn_cache_create(&sys_dh->mpnc));
    __done |= __DCP_MC;

    // pub = g^priv mod p
    tt_mpn_init(&sys_dh->pub);
    TT_DO_G(dcp_fail,
            tt_mpn_powmod(&sys_dh->pub,
                          &sys_dh->g,
                          &sys_dh->priv,
                          &sys_dh->p,
                          &sys_dh->mpnc));
    __done |= __DCP_PUB;

    // secret
    tt_mpn_init(&sys_dh->secret);

    return TT_SUCCESS;

dcp_fail:

    if (__done & __DCP_P) {
        tt_mpn_destroy(&sys_dh->p);
    }

    if (__done & __DCP_G) {
        tt_mpn_destroy(&sys_dh->g);
    }

    if (__done & __DCP_PUB) {
        tt_mpn_destroy(&sys_dh->pub);
    }

    if (__done & __DCP_PRIV) {
        tt_mpn_destroy(&sys_dh->priv);
    }

    if (__done & __DCP_MC) {
        tt_mpn_cache_destroy(&sys_dh->mpnc);
    }

    return TT_FAIL;
}

tt_result_t tt_dh_create_keypair_ntv(IN tt_dh_ntv_t *sys_dh,
                                     IN tt_blob_t *prime,
                                     IN tt_blob_t *generator,
                                     IN tt_blob_t *pub,
                                     IN tt_blob_t *priv,
                                     IN struct tt_dh_attr_s *attr)
{
    tt_u32_t __done = 0;
#define __DCK_P (1 << 0)
#define __DCK_G (1 << 1)
#define __DCK_PUB (1 << 2)
#define __DCK_PRIV (1 << 3)
#define __DCK_MC (1 << 4)

    // p
    TT_DO_G(dck_fail,
            tt_mpn_create(&sys_dh->p, prime->addr, prime->len, TT_FALSE, 0));
    __done |= __DCK_P;

    // g
    if (generator->addr == TT_DH_GENERATOR_2) {
        tt_mpn_init(&sys_dh->g);
        tt_mpn_set_u(&sys_dh->g, 2, TT_FALSE, 0);
    } else if (generator->addr == TT_DH_GENERATOR_5) {
        tt_mpn_init(&sys_dh->g);
        tt_mpn_set_u(&sys_dh->g, 5, TT_FALSE, 0);
    } else {
        TT_DO_G(dck_fail,
                tt_mpn_create(&sys_dh->g,
                              prime->addr,
                              prime->len,
                              TT_FALSE,
                              0));
    }
    __done |= __DCK_G;

    // pub
    TT_DO_G(dck_fail,
            tt_mpn_create(&sys_dh->pub, pub->addr, pub->len, TT_FALSE, 0));
    __done |= __DCK_PUB;

    // priv
    TT_DO_G(dck_fail,
            tt_mpn_create(&sys_dh->priv, priv->addr, priv->len, TT_FALSE, 0));
    __done |= __DCK_PRIV;

    // need we check whether pub == g^priv mod p??

    // secret
    tt_mpn_init(&sys_dh->secret);

    // mpn cache
    TT_DO_G(dck_fail, tt_mpn_cache_create(&sys_dh->mpnc));
    __done |= __DCK_MC;

    return TT_SUCCESS;

dck_fail:

    if (__done & __DCK_P) {
        tt_mpn_destroy(&sys_dh->p);
    }

    if (__done & __DCK_G) {
        tt_mpn_destroy(&sys_dh->g);
    }

    if (__done & __DCK_PUB) {
        tt_mpn_destroy(&sys_dh->pub);
    }

    if (__done & __DCK_PRIV) {
        tt_mpn_destroy(&sys_dh->priv);
    }

    if (__done & __DCK_MC) {
        tt_mpn_cache_destroy(&sys_dh->mpnc);
    }

    return TT_FAIL;
}

void tt_dh_destroy_ntv(IN tt_dh_ntv_t *sys_dh)
{
    tt_mpn_destroy(&sys_dh->p);
    tt_mpn_destroy(&sys_dh->g);
    tt_mpn_destroy(&sys_dh->pub);
    tt_mpn_destroy(&sys_dh->priv);
    tt_mpn_destroy(&sys_dh->secret);

    tt_mpn_cache_destroy(&sys_dh->mpnc);
}

tt_result_t tt_dh_get_pubkey_ntv(IN tt_dh_ntv_t *sys_dh,
                                 OUT OPT tt_u8_t *pubkey,
                                 IN OUT tt_u32_t *pubkey_len,
                                 IN tt_u32_t flag)
{
    tt_u32_t mpn_flag = 0;
    tt_u32_t len, prefix = 0;

    // required length
    if (flag & TT_CRYPTO_FMT_SIGNED) {
        mpn_flag |= TT_MPN_GET_SIGNED;
    }
    if (flag & TT_CRYPTO_FMT_SSH_MPINT) {
        mpn_flag |= TT_MPN_GET_SIGNED;
        prefix = 4;
    }
    if (!TT_OK(tt_mpn_get(&sys_dh->pub, NULL, &len, NULL, mpn_flag))) {
        return TT_FAIL;
    }

    if (pubkey == NULL) {
        *pubkey_len = (len + prefix);
        return TT_SUCCESS;
    }

    if (*pubkey_len < (len + prefix)) {
        TT_ERROR("less than required mpint size");
        return TT_FAIL;
    }

    if (flag & TT_CRYPTO_FMT_SSH_MPINT) {
        *pubkey++ = (tt_u8_t)(len >> 24);
        *pubkey++ = (tt_u8_t)(len >> 16);
        *pubkey++ = (tt_u8_t)(len >> 8);
        *pubkey++ = (tt_u8_t)(len >> 0);
    }
    // no need to adjust pubkey_len as it's already checked
    if (!TT_OK(tt_mpn_get(&sys_dh->pub, pubkey, pubkey_len, NULL, mpn_flag))) {
        return TT_FAIL;
    }
    *pubkey_len += prefix;

    return TT_SUCCESS;
}

tt_result_t tt_dh_compute_ntv(IN tt_dh_ntv_t *sys_dh,
                              IN tt_u8_t *peer_pub,
                              IN tt_u32_t peer_pub_len)
{
    tt_result_t result = TT_FAIL;
    tt_mpn_t *t = NULL;

    TT_DONN_G(dc_out, t = tt_mpn_alloc(&sys_dh->mpnc, 0));
    TT_DO_G(dc_out, tt_mpn_set(t, peer_pub, peer_pub_len, TT_FALSE, 0));

    TT_DO_G(dc_out,
            tt_mpn_powmod(&sys_dh->secret,
                          t,
                          &sys_dh->priv,
                          &sys_dh->p,
                          &sys_dh->mpnc));

    result = TT_SUCCESS;

dc_out:

    if (t != NULL) {
        tt_mpn_free(&sys_dh->mpnc, t);
    }

    return result;
}

tt_result_t tt_dh_get_secret_ntv(IN tt_dh_ntv_t *sys_dh,
                                 OUT OPT tt_u8_t *secret,
                                 IN OUT tt_u32_t *secret_len,
                                 IN tt_u32_t flag)
{
    tt_u32_t mpn_flag = 0;
    tt_u32_t prefix = 0, len;

    // required length
    if (flag & TT_CRYPTO_FMT_SIGNED) {
        mpn_flag |= TT_MPN_GET_SIGNED;
    }
    if (flag & TT_CRYPTO_FMT_SSH_MPINT) {
        mpn_flag |= TT_MPN_GET_SIGNED;
        prefix = 4;
    }
    if (!TT_OK(tt_mpn_get(&sys_dh->secret, NULL, &len, NULL, mpn_flag))) {
        return TT_FAIL;
    }

    if (secret == NULL) {
        *secret_len = (len + prefix);
        return TT_SUCCESS;
    }

    if (*secret_len < (len + prefix)) {
        TT_ERROR("less than required mpint size");
        return TT_FAIL;
    }

    if (flag & TT_CRYPTO_FMT_SSH_MPINT) {
        *secret++ = (tt_u8_t)(len >> 24);
        *secret++ = (tt_u8_t)(len >> 16);
        *secret++ = (tt_u8_t)(len >> 8);
        *secret++ = (tt_u8_t)(len >> 0);
    }
    // no need to adjust secret_len as it's already checked
    if (!TT_OK(
            tt_mpn_get(&sys_dh->secret, secret, secret_len, NULL, mpn_flag))) {
        return TT_FAIL;
    }
    *secret_len += prefix;

    return TT_SUCCESS;
}

#endif
