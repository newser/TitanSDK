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

#include <tt_dh_native.h>

#include <crypto/tt_dh.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>

#ifdef PLATFORM_CRYPTO_ENABLE

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
    DH *dh = NULL;
    BIGNUM *bn;
    int codes;

    dh = DH_new();
    if (dh == NULL) {
        TT_ERROR("fail to new DH");
        return TT_FAIL;
    }

    // prime
    bn = BN_bin2bn(prime->addr, prime->len, NULL);
    if (bn == NULL) {
        TT_ERROR("fail to new DH p");
        goto dhc_fail;
    }
    dh->p = bn;

    // generator
    if (generator->addr == TT_DH_GENERATOR_2) {
        bn = BN_new();
        if (bn == NULL) {
            TT_ERROR("fail to new DH g");
            goto dhc_fail;
        }
        BN_init(bn);

        if (BN_set_word(bn, 2) != 1) {
            TT_ERROR("fail to set DH g");
            goto dhc_fail;
        }
    } else if (generator->addr == TT_DH_GENERATOR_5) {
        bn = BN_new();
        if (bn == NULL) {
            TT_ERROR("fail to new DH g");
            goto dhc_fail;
        }
        BN_init(bn);

        if (BN_set_word(bn, 5) != 1) {
            TT_ERROR("fail to set DH g");
            goto dhc_fail;
        }
    } else {
        bn = BN_bin2bn(generator->addr, generator->len, NULL);
        if (bn == NULL) {
            TT_ERROR("fail to new DH g");
            goto dhc_fail;
        }
    }
    dh->g = bn;

    // validate p and g
    if (DH_check(dh, &codes) != 1) {
        TT_ERROR("invalid DH params");
        goto dhc_fail;
    }

    // genarete keys
    if (DH_generate_key(dh) != 1) {
        TT_ERROR("fail to generate DH keys");
        goto dhc_fail;
    }

    sys_dh->dh = dh;

    sys_dh->secret = NULL;
    sys_dh->secret_len = 0;

    return TT_SUCCESS;

dhc_fail:

    if (dh != NULL) {
        DH_free(dh);
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
    DH *dh = NULL;
    BIGNUM *bn;
    int codes;

    dh = DH_new();
    if (dh == NULL) {
        TT_ERROR("fail to new DH");
        return TT_FAIL;
    }

    // prime
    bn = BN_bin2bn(prime->addr, prime->len, NULL);
    if (bn == NULL) {
        TT_ERROR("fail to new DH p");
        goto dhc_fail;
    }
    dh->p = bn;

    // generator
    if (generator->addr == TT_DH_GENERATOR_2) {
        bn = BN_new();
        if (bn == NULL) {
            TT_ERROR("fail to new DH g");
            goto dhc_fail;
        }
        BN_init(bn);

        if (BN_set_word(bn, 2) != 1) {
            TT_ERROR("fail to set DH g");
            goto dhc_fail;
        }
    } else if (generator->addr == TT_DH_GENERATOR_5) {
        bn = BN_new();
        if (bn == NULL) {
            TT_ERROR("fail to new DH g");
            goto dhc_fail;
        }
        BN_init(bn);

        if (BN_set_word(bn, 5) != 1) {
            TT_ERROR("fail to set DH g");
            goto dhc_fail;
        }
    } else {
        bn = BN_bin2bn(generator->addr, generator->len, NULL);
        if (bn == NULL) {
            TT_ERROR("fail to new DH g");
            goto dhc_fail;
        }
    }
    dh->g = bn;

    // validate p and g
    if (DH_check(dh, &codes) != 1) {
        TT_ERROR("invalid DH params");
        goto dhc_fail;
    }

    // public key, note pub key should be g^priv mod p, but here
    // it's not verified
    bn = BN_bin2bn(pub->addr, pub->len, NULL);
    if (bn == NULL) {
        TT_ERROR("fail to new DH pub key");
        goto dhc_fail;
    }
    dh->pub_key = bn;

    // private key
    bn = BN_bin2bn(priv->addr, priv->len, NULL);
    if (bn == NULL) {
        TT_ERROR("fail to new DH priv key");
        goto dhc_fail;
    }
    dh->priv_key = bn;

    sys_dh->dh = dh;

    sys_dh->secret = NULL;
    sys_dh->secret_len = 0;

    return TT_SUCCESS;

dhc_fail:

    if (dh != NULL) {
        DH_free(dh);
    }

    return TT_FAIL;
}

void tt_dh_destroy_ntv(IN tt_dh_ntv_t *sys_dh)
{
    DH_free(sys_dh->dh);

    if (sys_dh->secret != NULL) {
        tt_free(sys_dh->secret);
    }
}

tt_result_t tt_dh_compute_ntv(IN tt_dh_ntv_t *sys_dh,
                              IN tt_u8_t *peer_pub,
                              IN tt_u32_t peer_pub_len)
{
    tt_u8_t *s = NULL;
    tt_u32_t s_memlen;
    BIGNUM *bn = NULL;
    int s_len;

    s_memlen = DH_size(sys_dh->dh);
    s = tt_malloc(s_memlen);
    if (s == NULL) {
        TT_ERROR("no mem for DH secret");
        return TT_FAIL;
    }

    bn = BN_bin2bn(peer_pub, peer_pub_len, NULL);
    if (bn == NULL) {
        TT_ERROR("fail to new dh peer pubkey");
        goto dhc_fail;
    }

    s_len = DH_compute_key(s, bn, sys_dh->dh);
    if (s_len < 0) {
        TT_ERROR("fail to compute DH secret");
        goto dhc_fail;
    }

    if (sys_dh->secret != NULL) {
        tt_free(sys_dh->secret);
    }
    sys_dh->secret = s;
    sys_dh->secret_len = (tt_u32_t)s_len;

    BN_free(bn);

    return TT_SUCCESS;

dhc_fail:

    if (bn != NULL) {
        BN_free(bn);
    }

    if (s != NULL) {
        tt_free(s);
    }

    return TT_FAIL;
}

tt_result_t tt_dh_get_secret_ntv(IN tt_dh_ntv_t *sys_dh,
                                 OUT OPT tt_u8_t *secret,
                                 IN OUT tt_u32_t *secret_len,
                                 IN tt_u32_t flag)
{
    if (sys_dh->secret_len == 0) {
        TT_ERROR("dh secret is not computed yet");
        return TT_FAIL;
    }

    if (secret == NULL) {
        *secret_len = sys_dh->secret_len;
        return TT_SUCCESS;
    }

    if (*secret_len < sys_dh->secret_len) {
        TT_ERROR("less than required dh secret length");
        return TT_FAIL;
    }
    tt_memcpy(secret, sys_dh->secret, sys_dh->secret_len);
    *secret_len = sys_dh->secret_len;

    return TT_SUCCESS;
}

tt_result_t tt_dh_get_pubkey_ntv(IN tt_dh_ntv_t *sys_dh,
                                 OUT OPT tt_u8_t *pubkey,
                                 IN OUT tt_u32_t *pubkey_len,
                                 IN tt_u32_t flag)
{
    int n = BN_num_bytes(sys_dh->dh->pub_key);

    if (pubkey == NULL) {
        *pubkey_len = n;
        return TT_SUCCESS;
    }

    if (*pubkey_len < n) {
        TT_ERROR("less than required dh pubkey length");
        return TT_FAIL;
    }
    *pubkey_len = BN_bn2bin(sys_dh->dh->pub_key, pubkey);

    return TT_SUCCESS;
}

#endif
