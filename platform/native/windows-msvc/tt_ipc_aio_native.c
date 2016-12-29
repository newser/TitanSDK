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

#include <tt_ipc_aio_native.h>

#include <event/tt_event_center.h>
#include <io/tt_ipc_aio.h>
#include <memory/tt_memory_alloc.h>

#include <tt_sys_error.h>
#include <tt_util_native.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_IPCAIO TT_ASSERT

#define __IPC_CHECK_EVC

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    TT_IPC_RANGE_AIO,
};

enum
{
    EV_IPC_START =
        TT_EV_MKID_SUB(TT_EV_RANGE_INTERNAL_IPC, TT_IPC_RANGE_AIO, 0),

    EV_IPC_CONNECT,
    EV_IPC_ACCEPT,
    EV_IPC_DESTROY,
    EV_IPC_SEND,
    EV_IPC_RECV,
    EV_IPC_DESTROY_WR,
    EV_IPC_DESTROY_RD,
    EV_IPC_Q_READ,
    EV_IPC_Q_WRITE,

    EV_IPC_END,
};

typedef struct
{
    // must put at head
    OVERLAPPED ov;

    tt_ipc_t *ipc;
    tt_char_t *remote_addr;
    tt_ipc_on_connect_t on_connect;

    tt_result_t result;
    void *cb_param;
} __ipc_connect_t;

typedef struct
{
    // must put at head
    OVERLAPPED ov;

    tt_ipc_t *listening_ipc;
    tt_ipc_t *new_ipc;
    tt_ipc_on_accept_t on_accept;

    tt_result_t result;
    void *cb_param;
} __ipc_accept_t;

typedef struct
{
    // must put at head
    OVERLAPPED ov;

    tt_ipc_t *ipc;
    tt_ev_t *data_ev; // from app
    tt_ipc_on_send_t on_send;
    tt_u32_t send_len;

    tt_result_t result;
    void *cb_param;
} __ipc_send_t;

typedef struct
{
    // must put at head
    OVERLAPPED ov;

    tt_ipc_t *ipc;
    tt_ev_t **data_ev; // from ipc to app
    tt_u32_t data_ev_num; // num of valid tev in data_ev
    tt_ipc_on_recv_t on_recv;

    tt_result_t result;
    void *cb_param;
} __ipc_recv_t;

typedef struct
{
    // must put at head
    OVERLAPPED ov;

    tt_ipc_t *ipc;
} __ipc_destroy_t;

typedef struct
{
    tt_ipc_t *ipc;
} __ipc_q_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_char_t tt_s_pipe_prefix[] = "\\\\.\\pipe\\";

static void __ipc_connect_on_destroy(IN struct tt_ev_s *ev);

static tt_ev_itf_t __ipc_connect_itf = {
    NULL, __ipc_connect_on_destroy,
};

static void __ipc_send_on_destroy(IN struct tt_ev_s *ev);

static tt_ev_itf_t __ipc_send_itf = {
    NULL, __ipc_send_on_destroy,
};

static void __ipc_recv_on_destroy(IN struct tt_ev_s *ev);

