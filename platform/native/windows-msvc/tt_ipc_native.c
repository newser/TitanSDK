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

#include <tt_ipc_native.h>

#include <io/tt_io_event.h>
#include <io/tt_ipc.h>
#include <memory/tt_memory_alloc.h>
#include <os/tt_fiber.h>
#include <os/tt_fiber_event.h>
#include <os/tt_task.h>
#include <time/tt_timer.h>

#include <tt_sys_error.h>
#include <tt_util_native.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_IPC TT_ASSERT

//#define __SIMU_FAIL_CreateFileA
//#define __SIMU_FAIL_CreateNamedPipeA
//#define __SIMU_FAIL_ConnectNamedPipe
//#define __SIMU_FAIL_CloseHandle
#define __SIMU_FAIL_ReadFile
#define __SIMU_FAIL_WriteFile

#ifdef __SIMU_FAIL_CreateFileA
#define CreateFileA __sf_CreateFileA
HANDLE WINAPI __sf_CreateFileA(LPCSTR lpFileName,
                               DWORD dwDesiredAccess,
                               DWORD dwShareMode,
                               LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                               DWORD dwCreationDisposition,
                               DWORD dwFlagsAndAttributes,
                               HANDLE hTemplateFile);
#endif

#ifdef __SIMU_FAIL_CreateNamedPipeA
#define CreateNamedPipeA __sf_CreateNamedPipeA
HANDLE WINAPI __sf_CreateNamedPipeA(LPCSTR lpName,
                                    DWORD dwOpenMode,
                                    DWORD dwPipeMode,
                                    DWORD nMaxInstances,
                                    DWORD nOutBufferSize,
                                    DWORD nInBufferSize,
                                    DWORD nDefaultTimeOut,
                                    LPSECURITY_ATTRIBUTES lpSecurityAttributes);
#endif

#ifdef __SIMU_FAIL_ConnectNamedPipe
#define ConnectNamedPipe __sf_ConnectNamedPipe
BOOL WINAPI __sf_ConnectNamedPipe(HANDLE hNamedPipe, LPOVERLAPPED lpOverlapped);
#endif

#ifdef __SIMU_FAIL_CloseHandle
#define CloseHandle __sf_CloseHandle
BOOL WINAPI __sf_CloseHandle(HANDLE hObject);
#endif

#ifdef __SIMU_FAIL_ReadFile
#define ReadFile __sf_ReadFile
BOOL WINAPI __sf_ReadFile(HANDLE hFile,
                          LPVOID lpBuffer,
                          DWORD nNumberOfBytesToRead,
                          LPDWORD lpNumberOfBytesRead,
                          LPOVERLAPPED lpOverlapped);
#endif

#ifdef __SIMU_FAIL_WriteFile
#define WriteFile __sf_WriteFile
BOOL WINAPI __sf_WriteFile(HANDLE hFile,
                           LPCVOID lpBuffer,
                           DWORD nNumberOfBytesToWrite,
                           LPDWORD lpNumberOfBytesWritten,
                           LPOVERLAPPED lpOverlapped);
#endif

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    __IPC_ACCEPT,
    __IPC_SEND,
    __IPC_RECV,

    __IPC_EV_NUM,
};

