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
@file tt_atomic.h
@brief atomic APIs

this file provides atomic APIs
*/

#ifndef __TT_ATOMIC__
#define __TT_ATOMIC__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_atomic_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// ========================================
// s32
// ========================================

#define tt_atomic_s32_add(a, val)                                              \
    do {                                                                       \
        tt_s32_t org_val = tt_atomic_s32_get((a));                             \
        while (!TT_OK(                                                         \
            tt_atomic_s32_cas((a), org_val, org_val + (tt_s32_t)(val)))) {     \
            org_val = tt_atomic_s32_get((a));                                  \
        }                                                                      \
    } while (0)
#define tt_atomic_s32_sub(a, val)                                              \
    do {                                                                       \
        tt_s32_t org_val = tt_atomic_s32_get((a));                             \
        while (!TT_OK(                                                         \
            tt_atomic_s32_cas((a), org_val, org_val - (tt_s32_t)(val)))) {     \
            org_val = tt_atomic_s32_get((a));                                  \
        }                                                                      \
    } while (0)

// ========================================
// s64
// ========================================

#define tt_atomic_s64_add(a, val)                                              \
    do {                                                                       \
        tt_s64_t org_val = tt_atomic_s64_get((a));                             \
        while (!TT_OK(                                                         \
            tt_atomic_s64_cas((a), org_val, org_val + (tt_s64_t)(val)))) {     \
            org_val = tt_atomic_s64_get((a));                                  \
        }                                                                      \
    } while (0)
#define tt_atomic_s64_sub(a, val)                                              \
    do {                                                                       \
        tt_s64_t org_val = tt_atomic_s64_get((a));                             \
        while (!TT_OK(                                                         \
            tt_atomic_s64_cas((a), org_val, org_val - (tt_s64_t)(val)))) {     \
            org_val = tt_atomic_s64_get((a));                                  \
        }                                                                      \
    } while (0)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

/**
@typedef tt_atomic_s32_t
atomic type, 4 bytes long
*/
typedef tt_atomic_s32_ntv_t tt_atomic_s32_t;

/**
@typedef tt_atomic_s64_t
atomic type, 8 bytes long
*/
typedef tt_atomic_s64_ntv_t tt_atomic_s64_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_atomic_component_register()
register ts atomic component
*/
extern void tt_atomic_component_register();

// ========================================
// s32
// ========================================

/**
@fn void tt_atomic_s32_init(IN tt_atomic_s32_t *a, IN tt_s32_t val)
initialize a ts atomic variant

@param [in] atomic atomic variant
@param [in] val initial value of atomic variant
*/
tt_inline void tt_atomic_s32_init(IN tt_atomic_s32_t *a, IN tt_s32_t val)
{
    tt_atomic_s32_init_ntv(a, val);
}

/**
@fn tt_s32_t tt_atomic_s32_get(IN tt_atomic_s32_t *a)
read a ts atomic variant

@param [in] atomic atomic variant to be readed

@return
the value of atomic variant
*/
tt_inline tt_s32_t tt_atomic_s32_get(IN tt_atomic_s32_t *a)
{
    return tt_atomic_s32_get_ntv(a);
}

/**
@fn tt_s32_t tt_atomic_s32_set(IN tt_atomic_s32_t *a, IN tt_s32_t val)
write a ts atomic variant and return the original value

@param [in] atomic atomic variant to be written
@param [in] val value to be written to atomic variant

@return
the original value
*/
tt_inline tt_s32_t tt_atomic_s32_set(IN tt_atomic_s32_t *a, IN tt_s32_t val)
{
    return tt_atomic_s32_set_ntv(a, val);
}

/**
@fn tt_s32_t tt_atomic_s32_inc(IN tt_atomic_s32_t *a)
increase a value by 1 atomically

@param [inout] atomic pointer to a atomic variant to be increased

@return
increased value
*/
tt_inline tt_s32_t tt_atomic_s32_inc(IN tt_atomic_s32_t *a)
{
    return tt_atomic_s32_inc_ntv(a);
}

/**
@fn tt_s32_t tt_atomic_s32_dec(IN tt_atomic_s32_t *a)
decrease a value by 1 atomically

@param [inout] atomic pointer to a atomic variant to be decreased

@return
decreased value
*/
tt_inline tt_s32_t tt_atomic_s32_dec(IN tt_atomic_s32_t *a)
{
    return tt_atomic_s32_dec_ntv(a);
}

/**
@fn tt_result_t tt_atomic_s32_cas(IN tt_atomic_s32_t *a,
                                  IN tt_s32_t comparand,
                                  IN tt_s32_t new_val)
compare and set. compare atomic with comparand and exchange with new_val
if atomic equals comparand

@param [inout] atomic pointer to a atomic variant to be exchanged
@param [in] comparand value to be compared with atomic
@param [in] new_val value to be exchanged with atomic

@return
- TT_SUCCESS, if exchanging done
- TT_FAIL, otherwise

@note
it's normal case that this function return TT_FAIL
*/
tt_inline tt_result_t tt_atomic_s32_cas(IN tt_atomic_s32_t *a,
                                        IN tt_s32_t comparand,
                                        IN tt_s32_t new_val)
{
    return tt_atomic_s32_cas_ntv(a, comparand, new_val);
}

