/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/init/component.h>

#include <tt/log/manager.h>
#include <tt/misc/rollback.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

namespace tt {

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

component_mgr component_mgr::s_instance;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

component_mgr::component_mgr()
{
    components_[component::e_log] = &log::log_component::instance();
}

bool component_mgr::start(void *reserved)
{
    int i = 0;

    auto _1 = make_rollback([this, &i]() {
        --i;
        for (; i >= 0; --i) { components_[i]->stop(); }
    });

    for (; i < component::id_num; ++i) {
        if (!components_[i]->start()) { return false; }
    }
    _1.dismiss();

    return true;
}

void component_mgr::stop()
{
    for (int i = 0; i < component::id_num; ++i) { components_[i]->stop(); }
}

}
