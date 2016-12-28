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

#include <tt_charset_convert_native.h>

#include <misc/tt_assert.h>
#include <misc/tt_charset_convert.h>

#include <tt_sys_error.h>

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

static UINT __charset_code_page[TT_CHARSET_NUM] = {
    // unicode encoding
    65001,
    1200,
    1201,
    12000,
    12001,

    // chinese encoding
    936,
    936,
    54936,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __charset_conv(IN tt_chsetconv_t *csconv,
                                  IN tt_u8_t *input,
                                  IN tt_u32_t input_len,
                                  IN tt_buf_t *output);

static tt_result_t __utf32le_to_utf16le(IN tt_u8_t *input,
                                        IN tt_u32_t input_len,
                                        IN tt_buf_t *output);
static tt_result_t __utf32be_to_utf16le(IN tt_u8_t *input,
                                        IN tt_u32_t input_len,
                                        IN tt_buf_t *output);

static tt_result_t __utf16le_to_utf32le(IN tt_u8_t *input,
                                        IN tt_u32_t input_len,
                                        IN tt_buf_t *output);
static tt_result_t __utf16le_to_utf32be(IN tt_u8_t *input,
                                        IN tt_u32_t input_len,
                                        IN tt_buf_t *output);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_chsetconv_create_ntv(IN struct tt_chsetconv_s *csconv)
{
    tt_chsetconv_ntv_t *sys_csconv = &csconv->sys_csconv;

    tt_buf_init(&sys_csconv->intermediate, NULL);

    return TT_SUCCESS;
}

void tt_chsetconv_destroy_ntv(IN struct tt_chsetconv_s *csconv)
{
    tt_chsetconv_ntv_t *sys_csconv = &csconv->sys_csconv;

    tt_buf_destroy(&sys_csconv->intermediate);
}

tt_result_t tt_chsetconv_input_ntv(IN struct tt_chsetconv_s *csconv,
                                   IN tt_u8_t *input,
                                   IN tt_u32_t input_len)
{
    tt_buf_t *converted = &csconv->converted;

    // convert head
    if (csconv->head_complete) {
        TT_ASSERT_CS(csconv->head_len != 0);

        TT_DO(tt_buf_reserve(converted,
                             TT_CHARSET_MAX_MBCHAR_LEN + (input_len << 1)));
        if (!TT_OK(__charset_conv(csconv,
                                  csconv->head,
                                  csconv->head_len,
                                  converted))) {
            return TT_FAIL;
        }

        csconv->head_len = 0;
        csconv->head_complete = TT_FALSE;
    }

    // convert content
    // - we do not know how many bytes should be reserved, twice of input_len is
    //   an estimated value
    // - input is assumed to be all complete multibyte char
    if (input_len != 0) {
        TT_DO(tt_buf_reserve(converted, (input_len << 1)));
        if (!TT_OK(__charset_conv(csconv, input, input_len, converted))) {
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}

void tt_chsetconv_reset_ntv(IN struct tt_chsetconv_s *csconv)
{
    tt_chsetconv_ntv_t *sys_csconv = &csconv->sys_csconv;

    tt_buf_reset_rwp(&sys_csconv->intermediate);
}

tt_result_t __charset_conv(IN tt_chsetconv_t *csconv,
                           IN tt_u8_t *input,
                           IN tt_u32_t input_len,
                           IN tt_buf_t *output)
{
    tt_chsetconv_ntv_t *sys_csconv = &csconv->sys_csconv;
    tt_buf_t *intermediate = &sys_csconv->intermediate;
    tt_buf_t *converted = &csconv->converted;
    int ret;

    LPCWSTR wc_p;
    int wc_n;

    // from => intermediate
    if (csconv->from == TT_CHARSET_UTF16LE) {
        wc_p = (LPCWSTR)input;
        wc_n = (int)(input_len / sizeof(wchar_t));
    } else if (csconv->from == TT_CHARSET_UTF16BE) {
        tt_u8_t *pos, *end;

        // convert utf16be to utf16le

        tt_buf_reset_rwp(intermediate);
        TT_DO(tt_buf_put(intermediate, input, input_len));

        pos = TT_BUF_RPOS(intermediate);
        end = pos + TT_BUF_RLEN(intermediate);
        while ((pos + 2) <= end) {
            tt_u8_t b = *pos;
            *pos = *(pos + 1);
            *(pos + 1) = b;
            pos += 2;
        }
        TT_ASSERT_CS(pos == end);

        wc_p = (LPCWSTR)TT_BUF_RPOS(intermediate);
        wc_n = (int)(TT_BUF_RLEN(intermediate) / sizeof(wchar_t));
    } else if (csconv->from == TT_CHARSET_UTF32LE) {
        // Unicode UTF-32, little endian byte order; available only to
        // managed applications

        tt_buf_reset_rwp(intermediate);
        if (!TT_OK(__utf32le_to_utf16le(input, input_len, intermediate))) {
            return TT_FAIL;
        }

        wc_p = (LPCWSTR)TT_BUF_RPOS(intermediate);
        wc_n = (int)(TT_BUF_RLEN(intermediate) / sizeof(wchar_t));
    } else if (csconv->from == TT_CHARSET_UTF32BE) {
        tt_buf_reset_rwp(intermediate);
        if (!TT_OK(__utf32be_to_utf16le(input, input_len, intermediate))) {
            return TT_FAIL;
        }

        wc_p = (LPCWSTR)TT_BUF_RPOS(intermediate);
        wc_n = (int)(TT_BUF_RLEN(intermediate) / sizeof(wchar_t));
    } else {
        ret = MultiByteToWideChar(__charset_code_page[csconv->from],
                                  0,
                                  (LPCSTR)input,
                                  (int)input_len,
                                  NULL,
                                  0);
        if (ret <= 0) {
            TT_ERROR_NTV("fail to calc intermediate length");
            return TT_FAIL;
        }

        tt_buf_reset_rwp(intermediate);
        TT_DO(tt_buf_reserve(intermediate, ret * sizeof(wchar_t)));
        ret = MultiByteToWideChar(__charset_code_page[csconv->from],
                                  0,
                                  (LPCSTR)input,
                                  (int)input_len,
                                  (LPWSTR)TT_BUF_WPOS(intermediate),
                                  (int)(TT_BUF_WLEN(intermediate) /
                                        sizeof(wchar_t)));
        if (ret <= 0) {
            TT_ERROR_NTV("fail to generate intermediate string");
            return TT_FAIL;
        }
        tt_buf_inc_wp(intermediate, ret * sizeof(wchar_t));

        wc_p = (LPCWSTR)TT_BUF_RPOS(intermediate);
        wc_n = (int)(TT_BUF_RLEN(intermediate) / sizeof(wchar_t));
    }

    // intermediate => to
    if (csconv->to == TT_CHARSET_UTF16LE) {
        TT_DO(tt_buf_put(converted,
                         (tt_u8_t *)wc_p,
                         (tt_u32_t)(wc_n * sizeof(wchar_t))));
    } else if (csconv->to == TT_CHARSET_UTF16BE) {
        tt_u8_t *pos, *end;
        tt_u32_t n;

        // convert utf16be to utf16le

        n = TT_BUF_RLEN(converted);
        TT_DO(tt_buf_put(converted,
                         (tt_u8_t *)wc_p,
                         (tt_u32_t)(wc_n * sizeof(wchar_t))));
        TT_ASSERT_CS(TT_BUF_RLEN(converted) >= n);

        pos = TT_BUF_RPOS(converted) + n;
        end = pos + TT_BUF_RLEN(converted) - n;
        while ((pos + 2) <= end) {
            tt_u8_t b = *pos;
            *pos = *(pos + 1);
            *(pos + 1) = b;
            pos += 2;
        }
        TT_ASSERT_CS(pos == end);
    } else if (csconv->to == TT_CHARSET_UTF32LE) {
        if (!TT_OK(__utf16le_to_utf32le((tt_u8_t *)wc_p,
                                        (tt_u32_t)(wc_n * sizeof(wchar_t)),
                                        converted))) {
            return TT_FAIL;
        }
    } else if (csconv->to == TT_CHARSET_UTF32BE) {
        if (!TT_OK(__utf16le_to_utf32be((tt_u8_t *)wc_p,
                                        (tt_u32_t)(wc_n * sizeof(wchar_t)),
                                        converted))) {
            return TT_FAIL;
        }
    } else {
        ret = WideCharToMultiByte(__charset_code_page[csconv->to],
                                  0,
                                  wc_p,
                                  wc_n,
                                  NULL,
                                  0,
                                  NULL,
                                  NULL);
        if (ret <= 0) {
            TT_ERROR_NTV("fail to calc converted length");
            return TT_FAIL;
        }

        TT_DO(tt_buf_reserve(converted, ret));
        ret = WideCharToMultiByte(__charset_code_page[csconv->to],
                                  0,
                                  wc_p,
                                  wc_n,
                                  (LPSTR)TT_BUF_WPOS(converted),
                                  (int)TT_BUF_WLEN(converted),
                                  NULL,
                                  NULL);
        if (ret <= 0) {
            TT_ERROR_NTV("fail to calc converted length");
            return TT_FAIL;
        }
        tt_buf_inc_wp(converted, ret);
    }

    return TT_SUCCESS;
}

tt_result_t __utf32le_to_utf16le(IN tt_u8_t *input,
                                 IN tt_u32_t input_len,
                                 IN tt_buf_t *output)
{
    tt_u8_t *pos, *end, *opos, *oend;

    if ((input_len & 0x3) != 0) {
        TT_ERROR("utf32 length should be multiple of 4");
        return TT_FAIL;
    }

    // estimated value...
    TT_DO(tt_buf_reserve(output, input_len));
    opos = TT_BUF_WPOS(output);
    oend = opos + TT_BUF_WLEN(output);

    pos = input;
    end = pos + input_len;
    while ((pos + 4) <= end) {
        tt_u32_t n =
            (pos[0] << 0) | (pos[1] << 8) | (pos[2] << 16) | (pos[3] << 24);
        if (n < 0x10000) {
            *opos++ = (tt_u8_t)(n & 0xFF);
            *opos++ = (tt_u8_t)((n >> 8) & 0xFF);
        } else if (n <= 0x10FFFF) {
            tt_u16_t v16;

            // 0x10000 to 0x10FFFF
            n -= 0x10000;

            v16 = (tt_u16_t)(((n >> 10) & 0x3FF) + 0xD800);
            *opos++ = (tt_u8_t)(v16 & 0xFF);
            *opos++ = (tt_u8_t)((v16 >> 8) & 0xFF);

            v16 = (tt_u16_t)((n & 0x3FF) + 0xDC00);
            *opos++ = (tt_u8_t)(v16 & 0xFF);
            *opos++ = (tt_u8_t)((v16 >> 8) & 0xFF);
        } else {
            TT_ERROR("code exceeding 0x10FFFF can not be converted");
            return TT_FAIL;
        }

        pos += 4;
    }
    TT_ASSERT_CS(pos == end);

    TT_ASSERT_CS(opos < oend);
    tt_buf_set_wptr(output, opos);

    return TT_SUCCESS;
}

tt_result_t __utf32be_to_utf16le(IN tt_u8_t *input,
                                 IN tt_u32_t input_len,
                                 IN tt_buf_t *output)
{
    tt_u8_t *pos, *end, *opos, *oend;

    if ((input_len & 0x3) != 0) {
        TT_ERROR("utf32 length should be multiple of 4");
        return TT_FAIL;
    }

    // estimated value...
    TT_DO(tt_buf_reserve(output, input_len));
    opos = TT_BUF_WPOS(output);
    oend = opos + TT_BUF_WLEN(output);

    pos = input;
    end = pos + input_len;
    while ((pos + 4) <= end) {
        tt_u32_t n =
            (pos[0] << 24) | (pos[1] << 16) | (pos[2] << 8) | (pos[3] << 0);
        if (n < 0x10000) {
            *opos++ = (tt_u8_t)(n & 0xFF);
            *opos++ = (tt_u8_t)((n >> 8) & 0xFF);
        } else if (n <= 0x10FFFF) {
            tt_u16_t v16;

            // 0x10000 to 0x10FFFF
            n -= 0x10000;

            v16 = (tt_u16_t)(((n >> 10) & 0x3FF) + 0xD800);
            *opos++ = (tt_u8_t)(v16 & 0xFF);
            *opos++ = (tt_u8_t)((v16 >> 8) & 0xFF);

            v16 = (tt_u16_t)((n & 0x3FF) + 0xDC00);
            *opos++ = (tt_u8_t)(v16 & 0xFF);
            *opos++ = (tt_u8_t)((v16 >> 8) & 0xFF);
        } else {
            TT_ERROR("code exceeding 0x10FFFF can not be converted");
            return TT_FAIL;
        }

        pos += 4;
    }
    TT_ASSERT_CS(pos == end);

    TT_ASSERT_CS(opos < oend);
    tt_buf_set_wptr(output, opos);

    return TT_SUCCESS;
}

tt_result_t __utf16le_to_utf32le(IN tt_u8_t *input,
                                 IN tt_u32_t input_len,
                                 IN tt_buf_t *output)
{
    tt_u8_t *pos, *end, *opos, *oend;

    if ((input_len & 0x1) != 0) {
        TT_ERROR("utf16 length should be multiple of 2");
        return TT_FAIL;
    }

    // estimated value...
    TT_DO(tt_buf_reserve(output, input_len << 1));
    opos = TT_BUF_WPOS(output);
    oend = opos + TT_BUF_WLEN(output);

    pos = input;
    end = pos + input_len;
    while ((pos + 4) <= end) {
        tt_u32_t n1 = (pos[0] << 0) | (pos[1] << 8);
        tt_u32_t n2 = (pos[2] << 0) | (pos[3] << 8);
        if ((n1 & 0xDC00) == 0xD800) {
            tt_u32_t n;

            n = ((n1 - 0xD8) << 10) + ((n2 - 0xDC) & 0x3FF) + 0x10000;
            *opos++ = ((n >> 0) & 0xFF);
            *opos++ = ((n >> 8) & 0xFF);
            *opos++ = ((n >> 16) & 0xFF);
            *opos++ = ((n >> 24) & 0xFF);
        } else {
            *opos++ = ((n1 >> 0) & 0xFF);
            *opos++ = ((n1 >> 8) & 0xFF);
            *opos++ = ((n1 >> 16) & 0xFF);
            *opos++ = ((n1 >> 24) & 0xFF);

            *opos++ = ((n2 >> 0) & 0xFF);
            *opos++ = ((n2 >> 8) & 0xFF);
            *opos++ = ((n2 >> 16) & 0xFF);
            *opos++ = ((n2 >> 24) & 0xFF);
        }
        pos += 4;
    }
    if ((pos + 2) <= end) {
        tt_u32_t n = (pos[0] << 0) | (pos[1] << 8);

        *opos++ = ((n >> 0) & 0xFF);
        *opos++ = ((n >> 8) & 0xFF);
        *opos++ = ((n >> 16) & 0xFF);
        *opos++ = ((n >> 24) & 0xFF);

        pos += 2;
    }
    TT_ASSERT_CS(pos == end);

    TT_ASSERT_CS(opos < oend);
    tt_buf_set_wptr(output, opos);

    return TT_SUCCESS;
}


tt_result_t __utf16le_to_utf32be(IN tt_u8_t *input,
                                 IN tt_u32_t input_len,
                                 IN tt_buf_t *output)
{
    tt_u8_t *pos, *end, *opos, *oend;

    if ((input_len & 0x1) != 0) {
        TT_ERROR("utf16 length should be multiple of 2");
        return TT_FAIL;
    }

    // estimated value...
    TT_DO(tt_buf_reserve(output, input_len << 1));
    opos = TT_BUF_WPOS(output);
    oend = opos + TT_BUF_WLEN(output);

    pos = input;
    end = pos + input_len;
    while ((pos + 4) <= end) {
        tt_u32_t n1 = (pos[0] << 0) | (pos[1] << 8);
        tt_u32_t n2 = (pos[2] << 0) | (pos[3] << 8);
        if ((n1 & 0xDC00) == 0xD800) {
            tt_u32_t n;

            n = ((n1 - 0xD8) << 10) + ((n2 - 0xDC) & 0x3FF) + 0x10000;
            *opos++ = ((n >> 24) & 0xFF);
            *opos++ = ((n >> 16) & 0xFF);
            *opos++ = ((n >> 8) & 0xFF);
            *opos++ = ((n >> 0) & 0xFF);
        } else {
            *opos++ = ((n1 >> 24) & 0xFF);
            *opos++ = ((n1 >> 16) & 0xFF);
            *opos++ = ((n1 >> 8) & 0xFF);
            *opos++ = ((n1 >> 0) & 0xFF);

            *opos++ = ((n2 >> 24) & 0xFF);
            *opos++ = ((n2 >> 16) & 0xFF);
            *opos++ = ((n2 >> 8) & 0xFF);
            *opos++ = ((n2 >> 0) & 0xFF);
        }
        pos += 4;
    }
    if ((pos + 2) <= end) {
        tt_u32_t n = (pos[0] << 0) | (pos[1] << 8);

        *opos++ = ((n >> 24) & 0xFF);
        *opos++ = ((n >> 16) & 0xFF);
        *opos++ = ((n >> 8) & 0xFF);
        *opos++ = ((n >> 0) & 0xFF);

        pos += 2;
    }
    TT_ASSERT_CS(pos == end);

    TT_ASSERT_CS(opos < oend);
    tt_buf_set_wptr(output, opos);

    return TT_SUCCESS;
}
