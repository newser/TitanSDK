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

#include <algorithm/tt_list.h>
#include <os/tt_spinlock.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

/**
@def tt_mempool_alloc(mempool, size)
allocate an block from memory pool

@param [in] mempool memory pool to allocate
@param [in] size required size

@return
the address of allocated block
*/
#ifdef TT_MEMORY_TAG_ENABLE
#define tt_mempool_alloc(mempool, size)                                        \
    tt_mempool_alloc_tag((mempool), (size), __FUNCTION__, __LINE__)
#else
#define tt_mempool_alloc(mempool, size) tt_mempool_alloc_tag((mempool), (size))
#endif

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    /**
    @var max_area_num
    limitation of area number, so max size of pool would be
    (max_expand_time * pagenum_per_expand) pages
    - TT_MEMPOOL_AREA_NOLIMIT, no limitation
    */
    tt_u32_t max_area_num;
/** no limitation on area number */
#define TT_MEMPOOL_AREA_NOLIMIT 0

    tt_bool_t hwcache_align : 1;
    tt_bool_t sync : 1;
} tt_mempool_attr_t;

typedef struct tt_mempool_s
{
    tt_mempool_attr_t attr;

    tt_u32_t area_size;
    tt_list_t area_list;
    tt_lnode_t *last_area;

    tt_spinlock_t lock;
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
extern void tt_mempool_component_register();

/**
@fn tt_result_t tt_mempool_create(IN tt_mempool_t *mempool,
                                  IN tt_u32_t pagenum_per_expand,
                                  IN struct tt_pgcenter_sched_s
*pgcenter_sched,
                                  IN tt_mempool_attr_t *attr)
create a memory pool

@param [inout] mempool the pool to be created
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
extern tt_result_t tt_mempool_create(IN tt_mempool_t *mempool,
                                     IN tt_u32_t max_block_size,
                                     IN tt_mempool_attr_t *attr);

/**
@fn void tt_mempool_destroy(IN tt_mempool_t *mempool)
destroy a memory pool

@param [in] mempool memory pool to be destroyed

@note
- be sure all memory blocks are returned, or the behavior is undetermined
*/
extern void tt_mempool_destroy(IN tt_mempool_t *mempool, IN tt_bool_t force);

/**
 @fn void tt_mempool_attr_default(OUT tt_mempool_attr_t *attr)
 initialize the attr with default values

 @param [in] attr memory pool attribute
 */
extern void tt_mempool_attr_default(OUT tt_mempool_attr_t *attr);

/**
@fn void* tt_mempool_alloc_tag(IN tt_mempool_t *mempool,
                               IN tt_u32_t size)
allocate memory from memory pool

@param [in] mempool the pool to be created
@param [in] size size to be allocated

@return
- the address of the memory block allocated
- NULL otherwise
*/
extern void *tt_mempool_alloc_tag(IN tt_mempool_t *mempool,
                                  IN tt_u32_t size
#ifdef TT_MEMORY_TAG_ENABLE
                                  ,
                                  IN const tt_char_t *function,
                                  IN tt_u32_t line
#endif
                                  );

/**
@fn void tt_mempool_free(IN void* pointer)
free memory

@param [in] pointer memory block ever allocated from the pool
*/
extern void tt_mempool_free(IN void *pointer);

/**
@fn void tt_mempool_show(IN tt_mempool_t *mempool, IN tt_u32_t option)
show memory pool information

@param [in] mempool memory pool to be shown
@param [in] option options to show memory pool
*/
extern void tt_mempool_show(IN tt_mempool_t *mempool);

#endif /* __TT_MEMORY_POOL__ */
