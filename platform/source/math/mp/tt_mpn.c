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

#include <math/mp/tt_mpn.h>

#include <math/mp/tt_mpn_cache.h>
#include <math/mp/tt_mpn_common.h>
#include <misc/tt_assert.h>
#include <os/tt_thread.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __IS_D(c) (((c) >= '0') && ((c) <= '9'))
#define __D2N(c) ((c) - '0')

#define __IS_H_U(c) (((c) >= 'A') && ((c) <= 'F'))
#define __H2N_U(c) (((c) - 'A') + 0xa)

#define __IS_H_L(c) (((c) >= 'a') && ((c) <= 'f'))
#define __H2N_L(c) (((c) - 'a') + 0xa)

#define __IS_H(c) (__IS_D(c) || __IS_H_U(c) || __IS_H_L(c))
#define __H2N(c)                                                               \
    TT_COND(__IS_D(c), __D2N(c), TT_COND(__IS_H_U(c), __H2N_U(c), __H2N_L(c)))

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

static tt_result_t __mpn_create(IN tt_mpn_t *a,
                                IN OPT tt_u8_t *val,
                                IN tt_u32_t val_byte,
                                IN tt_bool_t negative);
static tt_result_t __mpn_create_s(IN tt_mpn_t *a,
                                  IN const tt_char_t *val,
                                  IN tt_mpn_format_t format,
                                  IN tt_u32_t flag);
static tt_result_t __mpn_create_auto(IN tt_mpn_t *a,
                                     IN const tt_char_t *val,
                                     IN tt_bool_t negative,
                                     IN tt_u32_t flag);
static tt_result_t __mpn_create_hex(IN tt_mpn_t *a,
                                    IN const tt_char_t *val,
                                    IN tt_bool_t negative,
                                    IN tt_u32_t flag);
static tt_result_t __mpn_create_decimal(IN tt_mpn_t *a,
                                        IN const tt_char_t *val,
                                        IN tt_bool_t negative,
                                        IN tt_u32_t flag);

static void __mpn_free(IN tt_mpn_t *a);

static void __mpn_shrink(IN tt_mpn_t *a);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_mpn_create(IN tt_mpn_t *a,
                          IN OPT tt_u8_t *val,
                          IN tt_u32_t val_byte,
                          IN tt_bool_t negative,
                          IN tt_u32_t flag)
{
    tt_mpn_init(a);
    return tt_mpn_set(a, val, val_byte, negative, flag);
}

tt_result_t tt_mpn_create_cstr(IN tt_mpn_t *a,
                               IN OPT const tt_char_t *val,
                               IN tt_mpn_format_t format,
                               IN tt_u32_t flag)
{
    tt_mpn_init(a);
    return tt_mpn_set_cstr(a, val, format, flag);
}

void tt_mpn_destroy(IN tt_mpn_t *a)
{
    __mpn_free(a);
}

tt_result_t tt_mpn_set(IN tt_mpn_t *a,
                       IN OPT tt_u8_t *val,
                       IN tt_u32_t val_byte,
                       IN tt_bool_t negative,
                       IN tt_u32_t flag)
{
    if ((val != NULL) && (val_byte != 0)) {
        return __mpn_create(a, val, val_byte, negative);
    } else {
        tt_mpn_clear(a);
        return TT_SUCCESS;
    }
}

tt_result_t tt_mpn_set_cstr(IN tt_mpn_t *a,
                            IN OPT const tt_char_t *val,
                            IN tt_mpn_format_t format,
                            IN tt_u32_t flag)
{
    if ((val != NULL) && (val[0] != 0)) {
        return __mpn_create_s(a, val, format, flag);
    } else {
        tt_mpn_clear(a);
        return TT_SUCCESS;
    }
}

