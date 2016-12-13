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

#include <algorithm/tt_string_common.h>

#include <algorithm/tt_buffer_common.h>
#include <algorithm/tt_buffer_format.h>
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

tt_char_t tt_string_getchar(IN tt_string_t *s, IN tt_u32_t pos)
{
    tt_buf_t *buf = &s->buf;
    tt_u8_t *p = TT_BUF_RPOS(buf);
    tt_u32_t len = TT_BUF_RLEN(buf);

    if (pos < len) {
        return p[pos];
    } else {
        TT_WARN("pos[%u] is out of string bound[%u]", pos, len);
        return 0;
    }
}

tt_s32_t tt_string_cmp(IN tt_string_t *a, IN const tt_char_t *b)
{
    tt_buf_t b_buf;
    tt_s32_t ret;

    tt_buf_create_nocopy(&b_buf,
                         (tt_u8_t *)b,
                         (tt_u32_t)tt_strlen(b) + 1,
                         NULL);
    ret = tt_buf_cmp(&a->buf, &b_buf);
    tt_buf_destroy(&b_buf);

    return ret;
}

tt_s32_t tt_string_ncasecmp(IN tt_string_t *a, IN const tt_char_t *b)
{
    tt_buf_t *a_buf = &a->buf;
    tt_u32_t a_len, b_len;

    a_len = TT_BUF_RLEN(a_buf);
    b_len = (tt_u32_t)tt_strlen(b) + 1;
    if ((a_len != b_len) || (a_len == 0)) {
        return (tt_s32_t)(a_len - b_len);
    }

    return tt_memicmp(TT_BUF_RPOS(a_buf), (tt_u8_t *)b, a_len);
}

tt_result_t tt_string_copy(OUT tt_string_t *dst, IN tt_string_t *src)
{
    return tt_buf_copy(&dst->buf, &src->buf);
}

void tt_string_swap(IN tt_string_t *a, IN tt_string_t *b)
{
    tt_buf_swap(&a->buf, &b->buf);
}

tt_u32_t tt_string_find(IN tt_string_t *s, IN const tt_char_t *substr)
{
    tt_buf_t *buf = &s->buf;
    tt_u8_t *pos;

    if (*substr == 0) {
        // empty string is a substring of any other string
        return 0;
    }

    // as we know, s and substr are always has content and null terminated
    // so it's safe to use strstr()
    pos = (tt_u8_t *)tt_strstr(tt_string_cstr(s), substr);
    if (pos == NULL) {
        return TT_STRPOS_NULL;
    }

    TT_ASSERT((pos >= TT_BUF_RPOS(buf)) && (pos <= TT_BUF_WPOS(buf)));
    return (tt_u32_t)(pos - TT_BUF_RPOS(buf));
}

tt_u32_t tt_string_findfrom(IN tt_string_t *s,
                            IN tt_u32_t from,
                            IN const tt_char_t *substr)
{
    tt_buf_t *buf = &s->buf;
    const tt_char_t *s_from;
    tt_u8_t *pos;

    s_from = tt_string_subcstr(s, from, NULL);
    if (s_from == NULL) {
        return TT_STRPOS_NULL;
    }

    if (*substr == 0) {
        // empty string is a substring of any other string
        pos = (tt_u8_t *)s_from;
    } else {
        pos = (tt_u8_t *)tt_strstr(s_from, substr);
    }
    if (pos == NULL) {
        return TT_STRPOS_NULL;
    }

    TT_ASSERT((pos >= TT_BUF_RPOS(buf)) && (pos <= TT_BUF_WPOS(buf)));
    return (tt_u32_t)(pos - TT_BUF_RPOS(buf));
}

tt_u32_t tt_string_find_c(IN tt_string_t *s, IN tt_char_t c)
{
    tt_buf_t *buf = &s->buf;
    tt_u8_t *pos;

    pos = (tt_u8_t *)tt_strchr(tt_string_cstr(s), c);
    if (pos == NULL) {
        return TT_STRPOS_NULL;
    }

    TT_ASSERT((pos >= TT_BUF_RPOS(buf)) && (pos <= TT_BUF_WPOS(buf)));
    return (tt_u32_t)(pos - TT_BUF_RPOS(buf));
}

