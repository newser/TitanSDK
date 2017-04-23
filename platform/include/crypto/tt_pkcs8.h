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
@file tt_pkcs8.h
@brief pkcs8

this file defines pkcs8 api
*/

#ifndef __TT_PKCS8__
#define __TT_PKCS8__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blob.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/*
Note:
- the encrypted pkcs8 private key must be in PBES2 format and
  only aes-128-cbc and aes-256-cbc are supported, openssl can
  generate the expected format private key by param "-v2 aes-128-cbc"
  or "-v2 aes-256-cbc" of command "openssl pkcs8"
- plaintext->len should be larger than ciphertext->len
*/
extern tt_result_t tt_pkcs8_decrypt(IN tt_blob_t *ciphertext,
                                    IN tt_blob_t *password,
                                    OUT tt_u8_t *plaintext,
                                    IN OUT tt_u32_t *plaintext_len);

#endif /* __TT_PKCS8__ */
