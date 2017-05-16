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
@file tt_aes_def.h
@brief aes definitions

this file defines aes
*/

#ifndef __TT_AES_DEF__
#define __TT_AES_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_AES_BLOCK_SIZE 16

#define TT_AES_IV_SIZE 16

#define TT_AES128_KEY_SIZE 16

#define TT_AES256_KEY_SIZE 32

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_AES128,
    TT_AES192,
    TT_AES256,

    TT_AES_KEYBIT_NUM
} tt_aes_keybit_t;
#define TT_AES_KEYBIT_VALID(s) ((s) < TT_AES_KEYBIT_NUM)

typedef enum {
    TT_AES_ECB,
    TT_AES_CBC,
    TT_AES_CFB8,
    TT_AES_CFB128,
    TT_AES_CTR,

    TT_AES_MODE_NUM
} tt_aes_mode_t;
#define TT_AES_MODE_VALID(m) ((m) < TT_AES_MODE_NUM)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif
