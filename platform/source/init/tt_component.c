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

#include <init/tt_component.h>

#include <init/tt_profile.h>
#include <log/tt_log.h>
#include <misc/tt_assert.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_component_t *comp;

    tt_bool_t started;
} __comp_status_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_bool_t __comp_table_initialized = TT_FALSE;
static __comp_status_t __comp_table[TT_COMPONENT_NUM];

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_component_init(IN tt_component_t *comp,
                       IN tt_component_id_t cid,
                       IN const tt_char_t *name,
                       IN void *opaque,
                       IN tt_component_itf_t *itf)
{
    tt_memset(comp, 0, sizeof(tt_component_t));

    comp->cid = cid;
    comp->name = name;
    comp->opaque = opaque;

    tt_memcpy(&comp->itf, itf, sizeof(tt_component_itf_t));
}

void tt_component_register(IN tt_component_t *comp)
{
    TT_ASSERT(TT_COMPONENT_ID_VALID(comp->cid));
    TT_ASSERT(comp->itf.init != NULL);

    if (!__comp_table_initialized) {
        tt_u32_t i;
        for (i = 0; i < TT_COMPONENT_NUM; ++i) {
            __comp_table[i].comp = NULL;

            __comp_table[i].started = TT_FALSE;
        }

        __comp_table_initialized = TT_TRUE;
    }

    if (__comp_table[comp->cid].comp == NULL) {
        __comp_table[comp->cid].comp = comp;
    } else {
        TT_ERROR("component[%s] has already registered\n", comp->name);
    }
}

tt_result_t tt_component_start(IN tt_profile_t *profile)
{
    tt_u32_t i;

    for (i = 0; i < TT_COMPONENT_NUM; ++i) {
        __comp_status_t *comp_status = &__comp_table[i];
        tt_component_t *comp = comp_status->comp;

        if (!comp_status->started && (comp != NULL)) {
            if (TT_OK(comp->itf.init(comp, profile))) {
                TT_INFO("Intializing %-32s [Done]", comp->name);
                __comp_table[i].started = TT_TRUE;
            } else {
                TT_INFO("Intializing %-32s [Fail]", comp->name);
                return TT_FAIL;
            }
        }
    }

    return TT_SUCCESS;
}

tt_component_t *tt_component_find_id(IN tt_component_id_t cid)
{
    if (TT_COMPONENT_ID_VALID(cid)) {
        return __comp_table[cid].comp;
    } else {
        return NULL;
    }
}

tt_component_t *tt_component_find_name(IN const tt_char_t *name)
{
    tt_u32_t i;

    for (i = 0; i < TT_COMPONENT_NUM; ++i) {
        tt_component_t *comp = __comp_table[i].comp;
        if ((comp != NULL) && (tt_strcmp(comp->name, name) == 0)) {
            return comp;
        }
    }

    return NULL;
}

tt_bool_t tt_component_is_started(IN tt_component_id_t cid)
{
    TT_ASSERT(TT_COMPONENT_ID_VALID(cid));

    return __comp_table[cid].started;
}
