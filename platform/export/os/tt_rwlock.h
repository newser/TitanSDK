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
@file tt_rwlock.h
@brief read write lock APIs

this file specifies interfaces of read write lock
*/

#ifndef __TT_RWLOCK__
#define __TT_RWLOCK__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_rwlock_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#if (TT_RWLOCK_DEBUG_OPT & TT_RWLOCK_LOCKER_DEBUG)

/**
 @def tt_rwlock_acquire_r(m)
 acquire a rwlock, wrapper of @ref __rwlock_acquire_r

 @param [in] m the rwlock to be acquired

 @return
 - TT_SUCCESS, if locking done
 - TT_FAIL, otherwise

 @note
 - NEVER read acquire a rwlock which has already been acquired by same thread
 */
#define tt_rwlock_acquire_r(m) __rwlock_acquire_r(m, __FUNCTION__, __LINE__)

/**
 @def tt_rwlock_try_acquire_r(m)
 try to acquire a rwlock, wrapper of @ref __rwlock_try_acquire_r

 @param [in] m the rwlock to be acquired

 @return
 - TT_SUCCESS, if locking done
 - TT_FAIL, otherwise

 @note
 - NEVER read acquire a rwlock which has already been acquired by same thread
 */
#define tt_rwlock_try_acquire_r(m)                                             \
    __rwlock_try_acquire_r(m, __FUNCTION__, __LINE__)

/**
 @def tt_rwlock_acquire_w(m)
 acquire a rwlock, wrapper of @ref tt_rwlock_acquire_tag

 @param [in] m the rwlock to be acquired

 @return
 - TT_SUCCESS, if locking done
 - TT_FAIL, otherwise

 @note
 - NEVER write acquire a rwlock which has already been acquired by same thread
 */
#define tt_rwlock_acquire_w(m) __rwlock_acquire_w(m, __FUNCTION__, __LINE__)

/**
 @def tt_rwlock_try_acquire_w(m)
 try to acquire a rwlock, wrapper of @ref __rwlock_try_acquire_w

 @param [in] m the rwlock to be acquired

 @return
 - TT_SUCCESS, if locking done
 - TT_FAIL, otherwise

 @note
 - NEVER write acquire a rwlock which has already been acquired by same thread
 */
#define tt_rwlock_try_acquire_w(m)                                             \
    __rwlock_try_acquire_w(m, __FUNCTION__, __LINE__)

#else

#define tt_rwlock_acquire_r(m) __rwlock_acquire_r(m)

#define tt_rwlock_try_acquire_r(m) __rwlock_try_acquire_r(m)

#define tt_rwlock_acquire_w(m) __rwlock_acquire_w(m)

#define tt_rwlock_try_acquire_w(m) __rwlock_try_acquire_w(m)

#endif

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

/**
@struct tt_rwlock_attr_t
rwlock attribute
*/
typedef struct tt_rwlock_attr_s
{
    tt_u32_t reserved;
} tt_rwlock_attr_t;

/**
@struct tt_rwlock_t
rwlock struct
*/
typedef struct
{
    __TT_PRIVATE__

    /**
    @var attr
    rwlock attribute
    */
    tt_rwlock_attr_t attr;

    /**
    @var sys_rwlock
    the real system rwlock
    */
    tt_rwlock_ntv_t sys_rwlock;
} tt_rwlock_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_rwlock_component_register()
register ts rwlock component
*/
extern void tt_rwlock_component_register();

/**
@fn tt_result_t tt_rwlock_create(IN tt_rwlock_t *rwlock,
                                IN tt_rwlock_attr_t *attr)
create a rwlock

@param [in] rwlock the rwlock to be initialized
@param [in] attr reserved, set to NULL to use default

@return
- TT_SUCCESS, if the rwlock is created
- TT_FAIL, otherwise
*/
extern tt_result_t tt_rwlock_create(IN tt_rwlock_t *rwlock,
                                    IN tt_rwlock_attr_t *attr);

/**
@fn void tt_rwlock_destroy(IN tt_rwlock_t *rwlock)
delete a rwlock

@param [in] rwlock the rwlock to be deleted

@return
- TT_SUCCESS, if the rwlock is destroyed
- TT_FAIL, otherwise
*/
extern void tt_rwlock_destroy(IN tt_rwlock_t *rwlock);

/**
 @fn void tt_rwlock_attr_default(IN tt_rwlock_attr_t *attr)
 get default rwlock attribute

 @param [in] attr rwlock attribute
 */
extern void tt_rwlock_attr_default(IN tt_rwlock_attr_t *attr);

/**
@fn void __rwlock_acquire_r(IN tt_rwlock_t *rwlock)
acquire a rwlock to read

@param [in] rwlock the rwlock to be acquired
*/
extern void __rwlock_acquire_r(IN tt_rwlock_t *rwlock
#if (TT_RWLOCK_DEBUG_OPT & TT_RWLOCK_LOCKER_DEBUG)
                               ,
                               IN const tt_char_t *function,
                               IN tt_u32_t line
#endif
                               );

/**
@fn void __rwlock_try_acquire_r(IN tt_rwlock_t *rwlock)
try to acquire a rwlock to read

@param [in] rwlock the rwlock to be acquired

@return
- TT_SUCCESS, if locking done
- TT_TIME_OUT, if can not lock now

@note
- NEVER read acquire a rwlock which has already been acquired by same thread
*/
extern tt_result_t __rwlock_try_acquire_r(IN tt_rwlock_t *rwlock
#if (TT_RWLOCK_DEBUG_OPT & TT_RWLOCK_LOCKER_DEBUG)
                                          ,
                                          IN const tt_char_t *function,
                                          IN tt_u32_t line
#endif
                                          );

/**
@fn void tt_rwlock_release_r(IN tt_rwlock_t *rwlock)
release a rwlock after reading

@param [in] rwlock the rwlock to be unlocked
*/
extern void tt_rwlock_release_r(IN tt_rwlock_t *rwlock);

/**
@fn void __rwlock_acquire_w(IN tt_rwlock_t *rwlock)
acquire a rwlock to write

@param [in] rwlock the rwlock to be acquired
*/
extern void __rwlock_acquire_w(IN tt_rwlock_t *rwlock
#if (TT_RWLOCK_DEBUG_OPT & TT_RWLOCK_LOCKER_DEBUG)
                               ,
                               IN const tt_char_t *function,
                               IN tt_u32_t line
#endif
                               );

/**
@fn void __rwlock_try_acquire_w(IN tt_rwlock_t *rwlock)
try to acquire a rwlock to write

@param [in] rwlock the rwlock to be acquired

@return
- TT_SUCCESS, if locking done
- TT_TIME_OUT, if can not lock now

@note
- NEVER write acquire a rwlock which has already been acquired by same thread
*/
extern tt_result_t __rwlock_try_acquire_w(IN tt_rwlock_t *rwlock
#if (TT_RWLOCK_DEBUG_OPT & TT_RWLOCK_LOCKER_DEBUG)
                                          ,
                                          IN const tt_char_t *function,
                                          IN tt_u32_t line
#endif
                                          );

/**
@fn void tt_rwlock_release_w(IN tt_rwlock_t *rwlock)
release a rwlock after writeing

@param [in] rwlock the rwlock to be unlocked
*/
extern void tt_rwlock_release_w(IN tt_rwlock_t *rwlock);

#endif /* __TT_RWLOCK__ */
