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
@file tt_atomic_native.h
@brief system atomic APIs

this file implements atomic APIs at system level.

<hr>

<b>PORTING NOTES</b><br>
- a special requirement is that each atomic API with return value such
  like read, exchange, etc. but not write, should guarantee a proper
  memory barrier

*/

#ifndef __TT_ATOMIC_NATIVE__
#define __TT_ATOMIC_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;

// how/need to guarantee alignment of atomic vars?

/**
@typedef tt_atomic_s32_ntv_t
atomic type of signed 32 bit
*/
typedef tt_s32_t tt_atomic_s32_ntv_t __attribute__((aligned(4)));

/**
@typedef tt_atomic_s64_ntv_t
atomic type of signed 64 bit
*/
typedef tt_s64_t tt_atomic_s64_ntv_t __attribute__((aligned(8)));

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn tt_result_t tt_atomic_component_init_ntv()
initialize atomic portlayer

@return
- TT_SUCCESS if initialization succeeds
- TT_FAIL otherwise
*/
extern tt_result_t tt_atomic_component_init_ntv(
    IN struct tt_profile_s *profile);

tt_inline void tt_atomic_component_exit_ntv()
{
}

/**
@fn tt_s32_t tt_atomic_s32_get_ntv(IN tt_atomic_s32_ntv_t *a)
read value of an atomic variant

@param [in] atomic the atomic variant

@return
the value read
*/
tt_inline tt_s32_t tt_atomic_s32_get_ntv(IN tt_atomic_s32_ntv_t *a)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
    TT_PTR_ALIGNED(a, 2);
#endif

    return __atomic_load_n(a, __ATOMIC_SEQ_CST);
}

/**
@fn tt_s32_t tt_atomic_s32_set_ntv( \
                                   IN OUT tt_atomic_s32_ntv_t *a,
                                   IN tt_s32_t val)
write value to an atomic variant and reutrn original value

@param [in] atomic the atomic variant
@param [in] val the new value to be written to atomic

@return
the original value
*/
tt_inline void tt_atomic_s32_set_ntv(IN OUT tt_atomic_s32_ntv_t *a,
                                     IN tt_s32_t val)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
    TT_PTR_ALIGNED(a, 2);
#endif

    __atomic_store_n(a, val, __ATOMIC_SEQ_CST);
}

tt_inline tt_s32_t tt_atomic_s32_swap_ntv(IN OUT tt_atomic_s32_ntv_t *a,
                                          IN tt_s32_t val)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
    TT_PTR_ALIGNED(a, 2);
#endif

    return __atomic_exchange_n(a, val, __ATOMIC_SEQ_CST);
}

tt_inline tt_s32_t tt_atomic_s32_add_ntv(IN tt_atomic_s32_ntv_t *a,
                                         IN tt_s32_t val)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
    TT_PTR_ALIGNED(a, 2);
#endif

    return __atomic_fetch_add(a, val, __ATOMIC_SEQ_CST);
}

tt_inline tt_s32_t tt_atomic_s32_sub_ntv(IN tt_atomic_s32_ntv_t *a,
                                         IN tt_s32_t val)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
    TT_PTR_ALIGNED(a, 2);
#endif

    return __atomic_fetch_sub(a, val, __ATOMIC_SEQ_CST);
}

/**
@fn tt_s32_t tt_atomic_s32_inc_ntv(IN OUT tt_atomic_s32_ntv_t *a)
increase the value pointed by 1 atomically

@param [in] atomic the atomic variant to be increased

@return
new value after increasing
*/
tt_inline tt_s32_t tt_atomic_s32_inc_ntv(IN OUT tt_atomic_s32_ntv_t *a)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
    TT_PTR_ALIGNED(a, 2);
#endif

    return __atomic_add_fetch(a, 1, __ATOMIC_SEQ_CST);
}

/**
@fn tt_s32_t tt_atomic_s32_dec_ntv(IN OUT tt_atomic_s32_ntv_t *a)
decrease the value pointed by 1 atomically

@param [in] atomic the atomic variant to be decreased

@return
new value after decreasing
*/
tt_inline tt_s32_t tt_atomic_s32_dec_ntv(IN OUT tt_atomic_s32_ntv_t *a)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
    TT_PTR_ALIGNED(a, 2);
