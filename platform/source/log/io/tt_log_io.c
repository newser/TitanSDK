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

#include <log/io/tt_log_io.h>

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

tt_logio_t *tt_logio_create(IN tt_u32_t size, IN tt_logio_itf_t *itf)
{
    tt_logio_t *lio;

    lio = (tt_logio_t *)tt_mem_alloc(sizeof(tt_logio_t) + size);
    if (lio == NULL) {
        return NULL;
    }

    lio->itf = itf;

    if ((lio->itf->create != NULL) && !TT_OK(lio->itf->create(lio))) {
        tt_mem_free(lio);
        return NULL;
    }

    return lio;
}

void tt_logio_destroy(IN tt_logio_t *lio)
{
    if (lio->itf->destroy != NULL) {
        lio->itf->destroy(lio);
    }

    tt_mem_free(lio);
}
