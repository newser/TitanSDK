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
@file tt_environment_config_native.h
@brief environment configuration native definitions

this file load environment configurations
*/

#ifndef __TT_ENVIRONMENT_CONFIG_NATIVE__
#define __TT_ENVIRONMENT_CONFIG_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// ========================================
// app running os
// ========================================

#define TT_ENV_OS_VER_WINDOWS_VISTA (TT_ENV_OS_TYPE_WINDOWS | (1 & 0xFF))
#define TT_ENV_OS_VER_WINDOWS_7 (TT_ENV_OS_TYPE_WINDOWS | (2 & 0xFF))
#define TT_ENV_OS_VER_WINDOWS_8 (TT_ENV_OS_TYPE_WINDOWS | (3 & 0xFF))
#define TT_ENV_OS_VER_WINDOWS_10 (TT_ENV_OS_TYPE_WINDOWS | (4 & 0xFF))

// ========================================
// app running cpu
// ========================================

// ========================================
// app building toolchain
// ========================================

#define TT_ENV_TOOLCHAIN_MSVC_2010 (TT_ENV_TOOLCHAIN_VER_UNKNOWN + 1)
#define TT_ENV_TOOLCHAIN_MSVC_2012 (TT_ENV_TOOLCHAIN_VER_UNKNOWN + 2)
#define TT_ENV_TOOLCHAIN_MSVC_2013 (TT_ENV_TOOLCHAIN_VER_UNKNOWN + 3)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_ENVIRONMENT_CONFIG_NATIVE__ */
