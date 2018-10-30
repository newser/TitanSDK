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

#include <log/tt_log_init.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <log/io/tt_log_io.h>
#include <log/io/tt_log_io_standard.h>
#include <log/layout/tt_log_layout.h>
#include <log/layout/tt_log_layout_pattern.h>
#include <log/tt_log.h>

#include <param/tt_param.h>
#include <param/tt_param_u32.h>

#include <tt_log_native.h>

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
tt_loglyt_t *tt_s_loglyt[TT_LOG_LEVEL_NUM];

// log io
tt_logio_t *tt_s_logio[TT_LOG_LEVEL_NUM];

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __log_component_init(IN tt_component_t *comp,
                                        IN tt_profile_t *profile);

static void __log_component_exit(IN tt_component_t *comp);

static tt_result_t __logmgr_component_init(IN tt_component_t *comp,
                                           IN tt_profile_t *profile);

static void __logmgr_component_exit(IN tt_component_t *comp);

static tt_result_t __logmgr_config_component_init(IN tt_component_t *comp,
                                                  IN tt_profile_t *profile);

static void __logmgr_config_component_exit(IN tt_component_t *comp);

static tt_result_t __create_log_layout(IN tt_profile_t *profile);

static void __destroy_log_layout();

static tt_result_t __install_log_layout(IN tt_profile_t *profile);

static tt_result_t __create_log_io(IN tt_profile_t *profile);

static void __destroy_log_io();

static tt_result_t __install_log_io(IN tt_profile_t *profile);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_log_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __log_component_init, __log_component_exit,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_LOG, "Log", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

void tt_logmgr_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {__logmgr_component_init, __logmgr_component_exit};

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

    tt_component_itf_t itf = {__logmgr_config_component_init,
                              __logmgr_config_component_exit};

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_LOGMGR_CONFIG,
                      "Log Configuration",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

void tt_logmgr_layout_default(IN tt_logmgr_t *lmgr)
{
    tt_logmgr_set_layout(&tt_g_logmgr, TT_LOG_DEBUG, tt_s_loglyt[TT_LOG_DEBUG]);
    tt_logmgr_set_layout(&tt_g_logmgr, TT_LOG_INFO, tt_s_loglyt[TT_LOG_INFO]);
    tt_logmgr_set_layout(&tt_g_logmgr, TT_LOG_WARN, tt_s_loglyt[TT_LOG_WARN]);
    tt_logmgr_set_layout(&tt_g_logmgr, TT_LOG_ERROR, tt_s_loglyt[TT_LOG_ERROR]);
    tt_logmgr_set_layout(&tt_g_logmgr, TT_LOG_FATAL, tt_s_loglyt[TT_LOG_FATAL]);
}

