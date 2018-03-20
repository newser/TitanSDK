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

/**
@file tt_util.h
@brief some utilities

this file provided some basic utilities for platform usage
*/

#ifndef __TT_UTIL__
#define __TT_UTIL__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <config/tt_platform_config.h>
#include <init/tt_platform_info.h>
#include <misc/tt_assert.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

/**
@def TT_MAX(a, b)
return larger value
*/
#define TT_MAX(a, b) ((a) > (b) ? (a) : (b))
/**
@def TT_MIN(a, b)
return less value
*/
#define TT_MIN(a, b) ((a) < (b) ? (a) : (b))

/**
@def TT_PTR_INC(ptr, bytes)
move pointer forward

@param [in] type type of returned pointer
@param [in] ptr pointer to be moved
@param [in] bytes how long the pointer should be moved forward

@return
a pointer of type as specified, after moving pointer in parameter
*/
#define TT_PTR_INC(type, ptr, bytes) ((type *)(((tt_u8_t *)(ptr)) + (bytes)))
/**
@def TT_PTR_DEC(ptr, bytes)
move pointer backward

@param [in] type type of returned pointer
@param [in] ptr pointer to be moved
@param [in] bytes how long the pointer should be moved backward

@return
a pointer of type as specified, after moving pointer in parameter
*/
#define TT_PTR_DEC(type, ptr, bytes) ((type *)(((tt_u8_t *)(ptr)) - (bytes)))
/**
@def TT_PTR_DIFF(p1, p2)
calculate the distance in bytes between pointers specifed by params

@param [in] p1 pointer
@param [in] p2 pointer

@return
distance in bytes

@note
- returned distance is in byte unit
- type of returned distance is tt_ptrdiff_t, may need convert explicitly
*/
#define TT_PTR_DIFF(p1, p2)                                                    \
    TT_COND(((tt_u8_t *)(p1)) <= ((tt_u8_t *)(p2)),                            \
            ((tt_ptrdiff_t)(((tt_u8_t *)(p2)) - ((tt_u8_t *)(p1)))),           \
            ((tt_ptrdiff_t)(((tt_u8_t *)(p1)) - ((tt_u8_t *)(p2)))))

/**
@def TT_ALIGN_INC(n, m, n_type, wide_type)
align n in m order

@param [inout] n the number to be aligned
@param [in] m alignment order
@param [in] n_type type of n, this is to prevent from shifting overflow

@note
m must be less bit number of n_type substracting 1. for example, if type of n
is tt_u8_t, then maxmum value of m is 6. this is to prevent from overflow
*/
#define TT_ALIGN_INC(n, m, n_type)                                             \
    do {                                                                       \
        TT_ASSERT((m) < ((sizeof(n_type) << 3) - 1));                          \
        (n) += (n_type)((((n_type)1) << (m)) - 1);                             \
        (n) &= (n_type)(~((((n_type)1) << (m)) - 1));                          \
    } while (0)
#define TT_ALIGN_DEC(n, m, n_type)                                             \
    do {                                                                       \
        TT_ASSERT((m) < ((sizeof(n_type) << 3) - 1));                          \
        (n) &= (n_type)(~((((n_type)1) << (m)) - 1));                          \
    } while (0)

#define TT_U32_ALIGN_INC(n, m) TT_ALIGN_INC((n), (m), tt_u32_t)
#define TT_U32_ALIGN_DEC(n, m) TT_ALIGN_DEC((n), (m), tt_u32_t)

#define TT_PTR_ALIGN_INC(p, m)                                                 \
    do {                                                                       \
        tt_uintptr_t __p = (tt_uintptr_t)(p);                                  \
        TT_ALIGN_INC(__p, (m), tt_uintptr_t);                                  \
        p = (tt_ptr_t)__p;                                                     \
    } while (0)
#define TT_PTR_ALIGN_DEC(p, m)                                                 \
    do {                                                                       \
        tt_uintptr_t __p = (tt_uintptr_t)(p);                                  \
        TT_ALIGN_DEC(__p, (m), tt_uintptr_t);                                  \
        p = (tt_ptr_t)__p;                                                     \
    } while (0)

