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

#include <misc/tt_percent_encode.h>

#include <misc/tt_util.h>

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

tt_u32_t tt_percent_encode_len(IN const tt_char_t *str,
                               IN tt_u32_t len,
                               IN tt_char_t *enc_tbl)
{
    tt_u32_t i, n = 0;

    for (i = 0; i < len; ++i) {
        // if NULL, do percent encoding: "%xx" otherwise just 1 char
        n += TT_COND(enc_tbl[(tt_u8_t)str[i]] != 0, 1, 3);
    }

    return n;
}

tt_u32_t tt_percent_encode(IN const tt_char_t *str,
                           IN tt_u32_t len,
                           IN tt_char_t *enc_tbl,
                           OUT tt_char_t *dst)
{
    static tt_char_t c2s[] = "0123456789abcdef";
    tt_u32_t i, n = 0;
    tt_char_t *p = dst;

    for (i = 0; i < len; ++i) {
        // if NULL, do percent encoding: "%xx" otherwise just 1 char
        tt_char_t c = str[i];
        if (enc_tbl[(tt_u8_t)c] != 0) {
            *p++ = c;
        } else {
            *p++ = '%';
            *p++ = c2s[c >> 4];
            *p++ = c2s[c & 0xF];
        }
    }

    return (tt_u32_t)(p - dst);
}

tt_u32_t tt_percent_decode_len(IN const tt_char_t *str, IN tt_u32_t len)
{
    const tt_char_t *p, *end;
    tt_u32_t n;

    p = str;
    end = str + len;
    n = 0;
    while (p < end) {
        if ((*p == '%') && ((p + 2) < end) && tt_isxdigit(p[1]) &&
            tt_isxdigit(p[2])) {
            p += 3;
        } else {
            ++p;
        }
        ++n;
    }
    TT_ASSERT(p == end);

    return n;
}

tt_u32_t tt_percent_decode(IN const tt_char_t *str,
                           IN tt_u32_t len,
                           IN tt_bool_t plus2sp,
                           OUT tt_char_t *dst)
{
    const tt_char_t *p, *end;
    tt_char_t *d;

    p = str;
    end = str + len;
    d = dst;
    while (p < end) {
        if ((*p == '%') && ((p + 2) < end) && tt_isxdigit(p[1]) &&
            tt_isxdigit(p[2])) {
            *d++ = (tt_c2h(p[1], 0) << 4) | tt_c2h(p[2], 0);
            p += 3;
        } else if (plus2sp && (*p == '+')) {
            *d++ = ' ';
            ++p;
        } else {
            *d++ = *p;
            ++p;
        }
    }

    return d - dst;
}
