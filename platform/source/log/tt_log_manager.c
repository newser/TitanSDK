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

#include <log/tt_log_manager.h>

#include <algorithm/tt_buffer_format.h>

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
                             IN OPT const tt_char_t *logger,
                             IN OPT tt_logmgr_attr_t *attr)
{
    tt_logmgr_attr_t __attr;
    tt_u32_t i;

    if (lmgr == NULL) { return TT_FAIL; }

    if (attr == NULL) {
        tt_logmgr_attr_default(&__attr);
        attr = &__attr;
    }

    lmgr->logger = TT_COND(logger != NULL, logger, "");

    // default level
    lmgr->level = TT_LOG_WARN;

    if (!TT_OK(tt_mutex_create(&lmgr->lock, &attr->lock_attr))) {
        return TT_FAIL;
    }

    lmgr->seq_num = 0;

    tt_buf_init(&lmgr->buf, &attr->buf_attr);

    for (i = 0; i < TT_LOG_LEVEL_NUM; ++i) {
        if (!TT_OK(
                tt_logctx_create(&lmgr->ctx[i], i, NULL, &attr->ctx_attr[i]))) {
            tt_u32_t j;
            for (j = 0; j < i; ++j) { tt_logctx_destroy(&lmgr->ctx[j]); }
            tt_mutex_destroy(&lmgr->lock);
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}

void tt_logmgr_destroy(IN tt_logmgr_t *lmgr)
{
    tt_u32_t i;

    if (lmgr == NULL) { return; }

    tt_mutex_destroy(&lmgr->lock);

    tt_buf_destroy(&lmgr->buf);

    for (i = 0; i < TT_LOG_LEVEL_NUM; ++i) { tt_logctx_destroy(&lmgr->ctx[i]); }
}

void tt_logmgr_attr_default(IN tt_logmgr_attr_t *attr)
{
    tt_u32_t i;

    if (attr == NULL) { return; }

    tt_mutex_attr_default(&attr->lock_attr);

    tt_buf_attr_default(&attr->buf_attr);

    for (i = 0; i < TT_LOG_LEVEL_NUM; ++i) {
        tt_logctx_attr_default(&attr->ctx_attr[i]);
    }
}

void tt_logmgr_set_layout(IN tt_logmgr_t *lmgr, IN tt_log_level_t level,
                          IN TO struct tt_loglyt_s *lyt)
{
    if (TT_LOG_LEVEL_VALID(level)) {
        tt_logctx_set_layout(&lmgr->ctx[level], lyt);
    } else {
        tt_u32_t i;
        TT_ASSERT(level == TT_LOG_LEVEL_NUM);
        for (i = 0; i < TT_LOG_LEVEL_NUM; ++i) {
            tt_logctx_set_layout(&lmgr->ctx[i], lyt);
        }
    }
}

tt_result_t tt_logmgr_append_filter(IN tt_logmgr_t *lmgr,
                                    IN tt_log_level_t level,
                                    IN struct tt_logfltr_s *filter)
{
    if (TT_LOG_LEVEL_VALID(level)) {
        return tt_logctx_append_filter(&lmgr->ctx[level], filter);
    } else if (level == TT_LOG_LEVEL_NUM) {
        tt_u32_t i;
        for (i = 0; i < TT_LOG_LEVEL_NUM; ++i) {
            tt_logctx_append_filter(&lmgr->ctx[i], filter);
        }
        return TT_SUCCESS;
    } else {
        return TT_E_BADARG;
    }
}

tt_result_t tt_logmgr_append_io(IN tt_logmgr_t *lmgr, IN tt_log_level_t level,
                                IN TO struct tt_logio_s *lio)
{
    if (TT_LOG_LEVEL_VALID(level)) {
        return tt_logctx_append_io(&lmgr->ctx[level], lio);
    } else if (level == TT_LOG_LEVEL_NUM) {
        tt_u32_t i;
        for (i = 0; i < TT_LOG_LEVEL_NUM; ++i) {
            tt_logctx_append_io(&lmgr->ctx[i], lio);
        }
        return TT_SUCCESS;
    } else {
        return TT_E_BADARG;
    }
}

tt_result_t tt_logmgr_inputv(IN tt_logmgr_t *lmgr, IN tt_log_level_t level,
                             IN const tt_char_t *func, IN tt_u32_t line,
                             IN const tt_char_t *format, IN va_list ap)
{
    tt_log_entry_t entry = {0};
    tt_buf_t *buf = &lmgr->buf;
    tt_result_t result = TT_FAIL;

    if (level < lmgr->level) { return TT_SUCCESS; }

    entry.seq_num = lmgr->seq_num++;
    entry.logger = lmgr->logger;
    entry.level = level;
    entry.function = func;
    entry.line = line;

    tt_mutex_acquire(&lmgr->lock);

    tt_buf_clear(buf);
    if (TT_OK(tt_buf_putv(&lmgr->buf, format, ap)) &&
        TT_OK(tt_buf_put_u8(buf, 0))) {
        entry.content = (tt_char_t *)TT_BUF_RPOS(buf);

        result = tt_logctx_input(&lmgr->ctx[level], &entry);
    }

    tt_mutex_release(&lmgr->lock);

    return result;
}
