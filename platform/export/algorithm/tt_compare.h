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
@file tt_compare
@brief compare type
 */

#ifndef __TT_COMPARE__
#define __TT_COMPARE__

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

/**
 @typedef tt_s32_t (*tt_cmp_t)(IN void* l, IN void* r)
 a compare function type

 @param [IN] l left object
 @param [IN] r right object

 @return
 - >0 if "l" > "r"
 - 0 if "l" == "r"
 - <0 if "l" < "r"
 */
typedef tt_s32_t (*tt_cmp_t)(IN void *l, IN void *r);

/**
 @typedef tt_s32_t (*tt_cmpkey_t)(IN void* n,
 IN const tt_u8_t *key,
 IN tt_u32_t key_len)
 compare node with key

 @param [IN] n node in container
 @param [IN] key key
 @param [IN] key_len length of key in bytes

 @return
 - >0 if "n" > "key"
 - 0 if "n" == "key"
 - <0 if "n" < "key"
 */
typedef tt_s32_t (*tt_cmpkey_t)(IN void *n,
                                IN const tt_u8_t *key,
                                IN tt_u32_t key_len);

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_COMPARE__ */
