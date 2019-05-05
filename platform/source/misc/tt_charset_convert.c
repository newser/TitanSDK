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

#include <misc/tt_charset_convert.h>

#include <misc/tt_assert.h>
#include <tt_charset_convert_native.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

// this function does:
//  - append beginning bytes to csconv->head if necessary
//  - moves from to first complete char
//  - put ending bytes to csconv->head if from ends with some
//    incomplete multibyte char
typedef tt_result_t (*__parse_prepare_t)(IN tt_chsetconv_t *csconv,
                                         IN OUT void **input,
                                         IN OUT tt_u32_t *input_len);

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_result_t __parse_prepare_utf8(IN tt_chsetconv_t *csconv,
                                        IN OUT void **from,
                                        IN OUT tt_u32_t *from_len);
static tt_result_t __parse_prepare_utf16le(IN tt_chsetconv_t *csconv,
                                           IN OUT void **from,
                                           IN OUT tt_u32_t *from_len);
static tt_result_t __parse_prepare_utf16be(IN tt_chsetconv_t *csconv,
                                           IN OUT void **from,
                                           IN OUT tt_u32_t *from_len);
static tt_result_t __parse_prepare_utf32(IN tt_chsetconv_t *csconv,
                                         IN OUT void **from,
                                         IN OUT tt_u32_t *from_len);