tt_u32_t tt_string_findfrom_c(IN tt_string_t *s,
                              IN tt_u32_t from,
                              IN tt_char_t c)
{
    tt_buf_t *buf = &s->buf;
    const tt_char_t *s_from;
    tt_u8_t *pos;

    s_from = tt_string_subcstr(s, from, NULL);
    if (s_from == NULL) {
        return TT_STRPOS_NULL;
    }

    pos = (tt_u8_t *)tt_strchr(s_from, c);
    if (pos == NULL) {
        return TT_STRPOS_NULL;
    }

    TT_ASSERT((pos >= TT_BUF_RPOS(buf)) && (pos <= TT_BUF_WPOS(buf)));
    return (tt_u32_t)(pos - TT_BUF_RPOS(buf));
}

tt_u32_t tt_string_rfind_c(IN tt_string_t *s, IN tt_char_t c)
{
    tt_buf_t *buf = &s->buf;
    tt_u8_t *pos;

    pos = (tt_u8_t *)tt_strrchr(tt_string_cstr(s), c);
    if (pos == NULL) {
        return TT_STRPOS_NULL;
    }

    TT_ASSERT((pos >= TT_BUF_RPOS(buf)) && (pos <= TT_BUF_WPOS(buf)));
    return (tt_u32_t)(pos - TT_BUF_RPOS(buf));
}

void tt_string_remove(IN tt_string_t *s, IN tt_u32_t from, IN tt_u32_t len)
{
    tt_u32_t n;

    // must not remove the terminating null
    n = tt_string_len(s);
    if (from >= n) {
        return;
    }
    if ((from + len) >= n) {
        len = n - from;
    }

    tt_buf_remove_n(&s->buf, from, len);
}

tt_result_t tt_string_append(IN OUT tt_string_t *s, IN const tt_char_t *substr)
{
    tt_buf_t *buf = &s->buf;

    TT_ASSERT(buf->wr_pos > 0);
    --buf->wr_pos;
    if (TT_OK(tt_buf_put(buf,
                         (tt_u8_t *)substr,
                         (tt_u32_t)tt_strlen(substr) + 1))) {
        return TT_SUCCESS;
    } else {
        ++buf->wr_pos;
        return TT_FAIL;
    }
}

tt_result_t tt_string_append_c(IN OUT tt_string_t *s, IN tt_char_t c)
{
    tt_buf_t *buf = &s->buf;

    if (c == 0) {
        // do nothing
        return TT_SUCCESS;
    }

    TT_DO(tt_buf_reserve(buf, 1));

    TT_ASSERT(buf->wr_pos > 0);
    buf->addr[buf->wr_pos - 1] = c;
    buf->addr[buf->wr_pos] = 0;
    ++buf->wr_pos;
    return TT_SUCCESS;
}

tt_result_t tt_string_append_cn(IN OUT tt_string_t *s,
                                IN tt_char_t c,
                                IN tt_u32_t c_num)
{
    tt_buf_t *buf = &s->buf;

    if (c == 0) {
        // do nothing
        return TT_SUCCESS;
    }

    TT_ASSERT(buf->wr_pos > 0);
    --buf->wr_pos;
    TT_DO(tt_buf_put_rep(buf, c, c_num));
    TT_DO(tt_buf_put_u8(buf, 0));
    return TT_SUCCESS;
}

tt_result_t tt_string_append_sub(IN OUT tt_string_t *s,
                                 IN const tt_char_t *substr,
                                 IN tt_u32_t num)
{
    tt_buf_t *buf = &s->buf;
    tt_u32_t len = (tt_u32_t)tt_strlen(substr), rp, wp;

    len = TT_MIN(len, num);

    tt_buf_backup_rwp(buf, &rp, &wp);

    TT_ASSERT(buf->wr_pos > 0);
    --buf->wr_pos;
    if (TT_OK(tt_buf_put(buf, (tt_u8_t *)substr, len)) &&
        TT_OK(tt_buf_put_u8(buf, 0))) {
        return TT_SUCCESS;
    } else {
        tt_buf_restore_rwp(buf, &rp, &wp);
        return TT_FAIL;
    }
}

