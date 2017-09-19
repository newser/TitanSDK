/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
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
@file tt_x509_cert.h
@brief x509 certificate

this file defines x509 certificate APIs
*/

#ifndef __TT_X509_CERT__
#define __TT_X509_CERT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <x509_crt.h>

//  //////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_x509crl_s;

typedef struct tt_x509cert_s
{
    mbedtls_x509_crt crt;
} tt_x509cert_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_x509cert_init(IN tt_x509cert_t *x);

tt_export tt_result_t tt_x509cert_add(IN tt_x509cert_t *x,
                                      IN tt_u8_t *buf,
                                      IN tt_u32_t len);

tt_export tt_result_t tt_x509cert_add_file(IN tt_x509cert_t *x,
                                           IN const tt_char_t *path);

tt_export void tt_x509cert_destroy(IN tt_x509cert_t *x);

tt_export tt_result_t tt_x509cert_verify(IN tt_x509cert_t *x,
                                         IN tt_x509cert_t *ca,
                                         IN OPT struct tt_x509crl_s *crl,
                                         IN OPT const tt_char_t *name,
                                         OUT tt_u32_t *status);

tt_export tt_u32_t tt_x509cert_dump_verify_status(IN tt_u32_t status,
                                                  IN tt_char_t *buf,
                                                  IN tt_u32_t len);

tt_export tt_u32_t tt_x509cert_dump(IN tt_x509cert_t *x,
                                    IN tt_char_t *buf,
                                    IN tt_u32_t len);

#endif /* __TT_X509_CERT__ */
