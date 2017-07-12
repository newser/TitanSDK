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
@file tt_crypto.h
@brief crypto initialization

this file defines crypto initialization APIs
*/

#ifndef __TT_CRYPTO__
#define __TT_CRYPTO__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <log/tt_log.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define tt_crypto_error(...)                                                   \
    do {                                                                       \
        extern void mbedtls_strerror(int, char *, size_t);                     \
        tt_char_t buf[256];                                                    \
        TT_ERROR(__VA_ARGS__);                                                 \
        mbedtls_strerror(e, buf, sizeof(buf) - 1);                             \
        TT_ERROR("%s", buf);                                                   \
    } while (0)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_crypto_component_register()
register ts crypto component
*/
extern void tt_crypto_component_register();

#endif /* __TT_CRYPTO__ */
