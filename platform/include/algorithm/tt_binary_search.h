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
@file tt_binary_search.h
@brief binary search algorithm

this file defines binary search APIs
*/

#ifndef __TT_BINARY_SEARCH__
#define __TT_BINARY_SEARCH__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_compare.h>

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
@fn void* tt_bsearch(IN void *key,
                     IN void *base,
                     IN tt_u32_t num,
                     IN tt_u32_t width,
                     IN tt_cmp_t cmp)
search a element with key equaling the param

@param [in] key the key value to be matched
@param [in] base base address of element
@param [in] num number of elements
@param [in] width width in bytes of an element
@param [in] cmp the cmp

@return
- the address of the element of which the key value
  equals the param "key"
- NULL if no element matches

@note
- the function assumes that all elements are stored in a
  block of consecutive memory
- the function assumes that the elements has been sorted
*/
tt_export void *tt_bsearch(IN void *key,
                           IN void *base,
                           IN tt_u32_t num,
                           IN tt_u32_t width,
                           IN tt_cmp_t cmp);

/**
@fn void* tt_bsearch_gteq(IN void *key,
                                IN void *base,
                                IN tt_u32_t num,
                                IN tt_u32_t width,
                                IN tt_cmp_t cmp)
search the first element with key value larger than the "key"

@param [in] key the key value to be compared
@param [in] base base address of element
@param [in] num number of elements
@param [in] width width in bytes of an element
@param [in] cmp the cmp

@return
- the address of the first element of which the key value
  is larger than the param "key"
- NULL if no element's key value is larger

@note
- the function assumes that all elements are stored in a
  block of consecutive memory
- the function assumes that the elements has been sorted
- if there is an element with key equaling "key", then its
  position would be returned
*/
tt_export void *tt_bsearch_gteq(IN void *key,
                                IN void *base,
                                IN tt_u32_t num,
                                IN tt_u32_t width,
                                IN tt_cmp_t cmp);

/**
@fn void* tt_bsearch_lteq(IN void *key,
                              IN void *base,
                              IN tt_u32_t num,
                              IN tt_u32_t width,
                              IN tt_cmp_t cmp)
search the last element with key value less than "key"

@param [in] key the key value to be compared
@param [in] base base address of element
@param [in] num number of elements
@param [in] width width in bytes of an element
@param [in] cmp the cmp

@return
- the address of the last element of which the key value
  is less than the param "key"
- NULL if no element's key value is less

@note
- the function assumes that all elements are stored in a
  block of consecutive memory
- the function assumes that the elements has been sorted
- if there is an element with key equaling "key", then its
  position would be returned
*/
tt_export void *tt_bsearch_lteq(IN void *key,
                                IN void *base,
                                IN tt_u32_t num,
                                IN tt_u32_t width,
                                IN tt_cmp_t cmp);

#endif /* __TT_BINARY_SEARCH__ */
