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

#include <log/tt_log.h>

#include <init/tt_component.h>
#include <init/tt_config_u32.h>
#include <init/tt_init_config.h>
#include <init/tt_profile.h>
#include <log/io/tt_log_io.h>
#include <log/io/tt_log_io_standard.h>
#include <os/tt_spinlock.h>

#include <tt_cstd_api.h>

#include <stdarg.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __MAX_LIO_NUM (1)

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// tt_logmgr_t tt_g_logmgr;

// log io
static tt_logio_t *tt_s_logio_std;

static tt_u32_t tt_g_log_level;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __log_component_init(IN tt_component_t *comp,
                                        IN tt_profile_t *profile);
static tt_result_t __sync_log_component_init(IN tt_component_t *comp,
                                             IN tt_profile_t *profile);
static tt_result_t __log_config_component_init(IN tt_component_t *comp,
                                               IN tt_profile_t *profile);

static tt_result_t __create_log_fmt(IN tt_profile_t *profile);

static tt_result_t __create_log_io(IN tt_profile_t *profile);
static tt_result_t __install_log_io(IN tt_profile_t *profile);

static tt_result_t __enable_sync_log(IN tt_profile_t *profile);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_log_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __log_component_init,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_LOG, "Log", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

void tt_sync_log_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __sync_log_component_init,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_SYNC_LOG,
                      "Synchronous Log",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

void tt_log_config_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __log_config_component_init,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_LOG_CONFIG,
                      "Log Configuration",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t __log_component_init(IN tt_component_t *comp,
                                 IN tt_profile_t *profile)
{
    tt_result_t result;

#if 0
    // log manager
    result = tt_logmgr_create(&tt_g_logmgr, NULL);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    // log format
    result = __create_log_fmt(profile);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    // log io
    result = __create_log_io(profile);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    result = __install_log_io(profile);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    // enable all log level
    tt_logmgr_enable(&tt_g_logmgr, TT_LOG_LEVEL_DETAIL);
#endif

    return TT_SUCCESS;
}

tt_result_t __sync_log_component_init(IN tt_component_t *comp,
                                      IN tt_profile_t *profile)
{
    tt_result_t result;

    // enable all lock
    result = __enable_sync_log(profile);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __log_config_component_init(IN tt_component_t *comp,
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

    cnode =
        tt_cfgu32_create("log-level", NULL, NULL, &tt_g_log_level, NULL, &attr);
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

tt_result_t __create_log_fmt(IN tt_profile_t *profile)
{
    // use standard output by default
    // todo: make decision by profile

    tt_result_t result;
#if 0
    // detail log format
    result = tt_logmgr_create_format(&tt_g_logmgr,
                                     TT_LOG_LEVEL_DETAIL,
                                     "${content} <${function} - ${line}>\n",
                                     0,
                                     NULL);
    if (!TT_OK(result)) {
        TT_PRINTF("fail to create detail lfmt\n");
        return TT_FAIL;
    }

    // info log format
    result = tt_logmgr_create_format(&tt_g_logmgr,
                                     TT_LOG_LEVEL_INFO,
                                     "${content}\n",
                                     0,
                                     NULL);
    if (!TT_OK(result)) {
        TT_PRINTF("fail to create info lfmt\n");
        return TT_FAIL;
    }

    // warn log format
    result = tt_logmgr_create_format(&tt_g_logmgr,
                                     TT_LOG_LEVEL_WARN,
                                     "${time} ${level:%-6.6s} ${content}\n",
                                     0,
                                     NULL);
    if (!TT_OK(result)) {
        TT_PRINTF("fail to create warn lfmt\n");
        return TT_FAIL;
    }

    // error log format
    result = tt_logmgr_create_format(&tt_g_logmgr,
                                     TT_LOG_LEVEL_ERROR,
                                     "${time} ${level:%-6.6s} ${content} "
                                     "<${function}:${line}>\n",
                                     0,
                                     NULL);
    if (!TT_OK(result)) {
        TT_PRINTF("fail to create error lfmt\n");
        return TT_FAIL;
    }

    // fatal log format
    result = tt_logmgr_create_format(&tt_g_logmgr,
                                     TT_LOG_LEVEL_FATAL,
                                     "${time} ${level:%-6.6s} ${content} "
                                     "<${function}:${line}>\n",
                                     0,
                                     NULL);
    if (!TT_OK(result)) {
        TT_PRINTF("fail to create fatal lfmt\n");
        return TT_FAIL;
    }
#endif
    return TT_SUCCESS;
}

tt_result_t __create_log_io(IN tt_profile_t *profile)
{
    // standard io
    tt_s_logio_std = tt_logio_std_create(NULL);
    if (tt_s_logio_std == NULL) {
        TT_PRINTF("fail to create std lio\n");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __install_log_io(IN tt_profile_t *profile)
{
#if 0
    if (!TT_OK(
            tt_logmgr_add_io(&tt_g_logmgr, TT_LOG_LEVEL_NUM, tt_s_logio_std))) {
        TT_PRINTF("fail to install std lio\n");
        return TT_FAIL;
    }
#endif

    return TT_SUCCESS;
}

tt_result_t __enable_sync_log(IN tt_profile_t *profile)
{
    tt_u32_t i;

#if 0
    // lock of log format
    for (i = 0; i < TT_LOG_LEVEL_NUM; ++i) {
        tt_logfmt_t *lfmt = tt_g_logmgr.lfmt[i];
        if ((lfmt != NULL) && !TT_OK(tt_logfmt_enable_lock(lfmt))) {
            return TT_FAIL;
        }
    }

    // lock of log io
    if (!TT_OK(tt_logio_enable_lock(tt_s_logio_std))) {
        return TT_FAIL;
    }
#endif

    return TT_SUCCESS;
}
