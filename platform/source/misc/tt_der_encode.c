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

#include <misc/tt_der_encode.h>

#include <algorithm/tt_buffer_format.h>
#include <misc/tt_asn1_def.h>
#include <misc/tt_assert.h>

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

static tt_u32_t __octnum_of_length(IN tt_u32_t len);
static tt_u32_t __octnum_of_s32(IN tt_s32_t val_s32);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_der_encode_head(IN tt_buf_t *buf, IN tt_u32_t tag,
                               IN tt_u32_t length, IN tt_u32_t flag)
{
    TT_ASSERT(buf != NULL);

    // tag
    if ((tag & 0x1F) == 0x1F) {
        TT_ERROR("high-tag-number form is not supported");
        return TT_E_BADARG;
    }
    TT_DO(tt_buf_put_u8(buf, (tt_u8_t)tag));

    // length
    if (length & 0xFF000000) {
        TT_DO(tt_buf_put_u8(buf, 0x84));
        TT_DO(tt_buf_put_u32_n(buf, length));
    } else if (length & 0xFF0000) {
        TT_DO(tt_buf_put_u8(buf, 0x83));
        TT_DO(tt_buf_put_u8(buf, (length & 0xFF0000) >> 16));
        TT_DO(tt_buf_put_u16_n(buf, (tt_u16_t)length));
    } else if (length & 0xFF00) {
        TT_DO(tt_buf_put_u8(buf, 0x82));
        TT_DO(tt_buf_put_u16_n(buf, (tt_u16_t)length));
    } else if (length & 0x80) {
        // [128, 255]
        TT_DO(tt_buf_put_u8(buf, 0x81));
        TT_DO(tt_buf_put_u8(buf, (tt_u8_t)length));
    } else {
        // [0, 127]
        TT_DO(tt_buf_put_u8(buf, (tt_u8_t)length));
    }

    return TT_SUCCESS;
}

tt_u32_t tt_der_head_len(IN tt_u32_t tag, IN tt_u32_t length)
{
    TT_ASSERT((tag & 0x1F) != 0x1F);

    return 1 + __octnum_of_length(length);
}

tt_result_t tt_der_encode_sequence(IN tt_buf_t *buf, IN tt_u32_t length,
                                   IN tt_u32_t flag)
{
    return tt_der_encode_head(buf, TT_ASN1_SEQUENCE, length, flag);
}

tt_u32_t tt_der_sequence_len(IN tt_u32_t content_len)
{
    return tt_der_head_len(TT_ASN1_SEQUENCE, content_len) + content_len;
}

tt_result_t tt_der_encode_oid(IN tt_buf_t *buf, IN tt_u8_t *oid,
                              IN tt_u32_t oid_len, IN tt_u32_t flag)
{
    TT_DO(tt_der_encode_head(buf, TT_ASN1_OBJECT_IDENTIFIER, oid_len, 0));
    TT_DO(tt_buf_put(buf, oid, oid_len));

    return TT_SUCCESS;
}

tt_u32_t tt_der_oid_len(IN tt_u8_t *oid, IN tt_u32_t oid_len)
{
    return tt_der_head_len(TT_ASN1_OBJECT_IDENTIFIER, oid_len) + oid_len;
}

tt_result_t tt_der_encode_octstr(IN tt_buf_t *buf, IN tt_u8_t *octstr,
                                 IN tt_u32_t octstr_len, IN tt_u32_t flag)
{
    TT_DO(tt_der_encode_head(buf, TT_ASN1_OCTET_STRING, octstr_len, 0));
    TT_DO(tt_buf_put(buf, octstr, octstr_len));

    return TT_SUCCESS;
}

tt_u32_t tt_der_octstr_len(IN tt_u8_t *octstr, IN tt_u32_t octstr_len)
{
    return tt_der_head_len(TT_ASN1_OBJECT_IDENTIFIER, octstr_len) + octstr_len;
}

tt_result_t tt_der_encode_bitstr(IN tt_buf_t *buf, IN OPT tt_u8_t *bitstr,
                                 IN tt_u32_t bitstr_len,
                                 IN tt_u32_t pad_bit_num, IN tt_u32_t flag)
{
    TT_DO(tt_der_encode_head(buf, TT_ASN1_BIT_STRING, 1 + bitstr_len, 0));

    TT_ASSERT(pad_bit_num < 8);
    TT_DO(tt_buf_put_u8(buf, (tt_u8_t)pad_bit_num));

    if (bitstr != NULL) { TT_DO(tt_buf_put(buf, bitstr, bitstr_len)); }

    return TT_SUCCESS;
}

