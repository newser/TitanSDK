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
 @file tt_map_hashlist.h
 @brief map: hash list implementation

 this file map: hash list implementation
 */

#ifndef __TT_MAP_HASHLIST__
#define __TT_MAP_HASHLIST__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_hash.h>
#include <algorithm/tt_map.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_hash_alg_t hash_alg;
    tt_bool_t uniq_key : 1;
} tt_map_hl_attr_t;

typedef struct
{
    tt_dlist_t *dll;
    tt_hash_t hash;
    tt_mnode_key_t get_key;
    tt_hashctx_t hashctx;
    tt_u32_t dll_num;
    tt_bool_t uniq_key : 1;
} tt_map_hl_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_map_t *tt_map_hashlist_create(IN tt_mnode_key_t get_key,
                                        IN tt_u32_t list_num,
                                        IN OPT tt_map_hl_attr_t *attr);

extern void tt_map_hashlist_attr_default(IN tt_map_hl_attr_t *attr);

#endif /* __TT_MAP_HASHLIST__ */
