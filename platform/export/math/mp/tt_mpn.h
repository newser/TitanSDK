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
@file tt_mpn.h
@brief multi precision integer

this file defines multi precision integer basic and unary operations

- each operation should be available on all platforms, and
  may be optimized on specific platform
- each operation has default mode, _shift is by bits, _create
  is by bytes, to specify another mode, add some suffix like
  _shift_u, _reserve_um
- three levels:
-- tt_mpn_xx():
   - params are mpn, may return fail
   - sign processing
-- tt_umpn_xx():
   - params are mpn, may return fail
   - param normalize, param compare, param reorder
   - condition check(divide by 0),
   - handle trivial cases(div by 1...),
   - alloc mem for result and interdiate vars,
   - this level assumes all mpn are unsigned. and it would not
     normalize params
-- __mmpn_xx():
   - params are buffers, return unit num of result mpn
   - assume buffer size is enough
-- __kmpn_xx():
   - params are buffers, return value depends on operation
   - never write value outside specified buffer size
   - this is the place to be optimized for specific platforms
-- there could be other calls between these levels
- numbers are refined before operation, for instance, a and b
  would be refined when tt_mpn_add() is called, but result is
  not refined and could be pass other api to refine
- __kmpn_xx() should be able to put src to dst, shift api should
  be __kmpn_lsfhit(dst, src, bit) but not __kmpn_lsfhit(mpn, bit)
*/

#ifndef __TT_MPN__
#define __TT_MPN__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

/** unit size in byte */
#define TT_MPN_USIZE sizeof(tt_mpn_unit_t)
/** unit size in bit */
#define TT_MPN_USIZE_BIT (sizeof(tt_mpn_unit_t) << 3)

/** byte number of n units */
#define TT_MPN_USIZE_N(n) ((n) << TT_MPN_USIZE_ORDER)

#ifdef TT_MPN_SANITY_CHECK
#define TT_ASSERT_MPN TT_ASSERT
#else
#define TT_ASSERT_MPN(...)
#endif

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

#if TT_ENV_IS_64BIT
typedef tt_u64_t tt_mpn_unit_t;
typedef tt_s64_t tt_mpn_sunit_t;
#define TT_MPN_USIZE_ORDER 3 // (1 << 3) bytes per unit
#else
typedef tt_u32_t tt_mpn_unit_t;
typedef tt_s32_t tt_mpn_sunit_t;
#define TT_MPN_USIZE_ORDER 2 // (1 << 2) bytes per unit
#endif
#define TT_MPN_USIZE_BIT_ORDER (TT_MPN_USIZE_ORDER + 3)

typedef enum {
    TT_MPN_FMT_AUTO,
    TT_MPN_FMT_HEX,
    TT_MPN_FMT_DECIMAL,

    TT_MPN_FMT_NUM
} tt_mpn_format_t;
#define TT_MPN_FMT_VALID(f) ((f) < TT_MPN_FMT_NUM)

typedef struct
{
    tt_mpn_unit_t *unit;
    tt_u32_t unit_size;
    tt_u32_t unit_num;
    tt_mpn_unit_t unit_inline;
    tt_bool_t negative : 1;
    tt_bool_t invalid : 1;
} tt_mpn_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// - val is assumed in big endian format, i.e. val[0] is MSB.
//   if val is 0x123456, val[0] is 0x12(NOT 0x1!)
// - it never fail when val_byte <= 8
extern tt_result_t tt_mpn_create(IN tt_mpn_t *mpn,
                                 IN OPT tt_u8_t *val,
                                 IN tt_u32_t val_byte,
                                 IN tt_bool_t negative,
                                 IN tt_u32_t flag);

extern tt_result_t tt_mpn_create_cstr(IN tt_mpn_t *mpn,
                                      IN OPT const tt_char_t *val,
                                      IN tt_mpn_format_t format,
                                      IN tt_u32_t flag);

extern void tt_mpn_destroy(IN tt_mpn_t *mpn);

tt_inline void tt_mpn_init(IN tt_mpn_t *mpn)
{
    // set to 0
    mpn->unit = &mpn->unit_inline;
    mpn->unit_num = 1;
    mpn->unit_size = sizeof(mpn->unit_inline);
    mpn->unit_inline = 0;
    mpn->negative = TT_FALSE;
}

tt_inline void tt_mpn_init_u(IN tt_mpn_t *mpn,
                             IN tt_mpn_unit_t unit,
                             IN tt_bool_t negative)
{
    tt_mpn_init(mpn);
    mpn->unit[0] = unit;
    mpn->negative = negative;
}