/**
@def TT_PTR_ALIGNED(p, m)
check if pointer p (1 << m) bytes aligned

@param [in] p the pointer to be checked
@param [in] m alignment order
*/
#define TT_PTR_ALIGNED(p, m)                                                   \
    do {                                                                       \
        tt_ptr_t __aligned = (p);                                              \
        TT_PTR_ALIGN_INC(__aligned, (m));                                      \
        if (__aligned != (p)) {                                                \
            TT_ERROR("ptr[%p] is not %d-byte aligned", (p), (m));              \
            TT_ASSERT_ALWAYS(0);                                               \
        }                                                                      \
    } while (0);

/**
@def TT_SIZE_IN_ORDER(size, order)
caculate how many contiguous memory blocks, each of (1 << order) bytes,
are needed to cover requried size

@param [in] size the size in bytes to be stored
@param [in] order the size order of each memory block
*/
#define TT_SIZE_IN_ORDER(size, order)                                          \
    TT_COND((size) & ((1 << order) - 1),                                       \
            ((size) >> order) + 1,                                             \
            ((size) >> order))

/**
@def TT_OFFSET(owner_type, member_name)
get delta from member to the struct beginning

@param [in] owner_type name of owner type
@param [in] member_name name of struct member

@return
delta in bytes of the member off the beginning of owner struct
*/
#define TT_OFFSET(type, member) ((tt_ptrdiff_t)(&(((type *)0)->member)))

/**
@def TT_CONTAINER(member_ptr, owner_type, member_name)
get owner pointer

@param [in] member_ptr pointer to member
@param [in] owner_type owner_type name of owner type
@param [in] member_name name of struct member

@return
the pointer to the owner
*/
#define TT_CONTAINER(p, type, member)                                          \
    ((type *)((tt_u8_t *)p - TT_OFFSET(type, member)))

/**
@def TT_COND(condition, true_expression, false_expression)
this macro return different values according to whether the condition is
true(non-zero value) or false(zero)

@param [in] condition condition expression
@param [in] true_expression value to be returned when condition is true
@param [in] false_expression value to be returned when condition is false

@return
- value of true_expression, if condition is true
- value of false_expression, if condition is false
*/
#define TT_COND(condition, true_expression, false_expression)                  \
    ((condition) != 0 ? (true_expression) : (false_expression))

/**
@def TT_MUL_WOULD_OVFL(type, v1, v2, wide_type)
check whether v1*v2 would overflow

@param [in] type type of v1 and v2
@param [in] v1 first value
@param [in] v2 second value
@param [in] wide_type type to check overflow

@return
- nonzero if multiplication would overflow
- zero otherwise
*/
#define TT_MUL_WOULD_OVFL(type, v1, v2, wide_type)                             \
    ((((wide_type)(v1)) * ((wide_type)(v2))) > ((wide_type)(~((type)(0u)))))
/**
@def TT_U32_MUL_WOULD_OVFL(v1, v2)
check whether v1*v2 would overflow, where v1 and v2 are both of tt_u32_t
*/
#define TT_U32_MUL_WOULD_OVFL(v1, v2)                                          \
    TT_MUL_WOULD_OVFL(tt_u32_t, (v1), (v2), tt_u64_t)

/**
@def TT_ADD_WOULD_OVFL(type, v1, v2, wide_type)
check whether v1+v2 would overflow

@param [in] type type of v1 and v2
@param [in] v1 first value
@param [in] v2 second value
@param [in] wide_type type to check overflow

@return
- nonzero if addition would overflow
- zero otherwise
*/
#define TT_ADD_WOULD_OVFL(type, v1, v2, wide_type)                             \
    ((((wide_type)(v1)) + ((wide_type)(v2))) > ((wide_type)(~((type)(0u)))))
/**
@def TT_U32_ADD_WOULD_OVFL(v1, v2)
check whether v1+v2 would overflow, where v1 and v2 are both of tt_u32_t
*/
#define TT_U32_ADD_WOULD_OVFL(v1, v2)                                          \
    TT_ADD_WOULD_OVFL(tt_u32_t, (v1), (v2), tt_u64_t)

