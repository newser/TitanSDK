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

#include <tt_io_poller_native.h>

#include <memory/tt_memory_alloc.h>
#include <os/tt_fiber.h>
#include <os/tt_task.h>

#include <tt_file_system_native.h>
#include <tt_socket_native.h>
#include <tt_sys_error.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

// return true to continue
typedef tt_bool_t (*__io_handler_t)(IN tt_io_ev_t *io_ev);

enum
{
    __POLLER_EXIT,

    __POLLER_EV_NUM
};

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_bool_t __worker_io(IN tt_io_ev_t *io_ev);

static tt_bool_t __poller_io(IN tt_io_ev_t *io_ev);

static tt_bool_t __fs_io(IN tt_io_ev_t *io_ev);

static tt_bool_t __skt_io(IN tt_io_ev_t *io_ev);

static __io_handler_t __io_handler[TT_IO_NUM] = {
    __worker_io, __poller_io, __fs_io, __skt_io,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_io_poller_component_init_ntv()
{
    return TT_SUCCESS;
}

tt_result_t tt_io_poller_create_ntv(IN tt_io_poller_ntv_t *sys_iop)
{
    sys_iop->iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (sys_iop->iocp == NULL) {
        TT_ERROR_NTV("fail to create iocp");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_io_poller_destroy_ntv(IN tt_io_poller_ntv_t *sys_iop)
{
    if (!CloseHandle(sys_iop->iocp)) {
        TT_ERROR_NTV("fail to close iocp");
    }
}

tt_bool_t tt_io_poller_run_ntv(IN tt_io_poller_ntv_t *sys_iop,
                               IN tt_s64_t wait_ms)
{
    DWORD NumberOfBytes;
    ULONG_PTR CompletionKey;
    OVERLAPPED *Overlapped;
    DWORD dwMilliseconds;
    BOOL ret;

    if ((wait_ms == TT_TIME_INFINITE) || (wait_ms > (tt_s64_t)0x7FFFFFFF)) {
        dwMilliseconds = INFINITE;
    } else {
        dwMilliseconds = (DWORD)wait_ms;
    }

    Overlapped = NULL;
    ret = GetQueuedCompletionStatus(sys_iop->iocp,
                                    &NumberOfBytes,
                                    &CompletionKey,
                                    &Overlapped,
                                    dwMilliseconds);
    if (Overlapped != NULL) {
        tt_io_ev_t *io_ev = TT_CONTAINER(Overlapped, tt_io_ev_t, ov);

        io_ev->io_bytes = NumberOfBytes;

        if (ret) {
            io_ev->io_result = TT_SUCCESS;
        } else {
            DWORD dwError = GetLastError();
            if ((dwError == ERROR_HANDLE_EOF) ||
                (dwError == ERROR_BROKEN_PIPE) ||
                (dwError == WSAECONNABORTED) || (dwError == WSAECONNRESET)) {
                io_ev->io_result = TT_END;
            } else {
                TT_ERROR_NTV("GQCS failed");
                io_ev->io_result = TT_FAIL;
            }
        }

        if (!__io_handler[io_ev->io](io_ev)) {
            return TT_FALSE;
        }
    } else if (GetLastError() != WAIT_TIMEOUT) {
        TT_ERROR_NTV("GetQueuedCompletionStatus fail");
        return TT_FALSE;
    }

    return TT_TRUE;
}

tt_result_t tt_io_poller_exit_ntv(IN tt_io_poller_ntv_t *sys_iop)
{
    tt_io_ev_t *io_ev;

    io_ev = tt_malloc(sizeof(tt_io_ev_t));
    if (io_ev == NULL) {
        TT_ERROR("no mem for poller exit ev");
        return TT_FAIL;
    }

    tt_memset(&io_ev->ov, 0, sizeof(OVERLAPPED));
    io_ev->src = NULL;
    io_ev->dst = NULL;
    tt_dnode_init(&io_ev->node);
    io_ev->io = TT_IO_POLLER;
    io_ev->ev = __POLLER_EXIT;

    if (!PostQueuedCompletionStatus(sys_iop->iocp,
                                    0,
                                    (ULONG_PTR)NULL,
                                    &io_ev->ov)) {
        TT_ERROR_NTV("fail to send poller exit");
        tt_free(io_ev);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_io_poller_finish_ntv(IN tt_io_poller_ntv_t *sys_iop,
                                    IN tt_io_ev_t *io_ev)
{
    io_ev->io = TT_IO_WORKER;

    if (!PostQueuedCompletionStatus(sys_iop->iocp,
                                    0,
                                    (ULONG_PTR)NULL,
                                    &io_ev->ov)) {
        TT_ERROR_NTV("fail to send poller finish");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_io_poller_send_ntv(IN tt_io_poller_ntv_t *sys_iop,
                                  IN tt_io_ev_t *io_ev)
{
    io_ev->io = TT_IO_POLLER;

    if (!PostQueuedCompletionStatus(sys_iop->iocp,
                                    0,
                                    (ULONG_PTR)NULL,
                                    &io_ev->ov)) {
        TT_ERROR_NTV("fail to send to poller");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_bool_t __worker_io(IN tt_io_ev_t *io_ev)
{
    TT_ASSERT(io_ev->src != NULL);
    tt_fiber_resume(io_ev->src, TT_TRUE);

    return TT_TRUE;
}

tt_bool_t __poller_io(IN tt_io_ev_t *io_ev)
{
    if (io_ev->ev == __POLLER_EXIT) {
        tt_free(io_ev);
        return TT_FALSE;
    }

    // a message to this fiber
    // todo: add to fiber and awake the fiber if needed

    if (io_ev->src != NULL) {
        tt_task_finish(io_ev->dst->fs->thread->task, io_ev);
    } else {
        tt_free(io_ev);
    }

    return TT_TRUE;
}

tt_bool_t __fs_io(IN tt_io_ev_t *io_ev)
{
    if (tt_fs_poller_io(io_ev)) {
        TT_ASSERT(io_ev->src != NULL);
        tt_fiber_resume(io_ev->src, TT_TRUE);
    }

    return TT_TRUE;
}

tt_bool_t __skt_io(IN tt_io_ev_t *io_ev)
{
    if (tt_skt_poller_io(io_ev)) {
        TT_ASSERT(io_ev->src != NULL);
        tt_fiber_resume(io_ev->src, TT_TRUE);
    }

    return TT_TRUE;
}