#endif

    return __atomic_sub_fetch(a, 1, __ATOMIC_SEQ_CST);
}

/**
@fn tt_result_t tt_atomic_s32_cas_ntv( \
                                    IN OUT tt_atomic_s32_ntv_t *a,
                                    IN tt_s32_t comparand,
                                    IN tt_s32_t val)
compare atomic with comparand, if equal, assign val to p

@param [inout] atomic pointer to the variant
@param [in] comparand value to be compared
@param [inout] val value to be exchanged with p, if p equals comparand

@return
- TT_SUCCESS, if exchanging done
- TT_FAIL, otherwise
*/
tt_inline tt_bool_t tt_atomic_s32_cas_ntv(IN OUT tt_atomic_s32_ntv_t *a,
                                          IN tt_s32_t comparand,
                                          IN tt_s32_t val)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
    TT_PTR_ALIGNED(a, 2);
#endif

    return TT_BOOL(__atomic_compare_exchange_n(a,
                                               &comparand,
                                               val,
                                               0,
                                               __ATOMIC_SEQ_CST,
                                               __ATOMIC_SEQ_CST));
}

/**
@fn tt_s64_t tt_atomic_s64_get_ntv(IN tt_atomic_s64_ntv_t *a)
read value of an 64 bit atomic variant

@param [in] atomic the atomic variant

@return
the value read
*/
tt_inline tt_s64_t tt_atomic_s64_get_ntv(IN tt_atomic_s64_ntv_t *a)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
    TT_PTR_ALIGNED(a, 3);
#endif

    return __atomic_load_n(a, __ATOMIC_SEQ_CST);
}

/**
@fn tt_s64_t tt_atomic_s64_set_ntv(IN OUT tt_atomic_s64_ntv_t *a,
                                     IN tt_s64_t val)
write value to an atomic variant and return the original value

@param [in] atomic the atomic variant
@param [in] val the new value to be written to atomic

@return
the original value
*/
tt_inline void tt_atomic_s64_set_ntv(IN OUT tt_atomic_s64_ntv_t *a,
                                     IN tt_s64_t val)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
    TT_PTR_ALIGNED(a, 3);
#endif

    __atomic_store_n(a, val, __ATOMIC_SEQ_CST);
}

tt_inline tt_s64_t tt_atomic_s64_swap_ntv(IN OUT tt_atomic_s64_ntv_t *a,
                                          IN tt_s64_t val)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
    TT_PTR_ALIGNED(a, 2);
#endif

    return __atomic_exchange_n(a, val, __ATOMIC_SEQ_CST);
}

tt_inline tt_s64_t tt_atomic_s64_add_ntv(IN tt_atomic_s64_ntv_t *a,
                                         IN tt_s64_t val)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
    TT_PTR_ALIGNED(a, 3);
#endif

    return __atomic_fetch_add(a, val, __ATOMIC_SEQ_CST);
}

tt_inline tt_s64_t tt_atomic_s64_sub_ntv(IN tt_atomic_s64_ntv_t *a,
                                         IN tt_s64_t val)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
    TT_PTR_ALIGNED(a, 3);
#endif

    return __atomic_fetch_sub(a, val, __ATOMIC_SEQ_CST);
}

/**
@fn tt_s64_t tt_atomic_s64_inc_ntv(IN OUT tt_atomic_s64_ntv_t *a)
increase the parameter by 1 and return the increased value atomically

@param [in] atomic pointer to the variant to be increased
@param [out] out increased value

@return
increased value
*/
tt_inline tt_s64_t tt_atomic_s64_inc_ntv(IN OUT tt_atomic_s64_ntv_t *a)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
    TT_PTR_ALIGNED(a, 3);
#endif

    return __atomic_add_fetch(a, 1, __ATOMIC_SEQ_CST);
}

/**
@fn tt_s64_t tt_atomic_s64_dec_ntv(IN tt_atomic_s64_ntv_t *a)
decrease the parameter by 1 and return the decreased value atomically

@param [in] atomic pointer to the variant to be decreased

@return
decreased value
*/
tt_inline tt_s64_t tt_atomic_s64_dec_ntv(IN OUT tt_atomic_s64_ntv_t *a)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
    TT_PTR_ALIGNED(a, 3);