tt_result_t tt_mpn_get(IN tt_mpn_t *mpn,
                       OUT OPT tt_u8_t *val,
                       OUT tt_u32_t *val_byte,
                       OUT OPT tt_bool_t *negative,
                       IN tt_u32_t flag)
{
    tt_u32_t n = tt_mpn_bytenum(mpn);
    tt_u32_t val_idx, i, bn, k, prefix;
    tt_mpn_unit_t u;

    if (n == 0) {
        // 0
        if (val == NULL) {
            *val_byte = 1;
            TT_SAFE_ASSIGN(negative, mpn->negative);
            return TT_SUCCESS;
        }

        if (*val_byte < 1) {
            TT_ERROR("less than required length");
            return TT_FAIL;
        }
        *val = 0;
        *val_byte = 1;
        TT_SAFE_ASSIGN(negative, mpn->negative);
        return TT_SUCCESS;
    }

    if ((flag & TT_MPN_GET_SIGNED) &&
        (mpn->unit[mpn->unit_num - 1] &
         (((tt_mpn_unit_t)1) << (TT_MPN_USIZE_BIT - 1)))) {
        prefix = 1;
    } else {
        prefix = 0;
    }

    if (val == NULL) {
        *val_byte = n + prefix;
        TT_SAFE_ASSIGN(negative, mpn->negative);
        return TT_SUCCESS;
    }

    if (*val_byte < (n + prefix)) {
        TT_ERROR("less than required dh secret length");
        return TT_FAIL;
    }

    val_idx = 0;
    if (prefix != 0) {
        val[val_idx++] = 0;
    }

    // first unit may need less than TT_MPN_USIZE bytes
    i = mpn->unit_num - 1;
    bn = n & ((1 << TT_MPN_USIZE_ORDER) - 1);
    if (bn != 0) {
        u = mpn->unit[i];
        for (k = bn - 1; k != ~0; --k) {
            val[val_idx++] = (tt_u8_t)(u >> (k << 3));
        }
        --i;
    }

    // each following unit need TT_MPN_USIZE bytes
    for (; i != ~0; --i) {
        u = mpn->unit[i];
        for (k = TT_MPN_USIZE - 1; k != ~0; --k) {
            val[val_idx++] = (tt_u8_t)(u >> (k << 3));
        }
    }
    TT_ASSERT_MPN(val_idx == (n + prefix));

    *val_byte = (n + prefix);
    TT_SAFE_ASSIGN(negative, mpn->negative);

    return TT_SUCCESS;
}

tt_result_t __mpn_reserve(IN tt_mpn_t *a, IN tt_u32_t val_byte)
{
    tt_mpn_unit_t *new_unit;
    tt_u32_t i;

    TT_ASSERT_MPN(a->unit_size < val_byte);

    // allocate new units
    val_byte = tt_mem_size(val_byte);
    new_unit = (tt_mpn_unit_t *)tt_mem_alloc(val_byte);
    if (new_unit == NULL) {
        TT_ERROR("no mem for a reserving");
        return TT_FAIL;
    }
    // tt_memset(new_unit, 0, val_byte);
    for (i = 0; i < a->unit_num; ++i) {
        new_unit[i] = a->unit[i];
    }

    // free old units
    __mpn_free(a);
    a->unit = new_unit;
    a->unit_size = val_byte;

    return TT_SUCCESS;
}

void tt_mpn_refine(IN tt_mpn_t *a)
{
    tt_mpn_normalize(a);

    __mpn_shrink(a);
}

void tt_mpn_show(IN const tt_char_t *prefix, IN tt_mpn_t *a, IN tt_u32_t flag)
{
    tt_u32_t buf_size, i, n;
    tt_char_t *buf;
    tt_mpn_unit_t v;

    buf_size = (tt_u32_t)tt_strlen(prefix); // prefix
    buf_size += a->unit_num * (TT_MPN_USIZE * 2 + 2); // number
    buf_size += 10; // some extra space

    buf = (tt_char_t *)tt_mem_alloc(buf_size + 1);
    if (buf == NULL) {
        TT_ERROR("no mem to print a");
        return;
    }

    n = 0;

    // sign
    if (a->negative) {
        n += tt_snprintf(buf + n, buf_size - n, "-");
    }

    // hex
    n += tt_snprintf(buf + n, buf_size - n, "0x");

    // first unit
    for (i = a->unit_num - 1; i != ~0; --i) {
        v = a->unit[i];
        if (v != 0) {
            break;
        }
    }
    if (v == 0) {
        n += tt_snprintf(buf + n, buf_size - n, "0");
        goto prt;
    }
#if TT_ENV_IS_64BIT
    if ((v >> 32) != 0) {
        n += tt_snprintf(buf + n, buf_size - n, "%x", (tt_u32_t)(v >> 32));
        n += tt_snprintf(buf + n, buf_size - n, "%08x ", (tt_u32_t)v);
    } else {
        n += tt_snprintf(buf + n, buf_size - n, "%x ", (tt_u32_t)v);
    }
#else
    n += tt_snprintf(buf + n, buf_size - n, "%x ", (tt_u32_t)v);
#endif
    --i;

    // following units
    for (; i != ~0; --i) {
        v = a->unit[i];
#if TT_ENV_IS_64BIT
        n += tt_snprintf(buf + n, buf_size - n, "%08x", (tt_u32_t)(v >> 32));
#endif
        n += tt_snprintf(buf + n, buf_size - n, "%08x ", (tt_u32_t)v);
    }

prt:

    buf[n] = 0;

    // todo: make adaptive log buf size
    TT_INFO("%s%s", prefix, buf);

    tt_mem_free(buf);
}

