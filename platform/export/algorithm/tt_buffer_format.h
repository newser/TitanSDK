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

/**
@file tt_buffer_format.h
@brief byte buffer

this file defines buffer format APIs
*/

#ifndef __TT_BUFFER_FORMAT__
#define __TT_BUFFER_FORMAT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// ========================================
// u64 operation
// ========================================

tt_inline tt_result_t tt_buf_put_u64(IN tt_buf_t *buf, IN tt_u64_t val_u64)
{
    return tt_buf_put(buf, (tt_u8_t *)&val_u64, sizeof(tt_u64_t));
}

tt_inline tt_result_t tt_buf_put_u64_n(IN tt_buf_t *buf, IN tt_u64_t val_u64)
{
    val_u64 = tt_hton64(val_u64);
    return tt_buf_put(buf, (tt_u8_t *)&val_u64, sizeof(tt_u64_t));
}

tt_inline tt_result_t tt_buf_get_u64(IN tt_buf_t *buf, IN tt_u64_t *val_u64)
{
    return tt_buf_get(buf, (tt_u8_t *)val_u64, sizeof(tt_u64_t));
}

tt_inline tt_result_t tt_buf_get_u64_h(IN tt_buf_t *buf, IN tt_u64_t *val_u64)
{
    tt_u64_t __val_u64;
    tt_result_t result;

    result = tt_buf_get(buf, (tt_u8_t *)&__val_u64, sizeof(tt_u64_t));
    if (TT_OK(result)) {
        *val_u64 = tt_ntoh64(__val_u64);
        return TT_SUCCESS;
    } else {
        return result;
    }
}

// ========================================
// u32 operation
// ========================================

tt_inline tt_result_t tt_buf_put_u32(IN tt_buf_t *buf, IN tt_u32_t val_u32)
{
    return tt_buf_put(buf, (tt_u8_t *)&val_u32, sizeof(tt_u32_t));
}

tt_inline tt_result_t tt_buf_put_u32_n(IN tt_buf_t *buf, IN tt_u32_t val_u32)
{
    val_u32 = tt_hton32(val_u32);
    return tt_buf_put(buf, (tt_u8_t *)&val_u32, sizeof(tt_u32_t));
}

tt_inline tt_result_t tt_buf_get_u32(IN tt_buf_t *buf, IN tt_u32_t *val_u32)
{
    return tt_buf_get(buf, (tt_u8_t *)val_u32, sizeof(tt_u32_t));
}

tt_inline tt_result_t tt_buf_get_u32_h(IN tt_buf_t *buf, IN tt_u32_t *val_u32)
{
    tt_u32_t __val_u32;
    tt_result_t result;

    result = tt_buf_get(buf, (tt_u8_t *)&__val_u32, sizeof(tt_u32_t));
    if (TT_OK(result)) {
        *val_u32 = tt_ntoh32(__val_u32);
        return TT_SUCCESS;
    } else {
        return result;
    }
}

tt_inline tt_result_t tt_buf_peek_u32(IN tt_buf_t *buf, IN tt_u32_t *val_u32)
{
    return tt_buf_peek(buf, (tt_u8_t *)val_u32, sizeof(tt_u32_t));
}

tt_inline tt_result_t tt_buf_peek_u32_h(IN tt_buf_t *buf, IN tt_u32_t *val_u32)
{
    tt_u32_t __val_u32;
    tt_result_t result;

    result = tt_buf_peek(buf, (tt_u8_t *)&__val_u32, sizeof(tt_u32_t));
    if (TT_OK(result)) {
        *val_u32 = tt_ntoh32(__val_u32);
        return TT_SUCCESS;
    } else {
        return result;
    }
}

// ========================================
// u16 operation
// ========================================

tt_inline tt_result_t tt_buf_put_u16(IN tt_buf_t *buf, IN tt_u16_t val_u16)
{
    return tt_buf_put(buf, (tt_u8_t *)&val_u16, sizeof(tt_u16_t));
}

tt_inline tt_result_t tt_buf_put_u16_n(IN tt_buf_t *buf, IN tt_u16_t val_u16)
{
    val_u16 = tt_hton16(val_u16);
    return tt_buf_put(buf, (tt_u8_t *)&val_u16, sizeof(tt_u16_t));
}

tt_inline tt_result_t tt_buf_get_u16(IN tt_buf_t *buf, IN tt_u16_t *val_u16)
{
    return tt_buf_get(buf, (tt_u8_t *)val_u16, sizeof(tt_u16_t));
}

tt_inline tt_result_t tt_buf_get_u16_h(IN tt_buf_t *buf, IN tt_u16_t *val_u16)
{
    tt_u16_t __val_u16;
    tt_result_t result;

    result = tt_buf_get(buf, (tt_u8_t *)&__val_u16, sizeof(tt_u16_t));
    if (TT_OK(result)) {
        *val_u16 = tt_ntoh16(__val_u16);
        return TT_SUCCESS;
    } else {
        return result;
    }
}

// ========================================
// u8 operation
// ========================================

tt_inline tt_result_t tt_buf_put_u8(IN tt_buf_t *buf, IN tt_u8_t val_u8)
{
    return tt_buf_put(buf, &val_u8, sizeof(tt_u8_t));
}

tt_inline tt_result_t tt_buf_get_u8(IN tt_buf_t *buf, IN tt_u8_t *val_u8)
{
    return tt_buf_get(buf, val_u8, sizeof(tt_u8_t));
}

// ========================================
// cstring/hexidecimal
// ========================================

extern tt_result_t tt_buf_put_cstr2hex(IN tt_buf_t *buf,
                                       IN const tt_char_t *cstr);

extern tt_result_t tt_buf_put_hex2cstr(IN tt_buf_t *buf,
                                       IN tt_u8_t *hex,
                                       IN tt_u32_t hex_len);

// terminating null won't be put to buf
tt_inline tt_result_t tt_buf_put_cstr(IN tt_buf_t *buf,
                                      IN const tt_char_t *cstr)
{
    return tt_buf_put(buf, (tt_u8_t *)cstr, (tt_u32_t)tt_strlen(cstr));
}

tt_inline tt_result_t tt_buf_put_csubstr(IN tt_buf_t *buf,
                                         IN const tt_char_t *cstr,
                                         IN tt_u32_t len)
{
    tt_u32_t n = (tt_u32_t)tt_strlen(cstr);
    return tt_buf_put(buf, (tt_u8_t *)cstr, TT_MIN(len, n));
}

extern tt_result_t tt_buf_vput(IN tt_buf_t *buf,
                               IN const tt_char_t *format,
                               ...);

#endif /* __TT_BUFFER_FORMAT__ */
