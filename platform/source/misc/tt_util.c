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

#include <misc/tt_util.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>

#include <tt_cstd_api.h>
#include <tt_util_native.h>

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

tt_u32_t tt_least_2power(IN tt_u32_t value)
{
    tt_u32_t ret = 0;

    if (TT_OK(tt_high_bit_1(value, &ret))) {
        if (value > (1u << ret)) {
            ++ret;
        }
    }
    return ret;
}

tt_result_t tt_order(IN tt_u32_t value, OUT tt_u32_t *order)
{
    tt_u32_t ret = 0;

    TT_ASSERT(order != NULL);

    ret = tt_least_2power(value);
    // no need call TT_U32_ORDER_OVERFLOW
    if (value == (1u << ret)) {
        if (order != NULL) {
            *order = ret;
        }

        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

void tt_swap(IN void *a, IN void *b, IN tt_u32_t size)
{
    tt_u8_t *swap_a = NULL;
    tt_u8_t *swap_b = NULL;
    tt_u32_t swap_index = 0;

    TT_ASSERT(a != NULL);
    TT_ASSERT(b != NULL);
    TT_ASSERT(size != 0);

    if (a == b) {
        return;
    }

    swap_a = (tt_u8_t *)a;
    swap_b = (tt_u8_t *)b;
    while (swap_index < size) {
        tt_u8_t tmp = swap_a[swap_index];

        swap_a[swap_index] = swap_b[swap_index];
        swap_b[swap_index] = tmp;

        ++swap_index;
    }
}

tt_result_t tt_high_bit_1(IN tt_u32_t value, OUT tt_u32_t *pos)
{
    tt_u32_t ret = 0;

    tt_u32_t cur_value = value;
    tt_u32_t cur_shift = (sizeof(value) << 2);

    TT_ASSERT(pos != NULL);

    // reason that avoid using inline assembly
    //  - may harm compiler optimization
    //  - not portable
    //  - no much performance improvement

    if (value == 0) {
        return TT_FAIL;
    }

    // actually a binary search
    while (cur_shift != 0) {
        if ((cur_value >> cur_shift) != 0) {
            ret += cur_shift;

            // continue searching higher part
            cur_value = (cur_value >> cur_shift);
        }
        // else continue searching lower part

        // next half part
        cur_shift = (cur_shift >> 1);
    }

    *pos = ret;
    return TT_SUCCESS;
}

tt_result_t tt_low_bit_1(IN tt_u32_t value, OUT tt_u32_t *pos)
{
    tt_u32_t ret = 0;

    tt_u32_t cur_value = value;
    tt_u32_t cur_shift = (sizeof(value) << 2);

    TT_ASSERT(pos != NULL);

    if (value == 0) {
        return TT_FAIL;
    }

    // actually a binary search
    while (cur_shift != 0) {
        if ((cur_value & ((1 << cur_shift) - 1)) == 0) {
            ret += cur_shift;

            // continue searching higher part
            cur_value = (cur_value >> cur_shift);
        }
        // else continue searching lower part

        // next half part
        cur_shift = (cur_shift >> 1);
    }

    *pos = ret;
    return TT_SUCCESS;
}

tt_u32_t tt_gcd_u32(IN tt_u32_t a, IN tt_u32_t b)
{
    while (b != 0) {
        tt_u32_t temp = a % b;
        a = b;
        b = temp;
    }
    return a;
}

tt_u32_t tt_lcm_u32(IN tt_u32_t a, IN tt_u32_t b)
{
    tt_u32_t gcd = tt_gcd_u32(a, b);
    return (a / gcd) * b;
}

void tt_hex_dump(IN tt_u8_t *buf, IN tt_u32_t buf_len, IN tt_u32_t num_per_line)
{
    tt_u32_t i, k;

    // this is simply a debug function, we use printf...

    k = 0;
    for (i = 0; i < buf_len; ++i) {
        printf("0x%02x,", buf[i]);

        if (++k >= num_per_line) {
            printf("\n");
            k = 0;
        }
    }
    printf("\n");
}

tt_u8_t tt_c2h(IN tt_char_t c, IN tt_u8_t h_if_fail)
{
    if ((c >= '0') && (c <= '9')) {
        return (tt_u8_t)(c - '0');
    } else if ((c >= 'a') && (c <= 'f')) {
        return (tt_u8_t)(c - 'a' + 0xA);
    } else if ((c >= 'A') && (c <= 'F')) {
        return (tt_u8_t)(c - 'A' + 0xA);
    } else {
        return h_if_fail;
    }
}

tt_char_t tt_h2c(IN tt_u8_t h, IN tt_u8_t c_if_fail)
{
    if ((h >= 0) && (h <= 9)) {
        return (tt_char_t)(h + '0');
    } else if ((h >= 0xa) && (h <= 0xf)) {
        return (tt_char_t)(h - 0xa + 'a');
    } else {
        return c_if_fail;
    }
}

void tt_hex2str(IN void *hex, IN tt_u32_t hex_num, OUT tt_char_t *str)
{
    tt_u32_t i, k;
    for (i = 0, k = 0; i < hex_num; ++i) {
        tt_u8_t h = ((tt_u8_t *)hex)[i];
        str[k++] = tt_h2c(h >> 4, 0);
        str[k++] = tt_h2c(h & 0xF, 0);
    }
}

void tt_str2hex(IN tt_char_t *str, IN tt_u32_t str_num, OUT void *hex)
{
    tt_u32_t i, k;
    for (i = 0, k = 0; (i + 1) < str_num; i += 2) {
        ((tt_u8_t *)hex)[k++] =
            (tt_c2h(str[i], 0) << 4) | (tt_c2h(str[i + 1], 0));
    }
}

tt_char_t *tt_cstr_copy(IN const tt_char_t *cstr)
{
    tt_u32_t len = (tt_u32_t)tt_strlen(cstr);
    tt_char_t *new_cstr = tt_malloc(len + 1);
    if (new_cstr != NULL) {
        tt_memcpy(new_cstr, cstr, len + 1);
    }
    return new_cstr;
}

tt_char_t *tt_cstr_copy_n(IN const tt_char_t *cstr, IN tt_u32_t len)
{
    tt_char_t *new_cstr;

    len = (tt_u32_t)tt_strnlen(cstr, len);

    new_cstr = tt_malloc(len + 1);
    if (new_cstr != NULL) {
        tt_memcpy(new_cstr, cstr, len);
        new_cstr[len] = 0;
    }
    return new_cstr;
}

tt_u8_t *tt_memdup(IN const tt_u8_t *addr, IN tt_u32_t len)
{
    tt_u8_t *new_mem = tt_malloc(len);
    if (new_mem != NULL) {
        tt_memcpy(new_mem, addr, len);
    }
    return new_mem;
}

tt_bool_t tt_trim_l(IN OUT tt_u8_t **p, IN OUT tt_u32_t *len, IN tt_u8_t b)
{
    tt_u8_t *beg = *p, *end = *p + *len;
    while ((beg < end) && (*beg == b)) {
        ++beg;
    }
    *p = beg;
    *len = end - beg;

    TT_ASSERT(beg <= end);
    return TT_BOOL(end == beg);
}

tt_bool_t tt_trim_r(IN OUT tt_u8_t **p, IN OUT tt_u32_t *len, IN tt_u8_t b)
{
    tt_u8_t *beg = *p, *end = *p + *len;
    --end;
    while ((end >= beg) && (*end == b)) {
        --end;
    }
    ++end;
    *p = beg;
    *len = end - beg;

    TT_ASSERT(beg <= end);
    return TT_BOOL(end == beg);
}

tt_bool_t tt_trim_lr(IN OUT tt_u8_t **p, IN OUT tt_u32_t *len, IN tt_u8_t b)
{
    return tt_trim_l(p, len, b) || tt_trim_r(p, len, b);
}
