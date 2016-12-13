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

#include <event/tt_event_base.h>

#include <memory/tt_memory_alloc.h>
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

tt_ev_t *tt_ev_create(IN tt_u32_t ev_id,
                      IN tt_u8_t hdr_size,
                      IN tt_u32_t data_size,
                      IN tt_ev_itf_t *itf)
{
    tt_ev_t *ev;

    ev = tt_mem_alloc(sizeof(tt_ev_t) + hdr_size + data_size);
    if (ev == NULL) {
        TT_ERROR("fail to alloc ev");
        return NULL;
    }

    ev->hdr_size = hdr_size;
    ev->inter_thread = TT_FALSE;
    ev->reserved_u16 = 0;

    ev->ev_id = ev_id;
    ev->data_size = data_size;
    ev->itf = itf;

    return ev;
}

void tt_ev_destroy(IN tt_ev_t *ev)
{
    if ((ev->itf != NULL) && (ev->itf->on_destroy != NULL)) {
        ev->itf->on_destroy(ev);
    }

    tt_mem_free(ev);
}

tt_ev_t *tt_ev_copy(IN tt_ev_t *ev)
{
    tt_u32_t size;
    tt_ev_t *new_ev;

    size = sizeof(tt_ev_t) + ev->hdr_size + ev->data_size;
    new_ev = (tt_ev_t *)tt_mem_alloc(size);
    if (new_ev == NULL) {
        TT_ERROR("no mem to copy ev");
        return NULL;
    }

    if ((ev->itf != NULL) && (ev->itf->on_copy != NULL)) {
        // copy ev
        tt_memcpy(new_ev, ev, sizeof(tt_ev_t));

        // copy header and data
        if (TT_OK(ev->itf->on_copy(new_ev, ev))) {
            return new_ev;
        } else {
            tt_mem_free(new_ev);
            return NULL;
        }
    } else {
        tt_memcpy(new_ev, ev, size);
        return new_ev;
    }
}
