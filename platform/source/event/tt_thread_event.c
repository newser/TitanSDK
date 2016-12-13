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

#include <event/tt_thread_event.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>

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

void tt_thread_ev_attr_default(OUT tt_thread_ev_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->on_thread_ev = NULL;
}

tt_ev_t *tt_thread_ev_create(IN tt_u32_t ev_id,
                             IN tt_u32_t data_size,
                             IN tt_ev_itf_t *itf)
{
    tt_ev_t *ev;
    tt_thread_ev_t *tev;

    // event
    ev = tt_ev_create(ev_id, sizeof(tt_thread_ev_t), data_size, itf);
    if (ev == NULL) {
        return NULL;
    }

    ev->inter_thread = TT_TRUE;

    // thread event
    tev = TT_EV_HDR(ev, tt_thread_ev_t);

    tt_lnode_init(&tev->node);

    return ev;
}
