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

#include <io/tt_io_poller.h>

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

tt_result_t tt_io_poller_create(IN tt_io_poller_t *iop,
                                IN OPT tt_io_poller_attr_t *attr)
{
    TT_ASSERT(iop != NULL);

    return tt_io_poller_create_ntv(&iop->sys_iop);
}

void tt_io_poller_destroy(IN tt_io_poller_t *iop)
{
    TT_ASSERT(iop != NULL);

    tt_io_poller_destroy_ntv(&iop->sys_iop);
}

void tt_io_poller_attr_default(IN tt_io_poller_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->reserved = 0;
}

void tt_io_poller_run(IN tt_io_poller_t *iop)
{
    do {
    } while (tt_io_poller_run_ntv(&iop->sys_iop, TT_TIME_INFINITE));
}
