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

#include <log/layout/tt_log_layout.h>

#include <memory/tt_memory_alloc.h>

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

tt_loglyt_t *tt_loglyt_create(IN tt_u32_t size, IN tt_loglyt_itf_t *itf)
{
    tt_loglyt_t *ll;

    if ((itf == NULL) || (itf->format == NULL)) {
        return NULL;
    }

    ll = tt_malloc(sizeof(tt_loglyt_t) + size);
    if (ll == NULL) {
        return NULL;
    }

    ll->itf = itf;

    if ((ll->itf->create != NULL) && !TT_OK(ll->itf->create(ll))) {
        tt_free(ll);
        return NULL;
    }

    return ll;
}

void tt_loglyt_destroy(IN tt_loglyt_t *ll)
{
    if (ll == NULL) {
        return;
    }

    if (ll->itf->destroy != NULL) {
        ll->itf->destroy(ll);
    }

    tt_free(ll);
}
