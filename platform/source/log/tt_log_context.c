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

#include <log/tt_log_context.h>

#include <algorithm/tt_buffer_format.h>
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
        return TT_FAIL;
    }

    if (attr == NULL) {
        tt_logctx_attr_default(&__attr);
        attr = &__attr;
    }

    lctx->level = level;
    lctx->lyt = lyt;

    tt_buf_init(&lctx->buf, &attr->buf_attr);
    tt_ptrlist_init(&lctx->filter_list);
    tt_ptrlist_init(&lctx->io_list);

    return TT_SUCCESS;
}

void tt_logctx_destroy(IN tt_logctx_t *lctx)
{
    if (lctx == NULL) {
        return;
    }

    tt_buf_destroy(&lctx->buf);

    while (tt_ptrlist_pophead(&lctx->filter_list, NULL))
        ;
    while (tt_ptrlist_pophead(&lctx->io_list, NULL))
        ;
}

void tt_logctx_attr_default(IN tt_logctx_attr_t *attr)
{
    if (attr == NULL) {
        return;
    }

    tt_buf_attr_default(&attr->buf_attr);
}

tt_result_t tt_logctx_append_filter(IN tt_logctx_t *lctx,
                                    IN tt_log_filter_t filter)
{
    if ((lctx == NULL) || (filter == NULL)) {
        return TT_FAIL;
    }

    return tt_ptrlist_pushtail(&lctx->filter_list, filter);
}

tt_result_t tt_logctx_append_io(IN tt_logctx_t *lctx, IN tt_logio_t *lio)
{
    if ((lctx == NULL) || (lio == NULL)) {
        return TT_FAIL;
    }

    return tt_ptrlist_pushtail(&lctx->io_list, lio);
}

tt_result_t tt_logctx_input(IN tt_logctx_t *lctx, IN tt_log_entry_t *entry)
{
    tt_buf_t *buf;
    tt_ptrlist_t *filter_list;
    tt_ptrlist_t *io_list;
    tt_ptrnode_t *node;
    tt_result_t result = TT_SUCCESS;

    if ((lctx == NULL) || (lctx->lyt == NULL) || (entry == NULL)) {
        return TT_FAIL;
    }
    buf = &lctx->buf;
    filter_list = &lctx->filter_list;
    io_list = &lctx->io_list;

    entry->level = lctx->level;

    // filter
    node = tt_ptrlist_head(filter_list);
    while (node != NULL) {
        tt_log_filter_t filter = node->p;
        if (!filter(entry)) {
            return TT_SUCCESS;
        }

        node = tt_ptrlist_next(filter_list, node);
    }

    // fomat
    tt_buf_clear(buf);
    if (!TT_OK(tt_loglyt_format(lctx->lyt, entry, buf)) ||
        !TT_OK(tt_buf_put_u8(buf, 0))) {
        return TT_FAIL;
    }

    // output
    node = tt_ptrlist_head(io_list);
    while (node != NULL) {
        if (!TT_OK(tt_logio_output(node->p,
                                   (tt_char_t *)TT_BUF_RPOS(buf),
                                   TT_BUF_RLEN(buf)))) {
            // continue even one of logio failed, but this function would
            // finally return TT_FAIL
            result = TT_FAIL;
        }

        node = tt_ptrlist_next(io_list, node);
    }

    return result;
}
