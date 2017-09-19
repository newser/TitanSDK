/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
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

#include <init/tt_component.h>
#include <init/tt_profile.h>
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

static tt_result_t __iop_component_init(IN tt_component_t *comp,
                                        IN tt_profile_t *profile);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_io_poller_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __iop_component_init,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_IO_POLLER, "IO Poller", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

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

tt_result_t __iop_component_init(IN tt_component_t *comp,
                                 IN tt_profile_t *profile)
{
    if (!TT_OK(tt_io_poller_component_init_ntv())) {
        TT_ERROR("fail to init native io poller component");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
