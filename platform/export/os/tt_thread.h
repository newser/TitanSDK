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
@file tt_thread.h
@brief thread APIs

this thread defines APIs wrapped system thread functions
*/

#ifndef __TT_THREAD__
#define __TT_THREAD__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>
#include <algorithm/tt_rng.h>

#include <tt_cstd_api.h>
#include <tt_thread_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_thread_s;
struct tt_evpoller_s;

/**
 * @typedef tt_result_t (*tt_thread_routine_t)(IN void *param,
 *                                             IN struct tt_thread_s *thread)
 * thread routine
 *
 * @param param [in] thread routine
 * @param thread [in] the thread descriptor
 *
 * @return
 * undefined
 */
typedef tt_result_t (*tt_thread_routine_t)(IN struct tt_thread_s *thread,
                                           IN void *param);

/**
 * @struct tt_thread_attribute_t
 * attribute used when creating a new thread
 */
typedef struct tt_thread_attr_s
{
    tt_bool_t detached : 1;
    tt_bool_t local_run : 1;
} tt_thread_attr_t;

/**
@struct tt_thread_t
thread struct
*/
typedef struct tt_thread_s
{
    __TT_PRIVATE__

    /**
    @var name
    thread name
    */
    tt_char_t name[TT_MAX_THREAD_NAME_LEN + 1];

    /**
    @var routine
    routine to run in thread
    */
    tt_thread_routine_t routine;
    /**
    @var param
    thread routing param
    */
    void *param;

    /**
    @var attribute
    extra thread attribute
    */
    tt_thread_attr_t attr;

    /**
    @var thread_lst_node
    list node to link all thread together
    */
    tt_lnode_t thread_lst_node;

    /**
    @var last_error
    recent error
    */
    tt_result_t last_error;

    /**
    @var evp
    event poller. NULL this thread is not an event poller
    */
    struct tt_evpoller_s *evp;

    /**
    @var result
    thread's random number generator
    */
    tt_rng_t *rng;

    /**
     @var sys_thread
     system thread handle
     */
    tt_thread_ntv_t sys_thread;
} tt_thread_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_thread_component_register()
register tt thread system
*/
extern void tt_thread_component_register();

/**
@fn tt_result_t tt_thread_create(IN tt_thread_t *thread,
                                 IN const tt_char_t *name,
                                 IN tt_thread_routine_t routine,
                                 IN void *routine_param,
                                 IN tt_thread_attr_t *attr)
create a thread

@param [in] thread the thread to be created
@param [in] name thread name, if NULL, the name is specified automatically
@param [in] routine thread routine
@param [in] routine_param param of thread routine
@param [in] attribute thread attribute, if NULL, use default settings

@return
- the created thread structure if it's not a detached thread
- 1 if it's a detached thread
- NULL if failed

@note
- due to callbacks, this function is not expected to be called frequently,
  correct usage should be creating necessaary thread during start up, keep
  it functioning and destroy thread when system/module stops
- if create thread is not terminated immediately after created, this function
  guarantee the structure @ref thread is consistent
*/
extern tt_thread_t *tt_thread_create(IN OPT const tt_char_t *name,
                                     IN tt_thread_routine_t routine,
                                     IN void *routine_param,
                                     IN OPT tt_thread_attr_t *attr);

extern tt_thread_t *tt_thread_create_local(IN const tt_char_t *name,
                                           IN OPT tt_thread_attr_t *attr);

/**
@fn void tt_thread_attr_default(IN tt_thread_attr_t * attr)
load default thread attributes

@param [in] attr thread attribute
*/
extern void tt_thread_attr_default(IN tt_thread_attr_t *attr);

/**
@fn tt_result_t tt_thread_wait(IN tt_thread_t *thread)
wait for a specific thread ending

@param [in] thread the thread to be waited

@return
- TT_SUCCESS the system thread is waited and destroyed
- TT_FAIL otherwise

@note
- when "thread" is ever created as detached, this function would lead
  to undetermined behavior
- when this function return, the thread specified by param has ended and
  destroyed
*/
extern tt_result_t tt_thread_wait(IN tt_thread_t *thread);

/**
@fn void tt_thread_exit()
exit current thread

@note
- this function has no parameter, so it can only be used to exit current
  thread
*/
extern void tt_thread_exit();

/**
@fn tt_thread_t *tt_current_thread()
get current thread strucct

@return
the current thread struct

@note
- it returns NULL if caller is not a tt thread
- this function may be of low performance on some platform, so do not use
  it if not mandatory
*/
extern tt_thread_t *tt_current_thread();

/**
@fn void tt_sleep(IN tt_u32_t millisec)
put calling thread into sleeping

@param [in] millisec how long it sleep in milliseconds
- if TT_THREAD_SLEEP_INFINITE, sleep for ever
*/
extern void tt_sleep(IN tt_u32_t millisec);

// can only be used in tt thread
tt_inline tt_u64_t tt_rand_u64()
{
    return tt_rng_u64(tt_current_thread()->rng);
}
#define tt_rand_u32() ((tt_u32_t)tt_rand_u64())

#endif /* __TT_THREAD__ */
