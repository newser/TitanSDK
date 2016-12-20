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

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_loglyt_t *tt_loglyt_create(IN tt_u32_t size,
                              IN OPT const tt_char_t *logger,
                              IN tt_loglyt_itf_t *itf)
{
    tt_loglyt_t *lyt;

    TT_ASSERT(itf != NULL);
    TT_ASSERT(itf->format != NULL);

    lyt = tt_mem_alloc(sizeof(tt_loglyt_t) + size);
    if (lyt == NULL) {
        TT_ERROR("no mem for log layout");
        return NULL;
    }

    lyt->logger = logger;
    lyt->itf = itf;

    if ((lyt->itf->create != NULL) && !TT_OK(lyt->itf->create(lyt))) {
        TT_ERROR("fail to create log layout");
        tt_mem_free(lyt);
        return NULL;
    }

    return lyt;
}

void tt_loglyt_destroy(IN tt_loglyt_t *lyt)
{
    TT_ASSERT(lyt != NULL);

    if (lyt->itf->destroy != NULL) {
        lyt->itf->destroy(lyt);
    }

    tt_mem_free(lyt);
}
