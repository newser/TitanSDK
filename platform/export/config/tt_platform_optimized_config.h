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
@file tt_platform_optimized_config.h
@brief platform optimized definitions

this file defines performance optimized definitions in macros for ts
*/

#ifndef __TT_PLATFORM_OPTIMIZED_CONFIG__
#define __TT_PLATFORM_OPTIMIZED_CONFIG__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// ========================================
// algorithm config
// ========================================

#ifdef TT_ARRAY_HEAP_SANITY_CHECK
#undef TT_ARRAY_HEAP_SANITY_CHECK
#endif

#ifdef TT_RBTREE_SANITY_CHECK
#undef TT_RBTREE_SANITY_CHECK
#endif

// ========================================
// log config
// ========================================

#ifdef TT_LOG_SYNC
#undef TT_LOG_SYNC
#endif

// ========================================
// os config
// ========================================

#ifdef TT_ATOMIC_ALIGNMENT_CHECK
#undef TT_ATOMIC_ALIGNMENT_CHECK
#endif

// ========================================
// memory config
// ========================================

#ifdef TT_MEMORY_TAG_ENABLE
#undef TT_MEMORY_TAG_ENABLE
#endif

// ========================================
// match config
// ========================================

#ifdef TT_MPN_SANITY_CHECK
#undef TT_MPN_SANITY_CHECK
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

#endif /* __TT_PLATFORM_OPTIMIZED_CONFIG__ */
