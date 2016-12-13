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

#include <misc/tt_base64.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_B64 TT_ASSERT

#define __B64_C_PAD '='

#define __B64_C_INVALID '?'
#define __B64_U_INVALID 0xFF

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_char_t tt_s_base64_u2c_table[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/',
};
static tt_u8_t tt_s_base64_c2u_table[256] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0x3F,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24,
    0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
    0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __b64_rm_boundary(IN OUT tt_u8_t **data,
                                     IN OUT tt_u32_t *data_len,
                                     IN const tt_char_t *start_boundary,
                                     IN const tt_char_t *end_boundary);
static tt_result_t __b64_validate(IN tt_u8_t *data, IN tt_u32_t data_len);
static tt_result_t __b64_decode(IN tt_u8_t *data,
                                IN tt_u32_t data_len,
                                IN tt_base64_decode_attr_t *attr,
                                OUT tt_u8_t *decoded,
                                IN OUT tt_u32_t *decoded_len);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_inline tt_char_t __b64_u2c(IN tt_u8_t u)
{
    TT_ASSERT_B64(u < sizeof(tt_s_base64_u2c_table));
    return tt_s_base64_u2c_table[u];
}

tt_inline tt_u8_t __b64_c2u(IN tt_char_t c)
{
    // TT_ASSERT_B64(tt_s_base64_c2u_table[c] != __B64_U_INVALID);
    return tt_s_base64_c2u_table[(tt_u8_t)c];
}

tt_result_t tt_base64_decode(IN tt_u8_t *data,
                             IN tt_u32_t data_len,
                             IN OPT tt_base64_decode_attr_t *attr,
                             OUT tt_u8_t *decoded,
                             IN OUT tt_u32_t *decoded_len)
{
    tt_base64_decode_attr_t __attr;

    TT_ASSERT(data != NULL);
    TT_ASSERT(decoded_len != NULL);

    if (data_len == 0) {
        *decoded_len = 0;
        return TT_SUCCESS;
    }
    if ((decoded != NULL) && (*decoded_len == 0)) {
        return TT_SUCCESS;
    }

    if (attr == NULL) {
        tt_base64_decode_attr_default(&__attr);
        attr = &__attr;
    }

    // remove boundary
    if ((attr->start_boundary != NULL) && (attr->end_boundary != NULL) &&
        !TT_OK(__b64_rm_boundary(&data,
                                 &data_len,
                                 attr->start_boundary,
                                 attr->end_boundary))) {
        TT_ERROR("no base64 boundary found");
        return TT_FAIL;
    }
    if (data_len == 0) {
        *decoded_len = 0;
        return TT_SUCCESS;
    }

    // validate content
    if (!TT_OK(__b64_validate(data, data_len))) {
        return TT_FAIL;
    }

    // do decoding
    return __b64_decode(data, data_len, attr, decoded, decoded_len);
}

tt_result_t tt_base64_decode_alloc(IN tt_u8_t *data,
                                   IN tt_u32_t data_len,
                                   IN OPT tt_base64_decode_attr_t *attr,
                                   OUT tt_u8_t **decoded,
                                   IN OUT tt_u32_t *decoded_len)
{
    tt_u8_t *__decoded;
    tt_u32_t __decoded_len;

    if (!TT_OK(tt_base64_decode(data, data_len, attr, NULL, &__decoded_len))) {
        return TT_FAIL;
    }
    if (__decoded_len == 0) {
        *decoded = NULL;
        *decoded_len = 0;
        return TT_SUCCESS;
    }

    __decoded = (tt_u8_t *)tt_mem_alloc(__decoded_len);
    if (__decoded == NULL) {
        TT_ERROR("no mem for base64 decoding");
        return TT_FAIL;
    }

    if (!TT_OK(tt_base64_decode(data,
                                data_len,
                                attr,
                                __decoded,
                                &__decoded_len))) {
        tt_mem_free(__decoded);
        return TT_FAIL;
    }

    *decoded = __decoded;
    *decoded_len = __decoded_len;
    return TT_SUCCESS;
}

void tt_base64_decode_attr_default(IN tt_base64_decode_attr_t *attr)
{
    tt_memset(attr, 0, sizeof(tt_base64_decode_attr_t));

    // no boundary by default: all data are base64 encoded
}

tt_result_t __b64_rm_boundary(IN OUT tt_u8_t **data,
                              IN OUT tt_u32_t *data_len,
                              IN const tt_char_t *start_boundary,
                              IN const tt_char_t *end_boundary)
{
    tt_char_t *begin, *end;

    begin = tt_strstr((tt_char_t *)*data, start_boundary);
    if (begin == NULL) {
        return TT_FAIL;
    }
    begin += (tt_u32_t)tt_strlen(start_boundary);

    end = tt_strstr(begin, end_boundary);
    if (end == NULL) {
        return TT_FAIL;
    }

    *data = (tt_u8_t *)begin;
    *data_len = (tt_u32_t)(end - begin);
    return TT_SUCCESS;
}