static tt_ev_itf_t __ipc_recv_itf = {
    NULL, __ipc_recv_on_destroy,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __mk_pipe_name(IN const tt_char_t *raw_name,
                                  OUT tt_char_t **pipe_name);

static tt_result_t __create_ipc(IN tt_ipc_ntv_t *sys_ipc,
                                IN tt_evcenter_t *evc,
                                IN tt_ipc_exit_t *ipc_exit,
                                IN tt_u32_t aio_ev_id);
static void __destroy_ipc(IN tt_ipc_ntv_t *sys_ipc,
                          IN tt_bool_t do_cb,
                          IN tt_bool_t check_free);

static tt_result_t __create_server_pipe(IN tt_ipc_ntv_t *sys_ipc,
                                        IN const tt_char_t *local_addr,
                                        IN tt_ipc_attr_t *attr);
static tt_result_t __create_client_pipe(IN tt_ipc_ntv_t *sys_ipc,
                                        IN const tt_char_t *remote_addr,
                                        IN tt_ipc_attr_t *attr);

// connect
static tt_bool_t __do_ipc_connect(IN __ipc_connect_t *aio,
                                  IN tt_result_t iocp_result);
static void __do_ipc_connect_cb(IN __ipc_connect_t *aio);

// accept
static tt_bool_t __do_ipc_accept(IN __ipc_accept_t *aio,
                                 IN tt_result_t iocp_result);
static tt_result_t __do_ipc_accept_io(IN __ipc_accept_t *aio);
static void __do_ipc_accept_cb(IN __ipc_accept_t *aio);

// send
static tt_bool_t __do_ipc_send(IN __ipc_send_t *aio,
                               IN tt_result_t iocp_result,
                               IN tt_u32_t send_num);
static tt_result_t __do_ipc_send_io(IN __ipc_send_t *aio, IN tt_u32_t send_num);
static void __do_ipc_send_cb(IN __ipc_send_t *aio);

// recv
static tt_bool_t __do_ipc_recv(IN __ipc_recv_t *aio,
                               IN tt_result_t iocp_result,
                               IN tt_u32_t recv_num);
static tt_result_t __do_ipc_recv_io(IN __ipc_recv_t *aio, IN tt_u32_t recv_num);
static tt_result_t __ipc_ev_parse(IN tt_buf_t *recv_buf, IN __ipc_recv_t *aio);
static void __do_ipc_recv_cb(IN __ipc_recv_t *aio);

// destroy
static tt_result_t __ipc_destroy_read(IN tt_ipc_t *ipc);
static void __do_ipc_destroy_read(IN __ipc_destroy_t *aio);
static tt_result_t __ipc_destroy_write(IN tt_ipc_t *ipc);
static void __do_ipc_destroy_write(IN __ipc_destroy_t *aio);

// aio q
static void __do_ipc_aio_q(IN tt_ipc_ntv_t *sys_ipc,
                           IN tt_list_t *aio_q,
                           IN tt_result_t iocp_result,
                           IN DWORD NumberOfBytes);
static void __clear_ipc_aio_q(IN tt_ipc_ntv_t *sys_ipc,
                              IN tt_list_t *aio_q,
                              IN tt_result_t aio_result);

// ========================================
// simulate failure
// ========================================

#ifdef __SIMULATE_IPC_AIO_FAIL

#define tt_evc_sendto_thread ipc_evc_send_thread_ev_SF
static tt_result_t ipc_evc_send_thread_ev_SF(IN tt_evcenter_t *evc,
                                             IN tt_thread_ev_t *tev);

#define PostQueuedCompletionStatus PostQueuedCompletionStatus_SF
static BOOL PostQueuedCompletionStatus_SF(HANDLE CompletionPort,
                                          DWORD dwNumberOfBytesTransferred,
                                          ULONG_PTR dwCompletionKey,
                                          LPOVERLAPPED lpOverlapped);

#define CreateFileA CreateFileA_SF
static HANDLE CreateFileA_SF(LPCSTR lpFileName,
                             DWORD dwDesiredAccess,
                             DWORD dwShareMode,
                             LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                             DWORD dwCreationDisposition,
                             DWORD dwFlagsAndAttributes,
                             HANDLE hTemplateFile);

#define CreateNamedPipeA CreateNamedPipeA_SF
static HANDLE CreateNamedPipeA_SF(LPCSTR lpName,
                                  DWORD dwOpenMode,
                                  DWORD dwPipeMode,
                                  DWORD nMaxInstances,
                                  DWORD nOutBufferSize,
                                  DWORD nInBufferSize,
                                  DWORD nDefaultTimeOut,
                                  LPSECURITY_ATTRIBUTES lpSecurityAttributes);

#define ConnectNamedPipe ConnectNamedPipe_SF
static BOOL ConnectNamedPipe_SF(HANDLE hNamedPipe, LPOVERLAPPED lpOverlapped);

#define ReadFile ReadFile_SF
static BOOL ReadFile_SF(HANDLE hFile,
                        LPVOID lpBuffer,
                        DWORD nNumberOfBytesToRead,
                        LPDWORD lpNumberOfBytesRead,
                        LPOVERLAPPED lpOverlapped);

#define WriteFile WriteFile_SF
static BOOL WriteFile_SF(HANDLE hFile,
                         LPCVOID lpBuffer,
                         DWORD nNumberOfBytesToRead,
                         LPDWORD lpNumberOfBytesRead,
                         LPOVERLAPPED lpOverlapped);

#endif

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_async_ipc_create_ntv(IN tt_ipc_ntv_t *sys_ipc,
                                    IN OPT const tt_char_t *local_addr,
                                    IN tt_ipc_attr_t *attr,
                                    IN tt_ipc_exit_t *ipc_exit)
{
    tt_evcenter_t *evc;

    evc = tt_evc_current();
    if (evc == NULL) {
        TT_ERROR("not in evc");
        return TT_BAD_PARAM;
    }

    // if local_addr is null, it's a client ipc, and connect aio
    // is added in write q, so EV_IPC_Q_WRITE
    if (!TT_OK(__create_ipc(sys_ipc,
                            evc,
                            ipc_exit,
                            TT_COND(local_addr == NULL,
                                    EV_IPC_Q_WRITE,
                                    EV_IPC_END)))) {
        return TT_FAIL;
    }

    if ((local_addr != NULL) &&
        !TT_OK(__mk_pipe_name(local_addr, &sys_ipc->local_addr))) {
        __destroy_ipc(sys_ipc, TT_FALSE, TT_FALSE);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_async_ipc_destroy_ntv(IN tt_ipc_ntv_t *sys_ipc, IN tt_bool_t immediate)
{
    tt_ipc_t *ipc = TT_CONTAINER(sys_ipc, tt_ipc_t, sys_ipc);

    if (immediate) {
        // if any aio has been submitted to iocp, later it would crash
        __clear_ipc_aio_q(sys_ipc, &sys_ipc->read_q, TT_CANCELLED);
        __clear_ipc_aio_q(sys_ipc, &sys_ipc->write_q, TT_CANCELLED);

        __destroy_ipc(sys_ipc, TT_TRUE, TT_TRUE);
    } else {
        tt_result_t des_rd, des_wr;

#ifdef __IPC_CHECK_EVC
        if (sys_ipc->evc != tt_evc_current()) {
            TT_ERROR("different ipc evc");
            return;
        }
#endif

        // destroying is not as easy as on other platform, as
        // some overlapped io may have been submitted. it has
        // to close pipe and then wait for iocp notifications
        des_rd = __ipc_destroy_read(ipc);
        des_wr = __ipc_destroy_write(ipc);
        if (TT_OK(des_rd) || TT_OK(des_wr)) {
            CloseHandle(sys_ipc->pipe);
            sys_ipc->pipe = INVALID_HANDLE_VALUE;
        }
    }
}

tt_result_t tt_ipc_connect_async_ntv(IN tt_ipc_ntv_t *sys_ipc,
                                     IN const tt_char_t *remote_addr,
                                     IN tt_ipc_on_connect_t on_connect,
                                     IN OPT void *cb_param)
{
    tt_ipc_t *ipc = TT_CONTAINER(sys_ipc, tt_ipc_t, sys_ipc);
    tt_evcenter_t *evc = sys_ipc->evc;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __ipc_connect_t *aio;

#ifdef __IPC_CHECK_EVC
    if (evc != tt_evc_current()) {
        TT_ERROR("different ipc evc");
        return TT_BAD_PARAM;
    }
#endif

    if (sys_ipc->wr_closing) {
        TT_ERROR("ipc is closing");
        return TT_BAD_PARAM;
    }
    if (sys_ipc->connecting) {
        TT_ERROR("ipc is connecting");
        return TT_FAIL;
    }
    if (sys_ipc->connected) {
        TT_ERROR("ipc is already connected");
        return TT_FAIL;
    }
    if (sys_ipc->local_addr != NULL) {
        TT_ERROR("not a client ipc");
        return TT_FAIL;
    }

    // aio
    ev = tt_thread_ev_create(EV_IPC_CONNECT,
                             sizeof(__ipc_connect_t),
                             &__ipc_connect_itf);
    if (ev == NULL) {
        TT_ERROR("fail to allocate destroy aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __ipc_connect_t);

    // init aio
    if (!TT_OK(__mk_pipe_name(remote_addr, &aio->remote_addr))) {
        tt_free(ev);
        return TT_FAIL;
    }

    tt_memset(&aio->ov, 0, sizeof(OVERLAPPED));
    aio->ipc = ipc;
    aio->on_connect = on_connect;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add request
    tt_list_push_tail(&sys_ipc->write_q, &tev->node);

    // 1. CreateFileA can run locally, sending ev and execute CreateFileA
    //    asynchronous is to keep consistent with other platforms
    // 2. sending sys_ipc->aio_ev rather than ev is to give an oppotunity
    //    for caller to tt_async_ipc_destroy_ntv immediately
    TT_ASSERT_IPCAIO(sys_ipc->aio_ev != NULL);
    if ((tt_list_count(&sys_ipc->write_q) == 1) &&
        !TT_OK(tt_evc_sendto_thread(evc, sys_ipc->aio_ev))) {
        TT_ERROR("fail to post ipc connect");

        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }

    sys_ipc->connecting = TT_TRUE;
    return TT_SUCCESS;
}

tt_result_t tt_ipc_accept_async_ntv(IN tt_ipc_ntv_t *sys_ipc,
                                    IN struct tt_ipc_s *new_ipc,
                                    IN OPT struct tt_ipc_attr_s *new_ipc_attr,
                                    IN tt_ipc_exit_t *new_ipc_exit,
                                    IN tt_ipc_on_accept_t on_accept,
                                    IN OPT void *cb_param)
{
    tt_ipc_t *ipc = TT_CONTAINER(sys_ipc, tt_ipc_t, sys_ipc);
    tt_ipc_ntv_t *new_sys_ipc = &new_ipc->sys_ipc;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __ipc_accept_t *aio;

#ifdef __IPC_CHECK_EVC
    if (sys_ipc->evc != tt_evc_current()) {
        TT_ERROR("different ipc evc");
        return TT_BAD_PARAM;
    }
#endif

    if (sys_ipc->rd_closing) {
        return TT_BAD_PARAM;
    }
    if (sys_ipc->local_addr == NULL) {
        TT_ERROR("not a server ipc");
        return TT_FAIL;
    }

    // init new ipc
    if (!TT_OK(__create_ipc(new_sys_ipc,
                            sys_ipc->evc,
                            new_ipc_exit,
                            EV_IPC_Q_READ))) {
        return TT_FAIL;
    }

    if (new_ipc_attr != NULL) {
        memcpy(&new_ipc->attr, new_ipc_attr, sizeof(tt_ipc_attr_t));
    } else {
        tt_ipc_attr_default(&new_ipc->attr);
    }

    new_ipc->listening = TT_FALSE;

    // aio
    ev = tt_thread_ev_create(EV_IPC_ACCEPT, sizeof(__ipc_accept_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate destroy aio");

        __destroy_ipc(&new_ipc->sys_ipc, TT_FALSE, TT_FALSE);
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __ipc_accept_t);

    // init aio
    tt_memset(&aio->ov, 0, sizeof(OVERLAPPED));
    aio->listening_ipc = ipc;
    aio->new_ipc = new_ipc;
    aio->on_accept = on_accept;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add request
    // note aio should be added to new_sys_ipc'q rather than
    // sys_ipc as CompletionKey is binded to the pipe handle
    // of new_sys_ipc in __create_server_pipe() called by
    // __do_ipc_accept_io()
    tt_list_push_tail(&new_sys_ipc->read_q, &tev->node);

    // sending sys_ipc->aio_ev rather than ev is to give an oppotunity
    // for caller to tt_async_ipc_destroy_ntv immediately
    TT_ASSERT_IPCAIO(new_sys_ipc->aio_ev != NULL);
    if ((tt_list_count(&new_sys_ipc->read_q) == 1) &&
        !TT_OK(tt_evc_sendto_thread(new_sys_ipc->evc, new_sys_ipc->aio_ev))) {
        TT_ERROR("fail to post ipc accept");

        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        __destroy_ipc(new_sys_ipc, TT_FALSE, TT_FALSE);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_ipc_send_async_ntv(IN tt_ipc_ntv_t *sys_ipc,
                                  IN tt_ev_t *data_ev,
                                  IN tt_ipc_on_send_t on_send,
                                  IN OPT void *cb_param)
{
    tt_ipc_t *ipc = TT_CONTAINER(sys_ipc, tt_ipc_t, sys_ipc);
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __ipc_send_t *aio;

#ifdef __IPC_CHECK_EVC
    if (sys_ipc->evc != tt_evc_current()) {
        TT_ERROR("different ipc evc");
        return TT_BAD_PARAM;
    }
#endif

    if (sys_ipc->wr_closing) {
        TT_ERROR("ipc is closing");
        return TT_BAD_PARAM;
    }
    if (!sys_ipc->connected) {
        TT_ERROR("ipc is not connected");
        return TT_FAIL;
    }

    // aio
    ev =
        tt_thread_ev_create(EV_IPC_SEND, sizeof(__ipc_send_t), &__ipc_send_itf);
    if (ev == NULL) {
        TT_ERROR("fail to allocate destroy aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __ipc_send_t);

    // init aio
    tt_memset(&aio->ov, 0, sizeof(OVERLAPPED));
    aio->ipc = ipc;
    aio->data_ev = data_ev;
    aio->on_send = on_send;
    aio->send_len = 0;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add request
    tt_list_push_tail(&sys_ipc->write_q, &tev->node);

    // start iocp if it's head aio
    if ((tt_list_count(&sys_ipc->write_q) == 1) &&
        (__do_ipc_send_io(aio, 0) != TT_PROCEEDING)) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_ipc_recv_async_ntv(IN tt_ipc_ntv_t *sys_ipc,
                                  IN tt_ipc_on_recv_t on_recv,
                                  IN OPT void *cb_param)
{
    tt_ipc_t *ipc = TT_CONTAINER(sys_ipc, tt_ipc_t, sys_ipc);
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __ipc_recv_t *aio;

#ifdef __IPC_CHECK_EVC
    if (sys_ipc->evc != tt_evc_current()) {
        TT_ERROR("different socket evc");
        return TT_BAD_PARAM;
    }
#endif

    if (sys_ipc->rd_closing) {
        TT_ERROR("ipc is closing");
        return TT_BAD_PARAM;
    }
    if (!sys_ipc->connected) {
        TT_ERROR("ipc is not connected");
        return TT_FAIL;
    }

    // aio
    ev =
        tt_thread_ev_create(EV_IPC_RECV, sizeof(__ipc_recv_t), &__ipc_recv_itf);
    if (ev == NULL) {
        TT_ERROR("fail to allocate destroy aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __ipc_recv_t);

    // init aio
    tt_memset(&aio->ov, 0, sizeof(OVERLAPPED));
    aio->ipc = ipc;
    aio->data_ev = NULL;
    aio->data_ev_num = 0;
    aio->on_recv = on_recv;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add request
    tt_list_push_tail(&sys_ipc->read_q, &tev->node);

    // start iocp if it's head aio
    if ((tt_list_count(&sys_ipc->read_q) == 1) &&
        (__do_ipc_recv_io(aio, 0) != TT_PROCEEDING)) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_ipc_tev_handler(IN struct tt_evpoller_s *evp, IN tt_ev_t *ev)
{
    tt_thread_ev_t *tev = TT_EV_HDR(ev, tt_thread_ev_t);
    tt_lnode_t *node = &tev->node;

    TT_ASSERT(ev != NULL);
    TT_ASSERT(TT_EV_RANGE(ev->ev_id) == TT_EV_RANGE_INTERNAL_IPC);

    switch (ev->ev_id) {
        case EV_IPC_Q_READ: {
            __ipc_q_t *aio = TT_EV_DATA(ev, __ipc_q_t);
            tt_ipc_ntv_t *sys_ipc = &aio->ipc->sys_ipc;

            __do_ipc_aio_q(sys_ipc, &sys_ipc->read_q, TT_SUCCESS, 0);
            // no need to destroy ev
        } break;
        case EV_IPC_Q_WRITE: {
            __ipc_q_t *aio = TT_EV_DATA(ev, __ipc_q_t);
            tt_ipc_ntv_t *sys_ipc = &aio->ipc->sys_ipc;

            __do_ipc_aio_q(sys_ipc, &sys_ipc->write_q, TT_SUCCESS, 0);
            // no need to destroy ev
        } break;

        default: {
            TT_FATAL("unknown event: %x", ev->ev_id);
            tt_ev_destroy(ev);
        } break;
    }
    return TT_SUCCESS;
}

tt_result_t tt_ipc_iocp_handler(IN tt_ipc_t *ipc,
                                IN DWORD NumberOfBytes,
                                IN OVERLAPPED *Overlapped,
                                IN tt_result_t iocp_result)
{
    tt_ipc_ntv_t *sys_ipc = &ipc->sys_ipc;
    // Overlapped is always the head of some aio
    tt_thread_ev_t *tev = TT_TEV_OF(Overlapped);
    tt_lnode_t *node = &tev->node;

    TT_ASSERT_IPCAIO(iocp_result != TT_PROCEEDING);
    TT_ASSERT_IPCAIO((node->lst == &sys_ipc->read_q) ||
                     (node->lst == &sys_ipc->write_q));
    TT_ASSERT_IPCAIO(node == tt_list_head(node->lst));

    __do_ipc_aio_q(sys_ipc, node->lst, iocp_result, NumberOfBytes);
    return TT_SUCCESS;
}

tt_result_t __mk_pipe_name(IN const tt_char_t *raw_name,
                           OUT tt_char_t **pipe_name)
{
    tt_char_t *name;
    tt_u32_t name_len;
    tt_u32_t raw_name_len = (tt_u32_t)tt_strlen(raw_name) + 1;

    name_len = (tt_u32_t)sizeof(tt_s_pipe_prefix) - 1 + raw_name_len;
    if (name_len > 256) {
        TT_WARN("windows ipc name can only be at most 256 chars");
    }

    name = (tt_char_t *)tt_malloc(name_len);
    if (name == NULL) {
        TT_ERROR("no mem for ipc pipe name");
        return TT_FAIL;
    }
    tt_memcpy(name, tt_s_pipe_prefix, sizeof(tt_s_pipe_prefix) - 1);
    tt_memcpy(name + sizeof(tt_s_pipe_prefix) - 1, raw_name, raw_name_len);

    *pipe_name = name;
    return TT_SUCCESS;
}

tt_result_t __create_ipc(IN tt_ipc_ntv_t *sys_ipc,
                         IN tt_evcenter_t *evc,
                         IN tt_ipc_exit_t *ipc_exit,
                         IN tt_u32_t aio_ev_id)
{
    tt_ev_t *aio_ev = NULL;

    tt_memset(sys_ipc, 0, sizeof(tt_ipc_ntv_t));

    // must set to invalid
    sys_ipc->pipe = INVALID_HANDLE_VALUE;
    sys_ipc->local_addr = NULL;

    sys_ipc->evc = evc;
    sys_ipc->ev_mark = TT_EV_RANGE_INTERNAL_IPC;

    if (aio_ev_id != EV_IPC_END) {
        __ipc_q_t *aio;

        aio_ev = tt_thread_ev_create(aio_ev_id, sizeof(__ipc_q_t), NULL);
        if (aio_ev == NULL) {
            TT_ERROR("no mem for ipc aio ev");
            return TT_FAIL;
        }
        aio = TT_EV_DATA(aio_ev, __ipc_q_t);
        aio->ipc = TT_CONTAINER(sys_ipc, tt_ipc_t, sys_ipc);
    }
    sys_ipc->aio_ev = aio_ev;

    if (ipc_exit != NULL) {
        sys_ipc->on_destroy = ipc_exit->on_destroy;
        sys_ipc->on_destroy_param = ipc_exit->cb_param;
    } else {
        sys_ipc->on_destroy = NULL;
        sys_ipc->on_destroy_param = NULL;
    }

    tt_list_init(&sys_ipc->read_q);
    tt_list_init(&sys_ipc->write_q);
    tt_buf_init(&sys_ipc->recv_buf, NULL);

    return TT_SUCCESS;
}

void __destroy_ipc(IN tt_ipc_ntv_t *sys_ipc,
                   IN tt_bool_t do_cb,
                   IN tt_bool_t check_free)
{
    tt_ipc_on_destroy_t on_destroy = (tt_ipc_on_destroy_t)sys_ipc->on_destroy;
    tt_ipc_t *ipc = TT_CONTAINER(sys_ipc, tt_ipc_t, sys_ipc);
    tt_bool_t from_alloc = ipc->attr.from_alloc;

    if (sys_ipc->pipe != INVALID_HANDLE_VALUE) {
        CloseHandle(sys_ipc->pipe);
    }

    if (sys_ipc->local_addr != NULL) {
        tt_free(sys_ipc->local_addr);
    }

    if (sys_ipc->aio_ev != NULL) {
        tt_thread_ev_t *tev = TT_EV_HDR(sys_ipc->aio_ev, tt_thread_ev_t);

        // here the aio_ev may be removing from ev q of some evc without
        // holding any lock so the aio_ev must in the local evc
        TT_ASSERT((tev->node.lst == NULL) ||
                  (tev->node.lst == &tt_evc_current()->ev_q));
        tt_list_remove(&tev->node);
        tt_ev_destroy(sys_ipc->aio_ev);
    }

    if (!tt_list_empty(&sys_ipc->read_q)) {
        TT_WARN("still aio in ipc read q");
    }
    if (!tt_list_empty(&sys_ipc->write_q)) {
        TT_WARN("still aio in ipc write q");
    }

    tt_buf_destroy(&sys_ipc->recv_buf);

    if (do_cb) {
        on_destroy(ipc, sys_ipc->on_destroy_param);
    }
    // do not access aio->ipc from now on as application may freed aio->ipc

    if (check_free && from_alloc) {
        tt_free(ipc);
    }
}

tt_result_t __create_server_pipe(IN tt_ipc_ntv_t *sys_ipc,
                                 IN const tt_char_t *local_addr,
                                 IN tt_ipc_attr_t *attr)
{
    DWORD dwOpenMode, dwPipeMode;
    HANDLE pipe;

    // create server ipc
    TT_ASSERT_IPCAIO(local_addr != NULL);
    dwOpenMode = PIPE_ACCESS_DUPLEX | /*FILE_FLAG_WRITE_THROUGH |*/
                 FILE_FLAG_OVERLAPPED;
    dwPipeMode = PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | /*PIPE_NOWAIT |*/
                 PIPE_REJECT_REMOTE_CLIENTS;
    pipe = CreateNamedPipeA(local_addr,
                            dwOpenMode,
                            dwPipeMode,
                            PIPE_UNLIMITED_INSTANCES,
                            attr->recv_buf_size,
                            attr->recv_buf_size,
                            0,
                            NULL);
    if (pipe == INVALID_HANDLE_VALUE) {
        TT_ERROR_NTV("fail to create ipc server pipe");
        return TT_FAIL;
    }
    TT_INFO("listening on pipe[%s]", local_addr);

    if (CreateIoCompletionPort(pipe,
                               sys_ipc->evc->sys_evc.iocp,
                               (ULONG_PTR)(&sys_ipc->ev_mark),
                               0) == NULL) {
        TT_ERROR_NTV("fail to associate server pipe with iocp");

        CloseHandle(pipe);
        return TT_FAIL;
    }

    // data io space
    if (!TT_OK(tt_buf_reserve(&sys_ipc->recv_buf, attr->recv_buf_size))) {
        CloseHandle(pipe);
        return TT_FAIL;
    }

    sys_ipc->pipe = pipe;
    return TT_SUCCESS;
}

tt_result_t __create_client_pipe(IN tt_ipc_ntv_t *sys_ipc,
                                 IN const tt_char_t *remote_addr,
                                 IN tt_ipc_attr_t *attr)
{
    HANDLE pipe;

    // create client ipc, defautlt pipe has:
    //  - byte-mode
    //  - blocking wait
    //  - overlapped disabled (to change)
    //  - write through disabled (to change??)
    TT_ASSERT_IPCAIO(remote_addr != NULL);
    pipe = CreateFileA(remote_addr,
                       GENERIC_READ | GENERIC_WRITE,
                       0,
                       NULL,
                       OPEN_EXISTING,
                       FILE_FLAG_OVERLAPPED /*| FILE_FLAG_WRITE_THROUGH*/,
                       NULL);
    if (pipe == INVALID_HANDLE_VALUE) {
        TT_ERROR_NTV("fail to connect ipc pipe[%s]", remote_addr);
        return TT_FAIL;
    }

    if (CreateIoCompletionPort(pipe,
                               sys_ipc->evc->sys_evc.iocp,
                               (ULONG_PTR)(&sys_ipc->ev_mark),
                               0) == NULL) {
        TT_ERROR_NTV("fail to associate client pipe with iocp");

        CloseHandle(pipe);
        return TT_FAIL;
    }

    if (!TT_OK(tt_buf_reserve(&sys_ipc->recv_buf, attr->recv_buf_size))) {
        CloseHandle(pipe);
        return TT_FAIL;
    }

    sys_ipc->pipe = pipe;
    return TT_SUCCESS;
}

tt_bool_t __do_ipc_connect(IN __ipc_connect_t *aio, IN tt_result_t iocp_result)
{
    tt_ipc_t *ipc = aio->ipc;
    tt_ipc_ntv_t *sys_ipc = &ipc->sys_ipc;

    sys_ipc->connecting = TT_FALSE;

    if (TT_OK(iocp_result)) {
        aio->result =
            __create_client_pipe(sys_ipc, aio->remote_addr, &ipc->attr);
    } else {
        aio->result = iocp_result;
    }

    TT_ASSERT_IPCAIO(aio->result != TT_PROCEEDING);
    if (TT_OK(aio->result)) {
        sys_ipc->connected = TT_TRUE;

        // there won't be another accept/connect aio as sys_ipc->connected is
        // set
        TT_ASSERT_IPCAIO(sys_ipc->aio_ev != NULL);
        tt_ev_destroy(sys_ipc->aio_ev);
        sys_ipc->aio_ev = NULL;
    }

    __do_ipc_connect_cb(aio);
    return TT_TRUE;
}

void __do_ipc_connect_cb(IN __ipc_connect_t *aio)
{
    tt_ipc_aioctx_t aioctx;

    TT_ASSERT(aio->result != TT_PROCEEDING);

    // callback
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_connect(aio->ipc, aio->remote_addr, &aioctx);
}

void __ipc_connect_on_destroy(IN struct tt_ev_s *ev)
{
    __ipc_connect_t *aio = TT_EV_DATA(ev, __ipc_connect_t);
    if (aio->remote_addr != NULL) {
        tt_free(aio->remote_addr);
    }
}

tt_bool_t __do_ipc_accept(IN __ipc_accept_t *aio, IN tt_result_t iocp_result)
{
    tt_ipc_t *new_ipc = aio->new_ipc;
    tt_ipc_ntv_t *new_sys_ipc = &new_ipc->sys_ipc;

    if (TT_OK(iocp_result)) {
        aio->result = TT_SUCCESS;
    } else if (iocp_result == TT_PROCEEDING) {
        aio->result = __do_ipc_accept_io(aio);
        if (aio->result == TT_PROCEEDING) {
            return TT_FALSE;
        }
    } else {
        aio->result = iocp_result;
    }

    if (!TT_OK(aio->result)) {
        if (new_sys_ipc->pipe != INVALID_HANDLE_VALUE) {
            CloseHandle(new_sys_ipc->pipe);
            new_sys_ipc->pipe = INVALID_HANDLE_VALUE;
        }

        if (new_sys_ipc->local_addr != NULL) {
            tt_free(new_sys_ipc->local_addr);
        }

        TT_ASSERT_IPCAIO(tt_list_empty(&new_sys_ipc->read_q));
        TT_ASSERT_IPCAIO(tt_list_empty(&new_sys_ipc->write_q));
        tt_buf_destroy(&new_sys_ipc->recv_buf);
    }

    __do_ipc_accept_cb(aio);
    return TT_SUCCESS;
}

tt_result_t __do_ipc_accept_io(IN __ipc_accept_t *aio)
{
    tt_ipc_t *new_ipc = aio->new_ipc;
    tt_ipc_ntv_t *new_sys_ipc = &new_ipc->sys_ipc;

    // create server ipc
    if (!TT_OK(__create_server_pipe(new_sys_ipc,
                                    aio->listening_ipc->sys_ipc.local_addr,
                                    &new_ipc->attr))) {
        return TT_FAIL;
    }

    // accept client
    if (!ConnectNamedPipe(new_sys_ipc->pipe, &aio->ov) &&
        (GetLastError() != ERROR_IO_PENDING)) {
        return TT_FAIL;
    }

    return TT_PROCEEDING;
}

void __do_ipc_accept_cb(IN __ipc_accept_t *aio)
{
    tt_ipc_aioctx_t aioctx;
    tt_ipc_t *new_ipc = aio->new_ipc;
    tt_bool_t from_alloc = new_ipc->attr.from_alloc;

    if (TT_OK(aio->result)) {
        tt_ipc_ntv_t *new_sys_ipc = &new_ipc->sys_ipc;

        new_ipc->sys_ipc.connected = TT_TRUE;

        // there won't be another accept/connect aio as sys_ipc->connected is
        // set
        TT_ASSERT_IPCAIO(new_sys_ipc->aio_ev != NULL);
        tt_ev_destroy(new_sys_ipc->aio_ev);
        new_sys_ipc->aio_ev = NULL;
    }

    // callback
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_accept(aio->listening_ipc, new_ipc, &aioctx);

    if (!TT_OK(aioctx.result) && from_alloc) {
        tt_free(new_ipc);
    }
}

tt_bool_t __do_ipc_send(IN __ipc_send_t *aio,
                        IN tt_result_t iocp_result,
                        IN tt_u32_t send_num)
{
    // TT_SUCCESS: iocp indicates bytes are ioed
    // TT_PROCEEDING: new aio
    if (TT_OK(iocp_result) || (iocp_result == TT_PROCEEDING)) {
        aio->result = __do_ipc_send_io(aio, send_num);
    } else {
        aio->result = iocp_result;
    }

    if (aio->result != TT_PROCEEDING) {
        __do_ipc_send_cb(aio);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_result_t __do_ipc_send_io(IN __ipc_send_t *aio, IN tt_u32_t send_num)
{
    tt_ipc_ntv_t *sys_ipc = &aio->ipc->sys_ipc;
    tt_ev_t *data_ev = aio->data_ev;
    tt_u32_t size = sizeof(tt_ev_t) + data_ev->hdr_size + data_ev->data_size;

    tt_u8_t *lpBuffer;
    DWORD nNumberOfBytesToWrite;
    DWORD NumberOfBytesWritten = 0;
    DWORD dwError;

    // update
    aio->send_len += send_num;
    TT_ASSERT(aio->send_len <= size);
    if (aio->send_len == size) {
        return TT_SUCCESS;
    }

    if (sys_ipc->pipe == INVALID_HANDLE_VALUE) {
        // may be destroyed in tt_async_ipc_destroy()
        return TT_END;
    }

    lpBuffer = TT_PTR_INC(tt_u8_t, data_ev, aio->send_len);
    nNumberOfBytesToWrite = size - aio->send_len;
    if (!WriteFile(sys_ipc->pipe,
                   lpBuffer,
                   nNumberOfBytesToWrite,
                   &NumberOfBytesWritten,
                   &aio->ov) &&
        ((dwError = GetLastError()) != ERROR_IO_PENDING)) {
        TT_ERROR_NTV("ipc send fail");

        if (dwError == ERROR_BROKEN_PIPE) {
            return TT_END;
        } else {
            return TT_FAIL;
        }
    }

    return TT_PROCEEDING;
}

void __do_ipc_send_cb(IN __ipc_send_t *aio)
{
    tt_ipc_aioctx_t aioctx;

    // callback
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_send(aio->ipc, aio->data_ev, &aioctx);
}

void __ipc_send_on_destroy(IN struct tt_ev_s *ev)
{
    __ipc_send_t *aio = TT_EV_DATA(ev, __ipc_send_t);
    if (aio->data_ev != NULL) {
        tt_ev_destroy(aio->data_ev);
    }
}

tt_bool_t __do_ipc_recv(IN __ipc_recv_t *aio,
                        IN tt_result_t iocp_result,
                        IN tt_u32_t recv_num)
{
    // TT_SUCCESS: iocp indicates bytes are ioed
    // TT_PROCEEDING: new aio
    if (TT_OK(iocp_result) || (iocp_result == TT_PROCEEDING)) {
        aio->result = __do_ipc_recv_io(aio, recv_num);
    } else {
        aio->result = iocp_result;
    }
    TT_ASSERT_IPCAIO(!TT_OK(aio->result) || (aio->data_ev != NULL));

    if (aio->result != TT_PROCEEDING) {
        __do_ipc_recv_cb(aio);
        // all ipc events has been shown to application, the recv buffer
        // can be refined now
        tt_buf_refine(&aio->ipc->sys_ipc.recv_buf);

        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_result_t __do_ipc_recv_io(IN __ipc_recv_t *aio, IN tt_u32_t recv_num)
{
    tt_ipc_ntv_t *sys_ipc = &aio->ipc->sys_ipc;
    tt_ev_t *data_ev = NULL;
    tt_buf_t *recv_buf = &sys_ipc->recv_buf;
    tt_result_t parse_result;

    DWORD nNumberOfBytesToRead;
    DWORD NumberOfBytesRead = 0;
    DWORD dwError;

    TT_ASSERT_IPCAIO(aio->data_ev == NULL);

    // update recv stream and try parsing
    TT_ASSERT_IPCAIO(recv_num <= TT_BUF_WLEN(recv_buf));
    tt_buf_inc_wp(recv_buf, recv_num);
    parse_result = __ipc_ev_parse(recv_buf, aio);
    if (TT_OK(parse_result)) {
        // return although there may be more data which is not read out of
        // ipc socket yet
        return TT_SUCCESS;
    } else if (parse_result != TT_PROCEEDING) {
        TT_ERROR("ipc msg parsing failed, to close ipc");
        return TT_END;
    }

    if (sys_ipc->pipe == INVALID_HANDLE_VALUE) {
        // may be destroyed in tt_async_ipc_destroy()
        return TT_END;
    }

    nNumberOfBytesToRead = TT_BUF_WLEN(recv_buf);
    if (nNumberOfBytesToRead == 0) {
        // - no event can be parsed out and the ipc buffer is alreay full
        // - even discarding all content in recv_buf is not helpful,
        //   as msg boundary information is lost

        TT_ERROR("ipc buf is full");
        return TT_END;
    }

    if (!ReadFile(sys_ipc->pipe,
                  TT_BUF_WPOS(recv_buf),
                  nNumberOfBytesToRead,
                  &NumberOfBytesRead,
                  &aio->ov) &&
        ((dwError = GetLastError()) != ERROR_IO_PENDING)) {
        TT_ERROR_NTV("ipc recv fail");

        if (dwError == ERROR_BROKEN_PIPE) {
            return TT_END;
        } else {
            return TT_FAIL;
        }
    }

    return TT_PROCEEDING;
}

tt_result_t __ipc_ev_parse(IN tt_buf_t *recv_buf, IN __ipc_recv_t *aio)
{
    tt_ev_t **data_ev;
    tt_u32_t data_ev_num = 0;
    tt_u32_t data_ev_size;
    tt_u32_t rd_pos, wr_pos;

    TT_ASSERT(aio->data_ev == NULL);
    TT_ASSERT(aio->data_ev_num == 0);

    // this function should read all ipc msg in recv_buf
    // so as to avoid refining recv_buf which introduces
    // redundant memory copy

    // this function return
    //  - TT_SUCCESS, if some ipc msg are parsed
    //  - TT_PROCEEDING, if no ipc msg are parsed
    //  - TT_FAIL, if error occurred, caller must close ipc

    tt_buf_backup_rwp(recv_buf, &rd_pos, &wr_pos);

    // calculate how many ipc msg are in the buffer
    while (1) {
        tt_ev_t ev;
        tt_u32_t n;

        TT_DO_G(calc_done,
                tt_buf_get(recv_buf, (tt_u8_t *)&ev, sizeof(tt_ev_t)));

        n = ev.hdr_size + ev.data_size;
        TT_DO_G(calc_done, tt_buf_inc_rp(recv_buf, n));

        ++data_ev_num;
    }
calc_done:

    tt_buf_restore_rwp(recv_buf, &rd_pos, &wr_pos);

    if (data_ev_num == 0) {
        return TT_PROCEEDING;
    }
    data_ev_size = sizeof(tt_ev_t *) * data_ev_num;
    data_ev_num = 0;

    data_ev = (tt_ev_t **)tt_malloc(data_ev_size);
    if (data_ev == NULL) {
        TT_ERROR("no mem for ipc tev array");
        return TT_FAIL;
    }
    tt_memset(data_ev, 0, data_ev_size);

    // extract all ipc msg
    while (1) {
        tt_ev_t *ev = (tt_ev_t *)TT_BUF_RPOS(recv_buf);
        tt_u32_t n;

        tt_buf_backup_rwp(recv_buf, &rd_pos, &wr_pos);

        // ev
        TT_DO_G(extract_done, tt_buf_inc_rp(recv_buf, sizeof(tt_ev_t)));

        // ev data
        n = ev->hdr_size + ev->data_size;
        if (TT_BUF_RLEN(recv_buf) < n) {
            tt_buf_restore_rwp(recv_buf, &rd_pos, &wr_pos);
            break;
        }
        tt_buf_inc_rp(recv_buf, n);

        data_ev[data_ev_num++] = ev;
    }
extract_done:

    if (data_ev_num == 0) {
        tt_free(data_ev);
        return TT_PROCEEDING;
    }

    aio->data_ev = data_ev;
    aio->data_ev_num = data_ev_num;
    return TT_SUCCESS;
}

void __do_ipc_recv_cb(IN __ipc_recv_t *aio)
{
    tt_ipc_aioctx_t aioctx;

    // callback
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_recv(aio->ipc, &aioctx, aio->data_ev, aio->data_ev_num);
}

void __ipc_recv_on_destroy(IN struct tt_ev_s *ev)
{
    __ipc_recv_t *aio = TT_EV_DATA(ev, __ipc_recv_t);
    if (aio->data_ev != NULL) {
        tt_free(aio->data_ev);
    }
}

void __do_ipc_aio_q(IN tt_ipc_ntv_t *sys_ipc,
                    IN tt_list_t *aio_q,
                    IN tt_result_t iocp_result,
                    IN DWORD NumberOfBytes)
{
    tt_lnode_t *node;

    while ((node = tt_list_head(aio_q)) != NULL) {
        tt_thread_ev_t *tev = TT_CONTAINER(node, tt_thread_ev_t, node);
        tt_ev_t *ev = TT_EV_OF(tev);
        tt_bool_t aio_done = TT_TRUE;

        switch (ev->ev_id) {
            case EV_IPC_ACCEPT: {
                __ipc_accept_t *aio = TT_EV_DATA(ev, __ipc_accept_t);

                // it should be the head aio in read q
                TT_ASSERT_IPCAIO(&tev->node ==
                                 tt_list_head(&aio->new_ipc->sys_ipc.read_q));

                aio_done = __do_ipc_accept(aio, iocp_result);
            } break;
            case EV_IPC_CONNECT: {
                __ipc_connect_t *aio = TT_EV_DATA(ev, __ipc_connect_t);

                // it should be the head aio in write q
                TT_ASSERT_IPCAIO(&tev->node ==
                                 tt_list_head(&aio->ipc->sys_ipc.write_q));

                aio_done = __do_ipc_connect(aio, iocp_result);
            } break;
            case EV_IPC_SEND: {
                __ipc_send_t *aio = TT_EV_DATA(ev, __ipc_send_t);

                TT_ASSERT_IPCAIO(&tev->node ==
                                 tt_list_head(&aio->ipc->sys_ipc.write_q));

                aio_done = __do_ipc_send(aio, iocp_result, NumberOfBytes);

                // special handling, once it fails to send an ipc event, the ipc
                // should be destroyed as the event may be partially sent and
                // the
                // remote peer will always fail to decode following ipc events
                if (aio_done && !TT_OK(aio->result)) {
                    tt_async_ipc_destroy_ntv(&aio->ipc->sys_ipc, TT_FALSE);
                }
            } break;
            case EV_IPC_RECV: {
                __ipc_recv_t *aio = TT_EV_DATA(ev, __ipc_recv_t);

                TT_ASSERT_IPCAIO(&tev->node ==
                                 tt_list_head(&aio->ipc->sys_ipc.read_q));

                aio_done = __do_ipc_recv(aio, iocp_result, NumberOfBytes);

                // special handling, once it fails to recv an ipc event, the ipc
                // should be destroyed as it may fail to decode all following
                // ipc events
                if (aio_done && !TT_OK(aio->result)) {
                    tt_async_ipc_destroy_ntv(&aio->ipc->sys_ipc, TT_FALSE);
                }
            } break;
            case EV_IPC_DESTROY_RD: {
                __ipc_destroy_t *aio = TT_EV_DATA(ev, __ipc_destroy_t);

                TT_ASSERT_IPCAIO(&tev->node ==
                                 tt_list_head(&aio->ipc->sys_ipc.read_q));
                TT_ASSERT_IPCAIO(tt_list_count(aio_q) == 1);

                tt_list_remove(node);
                __do_ipc_destroy_read(aio);
                tt_ev_destroy(ev);
                return;
            } break;
            case EV_IPC_DESTROY_WR: {
                __ipc_destroy_t *aio = TT_EV_DATA(ev, __ipc_destroy_t);

                TT_ASSERT_IPCAIO(&tev->node ==
                                 tt_list_head(&aio->ipc->sys_ipc.write_q));
                TT_ASSERT_IPCAIO(tt_list_count(aio_q) == 1);

                tt_list_remove(node);
                __do_ipc_destroy_write(aio);
                tt_ev_destroy(ev);
                return;
            } break;

            default: {
                TT_ERROR("unknown aio: %x", ev->ev_id);
            } break;
        }

        if (!aio_done) {
            return;
        }
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);

        // pass to next aio
        NumberOfBytes = 0;
        iocp_result = TT_PROCEEDING;
    }
}

void __clear_ipc_aio_q(IN tt_ipc_ntv_t *sys_ipc,
                       IN tt_list_t *aio_q,
                       IN tt_result_t aio_result)
{
    tt_lnode_t *node;

    while ((node = tt_list_head(aio_q)) != NULL) {
        tt_thread_ev_t *tev = TT_CONTAINER(node, tt_thread_ev_t, node);
        tt_ev_t *ev = TT_EV_OF(tev);

        switch (ev->ev_id) {
            case EV_IPC_ACCEPT: {
                __ipc_accept_t *aio = TT_EV_DATA(ev, __ipc_accept_t);

                // it should be the head aio in read q
                TT_ASSERT_IPCAIO(&tev->node ==
                                 tt_list_head(&aio->new_ipc->sys_ipc.read_q));

                aio->result = aio_result;
                __do_ipc_accept_cb(aio);
            } break;
            case EV_IPC_CONNECT: {
                __ipc_connect_t *aio = TT_EV_DATA(ev, __ipc_connect_t);

                // it should be the head aio in write q
                TT_ASSERT_IPCAIO(&tev->node ==
                                 tt_list_head(&aio->ipc->sys_ipc.write_q));

                aio->result = aio_result;
                __do_ipc_connect_cb(aio);
            } break;
            case EV_IPC_SEND: {
                __ipc_send_t *aio = TT_EV_DATA(ev, __ipc_send_t);

                TT_ASSERT_IPCAIO(&tev->node ==
                                 tt_list_head(&aio->ipc->sys_ipc.write_q));

                aio->result = aio_result;
                __do_ipc_send_cb(aio);
            } break;
            case EV_IPC_RECV: {
                __ipc_recv_t *aio = TT_EV_DATA(ev, __ipc_recv_t);

                TT_ASSERT_IPCAIO(&tev->node ==
                                 tt_list_head(&aio->ipc->sys_ipc.read_q));

                aio->result = aio_result;
                __do_ipc_recv_cb(aio);
            } break;
            case EV_IPC_DESTROY_RD: {
                __ipc_destroy_t *aio = TT_EV_DATA(ev, __ipc_destroy_t);

                TT_ASSERT_IPCAIO(&tev->node ==
                                 tt_list_head(&aio->ipc->sys_ipc.read_q));
            } break;
            case EV_IPC_DESTROY_WR: {
                __ipc_destroy_t *aio = TT_EV_DATA(ev, __ipc_destroy_t);

                TT_ASSERT_IPCAIO(&tev->node ==
                                 tt_list_head(&aio->ipc->sys_ipc.write_q));
            } break;

            default: {
                TT_ERROR("unknown aio: %x", ev->ev_id);
            } break;
        }

        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        // next
    }
}

tt_result_t __ipc_destroy_read(IN tt_ipc_t *ipc)
{
    tt_ipc_ntv_t *sys_ipc = &ipc->sys_ipc;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __ipc_destroy_t *aio;

    if (sys_ipc->rd_closing) {
        return TT_FAIL;
    }

    // aio
    ev = tt_thread_ev_create(EV_IPC_DESTROY_RD,
                             sizeof(OVERLAPPED) + sizeof(__ipc_destroy_t),
                             NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate destroy aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __ipc_destroy_t);

    // init aio
    tt_memset(&aio->ov, 0, sizeof(OVERLAPPED));
    aio->ipc = ipc;

    // add request
    tt_list_push_tail(&sys_ipc->read_q, &tev->node);

    // start iocp if it's head aio
    if ((tt_list_count(&sys_ipc->read_q) == 1) &&
        !PostQueuedCompletionStatus(sys_ipc->evc->sys_evc.iocp,
                                    0,
                                    (ULONG_PTR)&sys_ipc->ev_mark,
                                    &aio->ov)) {
        TT_ERROR_NTV("fail to post ipc destroy rd");

        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }

    sys_ipc->rd_closing = TT_TRUE;
    return TT_SUCCESS;
}

void __do_ipc_destroy_read(IN __ipc_destroy_t *aio)
{
    tt_ipc_ntv_t *sys_ipc = &aio->ipc->sys_ipc;

    // all read aio must have been done
    TT_ASSERT_IPCAIO(tt_list_empty(&sys_ipc->read_q));

    TT_ASSERT_IPCAIO(!sys_ipc->rd_closed);
    sys_ipc->rd_closed = TT_TRUE;

    if (sys_ipc->wr_closed) {
        TT_ASSERT_IPCAIO(tt_list_empty(&sys_ipc->write_q));
        TT_ASSERT_IPCAIO(sys_ipc->pipe == INVALID_HANDLE_VALUE);
        __destroy_ipc(sys_ipc, TT_TRUE, TT_TRUE);
    }
}

tt_result_t __ipc_destroy_write(IN tt_ipc_t *ipc)
{
    tt_ipc_ntv_t *sys_ipc = &ipc->sys_ipc;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __ipc_destroy_t *aio;

    if (sys_ipc->wr_closing) {
        return TT_FAIL;
    }

    // aio
    ev = tt_thread_ev_create(EV_IPC_DESTROY_WR,
                             sizeof(OVERLAPPED) + sizeof(__ipc_destroy_t),
                             NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate destroy aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __ipc_destroy_t);

    // init aio
    tt_memset(&aio->ov, 0, sizeof(OVERLAPPED));
    aio->ipc = ipc;

    // add request
    tt_list_push_tail(&sys_ipc->write_q, &tev->node);

    // start iocp if it's head aio
    if ((tt_list_count(&sys_ipc->write_q) == 1) &&
        !PostQueuedCompletionStatus(sys_ipc->evc->sys_evc.iocp,
                                    0,
                                    (ULONG_PTR)&sys_ipc->ev_mark,
                                    &aio->ov)) {
        TT_ERROR_NTV("fail to post ipc destroy wr");

        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }

    sys_ipc->wr_closing = TT_TRUE;
    return TT_SUCCESS;
}

void __do_ipc_destroy_write(IN __ipc_destroy_t *aio)
{
    tt_ipc_ntv_t *sys_ipc = &aio->ipc->sys_ipc;

    // all write aio must have been done
    TT_ASSERT_IPCAIO(tt_list_empty(&sys_ipc->write_q));

    TT_ASSERT_IPCAIO(!sys_ipc->wr_closed);
    sys_ipc->wr_closed = TT_TRUE;

    if (sys_ipc->rd_closed) {
        TT_ASSERT_IPCAIO(tt_list_empty(&sys_ipc->read_q));
        TT_ASSERT_IPCAIO(sys_ipc->pipe == INVALID_HANDLE_VALUE);
        __destroy_ipc(sys_ipc, TT_TRUE, TT_TRUE);
    }
}

// ========================================
// simulate failure
// ========================================

#ifdef __SIMULATE_IPC_AIO_FAIL

tt_result_t ipc_evc_send_thread_ev_SF(IN tt_evcenter_t *evc,
                                      IN tt_thread_ev_t *tev)
{
    if (rand() % 2) {
        return TT_FAIL;
    } else {
        return tt_evc_sendto_thread(evc, tev);
    }
}

BOOL PostQueuedCompletionStatus_SF(HANDLE CompletionPort,
                                   DWORD dwNumberOfBytesTransferred,
                                   ULONG_PTR dwCompletionKey,
                                   LPOVERLAPPED lpOverlapped)
{
    if (
#if 1
        0 &&
#endif
        rand() % 2) {
        return TT_FAIL;
    } else {
#undef PostQueuedCompletionStatus
        return PostQueuedCompletionStatus(CompletionPort,
                                          dwNumberOfBytesTransferred,
                                          dwCompletionKey,
                                          lpOverlapped);
    }
}

HANDLE CreateFileA_SF(LPCSTR lpFileName,
                      DWORD dwDesiredAccess,
                      DWORD dwShareMode,
                      LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                      DWORD dwCreationDisposition,
                      DWORD dwFlagsAndAttributes,
                      HANDLE hTemplateFile)
{
    if (
#if 1
        0 &&
#endif
        rand() % 2) {
        return INVALID_HANDLE_VALUE;
    } else {
#undef CreateFileA
        return CreateFileA(lpFileName,
                           dwDesiredAccess,
                           dwShareMode,
                           lpSecurityAttributes,
                           dwCreationDisposition,
                           dwFlagsAndAttributes,
                           hTemplateFile);
    }
}

HANDLE CreateNamedPipeA_SF(LPCSTR lpName,
                           DWORD dwOpenMode,
                           DWORD dwPipeMode,
                           DWORD nMaxInstances,
                           DWORD nOutBufferSize,
                           DWORD nInBufferSize,
                           DWORD nDefaultTimeOut,
                           LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
    if (
#if 1
        0 &&
#endif
        rand() % 2) {
        return INVALID_HANDLE_VALUE;
    } else {
#undef CreateNamedPipeA
        return CreateNamedPipeA(lpName,
                                dwOpenMode,
                                dwPipeMode,
                                nMaxInstances,
                                nOutBufferSize,
                                nInBufferSize,
                                nDefaultTimeOut,
                                lpSecurityAttributes);
    }
}

BOOL ConnectNamedPipe_SF(HANDLE hNamedPipe, LPOVERLAPPED lpOverlapped)
{
    if (
#if 1
        0 &&
#endif
        rand() % 2) {
        return FALSE;
    } else {
#undef ConnectNamedPipe
        return ConnectNamedPipe(hNamedPipe, lpOverlapped);
    }
}

enum
{
    __SF_IO_END,
    __SF_PARTIAL,
    __SF_FAIL,
    __SF_SYS,

    __SF_NUM
};

BOOL ReadFile_SF(HANDLE hFile,
                 LPVOID lpBuffer,
                 DWORD nNumberOfBytesToRead,
                 LPDWORD lpNumberOfBytesRead,
                 LPOVERLAPPED lpOverlapped)
{
    int v = rand() % __SF_NUM;

    if (nNumberOfBytesToRead < 50) {
        v = __SF_SYS;
    }

    if (v == __SF_IO_END) {
        nNumberOfBytesToRead = 0;
    } else if (v == __SF_PARTIAL) {
        nNumberOfBytesToRead -= (rand() % 50);
    } else if (v == __SF_FAIL) {
        hFile = INVALID_HANDLE_VALUE;
    }

#undef ReadFile
    return ReadFile(hFile,
                    lpBuffer,
                    nNumberOfBytesToRead,
                    lpNumberOfBytesRead,
                    lpOverlapped);
}

BOOL WriteFile_SF(HANDLE hFile,
                  LPCVOID lpBuffer,
                  DWORD nNumberOfBytesToWrite,
                  LPDWORD lpNumberOfBytesWritten,
                  LPOVERLAPPED lpOverlapped)
{
    int v = rand() % __SF_NUM;

    if (nNumberOfBytesToWrite < 50) {
        v = __SF_SYS;
    }

    if (v == __SF_IO_END) {
        nNumberOfBytesToWrite = 0;
    } else if (v == __SF_PARTIAL) {
        nNumberOfBytesToWrite -= (rand() % 50);
    } else if (v == __SF_FAIL) {
        hFile = INVALID_HANDLE_VALUE;
    }

#undef WriteFile
    return WriteFile(hFile,
                     lpBuffer,
                     nNumberOfBytesToWrite,
                     lpNumberOfBytesWritten,
                     lpOverlapped);
}

#endif
