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
@file tt_ecdh.h
@brief crypto: ECDH

this file defines elliptic curve diffie-hellman APIs
*/

#ifndef __TT_ECDH__
#define __TT_ECDH__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <ecdh.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_pk_s;

typedef enum {
    TT_ECGRP_SECP192R1,
    TT_ECGRP_SECP224R1,
    TT_ECGRP_SECP256R1,
    TT_ECGRP_SECP384R1,
    TT_ECGRP_SECP521R1,
    TT_ECGRP_BP256R1,
    TT_ECGRP_BP384R1,
    TT_ECGRP_BP512R1,
    TT_ECGRP_CURVE25519,
    TT_ECGRP_SECP192K1,
    TT_ECGRP_SECP224K1,
    TT_ECGRP_SECP256K1,

    TT_ECGRP_NUM,
} tt_ecgrp_t;
#define TT_ECGRP_VALID(g) ((g) < TT_ECGRP_NUM)

typedef struct
{
    mbedtls_ecdh_context ctx;
} tt_ecdh_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_ecdh_init(IN tt_ecdh_t *ecdh);

extern void tt_ecdh_destroy(IN tt_ecdh_t *ecdh);

extern tt_result_t tt_ecdh_load(IN tt_ecdh_t *ecdh, IN struct tt_pk_s *pk);

extern tt_result_t tt_ecdh_generate(IN tt_ecdh_t *ecdh, IN tt_ecgrp_t g);

extern tt_result_t tt_ecdh_get_pub(IN tt_ecdh_t *ecdh,
                                   IN tt_bool_t local,
                                   IN tt_bool_t compress,
                                   OUT tt_u8_t *pub,
                                   IN OUT tt_u32_t *len);

extern tt_result_t tt_ecdh_set_pub(IN tt_ecdh_t *ecdh,
                                   IN tt_bool_t local,
                                   IN tt_u8_t *pub,
                                   IN tt_u32_t len);

extern tt_result_t tt_ecdh_derive(IN tt_ecdh_t *ecdh);

extern tt_result_t tt_ecdh_get_secret(IN tt_ecdh_t *ecdh,
                                      OUT tt_u8_t *secret,
                                      IN OUT tt_u32_t *len);

#endif
