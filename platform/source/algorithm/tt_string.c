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

#include <algorithm/tt_string.h>

#include <algorithm/tt_buffer_format.h>
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

tt_result_t tt_string_create(IN tt_string_t *str,
                             IN const tt_char_t *cstr,
                             IN OPT tt_string_attr_t *attr)
{
    tt_buf_t *buf = &str->buf;
    tt_u32_t len = (tt_u32_t)tt_strlen(cstr);
    tt_string_attr_t __attr;

    if (attr == NULL) {
        tt_string_attr_default(&__attr);
        attr = &__attr;
    }

    if (!TT_OK(tt_buf_create_copy(buf,
                                  (tt_u8_t *)cstr,
                                  len + 1,
                                  &attr->buf_attr))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_string_create_n(IN tt_string_t *str,
                               IN const tt_char_t *cstr,
                               IN tt_u32_t len,
                               IN OPT tt_string_attr_t *attr)
{
    tt_buf_t *buf = &str->buf;
    tt_u32_t cstr_len = (tt_u32_t)tt_strlen(cstr);
    tt_string_attr_t __attr;

    if (len > cstr_len) {
        len = cstr_len;
    }

    if (attr == NULL) {
        tt_string_attr_default(&__attr);
        attr = &__attr;
    }

    if (!TT_OK(
            tt_buf_create_copy(buf, (tt_u8_t *)cstr, len, &attr->buf_attr))) {
        return TT_FAIL;
    }
    if (!TT_OK(tt_buf_put_u8(buf, 0))) {
        tt_buf_destroy(buf);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_string_create_nocopy(IN tt_string_t *str,
                                    IN const tt_char_t *cstr,
                                    IN OPT tt_string_attr_t *attr)
{
    tt_buf_t *buf = &str->buf;
    tt_u32_t len = (tt_u32_t)tt_strlen(cstr);
    tt_string_attr_t __attr;

    if (attr == NULL) {
        tt_string_attr_default(&__attr);
        attr = &__attr;
    }

    if (!TT_OK(tt_buf_create_nocopy(buf,
                                    (tt_u8_t *)cstr,
                                    len + 1,
                                    &attr->buf_attr))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_string_init(IN tt_string_t *str, IN OPT tt_string_attr_t *attr)
{
    tt_string_create(str, "", attr);
    // won't fail
}

void tt_string_destroy(IN tt_string_t *str)
{
    tt_buf_destroy(&str->buf);
}

void tt_string_attr_default(IN tt_string_attr_t *attr)
{
    tt_buf_attr_default(&attr->buf_attr);
}

tt_result_t tt_string_set(IN tt_string_t *str, IN const tt_char_t *cstr)
{
    tt_buf_clear(&str->buf);
    return tt_buf_put(&str->buf,
                      (tt_u8_t *)cstr,
                      (tt_u32_t)tt_strlen(cstr) + 1);
}

tt_result_t tt_string_setfrom(IN tt_string_t *str,
                              IN tt_u32_t from,
                              IN const tt_char_t *cstr)
{
    tt_u32_t len, n;

    len = tt_string_len(str);
    n = (tt_u32_t)tt_strlen(cstr);
    if ((from > len) || ((from + n) > len)) {
        TT_ERROR("invalid from[%u] and len[%u]", from, len);
        return TT_FAIL;
    }

    return tt_buf_set(&str->buf, from, (tt_u8_t *)cstr, n);
}

tt_result_t tt_string_setfrom_c(IN tt_string_t *str,
                                IN tt_u32_t from,
                                IN tt_char_t c)
{
    if (from >= tt_string_len(str)) {
        TT_ERROR("invalid from[%u]", from);
        return TT_FAIL;
    }

    return tt_buf_set(&str->buf, from, (tt_u8_t *)&c, 1);
}

void tt_string_clear(IN tt_string_t *str)
{
    tt_buf_t *buf = &str->buf;

    if (buf->readonly) {
        TT_FATAL("not copied string");
        return;
    }

    TT_ASSERT_STR(buf->size > 0);
    buf->rpos = 0;
    buf->wpos = 1;
    buf->p[0] = 0;
}

void tt_string_print(IN tt_string_t *str, IN tt_u32_t flag)
{
    TT_INFO("%s", TT_BUF_RPOS(&str->buf));
}

tt_u32_t tt_string_len(IN tt_string_t *str)
{
    tt_u32_t len = TT_BUF_RLEN(&str->buf);
    TT_ASSERT_STR(len > 0);
    return len - 1;
}

const tt_char_t *tt_string_cstr(IN tt_string_t *str)
{
    return (tt_char_t *)TT_BUF_RPOS(&str->buf);
}

const tt_char_t *tt_string_subcstr(IN tt_string_t *str,
                                   IN tt_u32_t from,
                                   OUT OPT tt_u32_t *subcstr_len)
{
    tt_u32_t len = tt_string_len(str);

    // from == len is ok, return the null string
    if (from <= len) {
        tt_u8_t *p = TT_BUF_RPOS(&str->buf);

        TT_SAFE_ASSIGN(subcstr_len, len - from);
        return (tt_char_t *)&p[from];
    } else {
        TT_SAFE_ASSIGN(subcstr_len, 0);
        return NULL;
    }
}
