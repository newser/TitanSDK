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

#include <errno.h>

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

static const char *__charset_string[TT_CHARSET_NUM] = {
    // unicode encoding
    "UTF-8",
    "UTF-16LE",
    "UTF-16BE",
    "UTF-32LE",
    "UTF-32BE",

    // chinese encoding
    "GB2312",
    "GBK",
    "GB18030",
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __charset_iconv(IN iconv_t ic,
                                   IN tt_u8_t *input,
                                   IN tt_u32_t input_len,
                                   IN tt_buf_t *output);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_chsetconv_create_ntv(IN struct tt_chsetconv_s *csconv)
{
    iconv_t ic;

    ic = iconv_open(__charset_string[csconv->to],
                    __charset_string[csconv->from]);
    if (ic == (iconv_t)-1) {
        TT_ERROR_NTV("fail to create iconv, %s -> %s",
                     __charset_string[csconv->from],
                     __charset_string[csconv->to]);
        return TT_FAIL;
    }
    csconv->sys_csconv.ic = ic;

    return TT_SUCCESS;
}

void tt_chsetconv_destroy_ntv(IN struct tt_chsetconv_s *csconv)
{
    iconv_close(csconv->sys_csconv.ic);
}

tt_result_t tt_chsetconv_input_ntv(IN struct tt_chsetconv_s *csconv,
                                   IN tt_u8_t *input,
                                   IN tt_u32_t input_len)
{
    tt_buf_t *converted = &csconv->converted;
    iconv_t ic = csconv->sys_csconv.ic;

    // convert head
    if (csconv->head_complete) {
        TT_ASSERT_CS(csconv->head_len != 0);

        TT_DO(tt_buf_reserve(converted,
                             TT_CHARSET_MAX_MBCHAR_LEN + (input_len << 1)));
        if (!TT_OK(__charset_iconv(ic,
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
        if (!TT_OK(__charset_iconv(ic, input, input_len, converted))) {
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}

void tt_chsetconv_reset_ntv(IN struct tt_chsetconv_s *csconv)
{
    iconv(csconv->sys_csconv.ic, NULL, 0, NULL, 0);
}

tt_result_t __charset_iconv(IN iconv_t ic,
                            IN tt_u8_t *input,
                            IN tt_u32_t input_len,
                            IN tt_buf_t *output)
{
    char *inbuf = (char *)input;
    size_t inbytesleft = input_len;
    char *outbuf;
    size_t outbytesleft, n;

    tt_u32_t output_len = 0;
    size_t ret;

ag:
    outbuf = (char *)TT_BUF_WPOS(output) + output_len;
    outbytesleft = TT_BUF_WLEN(output) - output_len;

    n = outbytesleft;
    ret = iconv(ic, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
    if (ret != (size_t)(-1)) {
        // the only case that returned value of iconv is not -1 is that all
        // input byte sequence has been entirely converted
        output_len += (n - outbytesleft);

        tt_buf_inc_wp(output, output_len);
        return TT_SUCCESS;
    } else if (errno == E2BIG) {
        // partial data has been converted, failure due to not enough output
        // space, so extend buffer
        output_len += (n - outbytesleft);
        // wrpos of output should only been updated when returning TT_SUCCESS

        TT_DO(tt_buf_extend(output));
        goto ag;
    } else {
        TT_ERROR_NTV("iconv failed");
        return TT_FAIL;
    }
}
