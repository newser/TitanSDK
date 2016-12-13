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
@file tt_hmac_def.h
@brief hmac definitions

this file defines HMAC
*/

#ifndef __TT_HMAC_DEF__
#define __TT_HMAC_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_HMAC_SHA1_DIGEST_LENGTH 20
#define TT_HMAC_SHA256_DIGEST_LENGTH 32
#define TT_HMAC_SHA512_DIGEST_LENGTH 64

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_HMAC_VER_SHA1,

    TT_HMAC_VER_NUM
} tt_hmac_ver_t;
#define TT_HMAC_VER_VALID(v) ((v) < TT_HMAC_VER_NUM)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_HMAC_DEF__ */
