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
#include <os/tt_fiber.h>
#include <os/tt_fiber_event.h>
#include <os/tt_task.h>
#include <time/tt_timer.h>

#include <tt_util_native.h>

#include <sys/socket.h>
#include <sys/un.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_IPC TT_ASSERT

//#define __SIMU_FAIL_SOCKET
//#define __SIMU_FAIL_CLOSE
//#define __SIMU_FAIL_BIND
//#define __SIMU_FAIL_LISTEN
//#define __SIMU_FAIL_ACCEPT
//#define __SIMU_FAIL_CONNECT
//#define __SIMU_FAIL_SEND
//#define __SIMU_FAIL_RECV

#ifdef __SIMU_FAIL_SOCKET
#define socket __sf_socket
int __sf_socket(int domain, int type, int protocol);
#endif

#ifdef __SIMU_FAIL_CLOSE
#define close __sf_close
int __sf_close(int fildes);
#endif

#ifdef __SIMU_FAIL_BIND
#define bind __sf_bind
int __sf_bind(int socket,
              const struct sockaddr *address,
              socklen_t address_len);
#endif

#ifdef __SIMU_FAIL_LISTEN
#define listen __sf_listen
int __sf_listen(int socket, int backlog);
#endif

#ifdef __SIMU_FAIL_ACCEPT
#define accept __sf_accept
int __sf_accept(int socket,
                struct sockaddr *restrict address,
                socklen_t *restrict address_len);
#endif

#ifdef __SIMU_FAIL_CONNECT
#define connect __sf_connect
int __sf_connect(int socket,
                 const struct sockaddr *address,
                 socklen_t address_len);
#endif

#ifdef __SIMU_FAIL_SEND
#define send __sf_send
ssize_t __sf_send(int socket, const void *buffer, size_t length, int flags);
#endif

#ifdef __SIMU_FAIL_RECV
#define recv __sf_recv
ssize_t __sf_recv(int socket, void *buffer, size_t length, int flags);
#endif

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    __IPC_ACCEPT,
    __IPC_CONNECT,
    __IPC_SEND,
    __IPC_RECV,

    __IPC_EV_NUM,
};

