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

#include <tt_cstd_api.h>
#include <tt_sys_error.h>
#include <tt_util_native.h>

#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

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

    EV_IPC_END,
};

typedef struct
{
    tt_ipc_t *ipc;
} __ipc_destroy_t;

typedef struct
{
    tt_ipc_t *ipc;
    struct sockaddr_un remote_addr;
    tt_ipc_on_connect_t on_connect;

    tt_result_t result;
    void *cb_param;
} __ipc_connect_t;

typedef struct
{
    tt_ipc_t *listening_ipc;
    tt_ipc_t *new_ipc;
    tt_ipc_on_accept_t on_accept;

    tt_result_t result;
    void *cb_param;
} __ipc_accept_t;

typedef struct
{
    tt_ipc_t *ipc;
    tt_ev_t *data_ev; // from app
    tt_ipc_on_send_t on_send;
    tt_u32_t send_len;

    tt_result_t result;
    void *cb_param;
} __ipc_send_t;

typedef struct
{
    tt_ipc_t *ipc;
    tt_ev_t **data_ev; // from ipc to app
    tt_u32_t data_ev_num; // num of valid tev in data_ev
    tt_ipc_on_recv_t on_recv;

    tt_result_t result;
    void *cb_param;
} __ipc_recv_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

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

static void __init_ipc(IN tt_ipc_ntv_t *sys_ipc,
                       IN tt_evcenter_t *evc,
                       IN tt_ipc_exit_t *ipc_exit);

static tt_result_t __create_ipc_skt(IN tt_ipc_ntv_t *sys_ipc,
                                    IN OPT const tt_char_t *local_addr,
                                    IN tt_ipc_attr_t *attr,
                                    IN tt_evcenter_t *evc);

static tt_result_t __init_ipc_addr(IN struct sockaddr_un *skt_addr,
                                   IN const tt_char_t *addr);

// connect
static tt_bool_t __do_ipc_connect(IN __ipc_connect_t *aio,
                                  IN tt_result_t ep_result);
static void __do_ipc_connect_cb(IN __ipc_connect_t *aio);

// accept
static tt_bool_t __do_ipc_accept(IN __ipc_accept_t *aio,
                                 IN tt_result_t ep_result);
static void __do_ipc_accept_cb(IN __ipc_accept_t *aio);

// send
static tt_bool_t __do_ipc_send(IN __ipc_send_t *aio, IN tt_result_t ep_result);
static tt_result_t __do_ipc_send_io(IN __ipc_send_t *aio);
static void __do_ipc_send_cb(IN __ipc_send_t *aio);

// recv
static tt_bool_t __do_ipc_recv(IN __ipc_recv_t *aio, IN tt_result_t ep_result);
static tt_result_t __do_ipc_recv_io(IN __ipc_recv_t *aio);
static tt_result_t __ipc_ev_parse(IN tt_buf_t *recv_buf, IN __ipc_recv_t *aio);
static void __do_ipc_recv_cb(IN __ipc_recv_t *aio);

// destroy
static void __do_ipc_destroy(IN __ipc_destroy_t *aio);

// aio q
static void __do_ipc_aio_q(IN tt_ipc_ntv_t *sys_ipc,
                           IN tt_list_t *aio_q,
                           IN tt_result_t ep_result);
static void __clear_ipc_aio_q(IN tt_ipc_ntv_t *sys_ipc,
                              IN tt_list_t *aio_q,
                              IN tt_result_t aio_result);

