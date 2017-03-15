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

#include <os/tt_poller.h>

#include <misc/tt_assert.h>
#include <os/tt_fiber.h>

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

tt_result_t tt_poller_create(IN tt_poller_t *poller,
                             IN OPT tt_poller_attr_t *attr)
{
    TT_ASSERT(poller != NULL);

    return tt_poller_create_ntv(&poller->sys_poller);
}

void tt_poller_destroy(IN tt_poller_t *poller)
{
    TT_ASSERT(poller != NULL);

    tt_poller_destroy_ntv(&poller->sys_poller);
}

void tt_poller_attr_default(IN tt_poller_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->reserved = 0;
}

tt_result_t tt_poller_run(IN tt_poller_t *poller)
{
    tt_poller_ev_t ev;
    void *data;

    /*while (TT_OK(tt_poller_run_ntv(poller, &ev, &data))) {
        switch (ev) {
            case TT_POLLER_EV_TIMER: {
            } break;
            case TT_POLLER_EV_FILE: {
            } break;

            default: {
                TT_FATAL("unknown event: %d", ev);
                tt_throw_exception_ntv(NULL);
                return TT_FAIL;
            } break;
        }
    }*/

    return TT_SUCCESS;
}
