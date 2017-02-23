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
@file tt_dh_def.h
@brief diffie-hellman definitions

this file defines dh
*/

#ifndef __TT_DH_DEF__
#define __TT_DH_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blob.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// set to tt_blob_t::addr
#define TT_DH_GENERATOR_2 ((tt_u8_t *)2)
#define TT_DH_GENERATOR_5 ((tt_u8_t *)5)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_DH_FORMAT_PARAM,
    TT_DH_FORMAT_KEYPAIR,
    TT_DH_FORMAT_PKCS3,
    TT_DH_FORMAT_PKCS8,

    TT_DH_FORMAT_NUM
} tt_dh_format_t;
#define TT_DH_FORMAT_VALID(p) ((p) < TT_DH_FORMAT_NUM)

typedef struct
{
    tt_blob_t prime;
    tt_blob_t generator;
} tt_dh_keydata_param_t;

typedef struct
{
    tt_blob_t prime;
    tt_blob_t generator;
    tt_blob_t pub;
    tt_blob_t priv;
} tt_dh_keydata_keypair_t;

typedef union
{
    tt_dh_keydata_param_t param;
    tt_dh_keydata_keypair_t keypair;
} tt_dh_keydata_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_DH_DEF__ */
