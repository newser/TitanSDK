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
@file tt_spinlock_native_cs.h
@brief spin lock implemented by windows critical section

this files defines system APIs of spin lock implemented by windows critical
section
*/

#ifndef __TT_SPINLOCK_NATIVE__
#define __TT_SPINLOCK_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// ========================================
// configuration
// ========================================

// these configurations would not be put to tt_platform_config.h

/**
@def TT_SPINLOCK_NTV_TYPE_CS
spin lock is implemented as a windows critical section
*/
#define TT_SPINLOCK_NTV_TYPE_CS 1
/**
@def TT_SPINLOCK_NTV_TYPE_UL
spin lock is implemented by pure spinning at user level
*/
#define TT_SPINLOCK_NTV_TYPE_UL 2

/**
@def TT_SPINLOCK_NTV_TYPE
this macro determines the spin lock implementation, currently it could be:
- TT_SPINLOCK_NTV_TYPE_CS
- TT_SPINLOCK_NTV_TYPE_UL

@note
"ul" runs much fast then "cs" after compiler optimizing it
*/
#define TT_SPINLOCK_NTV_TYPE TT_SPINLOCK_NTV_TYPE_UL

// ========================================
// macro wrappers
// ========================================

#if (TT_SPINLOCK_NTV_TYPE == TT_SPINLOCK_NTV_TYPE_CS)

#include <tt_spinlock_native_cs.h>

#define tt_spinlock_ntv_t tt_spinlock_ntv_cs_t

#define tt_spinlock_component_init_ntv tt_spinlock_component_init_ntv_cs

#define tt_spinlock_create_ntv tt_spinlock_create_ntv_cs
#define tt_spinlock_destroy_ntv tt_spinlock_destroy_ntv_cs

#define tt_spinlock_acquire_ntv tt_spinlock_acquire_ntv_cs
#define tt_spinlock_try_acquire_ntv tt_spinlock_try_acquire_ntv_cs
#define tt_spinlock_release_ntv tt_spinlock_release_ntv_cs

#elif (TT_SPINLOCK_NTV_TYPE == TT_SPINLOCK_NTV_TYPE_UL)

#include <tt_spinlock_native_ul.h>

#define tt_spinlock_ntv_t tt_spinlock_ntv_ul_t

#define tt_spinlock_component_init_ntv tt_spinlock_component_init_ntv_ul

#define tt_spinlock_create_ntv tt_spinlock_create_ntv_ul
#define tt_spinlock_destroy_ntv tt_spinlock_destroy_ntv_ul

#define tt_spinlock_acquire_ntv tt_spinlock_acquire_ntv_ul
#define tt_spinlock_try_acquire_ntv tt_spinlock_try_acquire_ntv_ul
#define tt_spinlock_release_ntv tt_spinlock_release_ntv_ul

#else

#error "unknown spin lock implementation"

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

#endif // __TT_SPINLOCK_NATIVE__
