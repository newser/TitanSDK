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

#include <log/tt_log_init.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <log/io/tt_log_io.h>
#include <log/io/tt_log_io_standard.h>
#include <log/layout/tt_log_layout.h>
#include <log/layout/tt_log_layout_pattern.h>
#include <log/tt_log.h>

#include <init/tt_config_u32.h>
#include <init/tt_init_config.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_logmgr_t tt_g_logmgr;

tt_bool_t tt_g_logmgr_ok = TT_FALSE;

// log layout
static tt_loglyt_t *tt_s_loglyt[TT_LOG_LEVEL_NUM];

// log io
static tt_logio_t *tt_s_logio_std;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __logmgr_component_init(IN tt_component_t *comp,
                                           IN tt_profile_t *profile);
static tt_result_t __logmgr_config_component_init(IN tt_component_t *comp,
                                                  IN tt_profile_t *profile);

static tt_result_t __create_log_layout(IN tt_profile_t *profile);
static tt_result_t __install_log_layout(IN tt_profile_t *profile);

static tt_result_t __create_log_io(IN tt_profile_t *profile);
static tt_result_t __install_log_io(IN tt_profile_t *profile);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_log_component_register()
{
}

void tt_logmgr_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __logmgr_component_init,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_LOG_MANAGER,
                      "Log Manager",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

void tt_logmgr_config_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __logmgr_config_component_init,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_LOGMGR_CONFIG,
                      "Log Configuration",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t __logmgr_component_init(IN tt_component_t *comp,
                                    IN tt_profile_t *profile)
{
    if (!TT_OK(tt_logmgr_create(&tt_g_logmgr, NULL, NULL))) {
        TT_ERROR("fail to create global log manager");
        return TT_FAIL;
    }

    // log layout
    if (!TT_OK(__create_log_layout(profile)) ||
        !TT_OK(__install_log_layout(profile))) {
        return TT_FAIL;
    }

    // log io
    if (!TT_OK(__create_log_io(profile)) || !TT_OK(__install_log_io(profile))) {
        return TT_FAIL;
    }

    // enable all levels
    tt_logmgr_set_level(&tt_g_logmgr, TT_LOG_DEBUG);

    tt_g_logmgr_ok = TT_TRUE;

    return TT_SUCCESS;
}

tt_result_t __logmgr_config_component_init(IN tt_component_t *comp,
                                           IN tt_profile_t *profile)
{
    tt_cfgnode_t *cnode;
    tt_cfgu32_attr_t attr;

    // create log level node:
    //  - name: log-level
    //  - default interface
    //  - no private data
    //  - no callback
    tt_cfgu32_attr_default(&attr);
    attr.cnode_attr.brief = "global log level";
    attr.cnode_attr.detail = "todo";
    attr.mode = TT_CFGVAL_MODE_GS;

    cnode = tt_cfgu32_create("log-level",
                             NULL,
                             NULL,
                             (tt_u32_t *)&tt_g_logmgr.level,
                             NULL,
                             &attr);
    if (cnode == NULL) {
        TT_ERROR("fail to create config node: log-level");
        return TT_FAIL;
    }
    if (!TT_OK(tt_config_add2plat(cnode))) {
        TT_ERROR("fail to add config node: log-level");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __create_log_layout(IN tt_profile_t *profile)
{
    tt_loglyt_t *lyt;

    // debug log layout
    lyt = tt_loglyt_pattern_create("${content} <${function} - ${line}>\n");
    if (lyt == NULL) {
        TT_ERROR("fail to create debug log pattern\n");
        return TT_FAIL;
    }
    tt_s_loglyt[TT_LOG_DEBUG] = lyt;

    // info log layout
    lyt = tt_loglyt_pattern_create("${content}\n");
    if (lyt == NULL) {
        TT_ERROR("fail to create info log pattern\n");
        return TT_FAIL;
    }
    tt_s_loglyt[TT_LOG_INFO] = lyt;

    // warn log layout
    lyt = tt_loglyt_pattern_create("${time} ${level:%-6.6s} ${content}\n");
    if (lyt == NULL) {
        TT_ERROR("fail to create warn log pattern\n");
        return TT_FAIL;
    }
    tt_s_loglyt[TT_LOG_WARN] = lyt;

    // error log layout
    lyt = tt_loglyt_pattern_create("${time} ${level:%-6.6s} ${content}\n");
    if (lyt == NULL) {
        TT_ERROR("fail to create error log pattern\n");
        return TT_FAIL;
    }
    tt_s_loglyt[TT_LOG_ERROR] = lyt;

    // fatal log layout
    lyt = tt_loglyt_pattern_create("${time} ${level:%-6.6s} ${content}\n");
    if (lyt == NULL) {
        TT_ERROR("fail to create fatal log pattern\n");
        return TT_FAIL;
    }
    tt_s_loglyt[TT_LOG_FATAL] = lyt;

    return TT_SUCCESS;
}

tt_result_t __install_log_layout(IN tt_profile_t *profile)
{
    tt_logmgr_set_layout(&tt_g_logmgr, TT_LOG_DEBUG, tt_s_loglyt[TT_LOG_DEBUG]);
    tt_logmgr_set_layout(&tt_g_logmgr, TT_LOG_INFO, tt_s_loglyt[TT_LOG_INFO]);
    tt_logmgr_set_layout(&tt_g_logmgr, TT_LOG_WARN, tt_s_loglyt[TT_LOG_WARN]);
    tt_logmgr_set_layout(&tt_g_logmgr, TT_LOG_ERROR, tt_s_loglyt[TT_LOG_ERROR]);
    tt_logmgr_set_layout(&tt_g_logmgr, TT_LOG_FATAL, tt_s_loglyt[TT_LOG_FATAL]);

    return TT_SUCCESS;
}

tt_result_t __create_log_io(IN tt_profile_t *profile)
{
    tt_logio_t *lio;

    // standard io
    lio = tt_logio_std_create(NULL);
    if (lio == NULL) {
        TT_ERROR("fail to create std lio\n");
        return TT_FAIL;
    }
    tt_s_logio_std = lio;

    return TT_SUCCESS;
}

tt_result_t __install_log_io(IN tt_profile_t *profile)
{
    if (!TT_OK(tt_logmgr_append_io(&tt_g_logmgr,
                                   TT_LOG_LEVEL_NUM,
                                   tt_s_logio_std))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
