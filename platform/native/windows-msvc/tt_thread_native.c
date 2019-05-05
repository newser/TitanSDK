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

#include <algorithm/tt_list.h>
#include <misc/tt_assert.h>
#include <os/tt_fiber.h>
#include <os/tt_thread.h>

#include <tt_cstd_api.h>
#include <tt_sys_error.h>

#include <process.h>

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

DWORD tt_g_thread_fls_index = 0;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static VOID WINAPI __thread_on_exit_ntv(IN PVOID *arg);

static void __cdecl __detached_routine_wrapper(IN void *param);

static unsigned __stdcall __joint_routine_wrapper(IN void *param);

static tt_result_t __thread_bind_numa(IN HANDLE hThread, IN USHORT Node);

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

void tt_thread_component_exit_ntv()
{
    FlsFree(tt_g_thread_fls_index);
}

tt_result_t tt_thread_create_ntv(IN tt_thread_t *thread)
{
    tt_thread_ntv_t *sys_thread = &thread->sys_thread;

    sys_thread->end = TT_FALSE;

    if (thread->detached) {
        sys_thread->h_thread =
            (HANDLE)_beginthread(__detached_routine_wrapper, 0, thread);
        if ((sys_thread->h_thread == 0) ||
            (sys_thread->h_thread == (HANDLE)-1L)) {
            TT_ERROR_NTV("fail to create detached thread");
            return TT_FAIL;
        }

        // remember to _endthread
        return TT_SUCCESS;
    } else {
        sys_thread->h_thread =
            (HANDLE)_beginthreadex(NULL, 0, __joint_routine_wrapper, thread, 0,
                                   NULL);
        if (sys_thread->h_thread == 0) {
            TT_ERROR_NTV("fail to create joint thread");
            return TT_FAIL;
        }

        // remember to _endthreadex
        return TT_SUCCESS;
    }
}

tt_result_t tt_thread_create_local_ntv(IN tt_thread_t *thread)
{
    tt_thread_ntv_t *sys_thread = &thread->sys_thread;

    if (tt_g_numa_node_id_thread != TT_NUMA_NODE_ID_UNSPECIFIED) {
        __thread_bind_numa(GetCurrentThread(),
                           (USHORT)tt_g_numa_node_id_thread);
    }

    TT_ASSERT(FlsGetValue(tt_g_thread_fls_index) == NULL);
    if (FlsSetValue(tt_g_thread_fls_index, (LPVOID)thread) == 0) {
        TT_ERROR_NTV("fail to set thread private data");
        return TT_FAIL;
    }

    __thread_on_create(thread);

    return TT_SUCCESS;
}

tt_result_t tt_thread_wait_ntv(IN tt_thread_t *thread)
{
    tt_thread_ntv_t *sys_thread = &thread->sys_thread;

    if (WaitForSingleObject(sys_thread->h_thread, INFINITE) != WAIT_OBJECT_0) {
        TT_ERROR_NTV("fail to wait joint thread");
        return TT_FAIL;
    }

    if (!CloseHandle(sys_thread->h_thread)) {
        TT_ERROR_NTV("fail to close thread handle");
    }

    return TT_SUCCESS;
}

tt_result_t tt_thread_wait_local_ntv(IN struct tt_thread_s *thread)
{
    __thread_on_exit(thread);

    if (!FlsSetValue(tt_g_thread_fls_index, NULL)) {
        TT_ERROR_NTV("fail to clear thread specific data");
        // return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_thread_exit_ntv()
{
    tt_thread_t *thread = (tt_thread_t *)FlsGetValue(tt_g_thread_fls_index);

    if (thread->detached) {
        _endthread();
    } else {
        _endthreadex(TT_SUCCESS);
    }
}

VOID WINAPI __thread_on_exit_ntv(IN PVOID *arg)
{
    tt_thread_t *thread = (tt_thread_t *)arg;

    if (thread == NULL) {
        // a fiber?
        return;
    }

    if (thread->sys_thread.end) {
        // this happens when destroying a thread which has serveral
        // running fibers
        return;
    }
    thread->sys_thread.end = TT_TRUE;

    __thread_on_exit(thread);
    // thread may already be freed
}

void __cdecl __detached_routine_wrapper(IN void *param)
{
    tt_thread_t *thread = (tt_thread_t *)param;
    tt_thread_ntv_t *sys_thread = &thread->sys_thread;

    if (tt_g_numa_node_id_thread != TT_NUMA_NODE_ID_UNSPECIFIED) {
        __thread_bind_numa(sys_thread->h_thread,
                           (USHORT)tt_g_numa_node_id_thread);
    }

    if (FlsSetValue(tt_g_thread_fls_index, (LPVOID)thread) == 0) {
        TT_ERROR_NTV("fail to set thread private data");
        return;
    }

    __thread_on_create(thread);

    thread->routine(thread->param);
}

unsigned __stdcall __joint_routine_wrapper(IN void *param)
{
    tt_thread_t *thread = (tt_thread_t *)param;
    tt_thread_ntv_t *sys_thread = &thread->sys_thread;

    if (tt_g_numa_node_id_thread != TT_NUMA_NODE_ID_UNSPECIFIED) {
        __thread_bind_numa(sys_thread->h_thread,
                           (USHORT)tt_g_numa_node_id_thread);
    }

    if (FlsSetValue(tt_g_thread_fls_index, (LPVOID)thread) == 0) {
        TT_ERROR_NTV("fail to set thread private data");
        return TT_SUCCESS;
    }

    __thread_on_create(thread);

    thread->routine(thread->param);

    return TT_SUCCESS;
}

tt_result_t __thread_bind_numa(IN HANDLE hThread, IN USHORT Node)
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
