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

tt_result_t tt_buf_put_cstr2hex(IN tt_buf_t *buf, IN const tt_char_t *cstr)
{
    tt_u8_t *pos, *end;

    pos = (tt_u8_t *)cstr;
    end = pos + (tt_u32_t)tt_strlen(cstr);
    while (pos < end) {
        tt_u8_t n;

        if ((pos + 2) <= end) {
            n = ((tt_c2h(pos[0], 0) & 0xF) << 4) | (tt_c2h(pos[1], 0) & 0xF);
            pos += 2;
        } else {
            n = ((tt_c2h(pos[0], 0) & 0xF) << 4);
            pos += 1;
        }

        TT_DO(tt_buf_put_u8(buf, n));
    }
    TT_ASSERT_BUF(pos == end);

    return TT_SUCCESS;
}

tt_result_t tt_buf_put_hex2cstr(IN tt_buf_t *buf,
                                IN tt_u8_t *hex,
                                IN tt_u32_t hex_len)
{
    tt_u8_t *pos, *end;

    pos = hex;
    end = pos + hex_len;
    while (pos < end) {
        TT_DO(tt_buf_put_u8(buf, tt_h2c((pos[0] >> 4) & 0xF, '?')));
        TT_DO(tt_buf_put_u8(buf, tt_h2c(pos[0] & 0xF, '?')));

        ++pos;
    }

    return TT_SUCCESS;
}

#if 0
tt_result_t tt_buf_putf(IN tt_buf_t *buf, IN const tt_char_t *format, ...)
{
    tt_u8_t *p;
    tt_u32_t len, n;
    va_list args;
    tt_bool_t retry = TT_TRUE;
    tt_result_t result = TT_SUCCESS;

again:
    p = TT_BUF_WPOS(buf);
    len = TT_BUF_WLEN(buf);

    va_start(args, format);
    n = tt_vsnprintf((char *)p, len, format, args);
    va_end(args);
    if ((n + 1) >= len) {
        if (retry && TT_OK(tt_buf_expand(buf, 0))) {
            retry = TT_FALSE;
            goto again;
        } else {
            result = TT_FAIL;
        }
    } else {
        // n is the bytes put, not including 0
        tt_buf_inc_wp(buf, n);
    }

    return result;
}
#else
tt_result_t tt_buf_putf(IN tt_buf_t *buf, IN const tt_char_t *format, ...)
{
    va_list args;
    tt_result_t result;

    va_start(args, format);
    result = tt_buf_putv(buf, format, args);
    va_end(args);

    return result;
}
#endif

tt_result_t tt_buf_putv(IN tt_buf_t *buf,
                        IN const tt_char_t *format,
                        IN va_list ap)
{
    tt_u8_t *p;
    tt_u32_t len, n;
    va_list args;
    tt_bool_t retry = TT_TRUE;
    tt_result_t result = TT_SUCCESS;

again:
    p = TT_BUF_WPOS(buf);
    len = TT_BUF_WLEN(buf);
    va_copy(args, ap);

    n = tt_vsnprintf((char *)p, len, format, args);
    if ((n + 1) >= len) {
        if (retry && TT_OK(tt_buf_expand(buf, 0))) {
            retry = TT_FALSE;
            goto again;
        } else {
            result = TT_FAIL;
        }
    } else {
        // n is the bytes put, not including 0
        tt_buf_inc_wp(buf, n);
    }

    return result;
}
