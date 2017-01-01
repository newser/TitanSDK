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

#include <algorithm/tt_map.h>

#include <misc/tt_assert.h>

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

static tt_result_t __check_map_itf(IN tt_map_itf_t *itf);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_map_t *tt_map_create(IN tt_u32_t size, IN tt_map_itf_t *itf)
{
    tt_map_t *map;

    TT_ASSERT(itf != NULL);

    if (!TT_OK(__check_map_itf(itf))) {
        return NULL;
    }

    map = tt_malloc(sizeof(tt_map_t) + size);
    if (map == NULL) {
        TT_ERROR("no mem to create map");
        return NULL;
    }

    map->itf = itf;

    if ((map->itf->create != NULL) && !TT_OK(map->itf->create(map))) {
        tt_free(map);
        return NULL;
    }

    return map;
}

tt_result_t __check_map_itf(IN tt_map_itf_t *itf)
{
    if (itf->add == NULL) {
        TT_ERROR("no itf->add");
        return TT_FAIL;
    }

    if (itf->clear == NULL) {
        TT_ERROR("no itf->clear");
        return TT_FAIL;
    }

    if (itf->count == NULL) {
        TT_ERROR("no itf->count");
        return TT_FAIL;
    }

    if (itf->find == NULL) {
        TT_ERROR("no itf->find");
        return TT_FAIL;
    }

    if (itf->foreach == NULL) {
        TT_ERROR("no itf->foreach");
        return TT_FAIL;
    }

    if (itf->remove == NULL) {
        TT_ERROR("no itf->remove");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