tt_u32_t tt_mpn_bitnum(IN tt_mpn_t *mpn)
{
    tt_mpn_unit_t v;
    tt_u32_t n;

    tt_mpn_normalize(mpn);

    v = mpn->unit[mpn->unit_num - 1];
    if (v == 0) {
        return 0;
    }

    n = __kmpn_h0bitnum(v);
    TT_ASSERT_MPN(n < TT_MPN_USIZE_BIT);
    n = TT_MPN_USIZE_BIT - n;

    return n + ((mpn->unit_num - 1) << TT_MPN_USIZE_BIT_ORDER);
}

tt_u32_t tt_mpn_h0bitnum(IN tt_mpn_t *mpn)
{
    tt_mpn_unit_t v = mpn->unit[mpn->unit_num - 1];

    if (v == 0) {
        return TT_MPN_USIZE_BIT;
    }
    return __kmpn_h0bitnum(v);
}

tt_u32_t __kmpn_h0bitnum(tt_mpn_unit_t v)
{
    tt_u32_t n = 0;
    tt_u32_t shift = (sizeof(tt_mpn_unit_t) << 2);

    TT_ASSERT_MPN(v != 0);
    while (shift != 0) {
        if ((v >> shift) != 0) {
            n += shift;
            v >>= shift;
        }

        shift >>= 1;
    }
    TT_ASSERT_MPN(n < TT_MPN_USIZE_BIT);

    return TT_MPN_USIZE_BIT - n - 1;
}

tt_result_t tt_mpn_rand(IN tt_mpn_t *mpn,
                        IN tt_u32_t byte_num,
                        IN tt_u32_t flag)
{
    tt_u32_t u_n = byte_num >> TT_MPN_USIZE_ORDER;
    tt_u32_t i;

    if (byte_num == 0) {
        tt_mpn_clear(mpn);
        return TT_SUCCESS;
    }

    if ((byte_num & ((1 << TT_MPN_USIZE_ORDER) - 1)) != 0) {
        ++u_n;
    }

    if (!TT_OK(tt_mpn_reserve_u(mpn, u_n))) {
        return TT_FAIL;
    }

    // generate random numbers
    for (i = 0; i < u_n; ++i) {
        mpn->unit[i] = (tt_mpn_unit_t)tt_rand_u64();
    }
    mpn->unit_num = u_n;

    // make sure the mpn has only byte_num bytes
    i = byte_num & ((1 << TT_MPN_USIZE_ORDER) - 1);
    if (i != 0) {
        mpn->unit[mpn->unit_num - 1] &= (((tt_mpn_unit_t)1) << (i << 3)) - 1;
    }

    if (flag & TT_MPN_RAND_FULL) {
        tt_mpn_unit_t h;

        if (i == 0) {
            i = TT_MPN_USIZE;
        }
        --i;

        while ((h = (tt_u8_t)tt_rand_u64()) == 0)
            ;

        mpn->unit[mpn->unit_num - 1] |= (h << (i << 3));
    }

    return TT_SUCCESS;
}

