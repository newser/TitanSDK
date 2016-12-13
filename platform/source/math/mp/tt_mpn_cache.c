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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <math/mp/tt_mpn_cache.h>

#include <init/tt_profile.h>
#include <log/tt_log.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __MPN_CACHE_BULK 32

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __mpn_instk_destroy(IN void *obj);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_mpn_cache_component_init(IN tt_profile_t *profile)
{
    return TT_SUCCESS;
}

tt_result_t tt_mpn_cache_create(IN tt_mpn_cache_t *mpnc)
{
    tt_slab_t *slab;
    tt_slab_attr_t slab_attr;
    tt_ptrstack_t *stack;
    tt_stack_attr_t stack_attr;
    tt_result_t result;

    TT_ASSERT(mpnc != NULL);

    // bn slab
    slab = &mpnc->slab;

    tt_slab_attr_default(&slab_attr);
    slab_attr.objnum_per_expand = __MPN_CACHE_BULK;

    result = tt_slab_create(slab, sizeof(tt_mpn_t), &slab_attr);
    if (!TT_OK(result)) {
        TT_ERROR("fail to create bn slab");
        return TT_FAIL;
    }

    // bn stack
    stack = &mpnc->stack;

    tt_stack_attr_default(&stack_attr);
    stack_attr.destroy_obj_when_push_fail = TT_TRUE;
    stack_attr.obj_destroy = __mpn_instk_destroy;

    result = tt_ptrstack_create(stack, __MPN_CACHE_BULK, &stack_attr);
    if (!TT_OK(result)) {
        TT_ERROR("fail to create bn stack");

        tt_slab_destroy(slab, TT_FALSE);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_mpn_cache_destroy(IN tt_mpn_cache_t *mpnc)
{
    TT_ASSERT(mpnc != NULL);

    // must destroy stack before the slab
    tt_ptrstack_destroy(&mpnc->stack);

    tt_slab_destroy(&mpnc->slab, TT_FALSE);
}

void __mpn_instk_destroy(IN void *obj)
{
    tt_mpn_t *b = *((tt_mpn_t **)obj);

    tt_mpn_destroy(b);
    tt_slab_free(b);
}
