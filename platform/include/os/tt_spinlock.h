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
@file tt_spinlock.h
@brief spin lock API

this files defines spin lock APIs
*/

#ifndef __TT_SPINLOCK__
#define __TT_SPINLOCK__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_spinlock_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#if (TT_SPINLOCK_DEBUG_OPT & TT_SPINLOCK_DEBUG_TAG)

#define tt_spinlock_acquire(s)                                                 \
    tt_spinlock_acquire_tag((s), __FUNCTION__, __LINE__)

#define tt_spinlock_try_acquire(s)                                             \
    tt_spinlock_try_acquire_tag((s), __FUNCTION__, __LINE__)

#else

#define tt_spinlock_acquire(s) tt_spinlock_acquire_tag((s))

#define tt_spinlock_try_acquire(s) tt_spinlock_try_acquire_tag((s))

#endif

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

/**
@typedef tt_spinlock_attr_t
attribute of spin lock
*/
typedef struct tt_spinlock_attr_s
{
    /** reserved */
    tt_u32_t reserved;
} tt_spinlock_attr_t;

/**
@typedef tt_spinlock_t
type of spin lock
*/
typedef struct tt_spinlock_s
{
    tt_spinlock_ntv_t sys_lock;
} tt_spinlock_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_spinlock_component_register();

/**
@fn tt_result_t tt_spinlock_create(IN tt_spinlock_t *slock,
                                   IN tt_spinlock_attr_t *attr)
create a spin lock

@param [in] slock pointer to a spin lock to be created
@param [in] attr spin lock attributes

@return
- TT_SUCCESS, if creating done
- TT_FAIL, some error occurs
*/
tt_export tt_result_t tt_spinlock_create(IN tt_spinlock_t *slock,
                                         IN OPT tt_spinlock_attr_t *attr);

/**
@fn void tt_spinlock_destroy(IN tt_spinlock_t *slock)
destroy a spin lock

@param [in] slock pointer to a spin lock to be destroyed

@return
- TT_SUCCESS, if destroying done
- TT_FAIL, some error occurs
*/
tt_export void tt_spinlock_destroy(IN tt_spinlock_t *slock);

/**
 @fn void tt_spinlock_attr_default(IN tt_spinlock_attr_t *attr)
 get default spinlock attribute

 @param [in] attr spinlock attribute
 */
tt_export void tt_spinlock_attr_default(IN tt_spinlock_attr_t *attr);

/**
@fn tt_result_t tt_spinlock_acquire(IN tt_spinlock_t *slock)
lock a spin lock

@param [in] slock pointer to a spin lock to be locked

@note
- NEVER put a thread blocked when it's holding some spinlock for keeping
  compatibility with all platforms
- NEVER acquire a spinlock which has already been acquired by same thread
*/
tt_inline void tt_spinlock_acquire_tag(IN tt_spinlock_t *slock
#if (TT_SPINLOCK_DEBUG_OPT & TT_SPINLOCK_DEBUG_TAG)
                                       ,
                                       IN const tt_char_t *function,
                                       IN tt_u32_t line
#endif
                                       )
{
    tt_spinlock_acquire_ntv(&slock->sys_lock);
}

/**
@fn tt_result_t tt_spinlock_try_acquire_tag(IN tt_spinlock_t *slock)
try to lock a spin lock

@param [in] slock pointer to a spin lock to be locked

@return
- TT_SUCCESS, if locking done
- TT_TIME_OUT, the spin lock is already locked by others

@note
- NEVER acquire a spinlock which has already been acquired by same thread
*/
tt_inline tt_bool_t tt_spinlock_try_acquire_tag(IN tt_spinlock_t *slock
#if (TT_SPINLOCK_DEBUG_OPT & TT_SPINLOCK_DEBUG_TAG)
                                                ,
                                                IN const tt_char_t *function,
                                                IN tt_u32_t line
#endif
                                                )
{
    return tt_spinlock_try_acquire_ntv(&slock->sys_lock);
}

/**
@fn void tt_spinlock_release(IN tt_spinlock_t *slock)
unlock a spin lock

@param [in] slock pointer to a spin lock to be unlocked
*/
tt_inline void tt_spinlock_release(IN tt_spinlock_t *slock)
{
    tt_spinlock_release_ntv(&slock->sys_lock);
}

#endif /* __TT_SPINLOCK__ */