static tt_result_t __ipc_start_epoll(IN tt_ipc_ntv_t *sys_ipc);

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

    __init_ipc(sys_ipc, evc, ipc_exit);

    if (!TT_OK(__create_ipc_skt(sys_ipc, local_addr, attr, evc))) {
        tt_buf_destroy(&sys_ipc->recv_buf);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_async_ipc_destroy_ntv(IN tt_ipc_ntv_t *sys_ipc, IN tt_bool_t immediate)
{
    tt_ipc_t *ipc = TT_CONTAINER(sys_ipc, tt_ipc_t, sys_ipc);

    if (immediate) {
        tt_ipc_on_destroy_t on_destroy =
            (tt_ipc_on_destroy_t)sys_ipc->on_destroy;
        tt_bool_t from_alloc = ipc->attr.from_alloc;

        __clear_ipc_aio_q(sys_ipc, &sys_ipc->read_q, TT_CANCELLED);
        __clear_ipc_aio_q(sys_ipc, &sys_ipc->write_q, TT_CANCELLED);
        if (sys_ipc->s != -1) {
            __RETRY_IF_EINTR(close(sys_ipc->s) != 0);
            sys_ipc->s = -1;
        }
        tt_buf_destroy(&sys_ipc->recv_buf);

        on_destroy(ipc, sys_ipc->on_destroy_param);
        // do not access aio->ipc from now on as application may freed aio->ipc

        if (from_alloc) {
            tt_mem_free(ipc);
        }
    } else {
        tt_ev_t *ev;
        __ipc_destroy_t *aio;
        tt_evcenter_t *evc = sys_ipc->evc;

#ifdef __IPC_CHECK_EVC
        if (evc != tt_evc_current()) {
            TT_ERROR("different ipc evc");
            return;
        }
#endif

        if (sys_ipc->rd_closing || sys_ipc->wr_closing) {
            TT_ASSERT_IPCAIO(sys_ipc->rd_closing && sys_ipc->wr_closing);
            // TT_ERROR("already closing");
            return;
        }

        // aio
        ev = tt_thread_ev_create(EV_IPC_DESTROY, sizeof(__ipc_destroy_t), NULL);
        if (ev == NULL) {
            TT_ERROR("fail to allocate destroy aio");
            return;
        }
        aio = TT_EV_DATA(ev, __ipc_destroy_t);

        // init aio
        aio->ipc = ipc;

        // post aio as a thread tev rather than appending to
        // read/write queue, or the process would crash
        if (!TT_OK(tt_evc_sendto_thread(evc, ev))) {
            TT_ERROR("fail to post ipc destroy");
            tt_ev_destroy(ev);
            return;
        }

        sys_ipc->rd_closing = TT_TRUE;
        sys_ipc->wr_closing = TT_TRUE;
    }
}

tt_result_t tt_ipc_connect_async_ntv(IN tt_ipc_ntv_t *sys_ipc,
                                     IN const tt_char_t *remote_addr,
                                     IN tt_ipc_on_connect_t on_connect,
                                     IN OPT void *cb_param)
{
    tt_ipc_t *ipc = TT_CONTAINER(sys_ipc, tt_ipc_t, sys_ipc);
    struct sockaddr_un skt_addr;
    tt_ev_t *ev;
    tt_thread_ev_t *tev;
    __ipc_connect_t *aio;

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
    if (sys_ipc->connecting) {
        TT_ERROR("ipc is connecting");
        return TT_FAIL;
    }
    if (sys_ipc->connected) {
        TT_ERROR("ipc is already connected");
        return TT_FAIL;
    }

    // aio
    ev = tt_thread_ev_create(EV_IPC_CONNECT, sizeof(__ipc_connect_t), NULL);
    if (ev == NULL) {
        TT_ERROR("fail to allocate destroy aio");
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __ipc_connect_t);

    if (!TT_OK(__init_ipc_addr(&aio->remote_addr, remote_addr))) {
        // ev is not well initialized, so use tt_mem_free
        tt_mem_free(ev);
        return TT_FAIL;
    }

conn_ag:
    if (connect(sys_ipc->s,
                (const struct sockaddr *)&aio->remote_addr,
                sizeof(struct sockaddr_un)) != 0) {
        if (errno == EINTR) {
            goto conn_ag;
        } else if (errno != EINPROGRESS) {
            TT_ERROR_NTV("fail to connect");

            tt_mem_free(ev);
            return TT_FAIL;
        }
    }
    // now connect is ongoing(or done...)

    // init aio
    aio->ipc = ipc;
    // aio->remote_addr has been initialized
    aio->on_connect = on_connect;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add request
    tt_list_addtail(&sys_ipc->write_q, &tev->node);

    // start kqueue if it's head aio
    if ((tt_list_count(&sys_ipc->write_q) == 1) &&
        !TT_OK(__ipc_start_epoll(sys_ipc))) {
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

    // init new ipc
    __init_ipc(&new_ipc->sys_ipc, sys_ipc->evc, new_ipc_exit);

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
        return TT_FAIL;
    }
    tev = TT_EV_HDR(ev, tt_thread_ev_t);
    aio = TT_EV_DATA(ev, __ipc_accept_t);

    // init aio
    aio->listening_ipc = ipc;
    aio->new_ipc = new_ipc;
    aio->on_accept = on_accept;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add request
    tt_list_addtail(&sys_ipc->read_q, &tev->node);

    // start kqueue if it's head aio
    if ((tt_list_count(&sys_ipc->read_q) == 1) &&
        !TT_OK(__ipc_start_epoll(sys_ipc))) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
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
    aio->ipc = ipc;
    aio->data_ev = data_ev;
    aio->on_send = on_send;
    aio->send_len = 0;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add request
    tt_list_addtail(&sys_ipc->write_q, &tev->node);

    // start kqueue if it's head aio
    if ((tt_list_count(&sys_ipc->write_q) == 1) &&
        !TT_OK(__ipc_start_epoll(sys_ipc))) {
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
    aio->ipc = ipc;
    aio->data_ev = NULL;
    aio->data_ev_num = 0;
    aio->on_recv = on_recv;

    aio->result = TT_PROCEEDING;
    aio->cb_param = cb_param;

    // add request
    tt_list_addtail(&sys_ipc->read_q, &tev->node);

    // start kqueue if it's head aio
    if ((tt_list_count(&sys_ipc->read_q) == 1) &&
        !TT_OK(__ipc_start_epoll(sys_ipc))) {
        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        return TT_FAIL;
    }
    return TT_SUCCESS;
}

tt_result_t tt_ipc_tev_handler(IN struct tt_evpoller_s *evp, IN tt_ev_t *ev)
{
    TT_ASSERT(ev != NULL);
    TT_ASSERT(TT_EV_RANGE(ev->ev_id) == TT_EV_RANGE_INTERNAL_IPC);

    switch (ev->ev_id) {
        case EV_IPC_DESTROY: {
            __do_ipc_destroy(TT_EV_DATA(ev, __ipc_destroy_t));
            tt_ev_destroy(ev);
        } break;

        default: {
            TT_FATAL("unknown event: %x", ev->ev_id);
            tt_ev_destroy(ev);
        } break;
    }
    return TT_SUCCESS;
}

tt_result_t tt_ipc_ep_handler(IN tt_ipc_t *ipc, IN tt_u32_t ep_events)
{
    tt_bool_t has_io = TT_FALSE;
    tt_ipc_ntv_t *sys_ipc = &ipc->sys_ipc;

    // 1. do io whenever epoll report socket is readable or writable
    if (ep_events & EPOLLIN) {
#if 0
        if (tt_list_empty(&sys_ipc->read_q)) {
            TT_WARN("EPOLLIN, but no read aio");
        }
#endif

        __do_ipc_aio_q(sys_ipc, &sys_ipc->read_q, TT_SUCCESS);
        has_io = TT_TRUE;
    }
    if (ep_events & EPOLLOUT) {
#if 0
        if (tt_list_empty(&sys_ipc->write_q)) {
            TT_WARN("EPOLLOUT, but no write aio");
        }
#endif

        __do_ipc_aio_q(sys_ipc, &sys_ipc->write_q, TT_SUCCESS);
        has_io = TT_TRUE;
    }
    if (has_io) {
        // it should first handle io flags until epoll indicates
        // no io can be done
        return TT_SUCCESS;
    }

    // 2. remote hang up
    if (ep_events & EPOLLRDHUP) {
        __do_ipc_aio_q(sys_ipc, &sys_ipc->read_q, TT_END);
        //__do_ipc_aio_q(sys_ipc, &sys_ipc->write_q, TT_END);
        return TT_SUCCESS;
    }

    // 3. there are only error events
    if ((ep_events & EPOLLERR) || (ep_events & EPOLLHUP)) {
        __do_ipc_aio_q(sys_ipc, &sys_ipc->read_q, TT_FAIL);
        __do_ipc_aio_q(sys_ipc, &sys_ipc->write_q, TT_FAIL);
        return TT_SUCCESS;
    }

    // TT_WARN("missing event: %x",
    //         ep_events & ~(EPOLLRDHUP | EPOLLERR | EPOLLHUP | EPOLLIN |
    //         EPOLLOUT));
    return TT_SUCCESS;
}

void __init_ipc(IN tt_ipc_ntv_t *sys_ipc,
                IN tt_evcenter_t *evc,
                IN tt_ipc_exit_t *ipc_exit)
{
    // must set to -1
    sys_ipc->s = -1;

    sys_ipc->evc = evc;
    sys_ipc->ev_mark = TT_EVP_MARK_EPOLL_IPC;

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

    sys_ipc->rd_closing = TT_FALSE;
    sys_ipc->wr_closing = TT_FALSE;
    sys_ipc->connecting = TT_FALSE;
    sys_ipc->connected = TT_FALSE;
}

tt_result_t __create_ipc_skt(IN tt_ipc_ntv_t *sys_ipc,
                             IN OPT const tt_char_t *local_addr,
                             IN tt_ipc_attr_t *attr,
                             IN tt_evcenter_t *evc)
{
    int ipc;
    int nosigpipe = 1;
    long skt_flags = 0;

    if (sys_ipc->s == -1) {
        sys_ipc->s = socket(AF_LOCAL, SOCK_STREAM, 0);
        if (sys_ipc->s == -1) {
            TT_ERROR_NTV("fail to create ipc socket");
            return TT_FAIL;
        }
    }
    ipc = sys_ipc->s;

    if (local_addr != NULL) {
        struct sockaddr_un skt_addr;

        if (!TT_OK(__init_ipc_addr(&skt_addr, local_addr))) {
            goto __cis_fail;
        }

        unlink(local_addr);
        if (bind(ipc,
                 (const struct sockaddr *)&skt_addr,
                 sizeof(struct sockaddr_un)) != 0) {
            TT_ERROR_NTV("fail to bind ipc");
            goto __cis_fail;
        }

        if (listen(ipc, SOMAXCONN) != 0) {
            TT_ERROR_NTV("fail to listen ipc");
            goto __cis_fail;
        }
    } else {
        // ipc with NULL local addr are either client ipc or
        // accepted ipc
        if (!TT_OK(tt_buf_reserve(&sys_ipc->recv_buf, attr->recv_buf_size))) {
            TT_ERROR("fail to reserve [%d] bytes ipc recv stream",
                     attr->recv_buf_size);
            goto __cis_fail;
        }
    }

    /*
    if (setsockopt(ipc,
                   SOL_SOCKET,
                   SO_NOSIGPIPE,
                   &nosigpipe,
                   sizeof(nosigpipe)) != 0)
    {
        TT_ERROR_NTV("fail to set SO_NOSIGPIPE");
        goto __cis_fail;
    }
    */

    if (((skt_flags = fcntl(ipc, F_GETFL, 0)) == -1) ||
        (fcntl(ipc, F_SETFL, skt_flags | O_NONBLOCK) == -1)) {
        TT_ERROR_NTV("fail to set ipc nonblock");
        goto __cis_fail;
    }

    // set close-on-exec
    if (((skt_flags = fcntl(ipc, F_GETFD, 0)) == -1) ||
        (fcntl(ipc, F_SETFD, skt_flags | FD_CLOEXEC) == -1)) {
        TT_ERROR_NTV("fail to set ipc close-on-exec");
        goto __cis_fail;
    }

    // add to epoll
    do {
        struct epoll_event ep_ev = {0};

        // EPOLLONESHOT is never used, or it lead to data missing
        ep_ev.events = EPOLLRDHUP;
        ep_ev.data.ptr = &sys_ipc->ev_mark;

        if (epoll_ctl(evc->sys_evc.ep_fd, EPOLL_CTL_ADD, ipc, &ep_ev) != 0) {
            TT_ERROR_NTV("fail to add to ep");
            goto __cis_fail;
        }
    } while (0);

    return TT_SUCCESS;

__cis_fail:

    __RETRY_IF_EINTR(close(ipc) != 0);
    sys_ipc->s = -1;

    return TT_FAIL;
}

tt_result_t __init_ipc_addr(IN struct sockaddr_un *skt_addr,
                            IN const tt_char_t *addr)
{
    int addr_len = strlen(addr);

    memset(skt_addr, 0, sizeof(struct sockaddr_un));

    // family
    skt_addr->sun_family = AF_LOCAL;

    // path
    if (addr_len >= sizeof(skt_addr->sun_path)) {
        TT_ERROR("ipc addr length exceed limit[%d]",
                 sizeof(skt_addr->sun_path));
        return TT_FAIL;
    }
    memcpy(skt_addr->sun_path, addr, addr_len);

    return TT_SUCCESS;
}

tt_bool_t __do_ipc_connect(IN __ipc_connect_t *aio, IN tt_result_t ep_result)
{
    tt_ipc_t *ipc = aio->ipc;
    tt_ipc_ntv_t *sys_ipc = &ipc->sys_ipc;

    // connecting is done when socket becomes writable
    aio->result = ep_result;

    sys_ipc->connecting = TT_FALSE;
    if (TT_OK(aio->result)) {
        sys_ipc->connected = TT_TRUE;
    } else {
        TT_ERROR("ipc connect failed");
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
    aio->on_connect(aio->ipc,
                    (const tt_char_t *)&aio->remote_addr.sun_path,
                    &aioctx);
}

tt_bool_t __do_ipc_accept(IN __ipc_accept_t *aio, IN tt_result_t ep_result)
{
    tt_ipc_t *listening_ipc = aio->listening_ipc;
    tt_ipc_ntv_t *listening_sys_ipc = &listening_ipc->sys_ipc;
    tt_ipc_t *new_ipc = aio->new_ipc;
    tt_ipc_ntv_t *new_sys_ipc = &new_ipc->sys_ipc;

    // need this?
    struct sockaddr_un remote_addr;
    socklen_t remote_addr_len = sizeof(struct sockaddr_un);

    TT_ASSERT_IPCAIO(new_sys_ipc->s == -1);

    if (TT_OK(ep_result)) {
    accept_ag:
        new_sys_ipc->s = accept(listening_sys_ipc->s,
                                (struct sockaddr *)&remote_addr,
                                &remote_addr_len);
        if (new_sys_ipc->s == -1) {
            if (errno == EINTR) {
                goto accept_ag;
            }

            // EAGAIN or EWOULDBLOCK may occur when listening socket has
            // no new socket to accept, so left the accept aio in q
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                if (!TT_OK(__ipc_start_epoll(listening_sys_ipc))) {
                    TT_FATAL("listening may never work");
                }
                return TT_FALSE;
            }

            TT_ERROR_NTV("fail to accept");
            aio->result = TT_FAIL;
        } else if (!TT_OK(__create_ipc_skt(new_sys_ipc,
                                           NULL,
                                           &new_ipc->attr,
                                           new_sys_ipc->evc))) {
            aio->result = TT_FAIL;
        } else {
            new_sys_ipc->connected = TT_TRUE;

            aio->result = TT_SUCCESS;
        }
    }
    TT_ASSERT_IPCAIO(aio->result != TT_PROCEEDING);

    if (!TT_OK(aio->result)) {
        if (new_sys_ipc->s != -1) {
            __RETRY_IF_EINTR(close(new_sys_ipc->s) != 0);
            new_sys_ipc->s = -1;
        }
    }

    __do_ipc_accept_cb(aio);
    return TT_TRUE;
}

void __do_ipc_accept_cb(IN __ipc_accept_t *aio)
{
    tt_ipc_aioctx_t aioctx;
    tt_ipc_t *new_ipc = aio->new_ipc;
    tt_bool_t from_alloc = new_ipc->attr.from_alloc;

    if (TT_OK(aio->result)) {
        new_ipc->sys_ipc.connected = TT_TRUE;
    }

    // callback
    aioctx.result = aio->result;
    aioctx.cb_param = aio->cb_param;
    aio->on_accept(aio->listening_ipc, new_ipc, &aioctx);

    if (!TT_OK(aioctx.result) && from_alloc) {
        tt_mem_free(new_ipc);
    }
}

tt_bool_t __do_ipc_send(IN __ipc_send_t *aio, IN tt_result_t ep_result)
{
    if (TT_OK(ep_result)) {
        // only do io when kq returned success
        aio->result = __do_ipc_send_io(aio);
    } else {
        aio->result = ep_result;
    }

    if (aio->result != TT_PROCEEDING) {
        __do_ipc_send_cb(aio);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_result_t __do_ipc_send_io(IN __ipc_send_t *aio)
{
    tt_ipc_ntv_t *sys_ipc = &aio->ipc->sys_ipc;
    tt_ev_t *data_ev = aio->data_ev;
    tt_u32_t size = sizeof(tt_ev_t) + data_ev->hdr_size + data_ev->data_size;

    // do until pending
    while (aio->send_len < size) {
        struct iovec iov;
        struct msghdr msg;
        ssize_t write_num = 0;

        // init iov
        iov.iov_base = TT_PTR_INC(char, data_ev, aio->send_len);
        iov.iov_len = size - aio->send_len;

        // init msg
        msg.msg_name = NULL;
        msg.msg_namelen = 0;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        msg.msg_flags = 0;

        write_num = sendmsg(sys_ipc->s, &msg, 0);
        if (write_num > 0) {
            aio->send_len += write_num;
            continue;
            // while loop would check if all are written
        } else if (write_num == 0) {
            TT_ERROR("send 0 bytes");
            return TT_END;
        } else {
            if (errno == EINTR) {
                continue;
            } else if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // can not send now, start polling
                if (TT_OK(__ipc_start_epoll(sys_ipc))) {
                    return TT_PROCEEDING;
                } else {
                    // to end socket io
                    return TT_FAIL;
                }
            } else if ((errno == ECONNRESET) || (errno == EPIPE)
                       // || (errno == ENETDOWN)
                       ) {
                return TT_END;
            } else {
                TT_ERROR_NTV("fail to send");
                return TT_FAIL;
            }
        }
    }
    TT_ASSERT_IPCAIO(aio->send_len == size);
    return TT_SUCCESS;
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

tt_bool_t __do_ipc_recv(IN __ipc_recv_t *aio, IN tt_result_t ep_result)
{
    if (TT_OK(ep_result)) {
        // only do io when kq returned success
        aio->result = __do_ipc_recv_io(aio);
    } else {
        aio->result = ep_result;
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

tt_result_t __do_ipc_recv_io(IN __ipc_recv_t *aio)
{
    tt_ipc_ntv_t *sys_ipc = &aio->ipc->sys_ipc;
    tt_ev_t *data_ev = NULL;
    tt_buf_t *recv_buf = &sys_ipc->recv_buf;
    tt_result_t parse_result;

    TT_ASSERT_IPCAIO(aio->data_ev == NULL);

    // there may be data in recv buf
    parse_result = __ipc_ev_parse(recv_buf, aio);
    if (TT_OK(parse_result)) {
        // return although there may be more data which is not read out of
        // ipc socket yet
        return TT_SUCCESS;
    } else if (parse_result != TT_PROCEEDING) {
        TT_ERROR("ipc msg parsing failed, to close ipc");
        return TT_FAIL;
    }

    // do until io_pending
    while (TT_BUF_WLEN(recv_buf) > 0) {
        struct iovec iov;
        struct msghdr msg;
        ssize_t read_num = 0;

        iov.iov_len = TT_BUF_WLEN(recv_buf);
        iov.iov_base = (char *)TT_BUF_WPOS(recv_buf);

        // init msg
        msg.msg_name = NULL;
        msg.msg_namelen = 0;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        msg.msg_flags = 0;

        read_num = recvmsg(sys_ipc->s, &msg, 0);
        if (read_num > 0) {
            TT_ASSERT_ALWAYS(TT_OK(tt_buf_inc_wp(recv_buf, read_num)));
            continue;
            // while loop would check if all are written
        } else if (read_num == 0) {
            return TT_END;
        } else {
            if (errno == EINTR) {
                continue;
            } else if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // here it's a full receive or no data received, so keep
                // doing io
                if (TT_OK(__ipc_start_epoll(sys_ipc))) {
                    break;
                } else {
                    return TT_FAIL;
                }
            } else if (errno == ECONNRESET
                       // || (errno == ENETDOWN)
                       ) {
                return TT_END;
            } else {
                TT_ERROR_NTV("fail to read");
                return TT_FAIL;
            }
        }
    }

    // parse received data
    parse_result = __ipc_ev_parse(recv_buf, aio);
    if (TT_OK(parse_result)) {
        // return although there may be more data which is not read out of
        // ipc socket yet
        return TT_SUCCESS;
    } else if (parse_result != TT_PROCEEDING) {
        TT_ERROR("ipc msg parsing failed, to close ipc");
        return TT_FAIL;
    }

    if (TT_BUF_WLEN(recv_buf) == 0) {
        // even discarding all content in recv_buf is not helpful,
        // as msg boundary information is lost
        TT_ERROR("ipc buf is full");
        return TT_FAIL;
    }

    // waiting for more data
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

    data_ev = (tt_ev_t **)tt_mem_alloc(data_ev_size);
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
        tt_mem_free(data_ev);
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
        tt_mem_free(aio->data_ev);
    }
}

void __do_ipc_destroy(IN __ipc_destroy_t *aio)
{
    tt_ipc_t *ipc = aio->ipc;
    tt_ipc_ntv_t *sys_ipc = &ipc->sys_ipc;
    tt_ipc_on_destroy_t on_destroy = (tt_ipc_on_destroy_t)sys_ipc->on_destroy;
    tt_bool_t from_alloc = ipc->attr.from_alloc;

    __clear_ipc_aio_q(sys_ipc, &sys_ipc->read_q, TT_CANCELLED);
    __clear_ipc_aio_q(sys_ipc, &sys_ipc->write_q, TT_CANCELLED);
    __RETRY_IF_EINTR(close(sys_ipc->s) != 0);
    sys_ipc->s = -1;
    tt_buf_destroy(&sys_ipc->recv_buf);

    on_destroy(aio->ipc, sys_ipc->on_destroy_param);
    // do not access aio->ipc from now on as application may freed aio->ipc

    if (from_alloc) {
        tt_mem_free(ipc);
    }
}

void __do_ipc_aio_q(IN tt_ipc_ntv_t *sys_ipc,
                    IN tt_list_t *aio_q,
                    IN tt_result_t ep_result)
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
                TT_ASSERT_IPCAIO(
                    &tev->node ==
                    tt_list_head(&aio->listening_ipc->sys_ipc.read_q));

                aio_done = __do_ipc_accept(aio, ep_result);
            } break;
            case EV_IPC_CONNECT: {
                __ipc_connect_t *aio = TT_EV_DATA(ev, __ipc_connect_t);

                // it should be the head aio in write q
                TT_ASSERT_IPCAIO(&tev->node ==
                                 tt_list_head(&aio->ipc->sys_ipc.write_q));

                aio_done = __do_ipc_connect(aio, ep_result);
            } break;
            case EV_IPC_SEND: {
                __ipc_send_t *aio = TT_EV_DATA(ev, __ipc_send_t);

                TT_ASSERT_IPCAIO(&tev->node ==
                                 tt_list_head(&aio->ipc->sys_ipc.write_q));

                aio_done = __do_ipc_send(aio, ep_result);

                // special handling, once it fails to send an ipc event, the ipc
                // should be destroyed as the event may be partially sent and
                // the remote peer will always fail to decode following ipc
                // events
                if (aio_done && !TT_OK(aio->result)) {
                    tt_async_ipc_destroy_ntv(&aio->ipc->sys_ipc, TT_FALSE);
                }
            } break;
            case EV_IPC_RECV: {
                __ipc_recv_t *aio = TT_EV_DATA(ev, __ipc_recv_t);

                TT_ASSERT_IPCAIO(&tev->node ==
                                 tt_list_head(&aio->ipc->sys_ipc.read_q));

                aio_done = __do_ipc_recv(aio, ep_result);

                // special handling, once it fails to recv an ipc event, the ipc
                // should be destroyed as it may fail to decode all following
                // ipc events
                if (aio_done && !TT_OK(aio->result)) {
                    tt_async_ipc_destroy_ntv(&aio->ipc->sys_ipc, TT_FALSE);
                }
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
        // next
    }

    // - if an aio is completed and it is the last aio in q and
    //   nothing is done in callback then we should stop polling
    //   the corresponding io status
    // - but this may be redundant: an read aio is the last aio
    //   and it post an write aio in callback, then here it's
    //   doing redundant things
    if (sys_ipc->s != -1) {
        __ipc_start_epoll(sys_ipc);
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
                TT_ASSERT_IPCAIO(
                    &tev->node ==
                    tt_list_head(&aio->listening_ipc->sys_ipc.read_q));

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

            default: {
                TT_ERROR("unknown aio: %x", ev->ev_id);
            } break;
        }

        tt_list_remove(&tev->node);
        tt_ev_destroy(ev);
        // next
    }
}

tt_result_t __ipc_start_epoll(IN tt_ipc_ntv_t *sys_ipc)
{
    tt_ipc_t *ipc = TT_CONTAINER(sys_ipc, tt_ipc_t, sys_ipc);
    tt_evcenter_ntv_t *sys_evc = &sys_ipc->evc->sys_evc;
    int ep_fd = sys_evc->ep_fd;
    struct epoll_event ep_ev = {0};

    // event
    if (!tt_list_empty(&sys_ipc->read_q)) {
        ep_ev.events |= (EPOLLIN | EPOLLRDHUP);
    }
    if (!tt_list_empty(&sys_ipc->write_q)) {
        ep_ev.events |= EPOLLOUT;
    }

    ep_ev.data.ptr = &sys_ipc->ev_mark;

#if 0
    TT_INFO("ipc[%d] ep[%d] events: %s, %s, %s",
            sys_ipc->s,
            ep_fd,
            ep_ev.events & EPOLLIN ? "EPOLLIN" : "",
            ep_ev.events & EPOLLOUT ? "EPOLLOUT" : "",
            ep_ev.events & EPOLLRDHUP ? "EPOLLRDHUP" : "");
#endif

    // start epoll
    if (epoll_ctl(ep_fd, EPOLL_CTL_MOD, sys_ipc->s, &ep_ev) != 0) {
        TT_ERROR_NTV("fail to start epoll ipc");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