typedef struct
{
    tt_io_ev_t io_ev;

    tt_ipc_ntv_t *ipc;
    tt_ipc_ntv_t *new_ipc;

    tt_result_t result;
} __ipc_accept_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_ipc_ntv_t *ipc;
    tt_u8_t *buf;
    tt_u32_t *sent;
    tt_u32_t len;

    tt_result_t result;
    tt_u32_t pos;
} __ipc_send_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_ipc_ntv_t *ipc;
    tt_u8_t *buf;
    tt_u32_t *recvd;
    tt_u32_t len;

    tt_result_t result;
    tt_bool_t done : 1;
    tt_bool_t canceled : 1;
} __ipc_recv_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_bool_t __do_accept(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_send(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_recv(IN tt_io_ev_t *io_ev);

static tt_poller_io_t __ipc_poller_io[__IPC_EV_NUM] = {
    __do_accept, __do_send, __do_recv,
};

static tt_char_t tt_s_pipe_prefix[] = "\\\\.\\pipe\\";

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_char_t *__pipe_name(IN const tt_char_t *addr);

static HANDLE __ipc_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_ipc_create_ntv(IN tt_ipc_ntv_t *ipc,
                              IN OPT const tt_char_t *addr,
                              IN OPT tt_ipc_attr_t *attr)
{
    if (addr != NULL) {
        ipc->name = __pipe_name(addr);
        if (ipc->name == NULL) {
            return TT_FAIL;
        }
    } else {
        ipc->name = NULL;
    }

    ipc->pipe = INVALID_HANDLE_VALUE;

    // TODO: change attr
    ipc->in_buf_size = (1 << attr->recv_buf_attr.max_extend);
    ipc->out_buf_size = (1 << attr->recv_buf_attr.max_extend);

    return TT_SUCCESS;
}

void tt_ipc_destroy_ntv(IN tt_ipc_ntv_t *ipc)
{
    if (ipc->pipe != INVALID_HANDLE_VALUE) {
        CloseHandle(ipc->pipe);
    }

    if (ipc->name != NULL) {
        tt_free(ipc->name);
    }
}

tt_result_t tt_ipc_connect_ntv(IN tt_ipc_ntv_t *ipc, IN const tt_char_t *addr)
{
    tt_char_t *name;
    HANDLE pipe, iocp;

    TT_ASSERT_IPC(ipc->pipe == INVALID_HANDLE_VALUE);

    // no need to suspend current fiber, as CreateFile won't block
    // caller:
    // If the CreateNamedPipe function was not successfully called on
    // the server prior to this operation, a pipe will not exist and
    // CreateFile will fail with ERROR_FILE_NOT_FOUND.

    name = __pipe_name(addr);
    if (name == NULL) {
        return TT_FAIL;
    }

    // create client ipc, defautlt pipe has:
    //  - byte-mode
    //  - blocking wait
    //  - overlapped disabled (to change)
    //  - write through disabled (to change??)
    pipe = CreateFileA(name,
                       GENERIC_READ | GENERIC_WRITE,
                       0,
                       NULL,
                       OPEN_EXISTING,
                       FILE_FLAG_OVERLAPPED /*| FILE_FLAG_WRITE_THROUGH*/,
                       NULL);
    tt_free(name);
    if (pipe == INVALID_HANDLE_VALUE) {
        TT_ERROR_NTV("fail to connect ipc pipe[%s]", addr);
        return TT_FAIL;
    }

    iocp = tt_current_fiber_sched()->thread->task->iop.sys_iop.iocp;
    if (CreateIoCompletionPort(pipe, iocp, (ULONG_PTR)ipc, 0) == NULL) {
        TT_ERROR_NTV("fail to bind client pipe to iocp");
        CloseHandle(pipe);
        return TT_FAIL;
    }

    ipc->pipe = pipe;

    return TT_SUCCESS;
}

tt_result_t tt_ipc_accept_ntv(IN tt_ipc_ntv_t *ipc, IN tt_ipc_ntv_t *new_ipc)
{
    DWORD dwOpenMode, dwPipeMode;
    HANDLE pipe, iocp;
    __ipc_accept_t ipc_accept;

    dwOpenMode = PIPE_ACCESS_DUPLEX | /*FILE_FLAG_WRITE_THROUGH |*/
                 FILE_FLAG_OVERLAPPED;
    dwPipeMode = PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | /*PIPE_NOWAIT |*/
                 PIPE_REJECT_REMOTE_CLIENTS;
    pipe = CreateNamedPipeA(ipc->name,
                            dwOpenMode,
                            dwPipeMode,
                            PIPE_UNLIMITED_INSTANCES,
                            ipc->out_buf_size,
                            ipc->in_buf_size,
                            0,
                            NULL);
    if (pipe == INVALID_HANDLE_VALUE) {
        TT_ERROR_NTV("fail to create server pipe");
        return TT_FAIL;
    }

    iocp = __ipc_ev_init(&ipc_accept.io_ev, __IPC_ACCEPT);
    ipc_accept.ipc = ipc;
    ipc_accept.new_ipc = new_ipc;
    ipc_accept.result = TT_FAIL;

    if (CreateIoCompletionPort(pipe, iocp, (ULONG_PTR)ipc, 0) == NULL) {
        TT_ERROR_NTV("fail to bind server pipe to iocp");
        CloseHandle(pipe);
        return TT_FAIL;
    }

    // wait for client
    if (!ConnectNamedPipe(pipe, &ipc_accept.io_ev.ov) &&
        (GetLastError() != ERROR_IO_PENDING)) {
        CloseHandle(pipe);
        return TT_FAIL;
    }

    new_ipc->name = NULL;
    new_ipc->pipe = pipe;
    tt_fiber_suspend();
    if (!TT_OK(ipc_accept.result)) {
        CloseHandle(pipe);
    }

    return ipc_accept.result;
}

tt_result_t tt_ipc_send_ntv(IN tt_ipc_ntv_t *ipc,
                            IN tt_u8_t *buf,
                            IN tt_u32_t len,
                            OUT OPT tt_u32_t *sent)
{
    __ipc_send_t ipc_send;
    DWORD dwError;

    __ipc_ev_init(&ipc_send.io_ev, __IPC_SEND);

    ipc_send.ipc = ipc;
    ipc_send.buf = buf;
    ipc_send.sent = sent;
    ipc_send.len = len;

    ipc_send.result = TT_FAIL;
    ipc_send.pos = 0;

    if (WriteFile(ipc->pipe, buf, len, NULL, &ipc_send.io_ev.ov) ||
        ((dwError = GetLastError()) == ERROR_IO_PENDING)) {
        tt_fiber_suspend();
        return ipc_send.result;
    }

    if (dwError == ERROR_BROKEN_PIPE) {
        TT_ERROR_NTV("ipc send fail");
        return TT_END;
    } else {
        TT_ERROR_NTV("ipc send fail");
        return TT_FAIL;
    }
}

tt_result_t tt_ipc_recv_ntv(IN tt_ipc_ntv_t *ipc,
                            OUT tt_u8_t *buf,
                            IN tt_u32_t len,
                            OUT tt_u32_t *recvd,
                            OUT tt_fiber_ev_t **p_fev,
                            OUT tt_tmr_t **p_tmr)
{
    __ipc_recv_t ipc_recv;
    tt_fiber_t *cfb;
    DWORD dwError;

    *recvd = 0;
    *p_fev = NULL;
    *p_tmr = NULL;

    __ipc_ev_init(&ipc_recv.io_ev, __IPC_RECV);
    cfb = ipc_recv.io_ev.src;

    // must try fiber recv, if some fiber send event during
    // current fiber sending, then current fiber won't be able
    // to see that event
    if (tt_fiber_recv_all(cfb, TT_FALSE, p_fev, p_tmr)) {
        return TT_SUCCESS;
    }

    ipc_recv.ipc = ipc;
    ipc_recv.buf = buf;
    ipc_recv.len = len;
    ipc_recv.recvd = recvd;

    ipc_recv.result = TT_FAIL;
    ipc_recv.done = TT_FALSE;
    ipc_recv.canceled = TT_FALSE;

    if (ReadFile(ipc->pipe, buf, len, NULL, &ipc_recv.io_ev.ov) ||
        ((dwError = GetLastError()) == ERROR_IO_PENDING)) {
        cfb->recving = TT_TRUE;
        // it should wait until notification comes, but once fiber awake
        // it need not set recving as either io is done or CancelIoEx()
        // is issued
        while (!ipc_recv.done) {
            tt_fiber_suspend();
            cfb->recving = TT_FALSE;

            if (!ipc_recv.canceled) {
                // if CancelIoEx() succeeds, wait for notification. or 
                // GetLastError() may be ERROR_NOT_FOUND which means io
                // is completed and has been queued
                if (CancelIoEx((HANDLE)ipc->pipe, &ipc_recv.io_ev.ov) ||
                    (GetLastError() == ERROR_NOT_FOUND)) {
                    ipc_recv.canceled = TT_TRUE;
                } else {
                    TT_ERROR("fail to cancel ipc recv");
                }
            }
        }

        if (tt_fiber_recv_all(cfb, TT_FALSE, p_fev, p_tmr)) {
            return TT_SUCCESS;
        }

        return ipc_recv.result;
    }

    if (dwError == ERROR_BROKEN_PIPE) {
        TT_ERROR_NTV("ipc recv fail");
        return TT_END;
    } else {
        TT_ERROR_NTV("ipc recv fail");
        return TT_FAIL;
    }
}

void tt_ipc_worker_io(IN tt_io_ev_t *io_ev)
{
}

tt_bool_t tt_ipc_poller_io(IN tt_io_ev_t *io_ev)
{
    return __ipc_poller_io[io_ev->ev](io_ev);
}

tt_char_t *__pipe_name(IN const tt_char_t *addr)
{
    tt_u32_t addr_len, len;
    tt_char_t *name;

    addr_len = (tt_u32_t)tt_strlen(addr) + 1;
    len = (tt_u32_t)sizeof(tt_s_pipe_prefix) - 1 + addr_len;
    if (len > 256) {
        TT_WARN("windows ipc name can only be at most 256 chars");
    }

    name = (tt_char_t *)tt_malloc(len);
    if (name == NULL) {
        TT_ERROR("no mem for ipc pipe name");
        return NULL;
    }
    tt_memcpy(name, tt_s_pipe_prefix, sizeof(tt_s_pipe_prefix) - 1);
    tt_memcpy(name + sizeof(tt_s_pipe_prefix) - 1, addr, addr_len);

    return name;
}

HANDLE __ipc_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev)
{
    io_ev->src = tt_current_fiber();
    io_ev->dst = NULL;
    tt_dnode_init(&io_ev->node);
    tt_memset(&io_ev->ov, 0, sizeof(OVERLAPPED));
    io_ev->io_bytes = 0;
    io_ev->io_result = TT_FAIL;
    io_ev->io = TT_IO_IPC;
    io_ev->ev = ev;

    return io_ev->src->fs->thread->task->iop.sys_iop.iocp;
}

tt_bool_t __do_accept(IN tt_io_ev_t *io_ev)
{
    __ipc_accept_t *ipc_accept = (__ipc_accept_t *)io_ev;

    ipc_accept->result = io_ev->io_result;
    return TT_TRUE;
}

tt_bool_t __do_send(IN tt_io_ev_t *io_ev)
{
    __ipc_send_t *ipc_send = (__ipc_send_t *)io_ev;

    DWORD dwError;

    ipc_send->pos += io_ev->io_bytes;
    if (ipc_send->pos == ipc_send->len) {
        TT_SAFE_ASSIGN(ipc_send->sent, ipc_send->pos);
        ipc_send->result = TT_SUCCESS;
        return TT_TRUE;
    }
    TT_ASSERT_IPC(ipc_send->pos < ipc_send->len);

    // return success whenever some data is sent
    if (!TT_OK(io_ev->io_result)) {
        if (ipc_send->pos > 0) {
            TT_SAFE_ASSIGN(ipc_send->sent, ipc_send->pos);
            ipc_send->result = TT_SUCCESS;
        } else {
            ipc_send->result = io_ev->io_result;
        }
        return TT_TRUE;
    }

    // send left data
    tt_memset(&ipc_send->io_ev.ov, 0, sizeof(OVERLAPPED));
    if (WriteFile(ipc_send->ipc->pipe,
                  TT_PTR_INC(char, ipc_send->buf, ipc_send->pos),
                  ipc_send->len - ipc_send->pos,
                  NULL,
                  &ipc_send->io_ev.ov) ||
        ((dwError = GetLastError()) == ERROR_IO_PENDING)) {
        return TT_FALSE;
    }

    // error
    if (ipc_send->pos > 0) {
        TT_SAFE_ASSIGN(ipc_send->sent, ipc_send->pos);
        ipc_send->result = TT_SUCCESS;
    } else if (dwError == ERROR_BROKEN_PIPE) {
        ipc_send->result = TT_END;
    } else {
        TT_ERROR_NTV("ipc send fail");
        ipc_send->result = TT_FAIL;
    }
    return TT_TRUE;
}

tt_bool_t __do_recv(IN tt_io_ev_t *io_ev)
{
    __ipc_recv_t *ipc_recv = (__ipc_recv_t *)io_ev;

    TT_ASSERT_IPC(io_ev->io_bytes <= ipc_recv->len);

    if (io_ev->io_bytes > 0) {
        TT_SAFE_ASSIGN(ipc_recv->recvd, io_ev->io_bytes);
        ipc_recv->result = TT_SUCCESS;
    } else if (TT_OK(io_ev->io_result)) {
        ipc_recv->result = TT_END;
    } else {
        ipc_recv->result = io_ev->io_result;
    }

    ipc_recv->done = TT_TRUE;
    return TT_TRUE;
}

#ifdef CreateFileA
#undef CreateFileA
HANDLE WINAPI __sf_CreateFileA(LPCSTR lpFileName,
                               DWORD dwDesiredAccess,
                               DWORD dwShareMode,
                               LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                               DWORD dwCreationDisposition,
                               DWORD dwFlagsAndAttributes,
                               HANDLE hTemplateFile)
{
    return INVALID_HANDLE_VALUE;
}
#endif

#ifdef CreateNamedPipeA
#undef CreateNamedPipeA
HANDLE WINAPI __sf_CreateNamedPipeA(LPCSTR lpName,
                                    DWORD dwOpenMode,
                                    DWORD dwPipeMode,
                                    DWORD nMaxInstances,
                                    DWORD nOutBufferSize,
                                    DWORD nInBufferSize,
                                    DWORD nDefaultTimeOut,
                                    LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
    return INVALID_HANDLE_VALUE;
}
#endif

#ifdef ConnectNamedPipe
#undef ConnectNamedPipe
BOOL WINAPI __sf_ConnectNamedPipe(HANDLE hNamedPipe, LPOVERLAPPED lpOverlapped)
{
    return FALSE;
}
#endif

#ifdef CloseHandle
#undef CloseHandle
BOOL WINAPI __sf_CloseHandle(HANDLE hObject)
{
    if (tt_rand_u32() % 2) {
        return FALSE;
    } else {
        return CloseHandle(hObject);
    }
}
#endif

#ifdef ReadFile
#undef ReadFile
BOOL WINAPI __sf_ReadFile(HANDLE hFile,
                          LPVOID lpBuffer,
                          DWORD nNumberOfBytesToRead,
                          LPDWORD lpNumberOfBytesRead,
                          LPOVERLAPPED lpOverlapped)
{
    if (0 && (tt_rand_u32() % 4) == 0) {
        return FALSE;
    }

    if (nNumberOfBytesToRead > 1) {
        nNumberOfBytesToRead = tt_rand_u32() % nNumberOfBytesToRead + 1;
    }
    return ReadFile(hFile,
                    lpBuffer,
                    nNumberOfBytesToRead,
                    lpNumberOfBytesRead,
                    lpOverlapped);
}
#endif

#ifdef WriteFile
#undef WriteFile
BOOL WINAPI __sf_WriteFile(HANDLE hFile,
                           LPCVOID lpBuffer,
                           DWORD nNumberOfBytesToWrite,
                           LPDWORD lpNumberOfBytesWritten,
                           LPOVERLAPPED lpOverlapped)
{
    if (0 && (tt_rand_u32() % 4) == 0) {
        return FALSE;
    }

    if (nNumberOfBytesToWrite > 1) {
        nNumberOfBytesToWrite = tt_rand_u32() % nNumberOfBytesToWrite + 1;
    }
    return WriteFile(hFile,
                     lpBuffer,
                     nNumberOfBytesToWrite,
                     lpNumberOfBytesWritten,
                     lpOverlapped);
}
#endif
