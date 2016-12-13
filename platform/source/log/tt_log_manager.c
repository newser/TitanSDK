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

#include <log/tt_log_manager.h>

#include <tt_cstd_api.h>

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

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_logmgr_create(IN tt_logmgr_t *lmgr,
                             IN OPT tt_logmgr_attr_t *attr)
{
    tt_memset(lmgr, 0, sizeof(tt_logmgr_t));

    if (attr != NULL) {
        tt_memcpy(&lmgr->attr, attr, sizeof(tt_logmgr_attr_t));
    } else {
        tt_logmgr_attr_default(&lmgr->attr);
    }

    return TT_SUCCESS;
}

void tt_logmgr_destroy(IN tt_logmgr_t *lmgr)
{
    tt_u32_t i;

    for (i = 0; i < TT_LOG_LEVEL_NUM; ++i) {
        if (lmgr->lfmt[i] != NULL) {
            tt_logfmt_destroy(lmgr->lfmt[i]);
            tt_free(lmgr->lfmt[i]);
        }
    }
}

void tt_logmgr_attr_default(IN tt_logmgr_attr_t *attr)
{
    attr->reserved = 0;
}

void tt_logmgr_enable(IN tt_logmgr_t *lmgr, IN tt_log_level_t level)
{
    tt_u32_t i;

    // lower level formats are disabled
    for (i = TT_LOG_LEVEL_DETAIL; i < (tt_u32_t)level; ++i) {
        if (lmgr->lfmt[i] != NULL) {
            tt_logfmt_disable(lmgr->lfmt[i]);
        }
    }

    // higher level formats are enabled
    for (; i < TT_LOG_LEVEL_NUM; ++i) {
        if (lmgr->lfmt[i] != NULL) {
            tt_logfmt_enable(lmgr->lfmt[i]);
        }
    }
}

tt_result_t tt_logmgr_create_format(IN tt_logmgr_t *lmgr,
                                    IN tt_log_level_t level,
                                    IN const tt_char_t *pattern,
                                    IN OPT tt_u32_t buf_size,
                                    IN OPT const tt_char_t *logger)
{
    tt_logfmt_t *lfmt;

    if (!TT_LOG_LEVEL_VALID(level)) {
        return TT_FAIL;
    }

    if (lmgr->lfmt[level] != NULL) {
        return TT_FAIL;
    }

    lfmt = tt_malloc(sizeof(tt_logfmt_t));
    if (lfmt == NULL) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_logfmt_create(lfmt, level, pattern, buf_size, logger))) {
        return TT_FAIL;
    }
    lmgr->lfmt[level] = lfmt;

    return TT_SUCCESS;
}

tt_result_t tt_logmgr_add_io(IN tt_logmgr_t *lmgr,
                             IN tt_log_level_t level,
                             IN tt_logio_t *lio)
{
    if (TT_LOG_LEVEL_VALID(level)) {
        tt_logfmt_t *lfmt = lmgr->lfmt[level];
        if (lfmt != NULL) {
            return tt_logfmt_add_io(lfmt, lio);
        } else {
            return TT_FAIL;
        }
    } else {
        tt_u32_t i;
        for (i = 0; i < TT_LOG_LEVEL_NUM; ++i) {
            tt_logfmt_t *lfmt = lmgr->lfmt[i];
            if ((lfmt != NULL) &&
                !TT_OK(tt_logfmt_add_io(lmgr->lfmt[i], lio))) {
                return TT_FAIL;
            }
        }
        return TT_SUCCESS;
    }
}
