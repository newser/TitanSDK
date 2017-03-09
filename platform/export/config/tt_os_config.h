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
@file tt_os_config.h
@brief os definitions

this file includes definitions in macros for ts os
*/

#ifndef __TT_OS_CONFIG__
#define __TT_OS_CONFIG__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// ========================================
// mutex
// ========================================

/**
@def TT_MUTEX_DEBUG_OPT
mutex debug options
*/
#define TT_MUTEX_DEBUG_OPT (TT_MUTEX_DEBUG_TAG)
/**
@def TT_MUTEX_DEBUG_TAG
if enabled, name of the function who locked the mutex would be recorded
*/
#define TT_MUTEX_DEBUG_TAG (1 << 0)

// ========================================
// semaphore
// ========================================

/**
@def TT_SEM_DEBUG_OPT
semaphore debug options
*/
#define TT_SEM_DEBUG_OPT (TT_SEM_DEBUG_TAG)
/**
@def TT_SEM_DEBUG_TAG
if enabled, each semaphore would record the locker name
*/
#define TT_SEM_DEBUG_TAG (1 << 1)

// ========================================
// spinlock
// ========================================

/**
@def TT_SPINLOCK_DEBUG_OPT
spinlock debug options
*/
#define TT_SPINLOCK_DEBUG_OPT (TT_SPINLOCK_LOCKER_DEBUG)
/**
@def TT_SPINLOCK_LOCKER_DEBUG
if enabled, name of the function who locked the spinlock would be recorded
*/
#define TT_SPINLOCK_LOCKER_DEBUG (1 << 0)

// ========================================
// read write lock
// ========================================

/**
@def TT_RWLOCK_DEBUG_OPT
read write lock debug options
*/
#define TT_RWLOCK_DEBUG_OPT (TT_RWLOCK_DEBUG_TAG)
/**
@def TT_RWLOCK_DEBUG_TAG
if enabled, name of the function who locked the rwlock would be recorded
*/
#define TT_RWLOCK_DEBUG_TAG (1 << 0)

// ========================================
// atomic
// ========================================

/**
@def TT_ATOMIC_ALIGNMENT_CHECK
enable alignment check
*/
#define TT_ATOMIC_ALIGNMENT_CHECK

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_OS_CONFIG__ */