tt_result_t tt_logmgr_io_default(IN tt_logmgr_t *lmgr)
{
    if (!TT_OK(tt_logmgr_append_io(lmgr,
                                   TT_LOG_DEBUG,
                                   tt_s_logio[TT_LOG_DEBUG]))) {
        TT_ERROR("fail to append debug io");
        return TT_FAIL;
    }

    if (!TT_OK(
            tt_logmgr_append_io(lmgr, TT_LOG_INFO, tt_s_logio[TT_LOG_INFO]))) {
        TT_ERROR("fail to append debug io");
        return TT_FAIL;
    }

    if (!TT_OK(
            tt_logmgr_append_io(lmgr, TT_LOG_WARN, tt_s_logio[TT_LOG_WARN]))) {
        TT_ERROR("fail to append warn io");
        return TT_FAIL;
    }

    if (!TT_OK(tt_logmgr_append_io(lmgr,
                                   TT_LOG_ERROR,
                                   tt_s_logio[TT_LOG_ERROR]))) {
        TT_ERROR("fail to append error io");
        return TT_FAIL;
    }

    if (!TT_OK(tt_logmgr_append_io(lmgr,
                                   TT_LOG_FATAL,
                                   tt_s_logio[TT_LOG_FATAL]))) {
        TT_ERROR("fail to append fatal io");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __log_component_init(IN tt_component_t *comp,
                                 IN tt_profile_t *profile)
{
    if (!TT_OK(tt_log_component_init_ntv(profile))) {
        TT_ERROR("fail to initialize log system native");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void __log_component_exit(IN tt_component_t *comp)
{
    tt_log_component_exit_ntv();
}

tt_result_t __logmgr_component_init(IN tt_component_t *comp,
                                    IN tt_profile_t *profile)
{
    if (!TT_OK(tt_logmgr_create(&tt_g_logmgr, NULL, NULL))) {
        TT_ERROR("fail to create global log manager");
        return TT_FAIL;
    }

    // log layout
    if (!TT_OK(__create_log_layout(profile))) {
        tt_logmgr_destroy(&tt_g_logmgr);
        return TT_FAIL;
    }
    tt_logmgr_layout_default(&tt_g_logmgr);

    // log io
    if (!TT_OK(__create_log_io(profile))) {
        __destroy_log_layout();
        tt_logmgr_destroy(&tt_g_logmgr);
        return TT_FAIL;
    }

    if (!TT_OK(tt_logmgr_io_default(&tt_g_logmgr))) {
        __destroy_log_io();
        __destroy_log_layout();
        tt_logmgr_destroy(&tt_g_logmgr);
        return TT_FAIL;
    }

    // enable all levels
    tt_logmgr_set_level(&tt_g_logmgr, TT_LOG_DEBUG);

    tt_g_logmgr_ok = TT_TRUE;

    return TT_SUCCESS;
}

void __logmgr_component_exit(IN tt_component_t *comp)
{
    tt_g_logmgr_ok = TT_FALSE;

    tt_logmgr_destroy(&tt_g_logmgr);
    __destroy_log_layout();
    __destroy_log_io();
}

tt_result_t __logmgr_config_component_init(IN tt_component_t *comp,
                                           IN tt_profile_t *profile)
{
#if 0
    tt_param_t *cnode;
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
#endif

    return TT_SUCCESS;
}

void __logmgr_config_component_exit(IN tt_component_t *comp)
{
}

tt_result_t __create_log_layout(IN tt_profile_t *profile)
{
    tt_loglyt_t *lyt;

    // debug log layout
    if (tt_s_loglyt[TT_LOG_DEBUG] == NULL) {
        lyt = tt_loglyt_pattern_create("${content} <${function} - ${line}>\n");
        if (lyt == NULL) {
            TT_ERROR("fail to create debug log pattern\n");
            goto fail;
        }
        tt_s_loglyt[TT_LOG_DEBUG] = lyt;
    }

    // info log layout
    if (tt_s_loglyt[TT_LOG_INFO] == NULL) {
        lyt = tt_loglyt_pattern_create("${content}\n");
        if (lyt == NULL) {
            TT_ERROR("fail to create info log pattern\n");
            goto fail;
        }
        tt_s_loglyt[TT_LOG_INFO] = lyt;
    }

    // warn log layout
    if (tt_s_loglyt[TT_LOG_WARN] == NULL) {
        lyt = tt_loglyt_pattern_create(
            "${time} ${level:%-6.6s} ${content} <${function} - ${line}>\n");
        if (lyt == NULL) {
            TT_ERROR("fail to create warn log pattern\n");
            goto fail;
        }
        tt_s_loglyt[TT_LOG_WARN] = lyt;
    }

    // error log layout
    if (tt_s_loglyt[TT_LOG_ERROR] == NULL) {
        lyt = tt_loglyt_pattern_create(
            "${time} ${level:%-6.6s} ${content} <${function} - ${line}>\n");
        if (lyt == NULL) {
            TT_ERROR("fail to create error log pattern\n");
            goto fail;
        }
        tt_s_loglyt[TT_LOG_ERROR] = lyt;
    }

    // fatal log layout
    if (tt_s_loglyt[TT_LOG_FATAL] == NULL) {
        lyt = tt_loglyt_pattern_create(
            "${time} ${level:%-6.6s} ${content} <${function} - ${line}>\n");
        if (lyt == NULL) {
            TT_ERROR("fail to create fatal log pattern\n");
            goto fail;
        }
        tt_s_loglyt[TT_LOG_FATAL] = lyt;
    }

    return TT_SUCCESS;

fail:

    if (tt_s_loglyt[TT_LOG_DEBUG] != NULL) {
        tt_loglyt_release(tt_s_loglyt[TT_LOG_DEBUG]);
    }

    if (tt_s_loglyt[TT_LOG_INFO] != NULL) {
        tt_loglyt_release(tt_s_loglyt[TT_LOG_INFO]);
    }

    if (tt_s_loglyt[TT_LOG_WARN] != NULL) {
        tt_loglyt_release(tt_s_loglyt[TT_LOG_WARN]);
    }

    if (tt_s_loglyt[TT_LOG_ERROR] != NULL) {
        tt_loglyt_release(tt_s_loglyt[TT_LOG_ERROR]);
    }

    if (tt_s_loglyt[TT_LOG_FATAL] != NULL) {
        tt_loglyt_release(tt_s_loglyt[TT_LOG_FATAL]);
    }

    return TT_FAIL;
}

void __destroy_log_layout()
{
    // debug log layout
    if (tt_s_loglyt[TT_LOG_DEBUG] != NULL) {
        tt_loglyt_release(tt_s_loglyt[TT_LOG_DEBUG]);
    }

    // info log layout
    if (tt_s_loglyt[TT_LOG_INFO] != NULL) {
        tt_loglyt_release(tt_s_loglyt[TT_LOG_INFO]);
    }

    // warn log layout
    if (tt_s_loglyt[TT_LOG_WARN] != NULL) {
        tt_loglyt_release(tt_s_loglyt[TT_LOG_WARN]);
    }

    // error log layout
    if (tt_s_loglyt[TT_LOG_ERROR] != NULL) {
        tt_loglyt_release(tt_s_loglyt[TT_LOG_ERROR]);
    }

    // fatal log layout
    if (tt_s_loglyt[TT_LOG_FATAL] != NULL) {
        tt_loglyt_release(tt_s_loglyt[TT_LOG_FATAL]);
    }
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

    if (tt_s_logio[TT_LOG_DEBUG] == NULL) {
        tt_s_logio[TT_LOG_DEBUG] = lio;
        tt_logio_ref(lio);
    }

    if (tt_s_logio[TT_LOG_INFO] == NULL) {
        tt_s_logio[TT_LOG_INFO] = lio;
        tt_logio_ref(lio);
    }

    if (tt_s_logio[TT_LOG_WARN] == NULL) {
        tt_s_logio[TT_LOG_WARN] = lio;
        tt_logio_ref(lio);
    }

    if (tt_s_logio[TT_LOG_ERROR] == NULL) {
        tt_s_logio[TT_LOG_ERROR] = lio;
        tt_logio_ref(lio);
    }

    if (tt_s_logio[TT_LOG_FATAL] == NULL) {
        tt_s_logio[TT_LOG_FATAL] = lio;
        tt_logio_ref(lio);
    }

    tt_logio_release(lio);
    return TT_SUCCESS;
}

void __destroy_log_io()
{
    if (tt_s_logio[TT_LOG_DEBUG] != NULL) {
        tt_logio_release(tt_s_logio[TT_LOG_DEBUG]);
    }

    if (tt_s_logio[TT_LOG_INFO] != NULL) {
        tt_logio_release(tt_s_logio[TT_LOG_INFO]);
    }

    if (tt_s_logio[TT_LOG_WARN] != NULL) {
        tt_logio_release(tt_s_logio[TT_LOG_WARN]);
    }

    if (tt_s_logio[TT_LOG_ERROR] != NULL) {
        tt_logio_release(tt_s_logio[TT_LOG_ERROR]);
    }

    if (tt_s_logio[TT_LOG_FATAL] != NULL) {
        tt_logio_release(tt_s_logio[TT_LOG_FATAL]);
    }
}