static __parse_prepare_t __parse_prepare[TT_CHARSET_NUM] = {
    // unicode encoding
    __parse_prepare_utf8,
    __parse_prepare_utf16le,
    __parse_prepare_utf16be,
    __parse_prepare_utf32,
    __parse_prepare_utf32,

    // chinese encoding
    NULL,
    NULL,
    NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_chsetconv_create(IN tt_chsetconv_t *csconv, IN tt_charset_t from,
                                IN tt_charset_t to,
                                IN OPT tt_chsetconv_attr_t *attr)
{
    TT_ASSERT(csconv != NULL);
    TT_ASSERT(TT_CHARSET_VALID(from));
    TT_ASSERT(TT_CHARSET_VALID(to));

    csconv->from = from;
    csconv->to = to;

    if (attr != NULL) {
        tt_memcpy(&csconv->attr, attr, sizeof(tt_chsetconv_attr_t));
    } else {
        tt_chsetconv_attr_default(&csconv->attr);
    }

    tt_memset(csconv->head, 0, TT_CHARSET_MAX_MBCHAR_LEN);
    tt_memset(csconv->tail, 0, TT_CHARSET_MAX_MBCHAR_LEN);
    csconv->head_len = 0;
    csconv->tail_len = 0;
    csconv->head_complete = TT_FALSE;

    tt_buf_init(&csconv->converted, NULL);

    // sys csconv
    if (csconv->from != csconv->to) {
        if (!TT_OK(tt_chsetconv_create_ntv(csconv))) { return TT_FAIL; }
    } else {
        tt_memset(&csconv->sys_csconv, 0, sizeof(tt_chsetconv_ntv_t));
    }

    return TT_SUCCESS;
}

void tt_chsetconv_destroy(IN tt_chsetconv_t *csconv)
{
    TT_ASSERT(csconv != NULL);

    // sys csconv
    if (csconv->from != csconv->to) { tt_chsetconv_destroy_ntv(csconv); }

    tt_buf_destroy(&csconv->converted);
}

void tt_chsetconv_attr_default(IN tt_chsetconv_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_memset(attr, 0, sizeof(tt_chsetconv_attr_t));
}

tt_result_t tt_chsetconv_input(IN tt_chsetconv_t *csconv, IN void *input,
                               IN tt_u32_t input_len)
{
    TT_ASSERT(csconv != NULL);
    TT_ASSERT(input != NULL);

    if (input_len == 0) { return TT_SUCCESS; }

    if ((__parse_prepare[csconv->from] != NULL) &&
        (!TT_OK(__parse_prepare[csconv->from](csconv, &input, &input_len)))) {
        return TT_FAIL;
    }

    if (csconv->from != csconv->to) {
        if (!TT_OK(tt_chsetconv_input_ntv(csconv, input, input_len))) {
            return TT_FAIL;
        }
    } else {
        TT_DO(tt_buf_reserve(&csconv->converted, csconv->head_len + input_len));

        if (csconv->head_complete) {
            TT_DO(
                tt_buf_put(&csconv->converted, csconv->head, csconv->head_len));
            csconv->head_len = 0;
            csconv->head_complete = TT_FALSE;
        }

        if (input_len != 0) {
            TT_DO(tt_buf_put(&csconv->converted, input, input_len));
        }
    }

    // append tail to head
    tt_memcpy(&csconv->head[csconv->head_len], csconv->tail, csconv->tail_len);
    csconv->head_len += csconv->tail_len;
    csconv->tail_len = 0;

    return TT_SUCCESS;
}

void tt_chsetconv_output(IN tt_chsetconv_t *csconv, OUT OPT void *output,
                         IN OUT tt_u32_t *output_len)
{
    TT_ASSERT(csconv != NULL);
    TT_ASSERT(output_len != NULL);

    if (output != NULL) {
        tt_buf_t *converted = &csconv->converted;
        tt_u32_t __output_len;

        __output_len = TT_BUF_RLEN(converted);
        if (__output_len > *output_len) { __output_len = *output_len; }

        tt_buf_get(converted, output, __output_len);
        *output_len = __output_len;

        if ((TT_BUF_RLEN(converted) == 0) ||
            (TT_BUF_REFINABLE(converted) > 1000)) {
            tt_buf_refine(converted);
        }
    } else {
        *output_len = TT_BUF_RLEN(&csconv->converted);
    }
}

void tt_chsetconv_output_ptr(IN tt_chsetconv_t *csconv,
                             OUT OPT tt_u8_t **output, OUT tt_u32_t *output_len)
{
    tt_u32_t len;

    TT_ASSERT(csconv != NULL);
    TT_ASSERT(output_len != NULL);

    len = TT_BUF_RLEN(&csconv->converted);
    if (len != 0) {
        TT_SAFE_ASSIGN(output, TT_BUF_RPOS(&csconv->converted));
        *output_len = len;
    } else {
        TT_SAFE_ASSIGN(output, NULL);
        *output_len = 0;
    }
}

void tt_chsetconv_reset(IN tt_chsetconv_t *csconv)
{
    TT_ASSERT(csconv != NULL);

    if (csconv->from != csconv->to) { tt_chsetconv_reset_ntv(csconv); }

    csconv->head_len = 0;
    csconv->tail_len = 0;
    csconv->head_complete = TT_FALSE;

    tt_buf_reset_rwp(&csconv->converted);
}

// static
tt_u32_t __utf8_len(tt_u8_t first_byte)
{
    if (first_byte <= 0x7F)
        return 1; // 0xxxxxxx
    else if (first_byte <= 0xBF)
        return 0; // illegal
    else if (first_byte <= 0xDF)
        return 2; // 110xxxxx 10xxxxxx
    else if (first_byte <= 0xEF)
        return 3; // 1110xxxx 10xxxxxx 10xxxxxx
    else if (first_byte <= 0xF7)
        return 4; // 11110xxx 10xxxxxx 10xxxxxx ...
    else if (first_byte <= 0xFB)
        return 5; // 111110xx 10xxxxxx 10xxxxxx ...
    else if (first_byte <= 0xFD)
        return 5; // 1111110x 10xxxxxx 10xxxxxx ...
    else
        return 0; // illegal
}

tt_result_t __parse_prepare_utf8(IN tt_chsetconv_t *csconv, IN OUT void **from,
                                 IN OUT tt_u32_t *from_len)
{
    tt_u8_t *__from = *from, *pos;
    tt_u32_t __from_len = *from_len, len, tail_len;

    // - append beginning bytes to csconv->head if necessary
    // - moves from to first complete char
    if (csconv->head_len != 0) {
        TT_ASSERT_CS(!csconv->head_complete);

        len = __utf8_len(csconv->head[0]);
        if (len == 0) {
            TT_ERROR("invalid utf8 bytes");
            return TT_FAIL;
        }
        TT_ASSERT_CS(len > csconv->head_len);
        len -= csconv->head_len;
        if (len <= __from_len) {
            csconv->head_complete = TT_TRUE;
        } else {
            len = __from_len;
        }

        tt_memcpy(&csconv->head[csconv->head_len], __from, len);
        csconv->head_len += len;
        __from += len;
        __from_len -= len;
    }

    if (__from_len == 0) { goto done; }

    // put ending bytes to csconv->tail if from ends with some
    // incomplete multibyte char
    pos = __from + __from_len - 1;
    while (pos >= __from) {
        // find first byte that is not 10xxxxxx
        if ((*pos & 0xC0) != 0x80) { break; }

        --pos;
    }
    if (pos < __from) {
        TT_ERROR("invalid utf8 bytes");
        return TT_FAIL;
    }

    len = __utf8_len(*pos);
    if (len == 0) {
        TT_ERROR("invalid utf8 bytes");
        return TT_FAIL;
    }
    TT_ASSERT_CS(__from + __from_len > pos);
    tail_len = (tt_u32_t)(__from + __from_len - pos);
    if (len < tail_len) {
        TT_ERROR("invalid utf8 bytes");
        return TT_FAIL;
    }
    if (len > tail_len) {
        tt_memcpy(csconv->tail, pos, tail_len);
        csconv->tail_len = tail_len;

        __from_len -= tail_len;
    }

done:

    *from = __from;
    *from_len = __from_len;
    return TT_SUCCESS;
}

static tt_u32_t __utf16le_len(tt_u8_t first_byte, tt_u8_t second_byte)
{
    if ((second_byte & 0xFC) == 0xD8)
        return 4; // 0xD800..0xDBFF
    else
        return 2;
}

tt_result_t __parse_prepare_utf16le(IN tt_chsetconv_t *csconv,
                                    IN OUT void **from,
                                    IN OUT tt_u32_t *from_len)
{
    tt_u8_t *__from = *from;
    tt_u32_t __from_len = *from_len, len;

    // - append beginning bytes to csconv->head if necessary
    // - moves from to first complete char
    if (csconv->head_len != 0) {
        while (__from_len != 0) {
            if (csconv->head_len == 1) {
                csconv->head[csconv->head_len++] = *__from;
                ++__from;
                --__from_len;

                len = __utf16le_len(csconv->head[0], csconv->head[1]);
                if (len == 2) {
                    csconv->head_complete = TT_TRUE;
                    break;
                }
            } else {
                TT_ASSERT_CS(csconv->head_len < 4);
                csconv->head[csconv->head_len++] = *__from;
                ++__from;
                --__from_len;

                if (csconv->head_len == 4) {
                    csconv->head_complete = TT_TRUE;
                    break;
                }
            }
            TT_ASSERT_CS(csconv->head_len <= 4);
        }
    }

    if (__from_len == 0) { goto done; }

    // put ending bytes to csconv->tail if from ends with some
    // incomplete multibyte char
    if (__from_len == 1) {
        csconv->tail[0] = *__from;
        csconv->tail_len = 1;

        __from_len = 0;
    } else if ((__from_len & 0x1) != 0) {
        TT_ASSERT_CS(__from_len >= 3);
        if ((__from[__from_len - 2] & 0xFC) == 0xD8) {
            csconv->tail[0] = __from[__from_len - 3];
            csconv->tail[1] = __from[__from_len - 2];
            csconv->tail[2] = __from[__from_len - 1];
            csconv->tail_len = 3;

            __from_len -= 3;
        } else {
            csconv->tail[0] = __from[__from_len - 1];
            csconv->tail_len = 1;

            __from_len -= 1;
        }
    } else {
        TT_ASSERT_CS(__from_len >= 2);
        if ((__from[__from_len - 1] & 0xFC) == 0xD8) {
            csconv->tail[0] = __from[__from_len - 2];
            csconv->tail[1] = __from[__from_len - 1];
            csconv->tail_len = 2;

            __from_len -= 2;
        }
    }

done:

    *from = __from;
    *from_len = __from_len;
    return TT_SUCCESS;
}

static tt_u32_t __utf16be_len(tt_u8_t first_byte, tt_u8_t second_byte)
{
    if ((first_byte & 0xFC) == 0xD8)
        return 4; // 0xD800..0xDBFF
    else
        return 2;
}

tt_result_t __parse_prepare_utf16be(IN tt_chsetconv_t *csconv,
                                    IN OUT void **from,
                                    IN OUT tt_u32_t *from_len)
{
    tt_u8_t *__from = *from;
    tt_u32_t __from_len = *from_len, len;

    // - append beginning bytes to csconv->head if necessary
    // - moves from to first complete char
    if (csconv->head_len != 0) {
        while (__from_len != 0) {
            if (csconv->head_len == 1) {
                csconv->head[csconv->head_len++] = *__from;
                ++__from;
                --__from_len;

                len = __utf16be_len(csconv->head[0], csconv->head[1]);
                if (len == 2) {
                    csconv->head_complete = TT_TRUE;
                    break;
                }
            } else {
                TT_ASSERT_CS(csconv->head_len < 4);
                csconv->head[csconv->head_len++] = *__from;
                ++__from;
                --__from_len;

                if (csconv->head_len == 4) {
                    csconv->head_complete = TT_TRUE;
                    break;
                }
            }
            TT_ASSERT_CS(csconv->head_len <= 4);
        }
    }

    if (__from_len == 0) { goto done; }

    // put ending bytes to csconv->tail if from ends with some
    // incomplete multibyte char
    if (__from_len == 1) {
        csconv->tail[0] = *__from;
        csconv->tail_len = 1;

        __from_len = 0;
    } else if ((__from_len & 0x1) != 0) {
        TT_ASSERT_CS(__from_len >= 3);
        if ((__from[__from_len - 3] & 0xFC) == 0xD8) {
            csconv->tail[0] = __from[__from_len - 3];
            csconv->tail[1] = __from[__from_len - 2];
            csconv->tail[2] = __from[__from_len - 1];
            csconv->tail_len = 3;

            __from_len -= 3;
        } else {
            csconv->tail[0] = __from[__from_len - 1];
            csconv->tail_len = 1;

            __from_len -= 1;
        }
    } else {
        TT_ASSERT_CS(__from_len >= 2);
        if ((__from[__from_len - 2] & 0xFC) == 0xD8) {
            csconv->tail[0] = __from[__from_len - 2];
            csconv->tail[1] = __from[__from_len - 1];
            csconv->tail_len = 2;

            __from_len -= 2;
        }
    }

done:

    *from = __from;
    *from_len = __from_len;
    return TT_SUCCESS;
}

tt_result_t __parse_prepare_utf32(IN tt_chsetconv_t *csconv, IN OUT void **from,
                                  IN OUT tt_u32_t *from_len)
{
    tt_u8_t *__from = *from;
    tt_u32_t __from_len = *from_len, len;

    // - append beginning bytes to csconv->head if necessary
    // - moves from to first complete char
    if (csconv->head_len != 0) {
        while (__from_len != 0) {
            TT_ASSERT_CS(csconv->head_len < 4);
            csconv->head[csconv->head_len++] = *__from;
            ++__from;
            --__from_len;

            if (csconv->head_len == 4) {
                csconv->head_complete = TT_TRUE;
                break;
            }
        }
        TT_ASSERT_CS(csconv->head_len <= 4);
    }

    if (__from_len == 0) { goto done; }

    // put ending bytes to csconv->tail if from ends with some
    // incomplete multibyte char
    len = __from_len & 0x3;
    if (len != 0) {
        tt_memcpy(&csconv->tail, __from, len);
        csconv->tail_len = len;

        __from_len -= len;
    }
    TT_ASSERT_CS((__from_len & 0x3) == 0);

done:

    *from = __from;
    *from_len = __from_len;
    return TT_SUCCESS;
}
