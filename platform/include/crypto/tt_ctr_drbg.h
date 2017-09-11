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
@file tt_ctr_drbg.h
@brief ctr drbg

this file defines ctr drbg APIs
*/

#ifndef __TT_CTR_DRBG__
#define __TT_CTR_DRBG__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <ctr_drbg.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_entropy_s;

typedef struct tt_ctr_drbg_s
{
    mbedtls_ctr_drbg_context ctx;
} tt_ctr_drbg_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_ctr_drbg_t *tt_ctr_drbg_create(IN struct tt_entropy_s *entropy,
                                            IN OPT tt_u8_t *opaque,
                                            IN tt_u32_t len);

tt_export void tt_ctr_drbg_destroy(IN tt_ctr_drbg_t *drbg);

tt_export tt_ctr_drbg_t *tt_current_ctr_drbg();

tt_export tt_result_t tt_ctr_drbg_rand(IN tt_ctr_drbg_t *drbg,
                                       OUT tt_u8_t *buf,
                                       IN tt_u32_t len);

tt_export int tt_ctr_drbg(IN void *param, IN unsigned char *buf, IN size_t len);

#endif
