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
@file tt_memory_config.h
@brief memory config definitions

this file includes definitions in macros for ts memory
*/

#ifndef __TT_MEMORY_CONFIG__
#define __TT_MEMORY_CONFIG__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

/**
@def TT_MEMORY_TAG_ENABLE
record position where memory is allocated
*/
#ifdef TT_PLATFORM_ENABLE_MEMORY_TAG
#define TT_MEMORY_TAG_ENABLE
#endif

// ========================================
// slab
// ========================================

/**
@def TT_SLAB_DEBUG_OPT
debug option of memory caches
*/
#define TT_SLAB_DEBUG_OPT

// ========================================
// memory pool
// ========================================

/**
@def TT_MEMPOOL_DEBUG_OPT
debug option of memory poll
*/
#define TT_MEMPOOL_DEBUG_OPT

// ===========================================
// page
// ===========================================

/** debug purpose, alloc page by malloc */
#define TT_PAGE_BY_MALLOC

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_MEMORY_CONFIG__ */
