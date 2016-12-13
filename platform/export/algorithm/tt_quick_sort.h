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
@file tt_quick_sort.h
@brief quick sort algorithm

this file defines quick sort algorithm APIs
*/

#ifndef __TT_QUICK_SORT__
#define __TT_QUICK_SORT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_algorithm_common.h>

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
@fn void tt_qsort(IN void *base,
                       IN tt_u32_t num,
                       IN tt_u32_t width,
                       IN tt_cmp_t cmp)
sort a element array

@param [in] base start address of element
@param [in] num number of elements
@param [in] width size in bytes of an element
@param [in] cmp the cmp

@return
void
*/
extern void tt_qsort(IN void *base,
                     IN tt_u32_t num,
                     IN tt_u32_t width,
                     IN tt_cmp_t cmp);

#endif /* __TT_QUICK_SORT__ */
