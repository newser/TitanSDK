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
@file tt_log_config.h
@brief log definitions

this file includes definitions in macros for ts log system
*/

#ifndef __TT_LOG_CONFIG__
#define __TT_LOG_CONFIG__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// ========================================
// log
// ========================================

/**
@def TT_LOG_ENABLE
enable trace
*/
#define TT_LOG_ENABLE

/**
@def TT_LOG_SYNC
enable trace sync, traces are printed exclusively. this harm performance
but is useful during debugging
*/
#define TT_LOG_SYNC

/**
@def TT_LOG_BUF_INIT_SIZE
if app does not specify log buf size, ts log will use this value as the
initial log buf size
*/
#define TT_LOG_BUF_INIT_SIZE 1000

/**
@def TT_LOG_BUF_MAX_SIZE
if app does not specify log buf size, ts log buf can be expanded but won't
exceed value specified here
*/
#define TT_LOG_BUF_MAX_SIZE (1 << 20) // 1M

// ========================================
// assert
// ========================================

/**
@def TT_ASSERT_ENABLE
enable assertion
*/
#define TT_ASSERT_ENABLE

// ========================================
// reference counter
// ========================================

/**
@def TT_SHOW_REF_COUNTER
show reference counter changes
*/
//#define TT_SHOW_REF_COUNTER

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_LOG_CONFIG__ */
