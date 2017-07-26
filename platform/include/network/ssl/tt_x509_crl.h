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
@file tt_x509_crl.h
@brief x509 crl

this file defines x509 crl APIs
*/

#ifndef __TT_X509_CRL__
#define __TT_X509_CRL__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <x509_crl.h>

//  //////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_x509crl_s
{
    mbedtls_x509_crl crl;
} tt_x509crl_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_x509crl_init(IN tt_x509crl_t *x);

tt_export tt_result_t tt_x509crl_add(IN tt_x509crl_t *x,
                                     IN tt_u8_t *buf,
                                     IN tt_u32_t len);

tt_export tt_result_t tt_x509crl_add_file(IN tt_x509crl_t *x,
                                          IN const tt_char_t *path);

tt_export void tt_x509crl_destroy(IN tt_x509crl_t *x);

tt_export tt_u32_t tt_x509crl_dump(IN tt_x509crl_t *x,
                                   IN tt_char_t *buf,
                                   IN tt_u32_t len);

#endif /* __TT_X509_CRL__ */
