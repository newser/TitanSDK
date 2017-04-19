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
@file tt_environment_config.h
@brief environment definitions

this file load environment definitions. tt_environment_def_native.h
only has macro definitions and detail detecting tool would choose
features from those macros and assemble them into this file.
*/

// clang-format off
#ifndef __TT_ENVIRONMENT_CONFIG__
#define __TT_ENVIRONMENT_CONFIG__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_environment_config_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_ENV_UNKNOWN_VAL 0

// basic os information
#define TT_ENV_OS_TYPE_WINDOWS (1 << 24)
#define TT_ENV_OS_TYPE_LINUX (2 << 24)
#define TT_ENV_OS_TYPE_MACOS (3 << 24)
#define TT_ENV_OS_TYPE_IOS (4 << 24)

#define TT_ENV_OS_TYPE(t) ((t) & ~0xFFFFFF)
#define TT_ENV_OS_IS_WINDOWS \
    (TT_ENV_OS_TYPE(TT_ENV_OS) == TT_ENV_OS_TYPE_WINDOWS)
#define TT_ENV_OS_IS_LINUX \
    (TT_ENV_OS_TYPE(TT_ENV_OS) == TT_ENV_OS_TYPE_LINUX)
#define TT_ENV_OS_IS_MACOS \
    (TT_ENV_OS_TYPE(TT_ENV_OS) == TT_ENV_OS_TYPE_MACOS)
#define TT_ENV_OS_IS_IOS \
    (TT_ENV_OS_TYPE(TT_ENV_OS) == TT_ENV_OS_TYPE_IOS)

// basic cpu information
#define TT_ENV_CPU_TYPE_X86 (1 << 24)
#define TT_ENV_CPU_TYPE_ARM (2 << 24)

#define TT_ENV_CPU_TYPE(t) ((t) & ~0xFFFFFF)
#define TT_ENV_CPU_IS_X86 \
    (TT_ENV_CPU_TYPE(TT_ENV_CPU) == TT_ENV_CPU_TYPE_X86)
#define TT_ENV_CPU_IS_ARM \
    (TT_ENV_CPU_TYPE(TT_ENV_CPU) == TT_ENV_CPU_TYPE_ARM)

// basic toolchain information
#define TT_ENV_TOOLCHAIN_MSVC (1 << 24)
#define TT_ENV_TOOLCHAIN_GNU (2 << 24)
#define TT_ENV_TOOLCHAIN_XCODE (3 << 24)

#define TT_ENV_TOOLCHAIN_TYPE(t) ((t) & ~0xFFFFFF)
#define TT_ENV_TOOLCHAIN_IS_MSVC \
    (TT_ENV_TOOLCHAIN_TYPE(TT_ENV_TOOLCHAIN) == TT_ENV_TOOLCHAIN_MSVC)
#define TT_ENV_TOOLCHAIN_IS_GNU \
    (TT_ENV_TOOLCHAIN_TYPE(TT_ENV_TOOLCHAIN) == TT_ENV_TOOLCHAIN_GNU)
#define TT_ENV_TOOLCHAIN_IS_XCODE \
    (TT_ENV_TOOLCHAIN_TYPE(TT_ENV_TOOLCHAIN) == TT_ENV_TOOLCHAIN_XCODE)

// ========================================
// app running os
// ========================================

/**
@def TT_ENV_OS
app running os id
*/
#define TT_ENV_OS TS_ENV_UNKNOWN_VAL

// if detail checking is not enabled, use macro to detect
#if (TT_ENV_OS == TT_ENV_UNKNOWN_VAL)
#undef TT_ENV_OS

#if defined(_WIN64) || defined(_WIN32) || defined(WIN32)
    #define TT_ENV_OS TT_ENV_OS_TYPE_WINDOWS
#elif defined(__APPLE__)
    
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        #define TT_ENV_OS TT_ENV_OS_TYPE_IOS
    #else
        #define TT_ENV_OS TT_ENV_OS_TYPE_MACOS
    #endif

#elif defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
    #define TT_ENV_OS TT_ENV_OS_TYPE_LINUX
#else
    #error unknown os
#endif

#endif

/**
@def TT_ENV_OS_VER
app running os version
*/
#define TT_ENV_OS_VER TS_ENV_UNKNOWN_VAL

/**
@def TT_ENV_OS_FEATURE
app running os features
*/
#define TT_ENV_OS_FEATURE TS_ENV_UNKNOWN_VAL

// ========================================
// app building toochain
// ========================================

/**
@def TT_ENV_TOOLCHAIN
app building toochain
*/
#define TT_ENV_TOOLCHAIN TS_ENV_UNKNOWN_VAL

// if detail checking is not enabled, set default toolchain
#if (TT_ENV_TOOLCHAIN == TT_ENV_UNKNOWN_VAL)
#undef TT_ENV_TOOLCHAIN

