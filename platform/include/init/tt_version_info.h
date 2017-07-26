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
@file tt_version_info.h
@brief version information of titan sdk

version information of titan sdk
*/

#ifndef __TT_VERSION_INFO__
#define __TT_VERSION_INFO__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// ========================================
// static version check
// ========================================

// check major number
#ifdef TT_VERSION_REQUIRE_MAJOR

#if (TT_VERSION_MAJOR != TT_VERSION_REQUIRE_MAJOR)
#error major number changed
#endif

#endif

// check minor number
#ifdef TT_VERSION_REQUIRE_MINOR

#if (TT_VERSION_MINOR != TT_VERSION_REQUIRE_MINOR)
#error minor number changed
#endif

#endif

// no need to check revsion and build number

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef tt_u32_t tt_ver_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_ver_component_register()
register ts version component
*/
tt_export void tt_ver_component_register();

tt_export tt_ver_t tt_ver_major();

tt_export tt_ver_t tt_ver_minor();

tt_export tt_ver_t tt_ver_revision();

tt_export const tt_char_t *tt_version_cstr();

tt_export void tt_ver_format(IN tt_char_t *buf,
                             IN tt_u32_t buf_len,
                             IN tt_u32_t how);
// how
#define TT_VER_FORMAT_BASIC 0 // major.minor
#define TT_VER_FORMAT_STANDARD 1 // major.minor.revision
#define TT_VER_FORMAT_FULL 2 // major.minor.revision(build)

// ========================================
// dynamic version check
// ========================================

tt_export tt_result_t tt_ver_require_major(IN tt_ver_t major);

tt_export tt_result_t tt_ver_require_minor(IN tt_ver_t minor);

tt_export tt_result_t tt_ver_require(IN tt_ver_t major, IN tt_ver_t minor);

#endif /* __TT_VERSION_INFO__ */
