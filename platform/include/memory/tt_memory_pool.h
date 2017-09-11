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
@file tt_memory_pool.h
@brief memory pool

this file specifies interfaces for memory pool operations
*/

#ifndef __TT_MEMORY_POOL__
#define __TT_MEMORY_POOL__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_single_linked_list.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    // 0 indicates no limit
    tt_u32_t max_pool_size;
} tt_mempool_attr_t;

typedef struct tt_mempool_s
{
    tt_slist_t frame_list;
    tt_u32_t frame_num;
    tt_u32_t frame_size;
    tt_u32_t max_frame_num;
} tt_mempool_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_mempool_component_register()
register ts memory pool system
*/
tt_export void tt_mempool_component_register();

/**
@fn tt_result_t tt_mempool_create(IN tt_mempool_t *mp,
                                  IN tt_u32_t pagenum_per_expand,
                                  IN struct tt_pgcenter_sched_s
*pgcenter_sched,
                                  IN tt_mempool_attr_t *attr)
create a memory pool

@param [inout] mp the pool to be created
@param [in] pagenum_per_expand number of pages each time expanding the pool
@param [in] pgcenter_sched base page center scheduler
- use default page center scheduler (on numa node 0) if NULL
@param [in] attr attributes of the memory pool
- use default setting if NULL

@return
- TT_SUCCESS, if if pool is created succefully
- TT_FAIL, otherwise

@note
the total available size in pool may be less than specified due to some
overhead of memory block allocated each time and alignment required in attr
*/
tt_export void tt_mempool_init(IN tt_mempool_t *mp,
                               IN tt_u32_t max_block_size,
                               IN OPT tt_mempool_attr_t *attr);

/**
@fn void tt_mempool_destroy(IN tt_mempool_t *mp)
destroy a memory pool

@param [in] mp memory pool to be destroyed

@note
- be sure all memory blocks are returned, or the behavior is undetermined
*/
tt_export void tt_mempool_destroy(IN tt_mempool_t *mp);

/**
 @fn void tt_mempool_attr_default(OUT tt_mempool_attr_t *attr)
 initialize the attr with default values

 @param [in] attr memory pool attribute
 */
tt_export void tt_mempool_attr_default(OUT tt_mempool_attr_t *attr);

/**
@fn void* tt_mp_alloc_tag(IN tt_mempool_t *mp,
                               IN tt_u32_t size)
allocate memory from memory pool

@param [in] mp the pool to be created
@param [in] size size to be allocated

@return
- the address of the memory block allocated
- NULL otherwise
*/
tt_export void *tt_mp_alloc(IN tt_mempool_t *mp, IN tt_u32_t size);

#endif /* __TT_MEMORY_POOL__ */