#endif

    return __atomic_sub_fetch(a, 1, __ATOMIC_SEQ_CST);
}

/**
@fn tt_result_t tt_atomic_s64_cas_ntv( \
                               IN OUT tt_atomic_s64_ntv_t *a,
                               IN tt_s64_t comparand,
                               IN tt_s64_t val)
compare p with comparand, if equal, exchange p with val

@param [inout] atomic pointer to the variant
@param [in] comparand value to be compared
@param [inout] val value to be exchanged with p, if p equals comparand

@return
- TT_SUCCESS, if p equals comparand and has been exchanged with val
- TT_FAIL, otherwise
*/
tt_inline tt_bool_t tt_atomic_s64_cas_ntv(IN OUT tt_atomic_s64_ntv_t *a,
                                          IN tt_s64_t comparand,
                                          IN tt_s64_t val)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
    TT_PTR_ALIGNED(a, 3);
#endif

    return TT_BOOL(__atomic_compare_exchange_n(a,
                                               &comparand,
                                               val,
                                               0,
                                               __ATOMIC_SEQ_CST,
                                               __ATOMIC_SEQ_CST));
}

/**
@fn tt_ptr_t tt_atomic_ptr_get_ntv(IN tt_ptr_t *a)
read value of an atomic pointer

@param [inout] atomic the atomic pointer

@return
value of an atomic pointer
*/
tt_inline tt_ptr_t tt_atomic_ptr_get_ntv(IN tt_ptr_t *a)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
#if TT_ENV_IS_64BIT
    TT_PTR_ALIGNED(a, 3);
#else
    TT_PTR_ALIGNED(a, 2);
#endif
#endif

    return __atomic_load_n(a, __ATOMIC_SEQ_CST);
}

/**
@fn tt_s32_t tt_atomic_ptr_set_ntv(IN OUT tt_ptr_t *a,
                                           IN tt_ptr_t val)
exchange atomic pointer with new value

@param [inout] atomic the atomic pointer
@param [in] val the new value to be written to atomic

@return
the original value
*/
tt_inline void tt_atomic_ptr_set_ntv(IN OUT tt_ptr_t *a, IN tt_ptr_t val)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
#if TT_ENV_IS_64BIT
    TT_PTR_ALIGNED(a, 3);
#else
    TT_PTR_ALIGNED(a, 2);
#endif
#endif

    __atomic_store_n(a, val, __ATOMIC_SEQ_CST);
}

tt_inline tt_ptr_t tt_atomic_ptr_swap_ntv(IN OUT tt_ptr_t *a, IN tt_ptr_t val)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
#if TT_ENV_IS_64BIT
    TT_PTR_ALIGNED(a, 3);
#else
    TT_PTR_ALIGNED(a, 2);
#endif
#endif

    return __atomic_exchange_n(a, val, __ATOMIC_SEQ_CST);
}

/**
@fn tt_result_t tt_atomic_ptr_cas_ntv(IN OUT tt_ptr_t *a,
                                            IN tt_ptr_t comparand,
                                            IN tt_ptr_t val)
compare p with comparand, if equal, exchange p with val

@param [inout] atomic pointer
@param [in] comparand value to be compared
@param [inout] val value to be exchanged with p, if p equals comparand

@return
- TT_SUCCESS, if p equals comparand and has been exchanged with val
- TT_FAIL, otherwise
*/
tt_inline tt_bool_t tt_atomic_ptr_cas_ntv(IN OUT tt_ptr_t *a,
                                          IN tt_ptr_t comparand,
                                          IN tt_ptr_t val)
{
#ifdef TT_ATOMIC_ALIGNMENT_CHECK
#if TT_ENV_IS_64BIT
    TT_PTR_ALIGNED(a, 3);
#else
    TT_PTR_ALIGNED(a, 2);
#endif
#endif

    return TT_BOOL(__atomic_compare_exchange_n(a,
                                               &comparand,
                                               val,
                                               0,
                                               __ATOMIC_SEQ_CST,
                                               __ATOMIC_SEQ_CST));
}

#endif /* __TT_ATOMIC_NATIVE__ */
