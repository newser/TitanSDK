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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_thread_native.h>

#include <init/tt_platform_info.h>
#include <log/tt_log.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <os/tt_thread.h>

#include <tt_cstd_api.h>
#include <tt_sys_error.h>

#include <process.h>
#include <windows.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __THREAD_INITIALIZING 0
#define __THREAD_CREATING 1
#define __THREAD_RUNNING 2

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

DWORD tt_g_thread_fls_index = 0;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static VOID WINAPI __thread_on_exit_ntv(IN PVOID *arg);

static void __cdecl detached_routine_wrapper(IN void *param);
static unsigned __stdcall joint_routine_wrapper(IN void *param);

static void __thread_exit_ntv(IN tt_thread_t *sys_thread);

static tt_result_t __thread_bind_numa_node(IN HANDLE hThread, IN USHORT Node);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_thread_component_init_ntv()
{
    tt_g_thread_fls_index = FlsAlloc(__thread_on_exit_ntv);
    if (tt_g_thread_fls_index == FLS_OUT_OF_INDEXES) {
        TT_ERROR_NTV("fail to init thread ntv");
        return TT_FAIL;
    }

    // when to FlsFree?
    return TT_SUCCESS;
}

tt_result_t tt_thread_create_ntv(IN struct tt_thread_s *thread)
{
    tt_thread_ntv_t *sys_thread = &thread->sys_thread;

    tt_memset(sys_thread, 0, sizeof(tt_thread_ntv_t));

    // init
    sys_thread->magic = __THREAD_MAGIC;
    sys_thread->status = __THREAD_INITIALIZING;

    if (thread->attr.detached) {
        sys_thread->thread_handle =
            (HANDLE)_beginthread(detached_routine_wrapper, 0, thread);
        if ((sys_thread->thread_handle == 0) ||
            (sys_thread->thread_handle == (HANDLE)-1L)) {
            TT_ERROR_NTV("fail to create detached thread");
            return TT_FAIL;
        }

        TT_ASSERT_ALWAYS(InterlockedCompareExchange(&sys_thread->status,
                                                    __THREAD_CREATING,
                                                    __THREAD_INITIALIZING) ==
                         __THREAD_INITIALIZING);

        // if child thread is detached, now the thread structure may have
        // been freed

        // remember to _endthread
        return TT_SUCCESS;
    } else {
        sys_thread->thread_handle = (HANDLE)
            _beginthreadex(NULL, 0, joint_routine_wrapper, thread, 0, NULL);
        if (sys_thread->thread_handle == 0) {
            TT_ERROR_NTV("fail to create joint thread");
            return TT_FAIL;
        }

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

        TT_ASSERT_ALWAYS(InterlockedCompareExchange(&sys_thread->status,
                                                    __THREAD_CREATING,
                                                    __THREAD_INITIALIZING) ==
                         __THREAD_INITIALIZING);

        // wait for thread update content sys_thread
        while (InterlockedCompareExchange(&sys_thread->status,
                                          __THREAD_RUNNING,
                                          __THREAD_RUNNING) != __THREAD_RUNNING)
            ;

        // remember to _endthreadex
        return TT_SUCCESS;
    }
}

tt_result_t tt_thread_create_local_ntv(IN struct tt_thread_s *thread)
{
    tt_thread_ntv_t *sys_thread = &thread->sys_thread;

    tt_memset(sys_thread, 0, sizeof(tt_thread_ntv_t));

    // init
    sys_thread->magic = __THREAD_MAGIC;
    sys_thread->status = __THREAD_INITIALIZING;

    if (tt_g_numa_node_id_thread != TT_NUMA_NODE_ID_UNSPECIFIED) {
        __thread_bind_numa_node(GetCurrentThread(),
                                (USHORT)tt_g_numa_node_id_thread);
    }

    if (FlsSetValue(tt_g_thread_fls_index, (LPVOID)thread) == 0) {
        TT_ERROR_NTV("fail to set thread private data");
        return TT_FAIL;
    }

    __thread_on_create(thread);
    sys_thread->status = __THREAD_RUNNING;

    return TT_SUCCESS;
}

