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
@file tt_environment_config_native.h
@brief environment configuration native definitions

this file load environment configurations
*/

#ifndef __TT_ENVIRONMENT_CONFIG_NATIVE__
#define __TT_ENVIRONMENT_CONFIG_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <TargetConditionals.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// ========================================
// running os
// ========================================

#if TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
#define ___IOS_SIMULATOR TT_ENV_OS_FEATURE_IOS_SIMULATOR
#else
#define ___IOS_SIMULATOR 0
#endif

#define TT_ENV_OS_FEATURE_NATIVE (___IOS_SIMULATOR)

// ========================================
// running cpu
// ========================================

// ========================================
// building toolchain
// ========================================

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
