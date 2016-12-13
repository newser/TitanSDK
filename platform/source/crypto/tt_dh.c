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

#include <crypto/tt_dh.h>

#include <algorithm/tt_buffer.h>
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

static tt_result_t __dh_create_param(IN tt_dh_t *dh,
                                     IN tt_dh_keydata_param_t *param,
                                     IN tt_dh_attr_t *attr);
static tt_result_t __dh_create_keypair(IN tt_dh_t *dh,
                                       IN tt_dh_keydata_keypair_t *keypair,
                                       IN tt_dh_attr_t *attr);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_dh_create(IN tt_dh_t *dh,
                         IN tt_dh_format_t format,
                         IN tt_dh_keydata_t *keydata,
                         IN tt_dh_attr_t *attr)
{
    TT_ASSERT(dh != NULL);
    TT_ASSERT(TT_DH_FORMAT_VALID(format));
    TT_ASSERT(keydata != NULL);

    if (attr != NULL) {
        tt_memcpy(&dh->attr, attr, sizeof(tt_dh_attr_t));
    } else {
        tt_dh_attr_default(&dh->attr);
    }

    switch (format) {
        case TT_DH_FORMAT_PARAM: {
            return __dh_create_param(dh, &keydata->param, &dh->attr);
        } break;
        case TT_DH_FORMAT_KEYPAIR: {
            return __dh_create_keypair(dh, &keydata->keypair, &dh->attr);
        } break;

        default: {
            TT_ERROR("dh format[%d] is not supported yet", format);
            return TT_FAIL;
        } break;
    }
}

void tt_dh_destroy(IN tt_dh_t *dh)
{
    TT_ASSERT(dh != NULL);

    tt_dh_destroy_ntv(&dh->sys_dh);
}

void tt_dh_attr_default(IN tt_dh_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->password = NULL;

    attr->pem_armor = TT_FALSE;
}

tt_result_t tt_dh_get_pubkey(IN tt_dh_t *dh,
                             OUT OPT tt_u8_t *pubkey,
                             IN OUT tt_u32_t *pubkey_len,
                             IN tt_u32_t flag)
{
    TT_ASSERT(dh != NULL);
    TT_ASSERT(pubkey_len != NULL);

    return tt_dh_get_pubkey_ntv(&dh->sys_dh, pubkey, pubkey_len, flag);
}

tt_result_t tt_dh_get_pubkey_buf(IN tt_dh_t *dh,
                                 OUT tt_buf_t *pubkey,
                                 IN tt_u32_t flag)
{
    tt_u32_t len;

    TT_ASSERT(dh != NULL);
    TT_ASSERT(pubkey != NULL);

    if (!TT_OK(tt_dh_get_pubkey_ntv(&dh->sys_dh, NULL, &len, flag))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_buf_reserve(pubkey, len))) {
        return TT_FAIL;
    }

    len = TT_BUF_WLEN(pubkey);
    if (!TT_OK(tt_dh_get_pubkey_ntv(&dh->sys_dh,
                                    TT_BUF_WPOS(pubkey),
                                    &len,
                                    flag))) {
        return TT_FAIL;
    }
    tt_buf_inc_wp(pubkey, len);

    return TT_SUCCESS;
}

tt_result_t tt_dh_compute(IN tt_dh_t *dh,
                          IN tt_u8_t *peer_pub,
                          IN tt_u32_t peer_pub_len)
{
    TT_ASSERT(dh != NULL);
    TT_ASSERT(peer_pub != NULL);
    TT_ASSERT(peer_pub_len > 0);

    return tt_dh_compute_ntv(&dh->sys_dh, peer_pub, peer_pub_len);
}

tt_result_t tt_dh_get_secret(IN tt_dh_t *dh,
                             OUT tt_u8_t *secret,
                             IN OUT tt_u32_t *secret_len,
                             IN tt_u32_t flag)
{
    TT_ASSERT(dh != NULL);
    TT_ASSERT(secret_len != NULL);

    return tt_dh_get_secret_ntv(&dh->sys_dh, secret, secret_len, flag);
}

tt_result_t tt_dh_get_secret_buf(IN tt_dh_t *dh,
                                 OUT tt_buf_t *secret,
                                 IN tt_u32_t flag)
{
    tt_u32_t len;

    TT_ASSERT(dh != NULL);
    TT_ASSERT(secret != NULL);

    if (!TT_OK(tt_dh_get_secret_ntv(&dh->sys_dh, NULL, &len, flag))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_buf_reserve(secret, len))) {
        return TT_FAIL;
    }

    len = TT_BUF_WLEN(secret);
    if (!TT_OK(tt_dh_get_secret_ntv(&dh->sys_dh,
                                    TT_BUF_WPOS(secret),
                                    &len,
                                    flag))) {
        return TT_FAIL;
    }
    tt_buf_inc_wp(secret, len);

    return TT_SUCCESS;
}

tt_result_t __dh_create_param(IN tt_dh_t *dh,
                              IN tt_dh_keydata_param_t *param,
                              IN tt_dh_attr_t *attr)
{
    return tt_dh_create_param_ntv(&dh->sys_dh,
                                  &param->prime,
                                  &param->generator,
                                  attr);
}

tt_result_t __dh_create_keypair(IN tt_dh_t *dh,
                                IN tt_dh_keydata_keypair_t *keypair,
                                IN tt_dh_attr_t *attr)
{
    return tt_dh_create_keypair_ntv(&dh->sys_dh,
                                    &keypair->prime,
                                    &keypair->generator,
                                    &keypair->pub,
                                    &keypair->priv,
                                    attr);
}
