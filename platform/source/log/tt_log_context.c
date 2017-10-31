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

#include <log/tt_log_context.h>

#include <algorithm/tt_buffer_format.h>
#include <log/filter/tt_log_filter.h>
#include <log/io/tt_log_io.h>
#include <log/layout/tt_log_layout.h>
#include <memory/tt_memory_alloc.h>

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

tt_result_t tt_logctx_create(IN tt_logctx_t *lctx,
                             IN tt_log_level_t level,
                             IN OPT tt_loglyt_t *lyt,
                             IN OPT tt_logctx_attr_t *attr)
{
    tt_logctx_attr_t __attr;

    if ((lctx == NULL) || !TT_LOG_LEVEL_VALID(level)) {
        return TT_E_BADARG;
    }

    if (attr == NULL) {
        tt_logctx_attr_default(&__attr);
        attr = &__attr;
    }

    lctx->lyt = lyt;
    tt_ptrq_init(&lctx->filter_q, &attr->filter_q_attr);
    tt_ptrq_init(&lctx->io_q, &attr->io_q_attr);
    tt_buf_init(&lctx->buf, &attr->buf_attr);
    lctx->level = level;

    return TT_SUCCESS;
}

void tt_logctx_destroy(IN tt_logctx_t *lctx)
{
    tt_logfltr_t *lf;
    tt_logio_t *lio;

    if (lctx == NULL) {
        return;
    }

    if (lctx->lyt != NULL) {
        tt_loglyt_destroy(lctx->lyt);
    }

    while ((lf = (tt_logfltr_t *)tt_ptrq_pop_head(&lctx->filter_q)) != NULL) {
        tt_logfltr_release(lf);
    }

    while ((lio = (tt_logio_t *)tt_ptrq_pop_head(&lctx->io_q)) != NULL) {
        tt_logio_release(lio);
    }

    tt_buf_destroy(&lctx->buf);
}

void tt_logctx_attr_default(IN tt_logctx_attr_t *attr)
{
    if (attr == NULL) {
        return;
    }

    tt_ptrq_attr_default(&attr->filter_q_attr);
    attr->filter_q_attr.ptr_per_frame = 8;

    tt_ptrq_attr_default(&attr->io_q_attr);
    attr->io_q_attr.ptr_per_frame = 8;

    tt_buf_attr_default(&attr->buf_attr);
}

void tt_logctx_set_layout(IN tt_logctx_t *lctx, IN TO tt_loglyt_t *lyt)
{
    if (lctx->lyt != NULL) {
        tt_loglyt_destroy(lctx->lyt);
    }
    lctx->lyt = lyt;
}

tt_result_t tt_logctx_append_filter(IN tt_logctx_t *lctx,
                                    IN tt_logfltr_t *filter)
{
    if ((lctx == NULL) || (filter == NULL)) {
        return TT_E_BADARG;
    }

    if (TT_OK(tt_ptrq_push_tail(&lctx->filter_q, filter))) {
        tt_logfltr_ref(filter);
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_logctx_append_io(IN tt_logctx_t *lctx, IN TO tt_logio_t *lio)
{
    if ((lctx == NULL) || (lio == NULL)) {
        return TT_E_BADARG;
    }

    if (TT_OK(tt_ptrq_push_tail(&lctx->io_q, lio))) {
        tt_logio_ref(lio);
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_logctx_input(IN tt_logctx_t *lctx, IN tt_log_entry_t *entry)
{
    tt_buf_t *buf;
    tt_ptrq_iter_t iter;
    tt_logfltr_t *filter;
    tt_logio_t *lio;
    tt_result_t result = TT_SUCCESS;

    if ((lctx == NULL) || (lctx->lyt == NULL) || (entry == NULL)) {
        return TT_E_BADARG;
    }
    buf = &lctx->buf;

    entry->level = lctx->level;

    // format
    tt_buf_clear(buf);
    if (!TT_OK(tt_loglyt_format(lctx->lyt, entry, buf)) ||
        !TT_OK(tt_buf_put_u8(buf, 0))) {
        return TT_FAIL;
    }

    // filter
    tt_ptrq_iter(&lctx->filter_q, &iter);
    while ((filter = (tt_logfltr_t *)tt_ptrq_iter_next(&iter)) != NULL) {
        tt_u32_t io = tt_logfltr_input(filter, entry, buf);
        if (!(io & TT_LOGFLTR_PASS)) {
            return TT_SUCCESS;
        }
    }

    // output
    tt_ptrq_iter(&lctx->io_q, &iter);
    while ((lio = tt_ptrq_iter_next(&iter)) != NULL) {
        if (!TT_OK(tt_logio_output(lio,
                                   entry,
                                   (tt_char_t *)TT_BUF_RPOS(buf),
                                   TT_BUF_RLEN(buf)))) {
            // continue even one of logio failed, but this function would
            // finally return TT_FAIL
            result = TT_FAIL;
        }
    }

    return result;
}
