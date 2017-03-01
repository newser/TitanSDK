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
@file tt_thread_native.h
@brief system thread APIs

this file specifies interfaces for system specific thread operations.
*/

#ifndef __TT_THREAD_NATIVE__
#define __TT_THREAD_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_sys_error.h>

#include <errno.h>
#include <pthread.h>
#include <string.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

/**
@def __THREAD_MAGIC
sys thread type identifier
*/
#define __THREAD_MAGIC (0xEFCCEFCC)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_thread_s;

typedef struct
{
    __TT_PRIVATE__

    pthread_t thread_handle;

    tt_u32_t magic;
    tt_u32_t status;
} tt_thread_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

extern pthread_key_t tt_g_thread_key;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn tt_result_t tt_thread_component_init_ntv()
initialize ts thread portlayer system

@return
- TT_SUCCESS if initialization succeed
- TT_FAIL otherwise
*/
extern tt_result_t tt_thread_component_init_ntv();

/**
@fn
tt_result_t tt_thread_create_ntv(IN struct tt_thread_s *thread)
create a windows system thread

@param thread [inout] ts thread

@return
- TT_SUCCESS the system thread is created successfully
- TT_FAIL otherwise

@note
- other members of @ref thread must have been properly initialized before
  calling this function.
*/
extern tt_result_t tt_thread_create_ntv(IN struct tt_thread_s *thread);

/**
@fn
tt_result_t tt_thread_create_local_ntv(IN struct tt_thread_s *thread)
simulate running a thread

@param sys_thread [inout] ts system thread
@param routine [in] ts system thread routine
@param routine_param [in] parameter of ts system thread routine
@param attr [in] ts thread attribute

@return
- TT_SUCCESS the thread has run and exited
- TT_FAIL the thread has not run due to some error
*/
extern tt_result_t tt_thread_create_local_ntv(IN struct tt_thread_s *thread);

/**
@fn tt_result_t tt_thread_wait_ntv(IN struct tt_thread_s *thread)
wait for a specific system thread ending

@param [in] sys_thread the system thread to be waited

@return
- TT_SUCCESS the system thread is waited and destroyed
- TT_FAIL otherwise

@note
- when "sys_thread" is ever created as detached, this function would lead
  to undetermined behavior
- when this function return, the thread specified by param has ended and
  destroyed
*/
extern tt_result_t tt_thread_wait_ntv(IN struct tt_thread_s *thread);

/**
@fn void tt_thread_exit_ntv()
exit current thread

@note
- this function has no parameter, so it can only be used to exit current
  thread
*/
extern void tt_thread_exit_ntv();

/**
@fn tt_result_t tt_sleep_ntv(IN tt_u32_t millisec)
put current thread into sleeping

@param [in] millisec the time in millisecond how long the thread would sleep
- if __TT_THREAD_SLEEP_INFINITE, the thread would sleep for ever
- if 0, the thread just yield cpu

@return
- TT_SUCCESS if sleeping done
- TT_FAIL if some error occurs
*/
tt_inline tt_result_t tt_sleep_ntv(IN tt_u32_t millisec)
{
    struct timespec req = {0};
    struct timespec rem = {0};

    req.tv_sec = millisec / 1000;
    req.tv_nsec = (millisec % 1000) * 1000000;
    do {
        if (nanosleep(&req, &rem) == 0) {
            break;
        } else if (errno == EINTR) {
            req.tv_sec = rem.tv_sec;
            req.tv_nsec = rem.tv_nsec;
            // give a warning?

            continue;
        } else {
            TT_ERROR_NTV("thread fails to sleep");
            return TT_FAIL;
        }
    } while (1);

    return TT_SUCCESS;
}

/**
@fn tt_thread_native_t *tt_current_thread_ntv()
get current sys thread

@return
- current sys thread struct
- NULL if current thread is not a ts thread

@note
- this function may impact performance on numa arch, so only call it when
  it's really needed
*/
tt_inline struct tt_thread_s *tt_current_thread_ntv()
{
    return (struct tt_thread_s *)pthread_getspecific(tt_g_thread_key);
}

#endif /* __TT_THREAD_NATIVE__ */
