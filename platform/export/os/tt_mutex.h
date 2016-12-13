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
@file tt_mutex.h
@brief mutex APIs

this file specifies interfaces of mutex
*/

#ifndef __TT_MUTEX__
#define __TT_MUTEX__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_mutex_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#if (TT_MUTEX_DEBUG_OPT & TT_MUTEX_LOCKER_DEBUG)

/**
 @def tt_mutex_acquire(m)
 acquire a mutex, wrapper of @ref __mutex_acquire

 @param [in] m the mutex to be acquired
 */
#define tt_mutex_acquire(m) __mutex_acquire(m, __FUNCTION__, __LINE__)

/**
 @def __mutex_try_acquire(m)
 try to acquire a mutex, wrapper of @ref __mutex_try_acquire

 @param [in] m the mutex to be acquired

 @return
 - TT_SUCCESS, if locking done
 - TT_TIMEOUT, if mutex is held by another thread
 */
#define tt_mutex_try_acquire(m) __mutex_try_acquire(m, __FUNCTION__, __LINE__)

#else

#define tt_mutex_acquire(m) __mutex_acquire(m)

#define tt_mutex_try_acquire(m) __mutex_try_acquire(m)

#endif

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

/**
@struct tt_mutex_attr_t
mutex attribute
*/
typedef struct tt_mutex_attr_s
{
    tt_bool_t config_recursive : 1;

    tt_bool_t recursive : 1;
} tt_mutex_attr_t;

/**
@struct tt_mutex_t
mutex struct
*/
typedef struct tt_mutex_s
{
    __TT_PRIVATE__

    /**
    @var attr
    mutex attribute
    */
    tt_mutex_attr_t attr;

    /**
    @var sys_mutex
    the real system mutex
    */
    tt_mutex_ntv_t sys_mutex;
} tt_mutex_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_mutex_component_register()
register ts mutex component
*/
extern void tt_mutex_component_register();

/**
@fn tt_result_t tt_mutex_create(IN tt_mutex_t *mutex,
                                IN tt_mutex_attr_t *attr)
create a mutex

@param [in] mutex the mutex to be initialized
@param [in] attr reserved, set to NULL to use default

@return
- TT_SUCCESS, if the mutex is created
- TT_FAIL, otherwise
*/
extern tt_result_t tt_mutex_create(IN tt_mutex_t *mutex,
                                   IN tt_mutex_attr_t *attr);

/**
@fn void tt_mutex_destroy(IN tt_mutex_t *mutex)
delete a mutex

@param [in] mutex the mutex to be deleted
*/
extern void tt_mutex_destroy(IN tt_mutex_t *mutex);

/**
 @fn void tt_mutex_attr_default(IN tt_mutex_attr_t *attr)
 get default mutex attribute

 @param [in] attr mutex attribute
 */
void tt_mutex_attr_default(IN tt_mutex_attr_t *attr);

/**
@fn void __mutex_acquire(IN tt_mutex_t *mutex)
acquire a mutex

@param [in] mutex the mutex to be acquired
*/
extern void __mutex_acquire(IN tt_mutex_t *mutex
#if (TT_MUTEX_DEBUG_OPT & TT_MUTEX_LOCKER_DEBUG)
                            ,
                            IN const tt_char_t *function,
                            IN tt_u32_t line
#endif
                            );

/**
@fn void __mutex_try_acquire(IN tt_mutex_t *mutex)
acquire a mutex

@param [in] mutex the mutex to be acquired

@return
- TT_SUCCESS, if locking done
- TT_TIME_OUT, if can not lock now
*/
extern tt_result_t __mutex_try_acquire(IN tt_mutex_t *mutex
#if (TT_MUTEX_DEBUG_OPT & TT_MUTEX_LOCKER_DEBUG)
                                       ,
                                       IN const tt_char_t *function,
                                       IN tt_u32_t line
#endif
                                       );

/**
@fn void tt_mutex_release(IN tt_mutex_t *mutex)
release a mutex

@param [in] mutex the mutex to be unlocked
*/
extern void tt_mutex_release(IN tt_mutex_t *mutex);

#endif /* __TT_MUTEX__ */
