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

#include <misc/tt_crash_trace.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <log/tt_log.h>

#include <tt_crash_trace_native.h>

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

static tt_result_t __ct_component_init(IN tt_component_t *comp,
                                       IN tt_profile_t *profile);

static void __ct_component_exit(IN tt_component_t *comp);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_crash_trace_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __ct_component_init, __ct_component_exit,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_CRASH_TRACE,
                      "Crash Trace",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t __ct_component_init(IN tt_component_t *comp,
                                IN tt_profile_t *profile)
{
    if (!TT_OK(tt_crash_trace_component_init_ntv(profile))) {
        TT_ERROR("fail to initialize crash trace native");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void __ct_component_exit(IN tt_component_t *comp)
{
    tt_crash_trace_component_exit_ntv();
}
