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

#include <tt/log/manager.h>

#include <tt/log/io/standard.h>
#include <tt/log/layout/pattern.h>
#include <tt/misc/rollback.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

namespace tt {

namespace log {

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

mgr *g_log_mgr;

log_component log_component::s_instance;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

bool log_component::do_start(void *reserved)
{
    assert(g_log_mgr == nullptr);
    g_log_mgr = new tt::log::mgr();
    auto _1 = make_rollback([]() { delete g_log_mgr; });

    auto p_debug = new pattern();
    if (!p_debug->parse("${content} <${function} - ${line}>\n")) {
        return false;
    }
    g_log_mgr->layout(e_debug, std::shared_ptr<i_layout>(p_debug));

    auto p_info = new pattern();
    if (!p_info->parse("${content}\n")) { return false; }
    g_log_mgr->layout(e_info, std::shared_ptr<i_layout>(p_info));

#if 0
    auto p_warn = new pattern();
    if (!p_warn->parse(
            "${time} ${level:%-6.6s} ${content} <${function} - ${line}>\n")) {
        return false;
    }
    g_log_mgr->layout(e_warn, std::shared_ptr<i_layout>(p_warn));

    auto p_error = new pattern();
    if (!p_error->parse(
            "${time} ${level:%-6.6s} ${content} <${function} - ${line}>\n")) {
        return false;
    }
    g_log_mgr->layout(e_error, std::shared_ptr<i_layout>(p_error));

    auto p_fatal = new pattern();
    if (!p_fatal->parse(
            "${time} ${level:%-6.6s} ${content} <${function} - ${line}>\n")) {
        return false;
    }
    g_log_mgr->layout(e_fatal, std::shared_ptr<i_layout>(p_fatal));
#else
    auto p_warn = new pattern();
    if (!p_warn->parse(
            "${level:%-6.6s} ${content} <${function} - ${line}>\n")) {
        return false;
    }
    g_log_mgr->layout(e_warn, std::shared_ptr<i_layout>(p_warn));

    auto p_error = new pattern();
    if (!p_error->parse(
            "${level:%-6.6s} ${content} <${function} - ${line}>\n")) {
        return false;
    }
    g_log_mgr->layout(e_error, std::shared_ptr<i_layout>(p_error));

    auto p_fatal = new pattern();
    if (!p_fatal->parse(
            "${level:%-6.6s} ${content} <${function} - ${line}>\n")) {
        return false;
    }
    g_log_mgr->layout(e_fatal, std::shared_ptr<i_layout>(p_fatal));
#endif

    g_log_mgr->append_io(std::shared_ptr<i_io>(new standard()));

    _1.dismiss();
    return true;
}

void log_component::do_stop()
{
    assert(g_log_mgr != nullptr);
    delete g_log_mgr;
}

}

}
