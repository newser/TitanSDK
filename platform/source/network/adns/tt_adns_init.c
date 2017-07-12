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

#include <network/adns/tt_adns_init.h>

#include <config/tt_platform_config.h>
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

static tt_result_t __adns_component_init(IN struct tt_component_s *comp,
                                         IN tt_profile_t *profile);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_adns_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __adns_component_init,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_MODULE_ADNS,
                      "Module: Async DNS",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t __adns_component_init(IN struct tt_component_s *comp,
                                  IN tt_profile_t *profile)
{
    return TT_SUCCESS;
}