/**
@def TT_SAFE_ASSIGN(p, v)
used when a pointer in code need be assigned if it's not null
*/
#define TT_SAFE_ASSIGN(p, v)                                                   \
    do {                                                                       \
        if (p != NULL) {                                                       \
            *(p) = (v);                                                        \
        }                                                                      \
    } while (0)

#define TT_SWAP(type, a, b)                                                    \
    do {                                                                       \
        type tmp = (a);                                                        \
        (a) = (b);                                                             \
        (b) = tmp;                                                             \
    } while (0)
#define TT_SWAP_U64(a, b) TT_SWAP(tt_u64_t, a, b)
#define TT_SWAP_U32(a, b) TT_SWAP(tt_u32_t, a, b)
#define TT_SWAP_U16(a, b) TT_SWAP(tt_u16_t, a, b)
#define TT_SWAP_U8(a, b) TT_SWAP(tt_u8_t, a, b)

#define TT_BOOL(condition) TT_COND(condition, TT_TRUE, TT_FALSE)

#define __TT_DO(exp, ...)                                                      \
    do {                                                                       \
        tt_result_t __result;                                                  \
        __result = __VA_ARGS__;                                                \
        if (TT_UNLIKELY(!TT_OK(__result))) {                                   \
            exp;                                                               \
        }                                                                      \
    } while (0)
#define TT_DO(...) __TT_DO(return __result, __VA_ARGS__)
#define TT_DO_G(lable, ...) __TT_DO(goto lable, __VA_ARGS__)
#define TT_DO_R(result, ...) __TT_DO(return (result), __VA_ARGS__)
#define TT_DO_V(...) __TT_DO(return, __VA_ARGS__)

#define __TT_NOT_NULL(exp, ...)                                                \
    do {                                                                       \
        if (TT_UNLIKELY((__VA_ARGS__) == NULL)) {                              \
            exp;                                                               \
        }                                                                      \
    } while (0)
#define TT_DONN(...) __TT_NOT_NULL(return TT_FAIL, __VA_ARGS__)
#define TT_DONN_G(lable, ...) __TT_NOT_NULL(goto lable, __VA_ARGS__)
#define TT_DONN_R(result, ...) __TT_NOT_NULL(return (result), __VA_ARGS__)

#define TT_LIMIT_MAX(v, max)                                                   \
    do {                                                                       \
        if ((v) > (max)) {                                                     \
            (v) = (max);                                                       \
        }                                                                      \
    } while (0)

#define TT_LIMIT_MIN(v, min)                                                   \
    do {                                                                       \
        if ((v) < (min)) {                                                     \
            (v) = (min);                                                       \
        }                                                                      \
    } while (0)

#define TT_LIMIT_RANGE(v, min, max)                                            \
    do {                                                                       \
        TT_LIMIT_MIN(v, min);                                                  \
        TT_LIMIT_MAX(v, max);                                                  \
    } while (0)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_E_ENDIAN_INVALID,

    TT_E_ENDIAN_BIG,
    TT_E_ENDIAN_LITTLE,

    TT_E_ENDIAN_NUM
} tt_endian_type_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn tt_u32_t tt_least_2power(IN tt_u32_t value)
return mininum n which satisfies "(1 << n) >= value"

@param [in] value value to be calculated

@note
return value may be larger than 31, for example it returns 32 when
value is 0x80000001
*/
tt_export tt_u32_t tt_least_2power(IN tt_u32_t value);

/**
@fn tt_result_t tt_order(IN tt_u32_t value, OUT tt_u32_t *order)
get order of the param

@param [in] value value to be calculated
@param [out] order return the order of the param, can be NULL

@return
- TT_SUCCESS if the value is aligned
- TT_FAIL otherwise, for instance, 0x101 return TT_FAIL

@note
the param "order" can be NULL, then such function can be used to check
if the value is aligned
*/
tt_export tt_result_t tt_order(IN tt_u32_t value, OUT tt_u32_t *order);