tt_result_t tt_thread_wait_ntv(IN struct tt_thread_s *thread)
{
    tt_thread_ntv_t *sys_thread = &thread->sys_thread;

    TT_ASSERT(sys_thread->thread_handle != NULL);
    TT_ASSERT(!thread->attr.detached);

    // wait untill the thread ends
    if (WaitForSingleObject(sys_thread->thread_handle, INFINITE) !=
        WAIT_OBJECT_0) {
        TT_ERROR_NTV("fail to wait joint thread");
        return TT_FAIL;
    }

    // close thread handle
    if (!CloseHandle(sys_thread->thread_handle)) {
        TT_ERROR_NTV("fail to close thread handle");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_thread_exit_ntv()
{
    tt_thread_t *thread = (tt_thread_t *)FlsGetValue(tt_g_thread_fls_index);

    __thread_exit_ntv(thread);
}

VOID __thread_on_exit_ntv(IN PVOID *arg)
{
    tt_thread_t *thread = (tt_thread_t *)arg;

    if (thread != NULL) {
        __thread_on_exit(thread);
    }
}

void __cdecl detached_routine_wrapper(IN void *param)
{
    tt_thread_t *thread = (tt_thread_t *)param;
    tt_thread_ntv_t *sys_thread = &thread->sys_thread;
    tt_result_t result;

    // note it's not safe to access sys_thread->thread_handle

    // wait for creater's done
    while (InterlockedCompareExchange(&sys_thread->status,
                                      __THREAD_CREATING,
                                      __THREAD_CREATING) != __THREAD_CREATING)
        ;
    // memory barrier is inserted by InterlockedCompareExchange()
    // now it's safe to access member of tt_thread_t

    if (tt_g_numa_node_id_thread != TT_NUMA_NODE_ID_UNSPECIFIED) {
        __thread_bind_numa_node(sys_thread->thread_handle,
                                (USHORT)tt_g_numa_node_id_thread);
    }

    if (FlsSetValue(tt_g_thread_fls_index, (LPVOID)thread) == 0) {
        TT_ERROR_NTV("fail to set thread private data");
        return;
    }

    __thread_on_create(thread);

    // notify creater
    TT_ASSERT_ALWAYS(InterlockedCompareExchange(&sys_thread->status,
                                                __THREAD_RUNNING,
                                                __THREAD_CREATING) ==
                     __THREAD_CREATING);

    result = thread->routine(thread, thread->param);
}

unsigned __stdcall joint_routine_wrapper(IN void *param)
{
    tt_thread_t *thread = (tt_thread_t *)param;
    tt_thread_ntv_t *sys_thread = &thread->sys_thread;
    tt_result_t result;

    // note it's not safe to access sys_thread->thread_handle

    // wait for creater's done
    while (InterlockedCompareExchange(&sys_thread->status,
                                      __THREAD_CREATING,
                                      __THREAD_CREATING) != __THREAD_CREATING)
        ;
    // memory barrier is inserted by InterlockedCompareExchange()
    // now it's safe to access member of tt_thread_t

    if (tt_g_numa_node_id_thread != TT_NUMA_NODE_ID_UNSPECIFIED) {
        __thread_bind_numa_node(sys_thread->thread_handle,
                                (USHORT)tt_g_numa_node_id_thread);
    }

    if (FlsSetValue(tt_g_thread_fls_index, (LPVOID)thread) == 0) {
        TT_ERROR_NTV("fail to set thread private data");
        return TT_SUCCESS;
    }

    __thread_on_create(thread);

    // notify creater
    TT_ASSERT_ALWAYS(InterlockedCompareExchange(&sys_thread->status,
                                                __THREAD_RUNNING,
                                                __THREAD_CREATING) ==
                     __THREAD_CREATING);

    result = thread->routine(thread, thread->param);

    return TT_SUCCESS;
}

void __thread_exit_ntv(IN tt_thread_t *thread)
{
    tt_thread_ntv_t *sys_thread = &thread->sys_thread;

    if (sys_thread->magic != __THREAD_MAGIC) {
        TT_ERROR("not a ts thread");
        return;
    }

    // sys_thread->thread_handle is null when exiting a local running thread
    // TT_ASSERT(sys_thread->thread_handle != NULL);

    if (thread->attr.detached) {
        _endthread();
        // memory barrier is inserted by _endthread()
        // do not change anything of tt_thread_t from now on

        return;
    } else {
        _endthreadex(TT_SUCCESS);
        // memory barrier is inserted by _endthreadex()
        // do not change anything of tt_thread_t from now on

        // CloseHandle() should be done by tt_thread_wait_ntv()
        return;
    }
}

tt_result_t __thread_bind_numa_node(IN HANDLE hThread, IN USHORT Node)
{
    GROUP_AFFINITY ProcessorMask;

    // how to handle the arch in which a numa node includes more than
    // 64 cpus??

    if (!GetNumaNodeProcessorMaskEx(Node, &ProcessorMask)) {
        TT_ERROR("fail to get numa node processor mask");
        return TT_FAIL;
    }

    if (!SetThreadGroupAffinity(hThread, &ProcessorMask, NULL)) {
        TT_ERROR("fail to bind thread to numa node[%d]", Node);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