typedef struct
{
    tt_io_ev_t io_ev;

    tt_ipc_ntv_t *ipc;
    struct sockaddr_un *saun;

    tt_result_t result;
} __ipc_connect_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_ipc_ntv_t *ipc;
    tt_ipc_ntv_t *new_ipc;
    struct sockaddr_un *saun;

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
    tt_u32_t kq;
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
    tt_u32_t kq;
} __ipc_recv_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_bool_t __do_accept(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_connect(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_send(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_recv(IN tt_io_ev_t *io_ev);

static tt_poller_io_t __ipc_poller_io[__IPC_EV_NUM] = {
    __do_accept, __do_connect, __do_send, __do_recv,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __init_ipc_addr(IN struct sockaddr_un *saun,
                                   IN const tt_char_t *addr);

static int __ipc_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_ipc_create_ntv(IN tt_ipc_ntv_t *ipc,
                              IN OPT const tt_char_t *addr,
                              IN OPT struct tt_ipc_attr_s *attr)
{
    int s, nosigpipe = 1, flag;

    s = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (s == -1) {
        TT_ERROR_NTV("fail to create ipc socket");
        return TT_FAIL;
    }

    if (addr != NULL) {
        struct sockaddr_un saun;

        if (!TT_OK(__init_ipc_addr(&saun, addr))) {
            goto fail;
        }

        unlink(addr);
        if (bind(s, (const struct sockaddr *)&saun, saun.sun_len) != 0) {
            TT_ERROR_NTV("fail to bind ipc");
            goto fail;
        }

        if (listen(s, SOMAXCONN) != 0) {
            TT_ERROR_NTV("fail to listen ipc");
            goto fail;
        }
    }

    if (setsockopt(s,
                   SOL_SOCKET,
                   SO_NOSIGPIPE,
                   &nosigpipe,
                   sizeof(nosigpipe)) != 0) {
        TT_ERROR_NTV("fail to set SO_NOSIGPIPE");
        // goto fail;
    }

    if (((flag = fcntl(s, F_GETFL, 0)) == -1) ||
        (fcntl(s, F_SETFL, flag | O_NONBLOCK) == -1)) {
        TT_ERROR_NTV("fail to set O_NONBLOCK");
        goto fail;
    }

    if (((flag = fcntl(s, F_GETFD, 0)) == -1) ||
        (fcntl(s, F_SETFD, flag | FD_CLOEXEC) == -1)) {
        TT_ERROR_NTV("fail to set FD_CLOEXEC");
        goto fail;
    }

    ipc->s = s;

    return TT_SUCCESS;

fail:

    __RETRY_IF_EINTR(close(s));

    return TT_FAIL;
}

void tt_ipc_destroy_ntv(IN tt_ipc_ntv_t *ipc)
{
    __RETRY_IF_EINTR(close(ipc->s));
}

tt_result_t tt_ipc_connect_ntv(IN tt_ipc_ntv_t *ipc, IN const tt_char_t *addr)
{
    struct sockaddr_un saun;
    __ipc_connect_t ipc_connect;
    int kq;

    if (!TT_OK(__init_ipc_addr(&saun, addr))) {
        return TT_FAIL;
    }

again:
    if (connect(ipc->s, (const struct sockaddr *)&saun, saun.sun_len) == 0) {
        return TT_SUCCESS;
    } else if (errno == EINTR) {
        goto again;
    } else if (errno != EINPROGRESS) {
        TT_ERROR_NTV("fail to connect");
        return TT_FAIL;
    }

    kq = __ipc_ev_init(&ipc_connect.io_ev, __IPC_CONNECT);

    ipc_connect.ipc = ipc;
    ipc_connect.saun = &saun;

    ipc_connect.result = TT_FAIL;

    tt_kq_write(kq, ipc->s, &ipc_connect.io_ev);
    tt_fiber_suspend();
    return ipc_connect.result;
}

tt_result_t tt_ipc_accept_ntv(IN tt_ipc_ntv_t *ipc, IN tt_ipc_ntv_t *new_ipc)
{
    __ipc_accept_t ipc_accept;
    int kq;
    struct sockaddr_un saun;

    kq = __ipc_ev_init(&ipc_accept.io_ev, __IPC_ACCEPT);

    ipc_accept.ipc = ipc;
    ipc_accept.new_ipc = new_ipc;
    ipc_accept.saun = &saun;

    ipc_accept.result = TT_FAIL;

    tt_kq_read(kq, ipc->s, &ipc_accept.io_ev);
    tt_fiber_suspend();
    return ipc_accept.result;
}

tt_result_t tt_ipc_send_ntv(IN tt_ipc_ntv_t *ipc,
                            IN tt_u8_t *buf,
                            IN tt_u32_t len,
                            OUT OPT tt_u32_t *sent)
{
    __ipc_send_t ipc_send;
    int kq;

    kq = __ipc_ev_init(&ipc_send.io_ev, __IPC_SEND);

    ipc_send.ipc = ipc;
    ipc_send.buf = buf;
    ipc_send.len = len;
    ipc_send.sent = sent;

    ipc_send.result = TT_FAIL;
    ipc_send.kq = kq;
    ipc_send.pos = 0;

    tt_kq_write(kq, ipc->s, &ipc_send.io_ev);
    tt_fiber_suspend();
    return ipc_send.result;
}

tt_result_t tt_ipc_recv_ntv(IN tt_ipc_ntv_t *ipc,
                            OUT tt_u8_t *buf,
                            IN tt_u32_t len,
                            OUT OPT tt_u32_t *recvd,
                            OUT tt_fiber_ev_t **p_fev,
                            OUT tt_tmr_t **p_tmr)
{
    __ipc_recv_t ipc_recv;
    int kq;
    tt_fiber_t *cfb;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;

    *recvd = 0;
    *p_fev = NULL;
    *p_tmr = NULL;

    kq = __ipc_ev_init(&ipc_recv.io_ev, __IPC_RECV);
    cfb = ipc_recv.io_ev.src;

    if ((tmr = tt_fiber_recv_timer(cfb, TT_FALSE)) != NULL) {
        *p_tmr = tmr;
        ipc_recv.result = TT_SUCCESS;
    }

    ipc_recv.ipc = ipc;
    ipc_recv.buf = buf;
    ipc_recv.len = len;
    ipc_recv.recvd = recvd;

    ipc_recv.result = TT_FAIL;
    ipc_recv.kq = kq;

    tt_kq_read(kq, ipc->s, &ipc_recv.io_ev);

    cfb->recving = TT_TRUE;
    tt_fiber_suspend();
    cfb->recving = TT_FALSE;

    // may be awaked due to new fiber event
    if ((fev = tt_fiber_recv(cfb, TT_FALSE)) != NULL) {
        *p_fev = fev;
        ipc_recv.result = TT_SUCCESS;
    }

    // may be awaked due to expired timer
    if ((tmr = tt_fiber_recv_timer(cfb, TT_FALSE)) != NULL) {
        *p_tmr = tmr;
        ipc_recv.result = TT_SUCCESS;
    }

    return ipc_recv.result;
}

void tt_ipc_worker_io(IN tt_io_ev_t *io_ev)
{
}

tt_bool_t tt_ipc_poller_io(IN tt_io_ev_t *io_ev)
{
    return __ipc_poller_io[io_ev->ev](io_ev);
}

tt_result_t __init_ipc_addr(IN struct sockaddr_un *saun,
                            IN const tt_char_t *addr)
{
    size_t len = strlen(addr);

    memset(saun, 0, sizeof(struct sockaddr_un));

    // family
    saun->sun_family = AF_LOCAL;

    // path
    if (len >= sizeof(saun->sun_path)) {
        TT_ERROR("ipc addr length exceed limit[%d]", sizeof(saun->sun_path));
        return TT_FAIL;
    }
    memcpy(saun->sun_path, addr, len);

    // len
    saun->sun_len = sizeof(struct sockaddr_un) - sizeof(saun->sun_path) + len;

    return TT_SUCCESS;
}

int __ipc_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev)
{
    io_ev->src = tt_current_fiber();
    io_ev->dst = NULL;
    tt_dnode_init(&io_ev->node);
    io_ev->io = TT_IO_IPC;
    io_ev->ev = ev;

    return io_ev->src->fs->thread->task->iop.sys_iop.kq;
}

tt_bool_t __do_accept(IN tt_io_ev_t *io_ev)
{
    __ipc_accept_t *ipc_accept = (__ipc_accept_t *)io_ev;

    socklen_t len = sizeof(struct sockaddr_un);
    int s, flag;

again:
    s = accept(ipc_accept->ipc->s, (struct sockaddr *)ipc_accept->saun, &len);
    if (s == -1) {
        if (errno == EINTR) {
            goto again;
        } else {
            TT_ERROR_NTV("accept fail");
            goto fail;
        }
    }

    if (((flag = fcntl(s, F_GETFL, 0)) == -1) ||
        (fcntl(s, F_SETFL, flag | O_NONBLOCK) == -1)) {
        TT_ERROR_NTV("fail to set O_NONBLOCK");
        goto fail;
    }

    if (((flag = fcntl(s, F_GETFD, 0)) == -1) ||
        (fcntl(s, F_SETFD, flag | FD_CLOEXEC) == -1)) {
        TT_ERROR_NTV("fail to set FD_CLOEXEC");
        goto fail;
    }

    ipc_accept->new_ipc->s = s;

    ipc_accept->result = TT_SUCCESS;
    return TT_TRUE;

fail:

    if (s != -1) {
        __RETRY_IF_EINTR(close(s));
    }

    ipc_accept->result = TT_FAIL;
    return TT_TRUE;
}

tt_bool_t __do_connect(IN tt_io_ev_t *io_ev)
{
    __ipc_connect_t *ipc_connect = (__ipc_connect_t *)io_ev;

    ipc_connect->result = TT_SUCCESS;
    return TT_TRUE;
}

tt_bool_t __do_send(IN tt_io_ev_t *io_ev)
{
    __ipc_send_t *ipc_send = (__ipc_send_t *)io_ev;

    ssize_t n;

again:
    n = send(ipc_send->ipc->s,
             TT_PTR_INC(void, ipc_send->buf, ipc_send->pos),
             ipc_send->len - ipc_send->pos,
             0);
    if (n > 0) {
        ipc_send->pos += n;
        TT_ASSERT_IPC(ipc_send->pos <= ipc_send->len);
        if (ipc_send->pos < ipc_send->len) {
            goto again;
        } else {
            TT_SAFE_ASSIGN(ipc_send->sent, ipc_send->pos);
            ipc_send->result = TT_SUCCESS;
            return TT_TRUE;
        }
    } else if (errno == EINTR) {
        goto again;
    } else if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
        tt_kq_write(ipc_send->kq, ipc_send->ipc->s, io_ev);
        return TT_FALSE;
    }

    // error
    if (ipc_send->pos > 0) {
        TT_SAFE_ASSIGN(ipc_send->sent, ipc_send->pos);
        ipc_send->result = TT_SUCCESS;
    } else if ((errno == ECONNRESET) || (errno == EPIPE)
               // || (errno == ENETDOWN)
               ) {
        ipc_send->result = TT_END;
    } else {
        TT_ERROR_NTV("send failed");
        ipc_send->result = TT_FAIL;
    }
    return TT_TRUE;
}

tt_bool_t __do_recv(IN tt_io_ev_t *io_ev)
{
    __ipc_recv_t *ipc_recv = (__ipc_recv_t *)io_ev;

    ssize_t n;

again:
    n = recv(ipc_recv->ipc->s, ipc_recv->buf, ipc_recv->len, 0);
    if (n > 0) {
        TT_SAFE_ASSIGN(ipc_recv->recvd, (tt_u32_t)n);
        ipc_recv->result = TT_SUCCESS;
        return TT_TRUE;
    } else if (n == 0) {
        ipc_recv->result = TT_END;
        return TT_TRUE;
    } else if (errno == EINTR) {
        goto again;
    } else if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
        tt_kq_read(ipc_recv->kq, ipc_recv->ipc->s, io_ev);
        return TT_FALSE;
    }

    // error
    if (errno == ECONNRESET
        // || (errno == ENETDOWN)
        ) {
        ipc_recv->result = TT_END;
    } else {
        TT_ERROR_NTV("recv failed");
        ipc_recv->result = TT_FAIL;
    }
    return TT_TRUE;
}

#ifdef __SIMU_FAIL_SOCKET
#undef socket
int __sf_socket(int domain, int type, int protocol)
{
    return -1;
}
#endif

#ifdef __SIMU_FAIL_CLOSE
#undef close
int __sf_close(int fildes)
{
    return -1;
}
#endif

#ifdef __SIMU_FAIL_BIND
#undef bind
int __sf_bind(int socket, const struct sockaddr *address, socklen_t address_len)

{
    return -1;
}
#endif

#ifdef __SIMU_FAIL_LISTEN
#undef listen
int __sf_listen(int socket, int backlog)
{
    return -1;
}
#endif

#ifdef __SIMU_FAIL_ACCEPT
#undef accept
int __sf_accept(int socket,
                struct sockaddr *restrict address,
                socklen_t *restrict address_len)
{
    return -1;
}
#endif

#ifdef __SIMU_FAIL_CONNECT
#undef connect
int __sf_connect(int socket,
                 const struct sockaddr *address,
                 socklen_t address_len)
{
    return -1;
}
#endif

#ifdef __SIMU_FAIL_SEND
#undef send
ssize_t __sf_send(int socket, const void *buffer, size_t length, int flags)
{
    if (0 && tt_rand_u32() % 10 == 0) {
        return -1;
    } else {
        return send(socket, buffer, tt_rand_u32() % length + 1, flags);
    }
}
#endif

#ifdef __SIMU_FAIL_RECV
#undef recv
ssize_t __sf_recv(int socket, void *buffer, size_t length, int flags)
{
    if (0 && tt_rand_u32() % 10 == 0) {
        return -1;
    } else {
        return recv(socket, buffer, tt_rand_u32() % length + 1, flags);
    }
}
#endif
