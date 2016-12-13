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
@file tt_rsa_def.h
@brief rsa definitions

this file defines rsa
*/

#ifndef __TT_RSA_DEF__
#define __TT_RSA_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_RSA_SIZE_1024BIT,
    TT_RSA_SIZE_2048BIT,

    TT_RSA_SIZE_NUM
} tt_rsa_size_t;
#define TT_RSA_SIZE_VALID(s) ((s) < TT_RSA_SIZE_NUM)

typedef enum {
    TT_RSA_TYPE_PUBLIC,
    TT_RSA_TYPE_PRIVATE,

    TT_RSA_TYPE_NUM,
    TT_RSA_TYPE_INVALID,
} tt_rsa_type_t;
#define TT_RSA_TYPE_VALID(t) ((t) < TT_RSA_TYPE_NUM)

typedef enum {
    TT_RSA_PADDING_NONE,

    // implies SHA1, MGF1, empty lable
    TT_RSA_PADDING_OAEP,

    // pkcs1-v1.5, max input len: key size - 11
    TT_RSA_PADDING_PKCS1,

    TT_RSA_PADDING_NUM
} tt_rsa_padding_t;
#define TT_RSA_PADDING_VALID(p) ((p) < TT_RSA_PADDING_NUM)

typedef enum {
    // pkcs1 format may be either defined by rfc3447 or wrapped by an
    // oid(rsaEncryption) as used in x509 certificate. the wrapped
    // format is commonly used as user can recognize what kine of the
    // public key is by checking the key content
    TT_RSA_FORMAT_PKCS1,
    TT_RSA_FORMAT_PKCS8,

    TT_RSA_FORMAT_NUM
} tt_rsa_format_t;
#define TT_RSA_FORMAT_VALID(p) ((p) < TT_RSA_FORMAT_NUM)

typedef struct
{
    tt_u32_t reserved;
} tt_rsa_padding_oaep_t;

typedef struct
{
    tt_rsa_padding_t mode;
    union
    {
        tt_rsa_padding_oaep_t oaep;
    };
} tt_rsa_padding_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_RSA_DEF__ */
