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

#include <misc/tt_distinguished_name.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_distname_init(IN tt_distname_t *dn)
{
    TT_ASSERT(dn != NULL);

    tt_list_init(&dn->rdn_list);
}

void tt_distname_destroy(IN tt_distname_t *dn)
{
    tt_lnode_t *node;

    TT_ASSERT(dn != NULL);

    while ((node = tt_list_pophead(&dn->rdn_list)) != NULL) {
        tt_rdn_t *attr = TT_CONTAINER(node, tt_rdn_t, node);

        tt_mem_free(attr);
    }
}

tt_result_t tt_distname_add_rdn(IN tt_distname_t *dn,
                                IN tt_rdn_type_t type,
                                IN const tt_char_t *value)
{
    tt_rdn_t *rdn;
    tt_u32_t len;

    TT_ASSERT(dn != NULL);
    TT_ASSERT(TT_RDN_TYPE_VALID(type));
    TT_ASSERT(value != NULL);

    len = (tt_u32_t)tt_strlen(value) + 1;
    rdn = (tt_rdn_t *)tt_mem_alloc(sizeof(tt_rdn_t) + len);
    if (rdn == NULL) {
        TT_ERROR("no mem for dn attr");
        return TT_FAIL;
    }

    tt_lnode_init(&rdn->node);

    rdn->type = type;
    rdn->value = TT_PTR_INC(const tt_char_t, rdn, sizeof(tt_rdn_t));
    tt_memcpy((tt_u8_t *)rdn->value, value, len);

    return TT_SUCCESS;
}
