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

#include <tt_log_native.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <log/io/tt_log_io.h>
#include <log/layout/tt_log_layout.h>
#include <log/layout/tt_log_layout_pattern.h>
#include <log/tt_log.h>
#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    const char *tag;
    int prio;
} __logcat_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern tt_loglyt_t *tt_s_loglyt[TT_LOG_LEVEL_NUM];

extern tt_logio_t *tt_s_logio[TT_LOG_LEVEL_NUM];

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_u32_t __logcat_output(IN tt_logio_t *lio,
                                IN const tt_char_t *data,
                                IN tt_u32_t len);

static tt_logio_itf_t __logcat_itf = {
    TT_LOGIO_LOGCAT,

    NULL,
    NULL,
    __logcat_output,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __create_log_layout_ntv(IN tt_profile_t *profile);

static tt_result_t __create_log_io_ntv(IN tt_profile_t *profile);

static tt_logio_t *__logcat_create(IN int prio, IN OPT const char *tag);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_log_component_init_ntv(IN tt_profile_t *profile)
{
    if (!TT_OK(__create_log_layout_ntv(profile))) {
        return TT_FAIL;
    }

    if (!TT_OK(__create_log_io_ntv(profile))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __create_log_layout_ntv(IN tt_profile_t *profile)
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
    lyt = tt_loglyt_pattern_create("${content}\n");
    if (lyt == NULL) {
        TT_ERROR("fail to create warn log pattern\n");
        return TT_FAIL;
    }
    tt_s_loglyt[TT_LOG_WARN] = lyt;

    // error log layout
    lyt = tt_loglyt_pattern_create("${content}\n");
    if (lyt == NULL) {
        TT_ERROR("fail to create error log pattern\n");
        return TT_FAIL;
    }
    tt_s_loglyt[TT_LOG_ERROR] = lyt;

    // fatal log layout
    lyt = tt_loglyt_pattern_create("${content}\n");
    if (lyt == NULL) {
        TT_ERROR("fail to create fatal log pattern\n");
        return TT_FAIL;
    }
    tt_s_loglyt[TT_LOG_FATAL] = lyt;

    return TT_SUCCESS;
}

tt_result_t __create_log_io_ntv(IN tt_profile_t *profile)
{
    tt_logio_t *lio;

    // debug log layout
    lio = __logcat_create(ANDROID_LOG_DEBUG, "platform");
    if (lio == NULL) {
        TT_ERROR("fail to create debug log io\n");
        return TT_FAIL;
    }
    tt_s_logio[TT_LOG_DEBUG] = lio;

    // info log layout
    lio = __logcat_create(ANDROID_LOG_INFO, "platform");
    if (lio == NULL) {
        TT_ERROR("fail to create info log io\n");
        return TT_FAIL;
    }
    tt_s_logio[TT_LOG_INFO] = lio;

    // warn log layout
    lio = __logcat_create(ANDROID_LOG_WARN, "platform");
    if (lio == NULL) {
        TT_ERROR("fail to create warn log io\n");
        return TT_FAIL;
    }
    tt_s_logio[TT_LOG_WARN] = lio;

    // error log layout
    lio = __logcat_create(ANDROID_LOG_ERROR, "platform");
    if (lio == NULL) {
        TT_ERROR("fail to create error log io\n");
        return TT_FAIL;
    }
    tt_s_logio[TT_LOG_ERROR] = lio;

    // fatal log layout
    lio = __logcat_create(ANDROID_LOG_FATAL, "platform");
    if (lio == NULL) {
        TT_ERROR("fail to create fatal log io\n");
        return TT_FAIL;
    }
    tt_s_logio[TT_LOG_FATAL] = lio;

    return TT_SUCCESS;
}

// ========================================
// android logcat
// ========================================

tt_logio_t *__logcat_create(IN int prio, IN OPT const char *tag)
{
    tt_logio_t *lio;
    __logcat_t *lc;

    lio = tt_logio_create(sizeof(__logcat_t), &__logcat_itf);
    if (lio == NULL) {
        return NULL;
    }

    lc = TT_LOGIO_CAST(lio, __logcat_t);

    lc->tag = TT_COND(tag != NULL, tag, "platform");
    lc->prio = prio;

    return lio;
}

tt_u32_t __logcat_output(IN tt_logio_t *lio,
                         IN const tt_char_t *data,
                         IN tt_u32_t len)
{
    __logcat_t *lc = TT_LOGIO_CAST(lio, __logcat_t);

    // tt_logmgr_inputv() guarantees data is a null-terminated string
    return (tt_u32_t)__android_log_write(lc->prio, lc->tag, data);
}
