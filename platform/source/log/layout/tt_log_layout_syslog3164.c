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

#include <log/layout/tt_log_layout_syslog3164.h>

#include <algorithm/tt_buffer_format.h>
#include <log/layout/tt_log_layout_pattern.h>
#include <os/tt_process.h>
#include <time/tt_date_format.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_loglyt_t *pattern;
    const tt_char_t *host;
    const tt_char_t *program;
    tt_u32_t host_len;
    tt_u32_t program_len;
    tt_u32_t pri_len;
    tt_char_t pri[8];
} tt_loglyt_syslog3164_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __lls_destroy(IN tt_loglyt_t *ll);

static tt_result_t __lls_format(IN tt_loglyt_t *ll,
                                IN tt_log_entry_t *entry,
                                OUT tt_buf_t *outbuf);

static tt_loglyt_itf_t __lls_itf = {
    NULL, __lls_destroy, __lls_format,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_loglyt_t *tt_loglyt_syslog3164_create(IN tt_syslog_facility_t f,
                                         IN tt_syslog_level_t l,
                                         IN const tt_char_t *host,
                                         IN OPT const tt_char_t *program,
                                         IN const tt_char_t *pattern)
{
    tt_loglyt_t *llp, *ll;
    tt_loglyt_syslog3164_t *lls;

    llp = tt_loglyt_pattern_create(pattern);
    if (llp == NULL) {
        return NULL;
    }

    ll = tt_loglyt_create(sizeof(tt_loglyt_syslog3164_t), &__lls_itf);
    if (ll == NULL) {
        tt_loglyt_destroy(llp);
        return NULL;
    }

    lls = TT_LOGLYT_CAST(ll, tt_loglyt_syslog3164_t);

    lls->pattern = llp;

    lls->host = host;
    lls->host_len = tt_strlen(lls->host);

    if (program != NULL) {
        lls->program = program;
    } else {
        lls->program = tt_process_name();
    }
    lls->program_len = tt_strlen(lls->program);

    tt_memset(lls->pri, 0, sizeof(lls->pri));
    tt_snprintf(lls->pri,
                sizeof(lls->pri) - 1,
                "<%d>",
                TT_SYSLOG_PRIORITY(f, l));
    lls->pri_len = tt_strlen(lls->pri);

    return ll;
}

void __lls_destroy(IN tt_loglyt_t *ll)
{
    tt_loglyt_syslog3164_t *lls = TT_LOGLYT_CAST(ll, tt_loglyt_syslog3164_t);

    tt_loglyt_destroy(lls->pattern);
}

tt_result_t __lls_format(IN tt_loglyt_t *ll,
                         IN tt_log_entry_t *entry,
                         OUT tt_buf_t *outbuf)
{
    tt_loglyt_syslog3164_t *lls = TT_LOGLYT_CAST(ll, tt_loglyt_syslog3164_t);
    tt_u32_t n;

    // ========================================
    // PRI Part
    // ========================================

    TT_DO(tt_buf_put(outbuf, (tt_u8_t *)lls->pri, lls->pri_len));

    // ========================================
    // HEADER Part
    // ========================================

    // Mmm dd hh:mm:ss
    TT_DO(tt_buf_reserve(outbuf, 16));
    n = tt_date_render_now("%b %d %H:%M:%S ",
                           (tt_char_t *)TT_BUF_WPOS(outbuf),
                           TT_BUF_WLEN(outbuf));
    tt_buf_inc_wp(outbuf, n);

    TT_DO(tt_buf_put(outbuf, (tt_u8_t *)lls->host, lls->host_len));
    TT_DO(tt_buf_put_u8(outbuf, ' '));

    // ========================================
    // MSG Part
    // ========================================

    TT_DO(tt_buf_put(outbuf, (tt_u8_t *)lls->program, lls->program_len));
    TT_DO(tt_buf_put(outbuf, (tt_u8_t *)": ", 2));

    tt_loglyt_format(lls->pattern, entry, outbuf);

    return TT_SUCCESS;
}
