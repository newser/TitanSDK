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

static tt_result_t __buf_size_align(IN tt_buf_attr_t *attr,
                                    IN OUT tt_u32_t *new_size);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_buf_swap(IN tt_buf_t *a, IN tt_buf_t *b)
{
    TT_ASSERT(!a->attr.not_copied);
    TT_ASSERT(!b->attr.not_copied);

    TT_SWAP(tt_u8_t *, a->addr, b->addr);
    TT_SWAP_U32(a->size, b->size);
    TT_SWAP_U32(a->wr_pos, b->wr_pos);
    TT_SWAP_U32(a->rd_pos, b->rd_pos);

    if ((a->addr == b->buf_inline) || (b->addr == a->buf_inline)) {
        tt_swap(a->buf_inline, b->buf_inline, TT_BUF_INLINE_SIZE);
        if (a->addr == b->buf_inline) {
            a->addr = a->buf_inline;
        }
        if (b->addr == a->buf_inline) {
            b->addr = b->buf_inline;
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

tt_s32_t tt_buf_cmp_cstr(IN tt_buf_t *a, IN const tt_char_t *b)
{
    tt_u32_t a_len, b_len;

    a_len = TT_BUF_RLEN(a);
    b_len = (tt_u32_t)tt_strlen(b);
    if ((a_len != b_len) || (a_len == 0)) {
        return (tt_s32_t)(a_len - b_len);
    }

    return tt_memcmp(TT_BUF_RPOS(a), b, a_len);
}

void tt_buf_remove(IN tt_buf_t *buf, IN tt_u32_t pos)
{
    tt_buf_remove_n(buf, pos, 1);
}

void tt_buf_remove_n(IN tt_buf_t *buf, IN tt_u32_t start, IN tt_u32_t len)
{
    // care overflow...
    if ((buf->rd_pos + start) >= buf->wr_pos) {
        return;
    }
    if ((buf->rd_pos + start + len) > buf->wr_pos) {
        len = buf->wr_pos - buf->rd_pos - start;
    }

    tt_memmove(&buf->addr[buf->rd_pos + start],
               &buf->addr[buf->rd_pos + start + len],
               buf->wr_pos - buf->rd_pos - start - len);
    buf->wr_pos -= len;
}

tt_result_t tt_buf_insert(IN tt_buf_t *buf,
                          IN tt_u32_t pos,
                          IN tt_u8_t *data,
                          IN tt_u32_t data_len)
{
    if ((buf->rd_pos + pos) > buf->wr_pos) {
        TT_ERROR("invalid pos: %u", pos);
        return TT_FAIL;
    }

    TT_DO(tt_buf_reserve(buf, data_len));
    tt_memmove(&buf->addr[buf->rd_pos + pos + data_len],
               &buf->addr[buf->rd_pos + pos],
               buf->wr_pos - buf->rd_pos - pos);
    tt_memcpy(&buf->addr[buf->rd_pos + pos], data, data_len);
    buf->wr_pos += data_len;

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
    if (end == NULL) {
        return TT_END;
    }

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
            if ((pos > tstart) || !(flag & TT_BUFTOK_IGNORE_EMPTY)) {
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
    if ((pos > tstart) || !(flag & TT_BUFTOK_IGNORE_EMPTY)) {
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
    if (buf->rd_pos < buf->wr_pos) {
        pos = buf->rd_pos;
        while ((pos < buf->wr_pos) && tt_isspace(buf->addr[pos])) {
            ++pos;
        }
        buf->rd_pos = pos;
    }

    // trim tail
    if (buf->rd_pos < buf->wr_pos) {
        pos = buf->wr_pos - 1;
        while ((pos > buf->rd_pos) && tt_isspace(buf->addr[pos])) {
            --pos;
        }
        buf->wr_pos = pos + 1;
    }
}
