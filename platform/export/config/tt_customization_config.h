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
@file tt_customization_config.h
@brief customization definitions

this file load customization definitions
*/

// clang-format off
#ifndef __TT_CUSTOMIZATION_CONFIG__
#define __TT_CUSTOMIZATION_CONFIG__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

/**
@def TT_PLATFORM_DEBUG_ENABLE
defined if debug mode is enabled
*/
#define TTCM_PLATFORM_DEBUG_ENABLE
#ifdef TTCM_PLATFORM_DEBUG_ENABLE
#define TT_PLATFORM_DEBUG_ENABLE
#endif

/**
@def TT_PLATFORM_DYNAMIC_LIB
defined if build platform as a shared library
*/
/* #undef TTCM_PLATFORM_BUILD_DYNAMIC */
#ifdef TTCM_PLATFORM_BUILD_DYNAMIC
#define TT_PLATFORM_DYNAMIC_LIB
#endif

/**
@def TT_PLATFORM_SSL_ENABLE
defined if SSL is enabled
*/
/* #undef TTCM_PLATFORM_SSL_ENABLE */
#ifdef TTCM_PLATFORM_SSL_ENABLE
#define TT_PLATFORM_SSL_ENABLE
#endif

/**
@def TT_PLATFORM_NUMA_ENABLE
defined if NUMA is enabled
*/
/* #undef TTCM_PLATFORM_NUMA_ENABLE */
#ifdef TTCM_PLATFORM_NUMA_ENABLE
#define TT_PLATFORM_NUMA_ENABLE
#endif

/**
@def TT_PLATFORM_CRYPTO_ENABLE
defined if CRYPTO is enabled
*/
/* #undef TTCM_PLATFORM_CRYPTO_ENABLE */
#ifdef TTCM_PLATFORM_CRYPTO_ENABLE
#define TT_PLATFORM_CRYPTO_ENABLE
#endif

// ========================================
// version
// ========================================

/**
@def TT_VERSION_MARJOR
major version number

@note
major version number is changed when the new version is not
compatible with last one
*/
#define TT_VERSION_MAJOR ((tt_ver_t)(0))

/**
@def TT_VERSION_MINOR
minor version number

@note
minor version number is changed when some changes have been
made while it keeps compatibility
*/
#define TT_VERSION_MINOR ((tt_ver_t)(1))

/**
@def TT_VERSION_REVISION
revision version number

@note
revision version number is changed when some trivial changes
are made
*/
#define TT_VERSION_REVISION ((tt_ver_t)(0))

/**
@def TT_VERSION_BUILD
build version number

@note
build version number merely indicates software is rebuilt while
while nearly no change is made
*/
#define TT_VERSION_BUILD __DATE__

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_CUSTOMIZATION_CONFIG__ */
// clang-format on
