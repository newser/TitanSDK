/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
  contributor license agreements.  See the NOTICE file distributed with
  this work for additional information regarding copyright ownership.
  The ASF licenses this file to You under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with
  the License.  You may obtain a copy of the License at
 *
      http://www.apache.org/licenses/LICENSE-2.0
 *
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
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

#include <algorithm/tt_rng.h>

#include <tt_thread_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_evpoller_s;
struct tt_fiber_sched_s;
struct tt_task_s;
struct tt_entropy_s;
struct tt_ctr_drbg_s;
struct tt_buf_s;

/**
 @typedef tt_result_t (*tt_thread_routine_t)(IN void *param)
 thread routine

 @param param [in] thread routine parameter passed from creator

 @return
 undefined
 */
typedef tt_result_t (*tt_thread_routine_t)(IN void *param);

/**
 @struct tt_thread_attribute_t
 attribute used when creating a new thread
 */
typedef struct tt_thread_attr_s
{
    const tt_char_t *name;
    tt_bool_t detached : 1;
    tt_bool_t enable_fiber : 1;
} tt_thread_attr_t;

typedef enum {
    TT_THREAD_LOG_DEFAULT,
    TT_THREAD_LOG_PRINTF,
    TT_THREAD_LOG_NONE,

    TT_THREAD_LOG_NUM
} tt_thread_log_t;
#define TT_THREAD_LOG_VALID(l) ((l) < TT_THREAD_LOG_NUM)

/**
@struct tt_thread_t
thread struct
*/
typedef struct tt_thread_s
{
    /** routine to run in thread */
    tt_thread_routine_t routine;
    /** thread routing param */
    void *param;
    /** thread name */
    const tt_char_t *name;

    /** event poller. NULL if thread is not an event poller */
    struct tt_evpoller_s *evp;

    /** random number generator */
    tt_rng_t *rng;

    struct tt_fiber_sched_s *fiber_sched;
    struct tt_task_s *task;
    struct tt_entropy_s *entropy;
    struct tt_ctr_drbg_s *ctr_drbg;
    struct tt_buf_s *backtrace;

    /** system thread handle */
    tt_thread_ntv_t sys_thread;

    tt_result_t last_error;
    tt_bool_t detached : 1;
    tt_bool_t local : 1;
    tt_bool_t enable_fiber : 1;
    tt_thread_log_t log : 2;
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
tt_export void tt_thread_component_register();

/**
@fn tt_result_t tt_thread_create(IN tt_thread_t *thread,
                                 IN const tt_char_t *name,
                                 IN tt_thread_routine_t routine,
                                 IN void *param,
                                 IN tt_thread_attr_t *attr)
create a thread

@param [in] thread the thread to be created
@param [in] name thread name, if NULL, the name is specified automatically
@param [in] routine thread routine
@param [in] param param of thread routine
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
tt_export tt_thread_t *tt_thread_create(IN tt_thread_routine_t routine,
                                        IN void *param,
                                        IN OPT tt_thread_attr_t *attr);

tt_export tt_result_t tt_thread_create_local(IN OPT tt_thread_attr_t *attr);

/**
@fn void tt_thread_attr_default(IN tt_thread_attr_t  attr)
load default thread attributes

@param [in] attr thread attribute
*/
tt_export void tt_thread_attr_default(IN tt_thread_attr_t *attr);

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
tt_export tt_result_t tt_thread_wait(IN tt_thread_t *thread);

tt_export tt_result_t tt_thread_wait_local();

/**
@fn void tt_thread_exit()
exit current thread

@note
- this function has no parameter, so it can only be used to exit current
  thread
*/
tt_inline void tt_thread_exit()
{
    tt_thread_exit_ntv();
}

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
tt_inline tt_thread_t *tt_current_thread()
{
    return tt_current_thread_ntv();
}

tt_inline tt_thread_log_t tt_thread_get_log(IN tt_thread_t *thread)
{
    if (thread == NULL) {
        thread = tt_current_thread();
    }
    return TT_COND(thread != NULL, thread->log, TT_THREAD_LOG_PRINTF);
}

tt_inline tt_thread_log_t tt_thread_set_log(IN tt_thread_t *thread,
                                            IN tt_thread_log_t l)
{
    tt_thread_log_t org = TT_THREAD_LOG_PRINTF;
    if (thread == NULL) {
        thread = tt_current_thread();
    }
    if (thread != NULL) {
        org = thread->log;
        thread->log = l;
    }
    return org;
}

/**
@fn void tt_sleep(IN tt_u32_t millisec)
put calling thread into sleeping

@param [in] millisec how long it sleep in milliseconds
- if TT_THREAD_SLEEP_INFINITE, sleep for ever
*/
tt_inline void tt_sleep(IN tt_u32_t millisec)
{
    tt_sleep_ntv(millisec);
}

// can only be used in tt thread
tt_inline tt_u64_t tt_rand_u64()
{
    return tt_rng_u64(tt_current_thread()->rng);
}
#define tt_rand_u32() ((tt_u32_t)tt_rand_u64())

#endif /* __TT_THREAD__ */