/**
@fn void tt_swap(IN void *a, IN void *b, IN tt_u32_t size)
swap two memory block

@param [in] a swap param
@param [in] b swap param
@param [in] size bytes to be swapped

@return
void
*/
tt_export void tt_swap(IN void *a, IN void *b, IN tt_u32_t size);

/**
@fn tt_result_t tt_high_bit_1(IN tt_u32_t value, OUT tt_u32_t *pos)
scan the value from high bit to low bit, return the position of the
first 1 bit

@param [in] value bitmap to be scaned
@param [out] pos return the position of highest 1 bit

@return
- TT_SUCCESS if bit 1 is found,
- TT_FAIL if value is 0

@note
the pos begins at 0, 0x80000001 would return 31
*/
tt_export tt_result_t tt_high_bit_1(IN tt_u32_t value, OUT tt_u32_t *pos);

/**
@fn tt_result_t tt_low_bit_1(IN tt_u32_t value, OUT tt_u32_t *pos)
scan the value from low bit to high bit, return the position of the
first 1 bit

@param [in] value bitmap to be scaned
@param [out] pos return the position of lowest 1 bit

@return
- TT_SUCCESS if bit 1 is found,
- TT_FAIL if value is 0

@note
the pos begins at 0, 0x80000001 would return 0
*/
tt_export tt_result_t tt_low_bit_1(IN tt_u32_t value, OUT tt_u32_t *pos);

/**
@fn tt_u32_t tt_gcd_u32(IN tt_u32_t a, IN tt_u32_t b)
calculate greatest common divisor

@param [in] a
@param [in] b

@return
greatest common divisor
*/
tt_export tt_u32_t tt_gcd_u32(IN tt_u32_t a, IN tt_u32_t b);

/**
@fn tt_u32_t tt_lcm_u32(IN tt_u32_t a, IN tt_u32_t b)
calculate least common multiple

@param [in] a
@param [in] b

@return
least common multiple

@note
result is undetermined if it's overflowed
*/
tt_export tt_u32_t tt_lcm_u32(IN tt_u32_t a, IN tt_u32_t b);

tt_inline tt_endian_type_t tt_endian()
{
    union
    {
        tt_u8_t __u8[2];
        tt_u16_t __u16;
    } t;

    t.__u16 = 0x0A0B;
    if (t.__u8[0] == 0x0B) {
        return TT_E_ENDIAN_LITTLE;
    } else {
        return TT_E_ENDIAN_BIG;
    }
}

tt_export void tt_hex_dump(IN tt_u8_t *buf,
                           IN tt_u32_t buf_len,
                           IN tt_u32_t num_per_line);

tt_export tt_u8_t tt_c2h(IN tt_char_t c, IN tt_u8_t h_if_fail);

tt_export tt_char_t tt_h2c(IN tt_u8_t h, IN tt_u8_t c_if_fail);

// copy a null terminated string
tt_export tt_char_t *tt_cstr_copy(IN const tt_char_t *cstr);

// if length of cstr exceeds @ref len, cstr is truncated, otherwise only
// whole cstr is copied
tt_export tt_char_t *tt_cstr_copy_n(IN const tt_char_t *cstr, IN tt_u32_t len);

#if 0
tt_inline tt_bool_t tt_compare_and_swap_16byte(IN __int128 *a,
                                     IN __int128 *comparand,
                                     IN __int128 *new_val)
{
    tt_bool_t  ret;
    
    // alignment check is ignored
    
    __asm__ __volatile__("lock cmpxchg16b %1;\n"
                         "sete %0;\n"
                         : "=m"(ret),
                         "+m"(*((volatile __int128*)atomic))
                         : "a"(*((tt_u64_t*)comparand)),
                         "d"(*(TT_PTR_INC(tt_u64_t, comparand, 8))),
                         "b"(*((tt_u64_t*)new_val)),
                         "c"(*(TT_PTR_INC(tt_u64_t, new_val, 8)))
                         : "memory");
    return ret;
}
#endif

#endif /* __TT_UTIL__ */