tt_result_t __b64_validate(IN tt_u8_t *data, IN tt_u32_t data_len)
{
#define __BV_INIT 0
#define __BV_PAD 1
#define __BV_END 2

#define __IS_CRLF(c) (((c) == '\r') || ((c) == '\n'))
#define __NOT_CRLF(c) (((c) != '\r') && ((c) != '\n'))

    tt_u32_t i;
    tt_u32_t state = __BV_INIT;

    i = 0;
    while (i < data_len) {
        tt_char_t c = data[i];

        if (state == __BV_PAD) {
            if ((c != __B64_C_PAD) && __NOT_CRLF(c)) {
                TT_ERROR("only accept padding char or crlf");
                return TT_FAIL;
            }

            state = __BV_END;
            ++i;
            continue;
        } else if (state == __BV_END) {
            if (__NOT_CRLF(c)) {
                TT_ERROR("only accept crlf");
                return TT_FAIL;
            }

            ++i;
            continue;
        }
        // else state == __BV_INIT

        // CRLF
        if ((c == '\r') || (c == '\n')) {
            ++i;
            continue;
        }

        // padding char
        if (c == __B64_C_PAD) {
            /*
            RFC2045:

            Special processing is performed if fewer than 24 bits are available
            at the end of the data being encoded.  A full encoding quantum is
            always completed at the end of a body.  When fewer than 24 input
            bits
            are available in an input group, zero bits are added (on the right)
            to form an integral number of 6-bit groups.  Padding at the end of
            the data is performed using the "=" character.  Since all base64
            input is an integral number of octets, only the following cases can
            arise: (1) the final quantum of encoding input is an integral
            multiple of 24 bits; here, the final unit of encoded output will be
            an integral multiple of 4 characters with no "=" padding, (2) the
            final quantum of encoding input is exactly 8 bits; here, the final
            unit of encoded output will be two characters followed by two "="
            padding characters, or (3) the final quantum of encoding input is
            exactly 16 bits; here, the final unit of encoded output will be
            three
            characters followed by one "=" padding character.
            */

            state = __BV_PAD;
            ++i;
            continue;
        }

        // encoded char
        if (__b64_c2u(c) == __B64_U_INVALID) {
            TT_ERROR("not base64 char[0x%x]", c);
            return TT_FAIL;
        }

        ++i;
    }

    return TT_SUCCESS;
}

tt_result_t __b64_decode(IN tt_u8_t *data,
                         IN tt_u32_t data_len,
                         IN tt_base64_decode_attr_t *attr,
                         OUT tt_u8_t *decoded,
                         IN OUT tt_u32_t *decoded_len)
{
    tt_u32_t i, n, val, pad_num;
    tt_u32_t decoded_idx = 0;

    i = 0;
    n = 0;
    val = 0;
    pad_num = 0;
    while (i < data_len) {
        tt_char_t c = (tt_char_t)data[i];
        tt_u8_t v;

        if ((c == '\r') || (c == '\n')) {
            ++i;
            continue;
        }

        if (c != __B64_C_PAD) {
            // c must be less than 64 as content are already validated
            v = __b64_c2u(c);
            TT_ASSERT_B64(v != __B64_U_INVALID);
        } else {
            v = 0;
            ++pad_num;
        }

        val <<= 6;
        val |= v;

        // read 4 chars each time, then val should be a 24bit value,
        // which could be decoded as 3 bytes
        ++n;
        if (n == 4) {
            TT_ASSERT_B64(pad_num <= 2);
            if (pad_num == 0) {
                // 0 padded => 3 bytes could be decoded
                if (decoded != NULL) {
                    if ((decoded_idx + 3) > *decoded_len) {
                        TT_ERROR("not enough space for base64 decoding");
                        return TT_FAIL;
                    }

                    decoded[decoded_idx] = (val >> 16) & 0xFF;
                    decoded[decoded_idx + 1] = (val >> 8) & 0xFF;
                    decoded[decoded_idx + 2] = val & 0xFF;
                }
                decoded_idx += 3;
            } else if (pad_num == 1) {
                // 1 padded => 2 bytes could be decoded
                if (decoded != NULL) {
                    if ((decoded_idx + 2) > *decoded_len) {
                        TT_ERROR("not enough space for base64 decoding");
                        return TT_FAIL;
                    }

                    decoded[decoded_idx] = (val >> 16) & 0xFF;
                    decoded[decoded_idx + 1] = (val >> 8) & 0xFF;
                }
                decoded_idx += 2;
            } else // if (pad_num == 2)
            {
                // 2 padded => 1 bytes could be decoded
                if (decoded != NULL) {
                    if ((decoded_idx + 1) > *decoded_len) {
                        TT_ERROR("not enough space for base64 decoding");
                        return TT_FAIL;
                    }

                    decoded[decoded_idx] = (val >> 16) & 0xFF;
                }
                decoded_idx += 1;
            }

            n = 0;
            val = 0;
        }

        ++i;
    }

    if (n != 0) {
        TT_ERROR("base64 content[%d] must be multiple of 4", decoded_idx);
        return TT_FAIL;
    }

    *decoded_len = decoded_idx;
    return TT_SUCCESS;
}
