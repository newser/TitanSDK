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

#include <misc/tt_der_decode.h>

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

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_der_decode_head(IN tt_buf_t *buf, OUT OPT tt_u32_t *tag,
                               OUT OPT tt_u32_t *length, IN tt_u32_t flag)
{
    tt_u8_t val_u8;
    tt_u32_t len = 0;

    TT_ASSERT(buf != NULL);

    // tag
    TT_DO(tt_buf_get_u8(buf, &val_u8));

    if ((val_u8 & 0x1F) == 0x1F) {
        TT_ERROR("high-tag-number form is not supported");
        return TT_FAIL;
    }
    TT_SAFE_ASSIGN(tag, (val_u8 & 0x1F));
    // here we ignored the tag class

    // length
    TT_DO(tt_buf_get_u8(buf, &val_u8));

    if (val_u8 & 0x80) {
        tt_u32_t i, n;

        n = val_u8 & 0x7F;
        if (n > 4) {
            TT_ERROR("too large length in [%d] bytes", n);
            return TT_FAIL;
        }

        for (i = 0; i < n; ++i) {
            TT_DO(tt_buf_get_u8(buf, &val_u8));
            len = (len << 8) + val_u8;
        }
    } else {
        len = val_u8;
    }
    TT_SAFE_ASSIGN(length, len);

    // content
    if (flag & TT_DER_PASS_CONTENT) { TT_DO(tt_buf_inc_rp(buf, len)); }

    return TT_SUCCESS;
}

tt_result_t tt_der_decode_sequence(IN tt_buf_t *buf, OUT OPT tt_u32_t *length,
                                   IN tt_u32_t flag)
{
    tt_u32_t tag, len;

    TT_ASSERT(buf != NULL);

    if (!TT_OK(tt_der_decode_head(buf, &tag, &len, 0))) { return TT_FAIL; }

    if (tag != TT_ASN1_TAG_SEQUENCE) {
        TT_ERROR("tag[%d] is not SEQUENCE", tag);
        return TT_FAIL;
    }

    if (flag & TT_DER_PASS_CONTENT) { TT_DO(tt_buf_inc_rp(buf, len)); }

    TT_SAFE_ASSIGN(length, len);
    return TT_SUCCESS;
}

tt_result_t tt_der_decode_oid(IN tt_buf_t *buf, OUT OPT tt_u8_t **oid,
                              OUT OPT tt_u32_t *oid_len, IN tt_u32_t flag)
{
    tt_u32_t tag, len;

    TT_ASSERT(buf != NULL);

    if (!TT_OK(tt_der_decode_head(buf, &tag, &len, 0))) { return TT_FAIL; }

    if (tag != TT_ASN1_TAG_OBJECT_IDENTIFIER) {
        TT_ERROR("tag[%d] is not OBJECT_IDENTIFIER", tag);
        return TT_FAIL;
    }

    TT_SAFE_ASSIGN(oid, TT_BUF_RPOS(buf));
    TT_SAFE_ASSIGN(oid_len, len);

    if (flag & TT_DER_PASS_CONTENT) { TT_DO(tt_buf_inc_rp(buf, len)); }

    return TT_SUCCESS;
}

tt_result_t tt_der_decode_octstr(IN tt_buf_t *buf, OUT OPT tt_u8_t **octstr,
                                 OUT OPT tt_u32_t *octstr_len, IN tt_u32_t flag)
{
    tt_u32_t tag, len;

    TT_ASSERT(buf != NULL);

    if (!TT_OK(tt_der_decode_head(buf, &tag, &len, 0))) { return TT_FAIL; }

    if (tag != TT_ASN1_TAG_OCTET_STRING) {
        TT_ERROR("tag[%d] is not OCTET_STRING", tag);
        return TT_FAIL;
    }

    TT_SAFE_ASSIGN(octstr, TT_BUF_RPOS(buf));
    TT_SAFE_ASSIGN(octstr_len, len);

    if (flag & TT_DER_PASS_CONTENT) { TT_DO(tt_buf_inc_rp(buf, len)); }

    return TT_SUCCESS;
}

tt_result_t tt_der_decode_bitstr(IN tt_buf_t *buf, OUT OPT tt_u8_t **bitstr,
                                 OUT OPT tt_u32_t *bitstr_len,
                                 OUT OPT tt_u32_t *pad_bit_num,
                                 IN tt_u32_t flag)
{
    tt_u32_t tag, len;
    tt_u8_t __padded_bit;

    TT_ASSERT(buf != NULL);

    if (!TT_OK(tt_der_decode_head(buf, &tag, &len, 0))) { return TT_FAIL; }

    if (tag != TT_ASN1_TAG_BIT_STRING) {
        TT_ERROR("tag[%d] is not OCTET_STRING", tag);
        return TT_FAIL;
    }

    // the first contents octet gives the number of bits by which the
    // length of the bit string is less than the next multiple of eight
    // (this is called the "number of unused bits")
    TT_DO(tt_buf_get_u8(buf, &__padded_bit));

    TT_SAFE_ASSIGN(bitstr, TT_BUF_RPOS(buf));
    // len includes the beginning byte which indicates padding bit num
    TT_SAFE_ASSIGN(bitstr_len, len - 1);
    TT_SAFE_ASSIGN(pad_bit_num, __padded_bit);

    if (flag & TT_DER_PASS_CONTENT) { TT_DO(tt_buf_inc_rp(buf, len)); }

    return TT_SUCCESS;
}

tt_result_t tt_der_decode_s32(IN tt_buf_t *buf, OUT OPT tt_u32_t *length,
                              OUT OPT tt_s32_t *val_s32, IN tt_u32_t flag)
{
    tt_u32_t tag, len;
    tt_u32_t i, val_u32, max_u32;
    tt_u8_t u8_buf[4];

    TT_ASSERT(buf != NULL);

    if (!TT_OK(tt_der_decode_head(buf, &tag, &len, 0))) { return TT_FAIL; }

    if (tag != TT_ASN1_TAG_INTEGER) {
        TT_ERROR("tag[%d] is not INTEGER", tag);
        return TT_FAIL;
    }

    // check length, as we only want a s32 value
    if (len > 4) {
        TT_ERROR("too long[%d] for s32", len);
        return TT_FAIL;
    }
    TT_SAFE_ASSIGN(length, len);

    // calc value
    TT_DO(tt_buf_get(buf, u8_buf, len));
    val_u32 = 0;
    max_u32 = 0;
    for (i = 0; i < len; ++i) {
        val_u32 <<= 8;
        val_u32 += u8_buf[i];

        max_u32 <<= 8;
        max_u32 += 0xFF;
    }
    if (u8_buf[0] & 0x80) {
        // negative value
        max_u32 -= val_u32;
        TT_SAFE_ASSIGN(val_s32, -((tt_s32_t)max_u32));
    } else {
        TT_SAFE_ASSIGN(val_s32, ((tt_u32_t)val_u32));
    }

    if (!(flag & TT_DER_PASS_CONTENT)) { buf->rpos -= len; }

    return TT_SUCCESS;
}

tt_result_t tt_der_decode_null(IN tt_buf_t *buf, IN tt_u32_t flag)
{
    tt_u32_t tag, len;

    TT_ASSERT(buf != NULL);

    if (!TT_OK(tt_der_decode_head(buf, &tag, &len, 0))) { return TT_FAIL; }

    if (tag != TT_ASN1_TAG_NULL) {
        TT_ERROR("not NULL", len);
        return TT_FAIL;
    }

    if (len != 0) {
        TT_ERROR("NULL's length must be 0", len);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
