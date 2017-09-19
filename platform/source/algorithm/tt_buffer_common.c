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

#include <algorithm/tt_buffer_common.h>

#include <misc/tt_assert.h>

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

void tt_buf_swap(IN tt_buf_t *a, IN tt_buf_t *b)
{
    TT_ASSERT(!a->readonly && !b->readonly);

    TT_SWAP(tt_u8_t *, a->p, b->p);
    TT_SWAP_U32(a->size, b->size);
    TT_SWAP_U32(a->wpos, b->wpos);
    TT_SWAP_U32(a->rpos, b->rpos);
    // mspg is not changed, so it's possible that swapping may
    // make buf size exceed the max_limit

    if ((a->p == b->initbuf) || (b->p == a->initbuf)) {
        tt_swap(a->initbuf, b->initbuf, TT_BUF_INIT_SIZE);
        if (a->p == b->initbuf) {
            a->p = a->initbuf;
        }
        if (b->p == a->initbuf) {
            b->p = b->initbuf;
        }
    }
}

tt_s32_t tt_buf_cmp(IN tt_buf_t *a, IN tt_buf_t *b)
{
    tt_u32_t a_len, b_len;

    if (a == b) {
        return 0;
    }

    a_len = TT_BUF_RLEN(a);
    b_len = TT_BUF_RLEN(b);
    if ((a_len != b_len) || (a_len == 0)) {
        return (tt_s32_t)(a_len - b_len);
    }

    return tt_memcmp(TT_BUF_RPOS(a), TT_BUF_RPOS(b), a_len);
}

tt_s32_t tt_buf_cmp_cstr(IN tt_buf_t *a, IN const tt_char_t *cstr)
{
    tt_u32_t a_len, cstr_len;

    a_len = TT_BUF_RLEN(a);
    cstr_len = (tt_u32_t)tt_strlen(cstr);
    if ((a_len != cstr_len) || (a_len == 0)) {
        return (tt_s32_t)(a_len - cstr_len);
    }

    return tt_memcmp(TT_BUF_RPOS(a), cstr, a_len);
}

void tt_buf_remove(IN tt_buf_t *buf, IN tt_u32_t idx)
{
    tt_buf_remove_range(buf, idx, idx + 1);
}

void tt_buf_remove_range(IN tt_buf_t *buf, IN tt_u32_t from, IN tt_u32_t to)
{
    tt_u32_t len = TT_BUF_RLEN(buf);

    TT_ASSERT_ALWAYS(from <= to);

    if ((buf->rpos + from) > from) {
        TT_ERROR("overflow");
        return;
    }

    if (from >= len) {
        return;
    }
    if (to >= len) {
        to = len;
    }

    tt_memmove(&buf->p[buf->rpos + from], &buf->p[buf->rpos + to], len - to);
    buf->wpos -= (to - from);
}

tt_result_t tt_buf_insert(IN tt_buf_t *buf,
                          IN tt_u32_t idx,
                          IN tt_u8_t *data,
                          IN tt_u32_t data_len)
{
    tt_u32_t len = TT_BUF_RLEN(buf);

    if ((buf->rpos + idx) > idx) {
        TT_ERROR("overflow");
        return TT_FAIL;
    }

    if (idx > len) {
        TT_ERROR("invalid idx: %u", idx);
        return TT_FAIL;
    }

    TT_DO(tt_buf_reserve(buf, data_len));
    tt_memmove(&buf->p[buf->rpos + idx + data_len],
               &buf->p[buf->rpos + idx],
               len - idx);
    tt_memcpy(&buf->p[buf->rpos + idx], data, data_len);
    buf->wpos += data_len;

    return TT_SUCCESS;
}

tt_result_t tt_buf_tok(IN tt_buf_t *buf,
                       IN tt_u8_t *sep,
                       IN tt_u32_t sep_num,
                       IN tt_u32_t flag,
                       IN OUT tt_u8_t **last,
                       IN OUT tt_u32_t *last_len)
{
    tt_u8_t *pos, *end, *tstart;

    TT_ASSERT(last != NULL);
    TT_ASSERT(last_len != NULL);

    end = TT_BUF_WPOS(buf);

    if (*last != NULL) {
        pos = *last;
        pos += *last_len;
        if (pos < end) {
            ++pos;
        } else {
            return TT_END;
        }
    } else {
        pos = TT_BUF_RPOS(buf);
    }

    tstart = pos;
    while (pos < end) {
        tt_u32_t i = 0;
        while ((i < sep_num) && (*pos != sep[i])) {
            ++i;
        }

        TT_ASSERT_BUF(i <= sep_num);
        if (i < sep_num) {
            TT_ASSERT_BUF(pos >= tstart);
            if ((pos > tstart) || !(flag & TT_BUFTOK_NOEMPTY)) {
                *last = tstart;
                *last_len = (tt_u32_t)(pos - tstart);
                return TT_SUCCESS;
            }

            // an empty token is ignored
            tstart = pos + 1;
        }

        ++pos;
    }

    TT_ASSERT_BUF(pos == end);
    TT_ASSERT_BUF(pos >= tstart);
    if ((pos > tstart) || !(flag & TT_BUFTOK_NOEMPTY)) {
        *last = tstart;
        *last_len = (tt_u32_t)(pos - tstart);
        return TT_SUCCESS;
    }
    return TT_END;
}

void tt_buf_trim_sp(IN tt_buf_t *buf)
{
    tt_u8_t pos;

    // trim head
    if (buf->rpos < buf->wpos) {
        pos = buf->rpos;
        while ((pos < buf->wpos) && tt_isspace(buf->p[pos])) {
            ++pos;
        }
        buf->rpos = pos;
    }

    // trim tail
    if (buf->rpos < buf->wpos) {
        pos = buf->wpos - 1;
        while ((pos > buf->rpos) && tt_isspace(buf->p[pos])) {
            --pos;
        }
        buf->wpos = pos + 1;
    }
}
