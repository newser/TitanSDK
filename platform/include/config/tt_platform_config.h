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
@file tt_platform_config.h
@brief platform configurations

this file defines integrate all other configurations
*/

#ifndef __TT_PLATFORM_CONFIG__
#define __TT_PLATFORM_CONFIG__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

// cmake
//  ||
//  \/
// pre-definition: collect user definitions
//  - tt_environment_config.h: environment, like cpu instruction set
//  - tt_module_config.h: module config
//  - tt_customization_config.h: all others
//  ||
//  \/
// definition: component definitions according to user input
//  - tt_algorithm_config.h
//  - ...
//  ||
//  \/
// post-definition: overwrite defitions if necessary
//  - tt_platform_optimized_config.h
//  - ...

// ========================================
// pre-definition
// ========================================

#include <config/tt_customization_config.h>
#include <config/tt_environment_config.h>

// ========================================
// definition
// ========================================

#include <config/tt_algorithm_config.h>
#include <config/tt_file_system_config.h>
#include <config/tt_log_config.h>
#include <config/tt_memory_config.h>
#include <config/tt_mpn_config.h>
#include <config/tt_os_config.h>

// ========================================
// post-definition
// ========================================

#ifndef TT_PLATFORM_ENABLE_DEBUG
#include <config/tt_platform_optimized_config.h>
#endif

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

#endif /* __TT_PLATFORM_CONFIG__ */
