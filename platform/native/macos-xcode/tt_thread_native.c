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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_thread_native.h>

#include <misc/tt_assert.h>
#include <os/tt_thread.h>

#include <tt_sys_error.h>

#include <signal.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern tt_result_t __thread_on_create(IN tt_thread_t *thread);
extern tt_result_t __thread_on_exit(IN tt_thread_t *thread);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

pthread_key_t tt_g_thread_key;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __thread_on_exit_ntv(void *arg);

static void *__thread_routine_wrapper(IN void *param);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_thread_component_init_ntv()
{
    int ret;

    ret = pthread_key_create(&tt_g_thread_key, __thread_on_exit_ntv);
    if (ret != 0) {
        TT_ERROR("fail to create thread key: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_thread_create_ntv(IN struct tt_thread_s *thread)
{
    tt_thread_ntv_t *sys_thread = &thread->sys_thread;
    int ret;
    pthread_attr_t pthread_attr;

    ret = pthread_attr_init(&pthread_attr);
    if (ret != 0) {
        TT_ERROR("fail to init thread attribute: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
    }

    ret = pthread_attr_setdetachstate(&pthread_attr,
                                      TT_COND(thread->detached,
                                              PTHREAD_CREATE_DETACHED,
                                              PTHREAD_CREATE_JOINABLE));
    if (ret != 0) {
        TT_ERROR("fail to set thread detach state: %d[%s]", ret, strerror(ret));
        pthread_attr_destroy(&pthread_attr);
        return TT_FAIL;
    }

    ret = pthread_create(&sys_thread->handle,
                         &pthread_attr,
                         __thread_routine_wrapper,
                         thread);
    pthread_attr_destroy(&pthread_attr);
    if (ret != 0) {
        TT_FATAL("fail to create thread: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
    }

#if 0
    // steps of thread creating between parent/child:
    //  - parent create child thread and could continue changing thread
    //    fields and set sys_thread->status to __THREAD_CREATING and wait
    //    until sys_thread->status becomes __THREAD_RUNNING
    //  - child thread should wait until sys_thread->status becomes
    //    __THREAD_CREATING, now parent thread have set all fields of
    //    thread and child thread could changing thread fields now and
    //    set sys_thread->status to __THREAD_RUNNING
    //  - now all thread fields are updated and parent have got a
    //    consistent thread structure and could return it

    TT_ASSERT_ALWAYS(__sync_bool_compare_and_swap(&sys_thread->status,
                                                  __THREAD_INITIALIZING,
                                                  __THREAD_CREATING));

    // if child thread is detached, now the thread structure may have
    // been freed
    if (!detached) {
        while (!__sync_bool_compare_and_swap(&sys_thread->status,
                                             __THREAD_RUNNING,
                                             __THREAD_RUNNING))
            ;
    }
#endif

    return TT_SUCCESS;
}

tt_result_t tt_thread_create_local_ntv(IN struct tt_thread_s *thread)
{
    sigset_t mask;

    if (pthread_setspecific(tt_g_thread_key, thread) != 0) {
        TT_ERROR("fail to set thread specific data");
        return TT_FAIL;
    }

    sigfillset(&mask);
    sigdelset(&mask, SIGABRT);
    sigdelset(&mask, SIGSEGV);
    sigdelset(&mask, SIGBUS);
    sigdelset(&mask, SIGILL);
    sigdelset(&mask, SIGFPE);
    sigdelset(&mask, SIGPIPE);
    sigdelset(&mask, SIGINT);
    if (pthread_sigmask(SIG_BLOCK, &mask, NULL) != 0) {
        TT_ERROR("fail to block signals");
    }

    __thread_on_create(thread);

    return TT_SUCCESS;
}

tt_result_t tt_thread_wait_ntv(IN struct tt_thread_s *thread)
{
    int ret;

    ret = pthread_join(thread->sys_thread.handle, NULL);
    if (ret != 0) {
        TT_ERROR("fail to wait thread: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_sleep_ntv(IN tt_u32_t millisec)
{
    struct timespec req = {0};
    struct timespec rem = {0};

    req.tv_sec = millisec / 1000;
    req.tv_nsec = (millisec % 1000) * 1000000;

again:
    if (nanosleep(&req, &rem) != 0) {
        if (errno == EINTR) {
            req.tv_sec = rem.tv_sec;
            req.tv_nsec = rem.tv_nsec;
            goto again;
        } else {
            TT_ERROR_NTV("thread fails to sleep");
        }
    }
}

void __thread_on_exit_ntv(void *arg)
{
    tt_thread_t *thread = (tt_thread_t *)arg;

    TT_ASSERT(thread != NULL);

#if 0
    // restore sigset for local running thread
    if (pthread_sigmask(SIG_SETMASK, &org_mask, NULL) != 0)
    {
        TT_ERROR("fail to restore signals");
    }
#endif

    __thread_on_exit(thread);
    // __thread_on_exit may free the thread structure
}

void *__thread_routine_wrapper(IN void *param)
{
    tt_thread_t *thread = (tt_thread_t *)param;
    tt_thread_ntv_t *sys_thread = &thread->sys_thread;
    sigset_t mask;

#if 0
    // note it's not safe to access sys_thread->handle

    // wait for creater's done
    while (!__sync_bool_compare_and_swap(&sys_thread->status,
                                         __THREAD_CREATING,
                                         __THREAD_CREATING))
        ;

    // memory barrier is inserted by __sync_bool_compare_and_swap()
    // now it's safe to access member of tt_thread_t
#endif

    sigfillset(&mask);
    sigdelset(&mask, SIGABRT);
    sigdelset(&mask, SIGSEGV);
    sigdelset(&mask, SIGBUS);
    sigdelset(&mask, SIGILL);
    sigdelset(&mask, SIGFPE);
    sigdelset(&mask, SIGPIPE);
    sigdelset(&mask, SIGINT);
    if (pthread_sigmask(SIG_BLOCK, &mask, NULL) != 0) {
        TT_ERROR("fail to block signals");
    }

    if (pthread_setspecific(tt_g_thread_key, thread) != 0) {
        TT_ERROR("fail to set thread specific data");
        return NULL;
    }

    // something may be needed from upper layer
    __thread_on_create(thread);

#if 0
    // notify creater
    TT_ASSERT_ALWAYS(__sync_bool_compare_and_swap(&sys_thread->status,
                                                  __THREAD_CREATING,
                                                  __THREAD_RUNNING));
#endif

    thread->routine(thread->param);

    return NULL;
}
