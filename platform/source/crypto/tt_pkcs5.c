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

#include <crypto/tt_pkcs5.h>

#include <algorithm/tt_buffer.h>
#include <crypto/tt_hmac.h>
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

tt_result_t tt_pbkdf2(IN tt_pkcs5_prf_t prf,
                      IN tt_blob_t *password,
                      IN tt_blob_t *salt,
                      IN tt_u32_t iter_count,
                      IN tt_u32_t derived_key_len,
                      OUT tt_u8_t *derived_key)
{
    tt_u32_t hlen, l, r, i, j, k;
    tt_u8_t ti[TT_HMAC_SHA1_DIGEST_LENGTH];
    tt_u8_t *pos = derived_key;
    tt_result_t result;

    TT_ASSERT(TT_PKCS5_PRF_VALID(prf));
    TT_ASSERT((password != NULL) && (password->addr != NULL) &&
              (password->len != 0));
    TT_ASSERT((salt != NULL) && (salt->addr != NULL) && (salt->len != 0));
    TT_ASSERT(iter_count != 0);
    TT_ASSERT(derived_key != NULL);

    if (derived_key_len == 0) {
        return TT_SUCCESS;
    }

    /*
    DK = PBKDF2(PRF, Password, Salt, c, dkLen)

    DK = T1 || T2 || ... || Tdklen/hlen
    Ti = F(Password, Salt, c, i)

    F(Password, Salt, c, i) = U1 ^ U2 ^ ... ^ Uc

    U1 = PRF(Password, Salt || INT_32_BE(i))
    U2 = PRF(Password, U1)
    ...
    Uc = PRF(Password, Uc-1)
    */

    // todo: other PRF
    if (prf == TT_PKCS5_PRF_HMAC_SHA1) {
        hlen = TT_HMAC_SHA1_DIGEST_LENGTH;
    } else {
        TT_ERROR("only support hmac sha1 as prf now");
        return TT_FAIL;
    }
    TT_ASSERT(hlen <= sizeof(ti));

    l = derived_key_len / hlen;
    r = derived_key_len % hlen;
    if (r != 0) {
        ++l;
    }
    for (i = 0; i < l; ++i) {
        tt_blob_t blob[2];
        tt_u32_t be_i = tt_hton32(i + 1);
        tt_u8_t u1[sizeof(ti)], u2[sizeof(ti)];
        tt_u8_t *prev_u, *cur_u;

        blob[0].addr = salt->addr;
        blob[0].len = salt->len;
        blob[1].addr = (tt_u8_t *)&be_i;
        blob[1].len = sizeof(tt_u32_t);
        result = tt_hmac_gather(TT_HMAC_VER_SHA1, password, blob, 2, u1);
        if (!TT_OK(result)) {
            return TT_FAIL;
        }
        // U1
        tt_memcpy(ti, u1, hlen);

        // not all hmac implementation can have input and
        // output buffer pointing to same memory address,
        // so use two buffers here
        prev_u = u1;
        cur_u = u2;
        for (j = 1; j < iter_count; ++j) {
            blob[0].addr = prev_u;
            blob[0].len = hlen;
            result = tt_hmac(TT_HMAC_VER_SHA1, password, blob, cur_u);
            if (!TT_OK(result)) {
                return TT_FAIL;
            }

            // Uj
            for (k = 0; k < hlen; ++k) {
                ti[k] ^= cur_u[k];
            }

            TT_SWAP(tt_u8_t *, prev_u, cur_u);
        }

        // Ti
        if ((i < (l - 1)) || (r == 0)) {
            tt_memcpy(pos, ti, hlen);
            pos += hlen;
        } else {
            tt_memcpy(pos, ti, r);
            // pos += r;
        }
    }

    return TT_SUCCESS;
}
