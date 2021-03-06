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

#include <time/tt_time_reference.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>

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

static tt_result_t __time_ref_component_init(IN tt_component_t *comp,
                                             IN tt_profile_t *profile);

static void __time_ref_component_exit(IN tt_component_t *comp);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_time_ref_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __time_ref_component_init, __time_ref_component_exit,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_TIME_REF,
                      "Time Reference",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t __time_ref_component_init(IN tt_component_t *comp,
                                      IN tt_profile_t *profile)
{
    if (!TT_OK(tt_time_ref_component_init_ntv())) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void __time_ref_component_exit(IN tt_component_t *comp)
{
    tt_time_ref_component_exit_ntv();
}