tt_result_t tt_mpn_rand_lessthan(IN tt_mpn_t *mpn,
                                 IN tt_mpn_t *b,
                                 IN tt_u32_t flag)
{
    tt_u32_t u_n = b->unit_num;
    tt_u32_t i;

    if (tt_mpn_zero(b)) {
        TT_ERROR("can not gen rand less than 0");
        return TT_FAIL;
    }

    if (!TT_OK(tt_mpn_reserve_u(mpn, u_n))) {
        return TT_FAIL;
    }

    // generate random numbers
    for (i = 0; i < u_n; ++i) {
        mpn->unit[i] = (tt_mpn_unit_t)tt_rand_u64();
    }
    mpn->unit_num = u_n;

    // should less than b
    mpn->unit[mpn->unit_num - 1] %= b->unit[b->unit_num - 1];

    if (flag & TT_MPN_RAND_FULL) {
        tt_mpn_unit_t bh, h;

        i = tt_mpn_bytenum(b) & ((1 << TT_MPN_USIZE_ORDER) - 1);
        if (i == 0) {
            i = TT_MPN_USIZE;
        }
        --i;

        bh = b->unit[b->unit_num - 1] >> (i << 3);
        TT_ASSERT_MPN(bh != 0);
        while ((h = (tt_rand_u64() % bh)) == 0)
            ;

        mpn->unit[mpn->unit_num - 1] &= ~(((tt_mpn_unit_t)0xff) << (i << 3));
        mpn->unit[mpn->unit_num - 1] |= (h << (i << 3));
    }

    return TT_SUCCESS;
}

tt_result_t __mpn_create(IN tt_mpn_t *a,
                         IN tt_u8_t *val,
                         IN tt_u32_t val_byte,
                         IN tt_bool_t negative)
{
    tt_u32_t i, j, n;
    tt_u32_t unit_num, unit_i, val_i;
    tt_mpn_unit_t unit;

    TT_ASSERT(val_byte > 0);
    n = val_byte;
    if ((val_byte % TT_MPN_USIZE) != 0) {
        n /= TT_MPN_USIZE;
        ++n;
        n *= TT_MPN_USIZE;
    }
    if (!TT_OK(tt_mpn_reserve(a, n))) {
        return TT_FAIL;
    }
    unit_num = n / TT_MPN_USIZE;
    unit_i = unit_num - 1;
    val_i = 0;

    // first unit
    n = val_byte % TT_MPN_USIZE;
    if (n != 0) {
        unit = 0;
        for (i = 0; i < n; ++i) {
            unit <<= 8;
            unit += val[val_i++];
        }
        a->unit[unit_i--] = unit;
    }

    // following units
    n = val_byte / TT_MPN_USIZE;
    for (j = 0; j < n; ++j) {
        unit = 0;
        for (i = 0; i < TT_MPN_USIZE; ++i) {
            unit <<= 8;
            unit += (tt_mpn_unit_t)val[val_i++];
        }
        a->unit[unit_i--] = unit;
    }
    TT_ASSERT(val_i == val_byte);
    TT_ASSERT(unit_i == ~0);

    // ignore beginning 0
    for (i = unit_num - 1; i > 0; --i) {
        if (a->unit[i] != 0) {
            break;
        }
    }
    a->unit_num = i + 1;

    a->negative = negative;
    a->invalid = TT_FALSE;

    return TT_SUCCESS;
}

tt_result_t __mpn_create_s(IN tt_mpn_t *a,
                           IN const tt_char_t *val,
                           IN tt_mpn_format_t format,
                           IN tt_u32_t flag)
{
    tt_bool_t negative;

    while (*val == ' ') {
        ++val;
    }

    if (*val == '+') {
        negative = TT_FALSE;
        ++val;
    } else if (*val == '-') {
        negative = TT_TRUE;
        ++val;
    } else {
        negative = TT_FALSE;
    }

    switch (format) {
        case TT_MPN_FMT_HEX: {
            return __mpn_create_hex(a, val, negative, flag);
        } break;
        case TT_MPN_FMT_DECIMAL: {
            return __mpn_create_decimal(a, val, negative, flag);
        } break;
        case TT_MPN_FMT_AUTO:
        default: {
            return __mpn_create_auto(a, val, negative, flag);
        } break;
    }
}