tt_u32_t tt_der_bitstr_len(IN tt_u8_t *bitstr, IN tt_u32_t bitstr_len,
                           IN tt_u32_t pad_bit_num)
{
    TT_ASSERT(pad_bit_num < 8);
    return tt_der_head_len(TT_ASN1_BIT_STRING, bitstr_len) + 1 + bitstr_len;
}

tt_result_t tt_der_encode_s32(IN tt_buf_t *buf, IN tt_s32_t val_s32,
                              IN tt_u32_t flag)
{
    tt_u32_t val_u32 = (tt_u32_t)val_s32;
    tt_u32_t n = __octnum_of_s32(val_s32);

    TT_DO(tt_der_encode_head(buf, TT_ASN1_INTEGER, n, flag));

    TT_ASSERT((n >= 1) && (n <= 4));
    if (n == 4) {
        TT_DO(tt_buf_put_u32_n(buf, val_u32));
    } else if (n == 3) {
        TT_DO(tt_buf_put_u8(buf, (val_u32 & 0xFF0000) >> 16));
        TT_DO(tt_buf_put_u16_n(buf, (tt_u16_t)val_u32));
    } else if (n == 2) {
        TT_DO(tt_buf_put_u16_n(buf, (tt_u16_t)val_u32));
    } else {
        TT_DO(tt_buf_put_u8(buf, (tt_u8_t)val_u32));
    }

    return TT_SUCCESS;
}

tt_u32_t tt_der_s32_len(IN tt_s32_t val_s32)
{
    tt_u32_t n = __octnum_of_s32(val_s32);

    return tt_der_head_len(TT_ASN1_INTEGER, n) + n;
}

tt_result_t tt_der_encode_null(IN tt_buf_t *buf, IN tt_u32_t flag)
{
    const tt_u16_t val_null = 0x0500;

    TT_DO(tt_buf_put_u16_n(buf, val_null));

    return TT_SUCCESS;
}

tt_u32_t tt_der_null_len()
{
    // 0x0500
    return 2;
}

tt_result_t tt_der_encode_integer(IN tt_buf_t *buf, IN tt_u8_t *integer,
                                  IN tt_u32_t integer_len, IN tt_u32_t flag)
{
    TT_DO(tt_der_encode_head(buf, TT_ASN1_INTEGER, integer_len, 0));
    TT_DO(tt_buf_put(buf, integer, integer_len));

    return TT_SUCCESS;
}

tt_u32_t tt_der_integer_len(IN tt_u8_t *integer, IN tt_u32_t integer_len)
{
    return tt_der_head_len(TT_ASN1_INTEGER, integer_len) + integer_len;
}

tt_u32_t __octnum_of_length(IN tt_u32_t length)
{
    if (length & 0xFF000000) {
        return 5; // 1 + 4
    } else if (length & 0xFF0000) {
        return 4; // 1 + 3
    } else if (length & 0xFF00) {
        return 3; // 1 + 2
    } else if (length & 0x80) {
        return 2; // 1 + 1
    } else {
        // [0, 127]
        return 1;
    }
}

tt_u32_t __octnum_of_s32(IN tt_s32_t val_s32)
{
    tt_u32_t val_u32 = (tt_u32_t)val_s32;

    if (val_s32 == 0) {
        return 1;
    } else if (val_s32 > 0) {
        if (val_u32 & 0xFF000000) {
            return 4;
        } else if (val_u32 & 0xFF0000) {
            return TT_COND((val_u32 & 0x800000), 4, 3);
        } else if (val_u32 & 0xFF00) {
            return TT_COND((val_u32 & 0x8000), 3, 2);
        } else {
            return TT_COND((val_u32 & 0x80), 2, 1);
        }
    } else {
        if ((val_u32 & 0xFF000000) != 0xFF000000) {
            return 4;
        } else if ((val_u32 & 0xFF0000) != 0xFF0000) {
            return TT_COND(((val_u32 & 0x800000) == 0), 4, 3);
        } else if ((val_u32 & 0xFF00) != 0xFF00) {
            return TT_COND(((val_u32 & 0x8000) == 0), 3, 2);
        } else {
            return TT_COND(((val_u32 & 0x80) == 0), 2, 1);
        }
    }
}
