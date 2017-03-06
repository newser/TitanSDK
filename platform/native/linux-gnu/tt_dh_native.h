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
@file tt_dh_native.h
@brief crypto: diffie-hellman

this file defines diffie-hellman native APIs
*/

#ifndef __TT_DH_NATIVE__
#define __TT_DH_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <crypto/tt_dh_def.h>

#ifdef TTCM_PLATFORM_CRYPTO_ENABLE
#include <openssl/dh.h>
#endif

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;

typedef struct
{
#ifdef TTCM_PLATFORM_CRYPTO_ENABLE
    DH *dh;
#endif

    tt_u8_t *secret;
    tt_u32_t secret_len;
} tt_dh_ntv_t;

struct tt_dh_attr_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_result_t tt_dh_component_init_ntv(IN struct tt_profile_s *profile)
{
    return TT_SUCCESS;
}

extern tt_result_t tt_dh_create_param_ntv(IN tt_dh_ntv_t *sys_dh,
                                          IN tt_blob_t *prime,
                                          IN tt_blob_t *generator,
                                          IN struct tt_dh_attr_s *attr);

extern tt_result_t tt_dh_create_keypair_ntv(IN tt_dh_ntv_t *sys_dh,
                                            IN tt_blob_t *prime,
                                            IN tt_blob_t *generator,
                                            IN tt_blob_t *pub,
                                            IN tt_blob_t *priv,
                                            IN struct tt_dh_attr_s *attr);

extern void tt_dh_destroy_ntv(IN tt_dh_ntv_t *sys_dh);

extern tt_result_t tt_dh_compute_ntv(IN tt_dh_ntv_t *sys_dh,
                                     IN tt_u8_t *peer_pub,
                                     IN tt_u32_t peer_pub_len);

extern tt_result_t tt_dh_get_secret_ntv(IN tt_dh_ntv_t *sys_dh,
                                        OUT OPT tt_u8_t *secret,
                                        IN OUT tt_u32_t *secret_len,
                                        IN tt_u32_t flag);

extern tt_result_t tt_dh_get_pubkey_ntv(IN tt_dh_ntv_t *sys_dh,
                                        OUT OPT tt_u8_t *pubkey,
                                        IN OUT tt_u32_t *pubkey_len,
                                        IN tt_u32_t flag);

#endif
