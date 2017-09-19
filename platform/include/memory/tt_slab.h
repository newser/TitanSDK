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
@file tt_slab.h
@brief slab

slab allocator
*/

#ifndef __TT_SLAB__
#define __TT_SLAB__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_double_linked_list.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

/**
@def tt_slab_alloc(slab)
allocate an object from slab

@param [in] slab slab to allocate object

@return
the address of allocated object
*/
#ifdef TT_MEMORY_TAG_ENABLE
#define tt_slab_alloc(slab) tt_slab_alloc_tag((slab), __FUNCTION__, __LINE__)
#else
#define tt_slab_alloc(slab) tt_slab_alloc_tag((slab))
#endif

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

/**
@struct tt_slab_t
attributes of slab memory cache
*/
typedef struct
{
    /**
    @var bulk_num
    the least object number that the slab would buffered

    @note
    if the number is too small, slab may change it
    */
    tt_u32_t bulk_num;

    /**
    @var cache_align
    set to TT_TRUE to make object aligned with hardware cache line.

    when it's turned on (set to TR_TRUE)
    - advantage would be improving performance because false sharing is avoided
    - disadvantage is there may be considerable memory wasted
    */
    tt_bool_t cache_align : 1;
} tt_slab_attr_t;

/**
@struct tt_slab_t
memory cache that store memory objects of fixed size
*/
typedef struct tt_slab_s
{
    /** list of free objects */
    tt_dlist_t obj_list;
    /** allocated pages */
    tt_dlist_t frame_list;
    /** object size */
    tt_u32_t obj_size;
    tt_u32_t obj_num;
    /** frame size */
    tt_u32_t frame_size;
    tt_bool_t cache_align : 1;
} tt_slab_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_slab_component_register()
register ts mempnc slab system
*/
tt_export void tt_slab_component_register();

/**
@fn tt_result_t tt_slab_create(IN tt_slab_t *slab,
                               IN tt_u32_t obj_size,
                               IN struct tt_pgcenter_sched_s *pgcenter_sched,
                               IN tt_slab_attr_t *attr)
create a slab

@param [in] slab slab memory cache to be created
@param [in] obj_size object size
@param [in] pgcenter_sched the cache allocate or free pages
@param [in] attr slab attributes, set to NULL to use default attributes

@return
- TT_SUCCESS, if creating succeeds
- TT_FAIL, otherwise
*/
tt_export tt_result_t tt_slab_create(IN tt_slab_t *slab,
                                     IN tt_u32_t obj_size,
                                     IN OPT tt_slab_attr_t *attr);

/**
@fn tt_result_t tt_slab_destroy(IN tt_slab_t *slab)
destroy a slab memory cache

@param [in] mempnc slab memory cache to be destroyed

@return
- TT_SUCCESS, if destroying succeeds
- TT_FAIL, otherwise

@note
this function is executed without any lock, so be sure no other thread is still
operating on this slab
*/
tt_export void tt_slab_destroy(IN tt_slab_t *slab);

/**
 @fn void tt_slab_attr_default(OUT tt_slab_attr_t *attr)
 initialize the attr with default values

 @param [in] attr slab memory cache attribute
 */
tt_export void tt_slab_attr_default(OUT tt_slab_attr_t *attr);

/**
@fn void* tt_slab_alloc_tag(IN tt_memdb_slab_t *slab)
allocate an object from slab memory cache

@param [in] slab slab memory cache to allocate object

@return
- object address
- NULL, otherwise
*/
tt_export void *tt_slab_alloc_tag(IN tt_slab_t *slab
#ifdef TT_MEMORY_TAG_ENABLE
                                  ,
                                  IN const tt_char_t *func,
                                  IN tt_u32_t line
#endif
                                  );

/**
@fn void tt_slab_free(IN void *obj)
free an object to its slab memory cache

@param [in] obj object to be freed

@return
- object address
- NULL, otherwise
*/
tt_export void tt_slab_free(IN void *obj);

#endif /* __TT_SLAB__ */
