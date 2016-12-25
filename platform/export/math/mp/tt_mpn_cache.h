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
@file tt_mpn_cache.h
@brief multiple precision integer cache

this file includes multiple precision integer cache
*/

#ifndef __TT_MPN_CACHE__
#define __TT_MPN_CACHE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <math/mp/tt_mpn.h>

#include <algorithm/tt_stack.h>
#include <memory/tt_memory_alloc.h>
#include <memory/tt_slab.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_MPN_CACHE_ENABLE

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;

typedef struct
{
    tt_slab_t slab;
    tt_ptrstack_t stack;
} tt_mpn_cache_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_mpn_cache_component_init(IN struct tt_profile_s *profile);

extern tt_result_t tt_mpn_cache_create(IN tt_mpn_cache_t *mpnc);

extern void tt_mpn_cache_destroy(IN tt_mpn_cache_t *mpnc);

tt_inline tt_mpn_t *tt_mpn_alloc(IN tt_mpn_cache_t *mpnc, IN tt_u32_t reserved)
{
#ifdef TT_MPN_CACHE_ENABLE
    tt_mpn_t *p = tt_ptrstack_pop(&mpnc->stack);
    if (p == NULL) {
        p = tt_slab_alloc(&mpnc->slab);
        if (p != NULL) {
            tt_mpn_init(p);
        }
    }
#else
    tt_mpn_t *p = (tt_mpn_t *)tt_malloc(sizeof(tt_mpn_t));
    if (p != NULL) {
        tt_mpn_init(p);
    }
#endif
    return p;
}

tt_inline void tt_mpn_free(IN tt_mpn_cache_t *mpnc, IN tt_mpn_t *p)
{
#ifdef TT_MPN_CACHE_ENABLE
    if (p != NULL) {
        tt_ptrstack_push(&mpnc->stack, p);
    }
#else
    if (p != NULL) {
        tt_mpn_destroy(p);
        tt_free(p);
    }
#endif
}

#endif /* __TT_MPN_CACHE__ */