// ========================================
// s64
// ========================================

/**
@fn void tt_atomic_s64_init(IN tt_atomic_s64_t *a, IN tt_s64_t val)
initialize a 64bit atomic variant

@param [in] atomic atomic variant
@param [in] val initial value of atomic variant
*/
tt_inline void tt_atomic_s64_init(IN tt_atomic_s64_t *a, IN tt_s64_t val)
{
    tt_atomic_s64_init_ntv(a, val);
}

/**
@fn tt_s64_t tt_atomic_s64_get(IN tt_atomic_s64_t *a)
read a 64bit atomic variant

@param [in] atomic atomic variant to be readed

@return
the value of atomic variant
*/
tt_inline tt_s64_t tt_atomic_s64_get(IN tt_atomic_s64_t *a)
{
    return tt_atomic_s64_get_ntv(a);
}

/**
@fn tt_s64_t tt_atomic_s64_set(IN tt_atomic_s64_t *a, IN tt_s64_t val)
write a 64bit atomic variant and return orginal value

@param [in] atomic atomic variant to be written
@param [in] val value to be written to atomic variant

@return
orginal value
*/
tt_inline tt_s64_t tt_atomic_s64_set(IN tt_atomic_s64_t *a, IN tt_s64_t val)
{
    return tt_atomic_s64_set_ntv(a, val);
}

/**
@fn tt_s64_t tt_atomic_s64_inc(IN tt_atomic_s64_t *a)
increase a value by 1 atomically

@param [inout] atomic pointer to a atomic variant to be increased

@return
the value of atomic variant increased
*/
tt_inline tt_s64_t tt_atomic_s64_inc(IN tt_atomic_s64_t *a)
{
    return tt_atomic_s64_inc_ntv(a);
}

/**
@fn tt_s64_t tt_atomic_s64_dec(IN tt_atomic_s64_t *a)
decrease a value by 1 atomically

@param [inout] atomic pointer to a atomic variant to be decreased

@return
the value of atomic variant decreased
*/
tt_inline tt_s64_t tt_atomic_s64_dec(IN tt_atomic_s64_t *a)
{
    return tt_atomic_s64_dec_ntv(a);
}

/**
@fn tt_result_t tt_atomic_s64_cas(IN tt_atomic_s64_t *a,
                                  IN tt_s64_t comparand,
                                  IN tt_s64_t new_val)
compare and set. compare 64bit atomic with comparand and exchange with
new_val if atomic equals comparand

@param [inout] atomic pointer to a atomic variant to be exchanged
@param [in] comparand value to be compared with atomic
@param [in] new_val value to be exchanged with atomic

@return
- TT_SUCCESS, if exchanging done
- TT_FAIL, otherwise

@note
it's normal case that this function return TT_FAIL
*/
tt_inline tt_result_t tt_atomic_s64_cas(IN tt_atomic_s64_t *a,
                                        IN tt_s64_t comparand,
                                        IN tt_s64_t new_val)
{
    return tt_atomic_s64_cas_ntv(a, comparand, new_val);
}

// ========================================
// ptr
// ========================================

/**
@fn tt_ptr_t tt_atomic_ptr_get(IN tt_ptr_t *p)
read pointer atomically

@param [in] atomic atomic pointer variant

@return
pointer value
*/
tt_inline tt_ptr_t tt_atomic_ptr_get(IN tt_ptr_t *p)
{
    return tt_atomic_ptr_get_ntv(p);
}

/**
@fn tt_ptr_t tt_atomic_ptr_set(IN OUT tt_ptr_t *p, IN tt_ptr_t ptr)
exchange pointer with new pointer atomically and return original pointer

@param [in] atomic pointer variant to be exchanged
@param [in] ptr new pointer to be exchanged to atomic pointer

@return
orginal pointer
*/
tt_inline tt_ptr_t tt_atomic_ptr_set(IN OUT tt_ptr_t *p, IN tt_ptr_t val)
{
    return tt_atomic_ptr_set_ntv(p, val);
}

/**
@fn tt_result_t tt_atomic_ptr_cas(IN OUT tt_ptr_t *p,
                                  IN tt_ptr_t comparand,
                                  IN tt_ptr_t new_ptr)
compare and set. compare atomic pointer with comparand and exchange with
new_ptr if atomic equals comparand

@param [inout] atomic pointer
@param [in] comparand value to be compared with atomic
@param [in] new_ptr value to be exchanged with atomic

@return
- TT_SUCCESS, if exchanging done
- TT_FAIL, otherwise
*/
tt_inline tt_result_t tt_atomic_ptr_cas(IN OUT tt_ptr_t *p,
                                        IN tt_ptr_t comparand,
                                        IN tt_ptr_t new_val)
{
    return tt_atomic_ptr_cas_ntv(p, comparand, new_val);
}

#endif /* __TT_ATOMIC__ */