extern tt_result_t tt_mpn_set(IN tt_mpn_t *mpn,
                              IN OPT tt_u8_t *val,
                              IN tt_u32_t val_byte,
                              IN tt_bool_t negative,
                              IN tt_u32_t flag);
extern tt_result_t tt_mpn_set_cstr(IN tt_mpn_t *mpn,
                                   IN OPT const tt_char_t *val,
                                   IN tt_mpn_format_t format,
                                   IN tt_u32_t flag);

tt_inline void tt_mpn_set_u(IN tt_mpn_t *mpn,
                            IN tt_mpn_unit_t val,
                            IN tt_bool_t negative,
                            IN tt_u32_t flag)
{
    mpn->unit[0] = val;
    mpn->unit_num = 1;
    mpn->negative = negative;
}

extern tt_result_t tt_mpn_get(IN tt_mpn_t *mpn,
                              OUT OPT tt_u8_t *val,
                              OUT tt_u32_t *val_byte,
                              OUT OPT tt_bool_t *negative,
                              IN tt_u32_t flag);
// flag
/** set val[0] to 0 if the highest bit of mpn is 1 */
#define TT_MPN_GET_SIGNED (1 << 0)

// - make sure mpn has enough space for storing multiple precision which
//   would occupy bn_byte bytes, (this is different with tt_buf_reserve())
// - for example, 6(or larger) should be passed as val_byte to
//   reserve space for value 0x1234567890A
extern tt_result_t __mpn_reserve(IN tt_mpn_t *mpn, IN tt_u32_t val_byte);
#define tt_mpn_reserve(mpn, val_byte)                                          \
    TT_COND((mpn)->unit_size >= (val_byte),                                    \
            TT_SUCCESS,                                                        \
            __mpn_reserve((mpn), (val_byte)))
#define tt_mpn_reserve_u(mpn, u_num) tt_mpn_reserve(mpn, TT_MPN_USIZE_N(u_num))
#define tt_mpn_reserve_um(mpn, u_num)                                          \
    tt_mpn_reserve(mpn, TT_MPN_USIZE_N((mpn)->unit_num + u_num))

tt_inline void __mpn_normalize(IN tt_mpn_t *mpn)
{
    tt_mpn_unit_t *u = mpn->unit;
    tt_u32_t i = mpn->unit_num;

    while ((u[--i] == 0) && (i > 0))
        ;
    mpn->unit_num = i + 1;
}
#define tt_mpn_normalize(mpn)                                                  \
    do {                                                                       \
        if ((mpn)->unit[(mpn)->unit_num - 1] == 0) {                           \
            __mpn_normalize((mpn));                                            \
        }                                                                      \
    } while (0)

// release unnecessary memory, use minimum space to store value
extern void tt_mpn_refine(IN tt_mpn_t *mpn);

extern void tt_mpn_show(IN const tt_char_t *prefix,
                        IN tt_mpn_t *mpn,
                        IN tt_u32_t flag);

extern tt_u32_t tt_mpn_bitnum(IN tt_mpn_t *mpn);

tt_inline tt_u32_t tt_mpn_bytenum(IN tt_mpn_t *mpn)
{
    tt_u32_t n = tt_mpn_bitnum(mpn);
    return TT_COND((n & 0x7) != 0, (n >> 3) + 1, n >> 3);
}

extern tt_u32_t tt_mpn_h0bitnum(IN tt_mpn_t *mpn);

extern tt_u32_t __kmpn_h0bitnum(IN tt_mpn_unit_t v);

extern tt_result_t tt_mpn_rand(IN tt_mpn_t *mpn,
                               IN tt_u32_t byte_num,
                               IN tt_u32_t flag);
// flag
/** make sure byte_num random bytes are generated */
#define TT_MPN_RAND_FULL (1 << 0)

extern tt_result_t tt_mpn_rand_lessthan(IN tt_mpn_t *mpn,
                                        IN tt_mpn_t *b,
                                        IN tt_u32_t flag);

tt_inline void tt_mpn_clear(IN tt_mpn_t *mpn)
{
    mpn->unit[0] = 0;
    mpn->unit_num = 1;
}

tt_inline tt_bool_t tt_mpn_zero(IN tt_mpn_t *mpn)
{
    tt_mpn_normalize(mpn);
    return (mpn->unit_num == 1) && (mpn->unit[0] == 0);
}

tt_inline tt_bool_t tt_mpn_neg(IN tt_mpn_t *mpn)
{
    return !tt_mpn_zero(mpn) && mpn->negative;
}

#endif /* __TT_MPN__ */
