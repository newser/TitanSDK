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
@file tt_ecdsa.h
@brief crypto: ECDSA

this file defines elliptic curve diffie-hellman APIs
*/

#ifndef __TT_ECDSA__
#define __TT_ECDSA__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <crypto/tt_ec_def.h>
#include <crypto/tt_message_digest.h>

#include <ecdsa.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_pk_s;

typedef struct
{
    mbedtls_ecdsa_context ctx;
} tt_ecdsa_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_ecdsa_init(IN tt_ecdsa_t *dsa);

tt_export void tt_ecdsa_destroy(IN tt_ecdsa_t *dsa);

tt_export tt_result_t tt_ecdsa_load(IN tt_ecdsa_t *dsa, IN struct tt_pk_s *pk);

tt_export tt_result_t tt_ecdsa_generate(IN tt_ecdsa_t *dsa, IN tt_ecgrp_t g);

// sig_len should be at least (2*group size+9)
tt_export tt_result_t tt_ecdsa_sign(IN tt_ecdsa_t *dsa,
                                    IN tt_u8_t *input,
                                    IN tt_u32_t len,
                                    IN tt_md_type_t md_type,
                                    IN tt_md_type_t sign_md,
                                    OUT tt_u8_t *sig,
                                    IN OUT tt_u32_t *sig_len);

tt_export tt_result_t tt_ecdsa_verify(IN tt_ecdsa_t *dsa,
                                      IN tt_u8_t *input,
                                      IN tt_u32_t len,
                                      IN tt_md_type_t md_type,
                                      IN tt_u8_t *sig,
                                      IN tt_u32_t sig_len);

#endif
