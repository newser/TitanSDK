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
@file tt_semaphore.h
@brief semaphore APIs

this file specifies interfaces of sem.
*/

#ifndef __TT_SEMAPHORE__
#define __TT_SEMAPHORE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_semaphore_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#if (TT_SEM_DEBUG_OPT & TT_SEM_DEBUG_TAG)

/**
 @def tt_sem_acquire(s, t)
 wrapper of @ref tt_sem_acquire_tag

 @param [in] s the semaphore to be acquired
 @param [in] t the wait time in millisecond
 - if TT_SEM_WAIT_INFINITE, wait for ever

 @return
 - TT_SUCCESS, if the semaphore is waited successfully
 - TT_TIME_OUT, if the semaphore can not be acquired and time specified expires

 @note
 - NENVER acquire a semaphore which has already been acquired by same thread
 */
#define tt_sem_acquire(s, t)                                                   \
    tt_sem_acquire_tag((s), (t), __FUNCTION__, __LINE__)

/**
 @def tt_sem_try_acquire(s, t)
 wrapper of @ref tt_sem_try_acquire_tag

 @param [in] s the semaphore to be waited

 @return
 - TT_SUCCESS, if the semaphore is waited successfully
 - TT_TIME_OUT, if the semaphore can not be acquired

 @note
 - NENVER acquire a semaphore which has already been acquired by same thread
 */
#define tt_sem_try_acquire(s)                                                  \
    tt_sem_try_acquire_tag((s), __FUNCTION__, __LINE__)

#else

#define tt_sem_acquire(s, t) tt_sem_acquire_tag((s), (t))

#define tt_sem_try_acquire(s) tt_sem_try_acquire_tag((s))

#endif

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

/**
@struct tt_sem_attr_t
semaphore attribute
*/
typedef struct tt_sem_attr_s
{
    tt_u32_t reserved;
} tt_sem_attr_t;

/**
@struct tt_sem_t
semaphore struct
*/
typedef struct
{
    /** system semaphore */
    tt_sem_ntv_t sys_sem;
} tt_sem_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_sem_component_register()
register ts semaphore component
*/
tt_export void tt_sem_component_register();

/**
@fn tt_result_t tt_sem_create(IN STATIC tt_sem_t *sem,
                              IN tt_sem_attr_t *attr)
create a semaphore

@param [in] sem the semaphore to be created
@param [in] attr reserved, NULL to use default attributes

@return
- TT_SUCCESS, if the semaphore is created
- TT_FAIL, otherwise
*/
tt_export tt_result_t tt_sem_create(IN tt_sem_t *sem,
                                    IN tt_u32_t count,
                                    IN OPT tt_sem_attr_t *attr);

/**
@fn void tt_sem_destroy(IN tt_sem_t *sem)
destroy a semaphore

@param [in] sem the semaphore to be destroyed

@return
- TT_SUCCESS, if the semaphore is destroyed
- TT_FAIL, otherwise
*/
tt_export void tt_sem_destroy(IN tt_sem_t *sem);

/**
 @fn void tt_sem_attr_default(IN tt_sem_attr_t *attr)
 get default semaphore attribute

 @param [in] attr semaphore attribute
 */
tt_export void tt_sem_attr_default(IN tt_sem_attr_t *attr);

/**
@fn tt_result_t tt_sem_acquire_tag(IN tt_sem_t *sem,
                                   IN tt_u32_t wait_ms)
wait a semaphore

@param [in] sem the semaphore to be waited
@param [in] wait_ms
- the time in millisec spent on waiting semaphore.
- if 0, this function is just a "try-wait", the return value is either
  TT_SUCCESS or TT_TIME_OUT.
- if TT_TIME_INFINITE, the function will never return till the semaphore
  is signaled

@return
- TT_SUCCESS, if the semaphore is waited successfully
- TT_TIME_OUT, if the semaphore is not waited and time specified expires
*/
tt_inline tt_bool_t tt_sem_acquire_tag(IN tt_sem_t *sem,
                                       IN tt_s64_t wait_ms
#if (TT_SEM_DEBUG_OPT & TT_SEM_DEBUG_TAG)
                                       ,
                                       IN const tt_char_t *function,
                                       IN tt_u32_t line
#endif
                                       )
{
    return tt_sem_acquire_ntv(&sem->sys_sem, wait_ms);
}

/**
@fn tt_result_t tt_sem_try_acquire_tag(IN tt_sem_t *sem)
wait a semaphore

@param [in] sem the semaphore to be waited

@return
- TT_SUCCESS, if the semaphore is waited successfully
- TT_TIME_OUT, if the semaphore can not be acquired
*/
tt_inline tt_bool_t tt_sem_try_acquire_tag(IN tt_sem_t *sem
#if (TT_SEM_DEBUG_OPT & TT_SEM_DEBUG_TAG)
                                           ,
                                           IN const tt_char_t *function,
                                           IN tt_u32_t line
#endif
                                           )
{
    return tt_sem_try_acquire_ntv(&sem->sys_sem);
}

/**
@fn void tt_sem_release(IN tt_sem_t *sem)
post a semaphore

@param [in] sem the semaphore to be posted
*/
tt_inline void tt_sem_release(IN tt_sem_t *sem)
{
    tt_sem_release_ntv(&sem->sys_sem);
}

#endif /* __TT_SEMAPHORE__ */
