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
@file tt_page.h
@brief memory page APIs

APIs to allocate/free memory pages from/to os
*/

#ifndef __TT_PAGE_OS__
#define __TT_PAGE_OS__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

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

/**
@fn void tt_page_component_register()
register numa memory component
*/
tt_export void tt_page_component_register();

/**
@fn void *tt_page_alloc(IN tt_u32_t size)
allocate pages from numa node, number of pages is specified as param

@param [in] size size in bytes

@return
- beginning address of allocated pages
- NULL, otherwise
*/
tt_export void *tt_page_alloc(IN tt_u32_t size);

/**
@fn void tt_page_free(IN void *page_begin,
                      IN tt_u32_t size)
free pages, start address of pages is specified by param

@param [in] page_begin the pages to be freed
@param [in] size size in bytes

@note
- the address in param must be allocated by tt_page_alloc
- size must be same as passed to tt_page_alloc
- if freeing fails, just an error trace would be printed
*/
tt_export void tt_page_free(IN void *addr, IN tt_u32_t size);

/**
@fn void *tt_page_alloc_align(IN tt_u32_t size_order,
                                OUT tt_uintptr_t *handle)
allocate pages, number of pages is specified as param and
address returned would be aligned with total size required

@param [in] size_order order of size in bytes of required pages
@param [in] handle a handle to be passed to free_aligned()

@return
- beginning address of allocated pages
- NULL, otherwise

@note
as an example, if size_order is 14, meaning required (1 << 14) bytes,
then allocated page would always begin with some address aligned with
(1 << 14)
*/
tt_export void *tt_page_alloc_align(IN tt_u32_t size_order,
                                    OUT tt_uintptr_t *handle);

/**
@fn void tt_page_free_align(IN void *page_begin,
                              IN tt_u32_t size_order,
                              IN tt_uintptr_t handle)
free pages, start address of pages is specified by param

@param [in] page_begin the pages to be freed
@param [in] size_order order of size in bytes of pages to be freed
@param [in] handle handle returned by alloc_aligned()

@note
- the address in param must be allocated by tt_page_alloc_align
- page_num_order must be same as passed to tt_page_alloc_align
- if freeing fails, just an error trace would be printed
*/
tt_export void tt_page_free_align(IN void *addr,
                                  IN tt_u32_t size_order,
                                  IN tt_uintptr_t handle);

#endif // __TT_PAGE_OS__