tt_result_t __mpn_create_auto(IN tt_mpn_t *a,
                              IN const tt_char_t *val,
                              IN tt_bool_t negative,
                              IN tt_u32_t flag)
{
    const tt_char_t *p;
    tt_char_t c;

    // begin with 0x: hex
    if ((tt_strncmp(val, "0x", 2) == 0) || (tt_strncmp(val, "0X", 2) == 0)) {
        return __mpn_create_hex(a, val, negative, flag);
    }

    // found any ABCDEF
    p = val;
    while ((c = *p++) != 0) {
        if (__IS_H_U(c) || __IS_H_L(c)) {
            return __mpn_create_hex(a, val, negative, flag);
        }
    }

    // otherwise: decimal
    return __mpn_create_decimal(a, val, negative, flag);
}

tt_result_t __mpn_create_hex(IN tt_mpn_t *a,
                             IN const tt_char_t *val,
                             IN tt_bool_t negative,
                             IN tt_u32_t flag)
{
    const tt_char_t *p;
    tt_char_t c;
    tt_u32_t len, u_num, u_idx, val_idx, i;
    tt_mpn_unit_t unit;

    if ((tt_strncmp(val, "0x", 2) == 0) || (tt_strncmp(val, "0x", 2) == 0)) {
        val += 2;
    }

    p = val;
    while ((c = *p++) != 0) {
        if (!__IS_H(c)) {
            TT_ERROR("invalid hex char: %c", c);
            return TT_FAIL;
        }
    }

    len = (tt_u32_t)tt_strlen(val);
    u_num = len / (TT_MPN_USIZE * 2);
    len %= (TT_MPN_USIZE * 2);
    if (len != 0) {
        ++u_num;
    }

    TT_ASSERT_MPN(u_num != 0);
    if (!TT_OK(tt_mpn_reserve_u(a, u_num))) {
        return TT_FAIL;
    }

    val_idx = 0;
    u_idx = u_num - 1;
    if (len != 0) {
        unit = 0;
        for (i = 0; i < len; ++i) {
            c = val[val_idx++];
            unit <<= 4;
            unit += __H2N(c);
        }
        a->unit[u_idx--] = unit;
    }
    for (; u_idx != ~0; --u_idx) {
        unit = 0;
        for (i = 0; i < (TT_MPN_USIZE * 2); ++i) {
            c = val[val_idx++];
            unit <<= 4;
            unit += __H2N(c);
        }
        a->unit[u_idx] = unit;
    }
    TT_ASSERT_MPN(val[val_idx] == 0);

    a->unit_num = u_num;
    a->negative = negative;
    tt_mpn_normalize(a);

    return TT_SUCCESS;
}

tt_result_t __mpn_create_decimal(IN tt_mpn_t *a,
                                 IN const tt_char_t *val,
                                 IN tt_bool_t negative,
                                 IN tt_u32_t flag)
{
    TT_ERROR("currently not supported");
    return TT_FAIL;
}

void __mpn_free(IN tt_mpn_t *a)
{
    if (a->unit != &a->unit_inline) {
        tt_mem_free(a->unit);
    }
}

void __mpn_shrink(IN tt_mpn_t *a)
{
    tt_u32_t new_size, i;
    tt_mpn_unit_t *new_unit;

    new_size = TT_MPN_USIZE_N(a->unit_num);
    if (new_size >= a->unit_size) {
        return;
    }

    if (new_size <= TT_MPN_USIZE) {
        new_size = TT_MPN_USIZE;
        new_unit = &a->unit_inline;
        TT_ASSERT(a->unit_num == 1);
        a->unit_inline = a->unit[0];
    } else {
        new_size = tt_mem_size(new_size);
        new_unit = (tt_mpn_unit_t *)tt_mem_alloc(new_size);
        if (new_unit) {
            TT_WARN("fail to refine a");
            return;
        }
        // tt_memset(new_unit, 0, new_size);
        for (i = 0; i < a->unit_num; ++i) {
            new_unit[i] = a->unit[i];
        }
    }

    __mpn_free(a);
    a->unit = new_unit;
    a->unit_size = new_size;
}
