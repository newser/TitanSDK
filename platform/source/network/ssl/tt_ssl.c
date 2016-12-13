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

#include <network/ssl/tt_ssl.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <log/tt_log.h>
#include <network/ssl/tt_ssl_cache.h>
#include <os/tt_atomic.h>

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

static tt_result_t __ssl_component_init(IN tt_component_t *comp,
                                        IN tt_profile_t *profile);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_ssl_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __ssl_component_init,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_SSL, "SSL", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

void tt_ssl_stat_show(IN tt_u32_t flag)
{
    TT_INFO("-------------------------------------");
    TT_INFO("[SSL] statistic");

    TT_INFO("- peer:");
    TT_INFO("    current peer number: %d", tt_sslpeer_num());

    TT_INFO("-------------------------------------");
}

tt_result_t __ssl_component_init(IN tt_component_t *comp,
                                 IN tt_profile_t *profile)
{
    tt_result_t result = TT_FAIL;

    // init platform specific
    result = tt_ssl_component_init_ntv();
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
