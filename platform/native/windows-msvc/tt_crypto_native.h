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
@file tt_crypto_native.h
@brief crypto native

this file defines crypto native APIs
*/

#ifndef __TT_CRYPTO_NATIVE__
#define __TT_CRYPTO_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#ifdef TT_PLATFORM_CRYPTO_ENABLE
#include <bcrypt.h>
#endif

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_crypto_init_ntv(IN struct tt_profile_s *profile);

#endif /* __TT_CRYPTO_NATIVE__ */