#if defined(_MSC_VER)
    #define TT_ENV_TOOLCHAIN TT_ENV_TOOLCHAIN_MSVC
#elif defined(__clang__)
    #define TT_ENV_TOOLCHAIN TT_ENV_TOOLCHAIN_XCODE
#elif defined(__GNUC__)
    #define TT_ENV_TOOLCHAIN TT_ENV_TOOLCHAIN_GNU
#else
    #error unknown toolchain
#endif

#endif

/**
@def TT_ENV_TOOLCHAIN_VER
app building toochain version
*/
#if defined(_MSC_VER)
    #if ((_MSC_VER >= 1600) && (_MSC_VER < 1700))
        #define TT_ENV_TOOLCHAIN_VER TT_ENV_TOOLCHAIN_MSVC_2010
    #elif ((_MSC_VER >= 1700) && (_MSC_VER < 1800))
        #define TT_ENV_TOOLCHAIN_VER TT_ENV_TOOLCHAIN_MSVC_2012
    #elif ((_MSC_VER >= 1800) && (_MSC_VER < 1900))
        #define TT_ENV_TOOLCHAIN_VER TT_ENV_TOOLCHAIN_MSVC_2013
    #elif ((_MSC_VER >= 1900) && (_MSC_VER < 2000))
        #define TT_ENV_TOOLCHAIN_VER TT_ENV_TOOLCHAIN_MSVC_2015
    #elif ((_MSC_VER >= 2000) && (_MSC_VER < 2100))
        #define TT_ENV_TOOLCHAIN_VER TT_ENV_TOOLCHAIN_MSVC_2017
    #else
        #define TT_ENV_TOOLCHAIN_VER TS_ENV_UNKNOWN_VAL
    #endif
#else
#define TT_ENV_TOOLCHAIN_VER TS_ENV_UNKNOWN_VAL
#endif

// ========================================
// app running cpu
// ========================================

/**
@def TT_ENV_CPU
app running cpu
*/
#define TT_ENV_CPU TS_ENV_UNKNOWN_VAL

// if detail checking is not enabled, use macro to detect
#if (TT_ENV_CPU == TT_ENV_UNKNOWN_VAL)
#undef TT_ENV_CPU

#if TT_ENV_TOOLCHAIN_IS_MSVC

    #if defined(_M_AMD64) || defined(_M_IX86) || defined(_M_X64)
        #define TT_ENV_CPU TT_ENV_CPU_TYPE_X86
    #elif defined(_M_ARM) || defined(_M_ARMT)
        #define TT_ENV_CPU TT_ENV_CPU_TYPE_ARM
    #else
        #error unknown cpu
    #endif

#elif TT_ENV_TOOLCHAIN_IS_XCODE || TT_ENV_TOOLCHAIN_IS_GNU

    #if defined(i386) || defined(__i386) || defined(__i386__) || \
        defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(__amd64__)
        #define TT_ENV_CPU TT_ENV_CPU_TYPE_X86
    #elif defined(__arm__) || defined(__arm64__)
        #define TT_ENV_CPU TT_ENV_CPU_TYPE_ARM
    #else
        #error unknown cpu
    #endif

#else
    #error unknown toolchain
#endif

#endif

/**
@def TT_ENV_CPU_FEATURE
app running cpu feature
*/
#define TT_ENV_CPU_FEATURE TS_ENV_UNKNOWN_VAL

// ========================================
// misc
// ========================================

// 64bit
#if TT_ENV_CPU_IS_X86
    #if TT_ENV_TOOLCHAIN_IS_MSVC
        #if defined(_WIN64)
            #define TT_ENV_IS_64BIT 1
        #else
            #define TT_ENV_IS_64BIT 0
        #endif
    #elif TT_ENV_TOOLCHAIN_IS_XCODE || TT_ENV_TOOLCHAIN_IS_GNU
        #if defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(__amd64__)
            #define TT_ENV_IS_64BIT 1
        #else
            #define TT_ENV_IS_64BIT 0
        #endif
    #endif
#elif TT_ENV_CPU_IS_ARM
    #if TT_ENV_TOOLCHAIN_IS_MSVC
        #if defined(_WIN64)
            #define TT_ENV_IS_64BIT 1
        #else
            #define TT_ENV_IS_64BIT 0
        #endif
    #elif TT_ENV_TOOLCHAIN_IS_XCODE || TT_ENV_TOOLCHAIN_IS_GNU
        #if defined(__arm64__)
            #define TT_ENV_IS_64BIT 1
        #else
            #define TT_ENV_IS_64BIT 0
        #endif
    #endif
#endif

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_ENVIRONMENT_CONFIG__ */
// clang-format on