tt_bool_t tt_string_startwith(IN tt_string_t *s, IN const tt_char_t *substr)
{
    tt_u32_t n = (tt_u32_t)tt_strlen(substr);

    if (n == 0) {
        return TT_TRUE;
    }

    if (tt_string_len(s) < n) {
        return TT_FALSE;
    }

    if (tt_strncmp(tt_string_cstr(s), substr, n) == 0) {
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_bool_t tt_string_endwith(IN tt_string_t *s, IN const tt_char_t *substr)
{
    tt_u32_t s_len = tt_string_len(s);
    tt_u32_t n = (tt_u32_t)tt_strlen(substr);

    if (n == 0) {
        return TT_TRUE;
    }

    if (s_len < n) {
        return TT_FALSE;
    }

    if (tt_strncmp(tt_string_cstr(s) + s_len - n, substr, n) == 0) {
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_result_t tt_string_substr(IN tt_string_t *s,
                             IN tt_u32_t from,
                             IN tt_u32_t len,
                             OUT tt_string_t *substr)
{
    tt_u8_t *p;
    tt_u32_t n;
    tt_buf_t *subs_buf;

    p = (tt_u8_t *)tt_string_subcstr(s, from, &n);
    if (p == NULL) {
        return TT_FAIL;
    }

    if (len > n) {
        len = n;
    }

    subs_buf = &substr->buf;
    tt_buf_clear(subs_buf);
    TT_DO(tt_buf_put(subs_buf, p, len));
    TT_DO(tt_buf_put_u8(subs_buf, 0));
    return TT_SUCCESS;
}

void tt_string_tolower(IN OUT tt_string_t *s)
{
    tt_char_t *p;

    if (s->buf.attr.not_copied) {
        TT_ERROR("can not change not copied string");
        return;
    }

    p = (tt_char_t *)tt_string_cstr(s);
    while (*p != 0) {
        *p = (tt_char_t)tt_tolower(*p);
        ++p;
    }
}

void tt_string_toupper(IN OUT tt_string_t *s)
{
    tt_char_t *p;

    if (s->buf.attr.not_copied) {
        TT_ERROR("can not change not copied string");
        return;
    }

    p = (tt_char_t *)tt_string_cstr(s);
    while (*p != 0) {
        *p = (tt_char_t)tt_toupper(*p);
        ++p;
    }
}

void tt_string_trim(IN OUT tt_string_t *s)
{
    tt_char_t *p, *begin, *end;
    tt_u32_t len;
    tt_buf_t *buf = &s->buf;

    if (s->buf.attr.not_copied) {
        TT_ERROR("can not change not copied string");
        return;
    }

    len = tt_string_len(s);
    if (len == 0) {
        return;
    }

    p = (tt_char_t *)tt_string_cstr(s);
    begin = p;
    end = p + len - 1;
    TT_ASSERT(begin <= end);

    while ((*begin != 0) && (*begin == ' '))
        ++begin;
    while ((end > begin) && (*end == ' '))
        --end;
    len = (tt_u32_t)(end - begin + 1);

    if (begin != p) {
        tt_memmove(p, begin, len);
    }
    TT_BUF_RPOS(buf)[len] = 0;
    ++len;
    buf->wr_pos = buf->rd_pos + len;
}

tt_result_t tt_string_insert(IN OUT tt_string_t *s,
                             IN tt_u32_t from,
                             IN const tt_char_t *substr)
{
    tt_u32_t n = tt_string_len(s);

    if (from >= n) {
        from = n;
    }

    TT_DO(tt_buf_insert(&s->buf,
                        from,
                        (tt_u8_t *)substr,
                        (tt_u32_t)tt_strlen(substr)));

    return TT_SUCCESS;
}

tt_result_t tt_string_insert_c(IN OUT tt_string_t *s,
                               IN tt_u32_t from,
                               IN tt_char_t c)
{
    tt_u32_t n;

    if (c == 0) {
        // inserting 0 violate string consistency
        return TT_FAIL;
    }

    n = tt_string_len(s);
    if (from >= n) {
        from = n;
    }

    TT_DO(tt_buf_insert(&s->buf, from, (tt_u8_t *)&c, 1));

    return TT_SUCCESS;
}
