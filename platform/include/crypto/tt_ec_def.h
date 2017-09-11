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
@file tt_ec_def.h
@brief ec definitions

this file defines ec
*/

#ifndef __TT_EC_DEF__
#define __TT_EC_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <ecp.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export mbedtls_ecp_group_id tt_g_ecgrp_map[TT_ECGRP_NUM];

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif
