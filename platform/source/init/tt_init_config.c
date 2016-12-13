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

#include <init/tt_init_config.h>

#include <init/tt_component.h>
#include <init/tt_config_group.h>
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

tt_cfgnode_t *tt_g_config_root;

tt_cfgnode_t *tt_g_config_platform;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __config_component_init(IN tt_component_t *comp,
                                           IN tt_profile_t *profile);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_config_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __config_component_init,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_CONFIG, "Config", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t __config_component_init(IN tt_component_t *comp,
                                    IN tt_profile_t *profile)
{
    tt_cfggrp_attr_t attr;

    // create config root node:
    //  - empty name
    //  - default interface
    //  - no private data
    //  - no callback
    tt_cfggrp_attr_default(&attr);

    tt_g_config_root = tt_cfggrp_create("", NULL, NULL, NULL, &attr);
    if (tt_g_config_root == NULL) {
        TT_ERROR("fail to create config node: root");
        return TT_FAIL;
    }

    // create config platform node:
    //  - name: "platform"
    //  - default interface
    //  - no private data
    //  - no callback
    tt_cfggrp_attr_default(&attr);
    attr.cnode_attr.brief = "platform configuration";
    attr.cnode_attr.detail = "platform configuration";

    tt_g_config_platform =
        tt_cfggrp_create("platform", NULL, NULL, NULL, &attr);
    if (tt_g_config_platform == NULL) {
        TT_ERROR("fail to create config node: platform");
        return TT_FAIL;
    }

    if (!TT_OK(tt_cfggrp_add(tt_g_config_root, tt_g_config_platform))) {
        TT_ERROR("fail to add config node: platform");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
