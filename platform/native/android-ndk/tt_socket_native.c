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

#define _GNU_SOURCE // accept4

#include <tt_socket_native.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <io/tt_file_system.h>
#include <io/tt_io_event.h>
#include <io/tt_socket.h>
#include <log/tt_log.h>
#include <os/tt_fiber_event.h>
#include <os/tt_task.h>
#include <time/tt_timer.h>

#include <tt_cstd_api.h>
#include <tt_epoll_compat.h>
#include <tt_util_native.h>

#include <fcntl.h>
#include <net/if.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/sendfile.h>
#include <unistd.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_SKT TT_ASSERT

//#define __SIMU_FAIL_SOCKET
//#define __SIMU_FAIL_CLOSE
//#define __SIMU_FAIL_SHUTDOWN
//#define __SIMU_FAIL_BIND
//#define __SIMU_FAIL_LISTEN
//#define __SIMU_FAIL_ACCEPT
//#define __SIMU_FAIL_CONNECT
//#define __SIMU_FAIL_SEND
//#define __SIMU_FAIL_RECV
//#define __SIMU_FAIL_SENDTO
//#define __SIMU_FAIL_RECVFROM

#ifdef __SIMU_FAIL_SOCKET
#define socket __sf_socket
int __sf_socket(int domain, int type, int protocol);
#endif

#ifdef __SIMU_FAIL_CLOSE
#define close __sf_close
int __sf_close(int fildes);
#endif

#ifdef __SIMU_FAIL_SHUTDOWN
#define shutdown __sf_shutdown
int __sf_shutdown(int socket, int how);
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
int __sf_accept(int socket, struct sockaddr *address, socklen_t *address_len);
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

#ifdef __SIMU_FAIL_SENDTO
#define sendto __sf_sendto
ssize_t __sf_sendto(int socket,
                    const void *buffer,
                    size_t length,
                    int flags,
                    const struct sockaddr *dest_addr,
                    socklen_t dest_len);
#endif

#ifdef __SIMU_FAIL_RECVFROM
#define recvfrom __sf_recvfrom
ssize_t __sf_recvfrom(int socket,
                      void *buffer,
                      size_t length,
                      int flags,
                      struct sockaddr *address,
                      socklen_t *address_len);
#endif

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    __SKT_NULL,
    __SKT_ACCEPT,
    __SKT_CONNECT,
    __SKT_SEND,
    __SKT_RECV,
    __SKT_SENDTO,
    __SKT_RECVFROM,
    __SKT_SENDFILE,

    __SKT_EV_NUM,
};

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    tt_sktaddr_t *addr;

    tt_skt_t *new_skt;
    int ep;
    tt_bool_t done : 1;
} __skt_accept_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    tt_sktaddr_t *addr;

    tt_result_t result;
} __skt_connect_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    tt_u8_t *buf;
    tt_u32_t *sent;
    tt_u32_t len;
    int flags;

    tt_result_t result;
    int ep;
    tt_u32_t pos;
} __skt_send_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    tt_u8_t *buf;
    tt_u32_t *recvd;
    tt_u32_t len;

    tt_result_t result;
    int ep;
    tt_bool_t done : 1;
} __skt_recv_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    tt_u8_t *buf;
    tt_u32_t *sent;
    tt_sktaddr_t *addr;
    tt_u32_t len;

    tt_result_t result;
    int ep;
    tt_u32_t pos;
} __skt_sendto_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    tt_u8_t *buf;
    tt_u32_t *recvd;
    tt_sktaddr_t *addr;
    tt_u32_t len;

    tt_result_t result;
    int ep;
    tt_bool_t done : 1;
} __skt_recvfrom_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    tt_file_t *f;

    tt_u64_t offset;
    tt_u64_t len;
    tt_result_t result;
    int ep;
} __skt_sendfile_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_bool_t __do_null(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_accept(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_connect(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_send(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_recv(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_sendto(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_recvfrom(IN tt_io_ev_t *io_ev);

static tt_bool_t __do_sendfile(IN tt_io_ev_t *io_ev);

static tt_poller_io_t __skt_poller_io[__SKT_EV_NUM] = {
    __do_null,
    __do_accept,
    __do_connect,
    __do_send,
    __do_recv,
    __do_sendto,
    __do_recvfrom,
    __do_sendfile,
};

static tt_io_ev_t __s_null_io_ev;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static int __skt_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev);

static tt_result_t __addr_to_mreq(IN tt_sktaddr_ip_t *addr,
                                  IN const tt_char_t *itf,
                                  OUT struct ip_mreq *mreq,
                                  IN int skt);
static tt_result_t __addr_to_mreq6(IN tt_sktaddr_ip_t *addr,
                                   IN const tt_char_t *itf,
                                   OUT struct ipv6_mreq *mreq);

static void __dump_socket_fdinfo(IN int s, IN tt_u32_t flag);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_skt_component_init_ntv(IN tt_profile_t *profile)
{
    __s_null_io_ev.src = NULL;
    __s_null_io_ev.dst = NULL;
    tt_dnode_init(&__s_null_io_ev.node);
    __s_null_io_ev.io = TT_IO_SOCKET;
    __s_null_io_ev.ev = __SKT_NULL;

    return TT_SUCCESS;
}

void tt_skt_component_exit_ntv()
{
}

void tt_skt_status_dump_ntv(IN tt_u32_t flag)
{
    char path[PATH_MAX + 1];
    int len;
    DIR *d;
    struct dirent entry;
    struct dirent *result = NULL;

    len = snprintf(path, PATH_MAX, "/proc/%d/fd/", getpid());

    d = opendir(path);
    if (d == NULL) {
        tt_printf("fail to open %s", path);
        return;
    }

    while ((readdir_r(d, &entry, &result) == 0) && (result != NULL)) {
        char link[PATH_MAX + 1];
        ssize_t n;
        int s;

        if ((strcmp(entry.d_name, ".") == 0) ||
            (strcmp(entry.d_name, "..") == 0)) {
            continue;
        }

        path[len] = 0;
        strncat(path, entry.d_name, PATH_MAX);
        if ((n = readlink(path, link, PATH_MAX)) < 0) {
            continue;
        }
        link[n] = 0;

        if (strncmp(link, "socket", 6) != 0) {
            // only show socket
            continue;
        }

        s = strtol(entry.d_name, NULL, 10);
        __dump_socket_fdinfo(s, flag);
    }

    closedir(d);

#if 0
    pid_t pid;
    int size;
    struct proc_fdinfo *fdinfo;

    pid = getpid();
    size = proc_pidinfo(pid, PROC_PIDLISTFDS, 0, NULL, 0);
    if (size <= 0) {
        return;
    }

    fdinfo = (struct proc_fdinfo *)malloc(size);
    if (fdinfo == NULL) {
        return;
    }

    size = proc_pidinfo(pid, PROC_PIDLISTFDS, 0, fdinfo, size);
    if (size > 0) {
        int n, i;

        n = size / PROC_PIDLISTFD_SIZE;
        for (i = 0; i < n; ++i) {
            if (fdinfo[i].proc_fdtype == PROX_FDTYPE_SOCKET) {
                struct socket_fdinfo si;
                int vs = proc_pidfdinfo(pid,
                                        fdinfo[i].proc_fd,
                                        PROC_PIDFDSOCKETINFO,
                                        &si,
                                        PROC_PIDFDSOCKETINFO_SIZE);
                if (vs == PROC_PIDFDSOCKETINFO_SIZE) {
                    __dump_socket_fdinfo(&fdinfo[i], &si, flag);
                }
            }
        }
    }

    free(fdinfo);
#endif
}

tt_result_t tt_skt_create_ntv(IN tt_skt_ntv_t *skt,
                              IN tt_net_family_t family,
                              IN tt_net_protocol_t protocol,
                              IN OPT tt_skt_attr_t *attr)
{
    int af, type, proto, s, flag, epfd;
    // int nosigpipe = 1;
    struct epoll_event event;

    if (family == TT_NET_AF_INET) {
        af = AF_INET;
    } else {
        TT_ASSERT_SKT(family == TT_NET_AF_INET6);
        af = AF_INET6;
    }

    if (protocol == TT_NET_PROTO_TCP) {
        type = SOCK_STREAM;
        proto = IPPROTO_TCP;
    } else {
        TT_ASSERT_SKT(protocol == TT_NET_PROTO_UDP);
        type = SOCK_DGRAM;
        proto = IPPROTO_UDP;
    }

    s = socket(af, type, proto);
    if (s < 0) {
        TT_ERROR_NTV("fail to create socket");
        return TT_FAIL;
    }

#if 0
    if (setsockopt(s,
                   SOL_SOCKET,
                   SO_NOSIGPIPE,
                   &nosigpipe,
                   sizeof(nosigpipe)) != 0) {
        TT_ERROR_NTV("fail to set SO_NOSIGPIPE");
        goto fail;
    }
#endif

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

    epfd = tt_current_task()->iop.sys_iop.ep;
    event.events = EPOLLRDHUP | EPOLLONESHOT;
    event.data.ptr = &__s_null_io_ev;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, s, &event) != 0) {
        TT_ERROR_NTV("fail to add skt to epoll");
        goto fail;
    }

    skt->s = s;

    return TT_SUCCESS;

fail:

    __RETRY_IF_EINTR(close(s) != 0);

    return TT_FAIL;
}

void tt_skt_destroy_ntv(IN tt_skt_ntv_t *skt)
{
    __RETRY_IF_EINTR(close(skt->s) != 0);
}

tt_result_t tt_skt_shutdown_ntv(IN tt_skt_ntv_t *skt, IN tt_skt_shut_t shut)
{
    int how;

    if (shut == TT_SKT_SHUT_RD) {
        how = SHUT_RD;
    } else if (shut == TT_SKT_SHUT_WR) {
        how = SHUT_WR;
    } else {
        TT_ASSERT_SKT(shut == TT_SKT_SHUT_RDWR);
        how = SHUT_RDWR;
    }

    if (shutdown(skt->s, how) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to shutdown socket");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_bind_ntv(IN tt_skt_ntv_t *skt, IN tt_sktaddr_t *addr)
{
    if (bind(skt->s,
             (struct sockaddr *)addr,
             sizeof(struct sockaddr_storage)) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("binding fail");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_listen_ntv(IN tt_skt_ntv_t *skt)
{
    if (listen(skt->s, SOMAXCONN) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("listening fail");
        return TT_FAIL;
    }
}

tt_skt_t *tt_skt_accept_ntv(IN tt_skt_ntv_t *skt,
                            OUT tt_sktaddr_t *addr,
                            OUT tt_fiber_ev_t **p_fev,
                            OUT tt_tmr_t **p_tmr)
{
    __skt_accept_t skt_accept;
    int ep;
    tt_fiber_t *cfb;

    *p_fev = NULL;
    *p_tmr = NULL;

    ep = __skt_ev_init(&skt_accept.io_ev, __SKT_ACCEPT);
    cfb = skt_accept.io_ev.src;

    if (tt_fiber_recv(cfb, TT_FALSE, p_fev, p_tmr)) {
        return NULL;
    }

    skt_accept.skt = skt;
    skt_accept.addr = addr;

    skt_accept.new_skt = NULL;
    skt_accept.ep = ep;
    skt_accept.done = TT_FALSE;

    tt_ep_read(ep, skt->s, &skt_accept.io_ev);

    cfb->recving = TT_TRUE;
    tt_fiber_suspend();
    cfb->recving = TT_FALSE;

    if (!skt_accept.done) {
        tt_ep_unread(ep, skt->s, &__s_null_io_ev);
    }

    tt_fiber_recv(cfb, TT_FALSE, p_fev, p_tmr);

    return skt_accept.new_skt;
}

tt_result_t tt_skt_connect_ntv(IN tt_skt_ntv_t *skt, IN tt_sktaddr_t *addr)
{
    socklen_t len;
    __skt_connect_t skt_connect;
    int ep;

    len = TT_COND(tt_sktaddr_get_family(addr) == TT_NET_AF_INET,
                  sizeof(struct sockaddr_in),
                  sizeof(struct sockaddr_in6));

again:
    if (connect(skt->s, (const struct sockaddr *)addr, len) == 0) {
        return TT_SUCCESS;
    } else if (errno == EINTR) {
        goto again;
    } else if (errno != EINPROGRESS) {
        TT_ERROR_NTV("fail to connect");
        return TT_FAIL;
    }

    ep = __skt_ev_init(&skt_connect.io_ev, __SKT_CONNECT);

    skt_connect.skt = skt;
    skt_connect.addr = addr;

    skt_connect.result = TT_FAIL;

    tt_ep_write(ep, skt->s, &skt_connect.io_ev);
    tt_fiber_suspend();
    return skt_connect.result;
}

tt_result_t tt_skt_local_addr_ntv(IN tt_skt_ntv_t *skt, OUT tt_sktaddr_t *addr)
{
    socklen_t len = sizeof(struct sockaddr_storage);

    if (getsockname(skt->s, (struct sockaddr *)addr, &len) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to get local address");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_remote_addr_ntv(IN tt_skt_ntv_t *skt, OUT tt_sktaddr_t *addr)
{
    socklen_t len = sizeof(struct sockaddr_storage);

    if (getpeername(skt->s, (struct sockaddr *)addr, &len) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to get remote address");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_recvfrom_ntv(IN tt_skt_ntv_t *skt,
                                OUT tt_u8_t *buf,
                                IN tt_u32_t len,
                                OUT OPT tt_u32_t *recvd,
                                OUT OPT tt_sktaddr_t *addr,
                                OUT tt_fiber_ev_t **p_fev,
                                OUT tt_tmr_t **p_tmr)
{
    __skt_recvfrom_t skt_recvfrom;
    int ep;
    tt_fiber_t *cfb;

    *recvd = 0;
    *p_fev = NULL;
    *p_tmr = NULL;

    ep = __skt_ev_init(&skt_recvfrom.io_ev, __SKT_RECVFROM);
    cfb = skt_recvfrom.io_ev.src;

    if (tt_fiber_recv(cfb, TT_FALSE, p_fev, p_tmr)) {
        return TT_SUCCESS;
    }

    skt_recvfrom.skt = skt;
    skt_recvfrom.buf = buf;
    skt_recvfrom.len = len;
    skt_recvfrom.recvd = recvd;
    skt_recvfrom.addr = addr;

    skt_recvfrom.result = TT_FAIL;
    skt_recvfrom.ep = ep;
    skt_recvfrom.done = TT_FALSE;

    tt_ep_read(ep, skt->s, &skt_recvfrom.io_ev);

    cfb->recving = TT_TRUE;
    tt_fiber_suspend();
    cfb->recving = TT_FALSE;

    if (!skt_recvfrom.done) {
        tt_ep_unread(ep, skt->s, &__s_null_io_ev);
    }

    if (tt_fiber_recv(cfb, TT_FALSE, p_fev, p_tmr)) {
        skt_recvfrom.result = TT_SUCCESS;
    }

    return skt_recvfrom.result;
}

tt_result_t tt_skt_sendto_ntv(IN tt_skt_ntv_t *skt,
                              IN tt_u8_t *buf,
                              IN tt_u32_t len,
                              OUT OPT tt_u32_t *sent,
                              IN tt_sktaddr_t *addr)
{
    __skt_sendto_t skt_sendto;
    int ep;

    ep = __skt_ev_init(&skt_sendto.io_ev, __SKT_SENDTO);

    skt_sendto.skt = skt;
    skt_sendto.buf = buf;
    skt_sendto.len = len;
    skt_sendto.sent = sent;
    skt_sendto.addr = addr;

    skt_sendto.result = TT_FAIL;
    skt_sendto.ep = ep;
    skt_sendto.pos = 0;

    tt_ep_write(ep, skt->s, &skt_sendto.io_ev);
    tt_fiber_suspend();
    return skt_sendto.result;
}

tt_result_t tt_skt_send_ntv(IN tt_skt_ntv_t *skt,
                            IN tt_u8_t *buf,
                            IN tt_u32_t len,
                            OUT OPT tt_u32_t *sent)
{
    __skt_send_t skt_send;
    int ep;

    ep = __skt_ev_init(&skt_send.io_ev, __SKT_SEND);

    skt_send.skt = skt;
    skt_send.buf = buf;
    skt_send.len = len;
    skt_send.sent = sent;
    skt_send.flags = 0;

    skt_send.result = TT_FAIL;
    skt_send.ep = ep;
    skt_send.pos = 0;

    tt_ep_write(ep, skt->s, &skt_send.io_ev);
    tt_fiber_suspend();
    return skt_send.result;
}

tt_result_t tt_skt_send_oob_ntv(IN tt_skt_ntv_t *skt, IN tt_u8_t b)
{
    __skt_send_t skt_send;
    int ep;

    ep = __skt_ev_init(&skt_send.io_ev, __SKT_SEND);

    skt_send.skt = skt;
    skt_send.buf = &b;
    skt_send.len = 1;
    skt_send.sent = NULL;
    skt_send.flags = MSG_OOB;

    skt_send.result = TT_FAIL;
    skt_send.ep = ep;
    skt_send.pos = 0;

    tt_ep_write(ep, skt->s, &skt_send.io_ev);
    tt_fiber_suspend();
    return skt_send.result;
}

tt_result_t tt_skt_sendfile_ntv(IN tt_skt_ntv_t *skt, IN tt_file_t *f)
{
    __skt_sendfile_t skt_sendfile;
    int ep;

    ep = __skt_ev_init(&skt_sendfile.io_ev, __SKT_SENDFILE);

    skt_sendfile.skt = skt;
    skt_sendfile.f = f;

    skt_sendfile.offset = 0;
    if (!TT_OK(
            tt_fseek_ntv(&f->sys_file, TT_FSEEK_END, 0, &skt_sendfile.len))) {
        return TT_FAIL;
    }
    skt_sendfile.result = TT_FAIL;
    skt_sendfile.ep = ep;

    tt_ep_write(ep, skt->s, &skt_sendfile.io_ev);
    tt_fiber_suspend();
    return skt_sendfile.result;
}

tt_result_t tt_skt_recv_ntv(IN tt_skt_ntv_t *skt,
                            OUT tt_u8_t *buf,
                            IN tt_u32_t len,
                            OUT OPT tt_u32_t *recvd,
                            OUT tt_fiber_ev_t **p_fev,
                            OUT tt_tmr_t **p_tmr)
{
    __skt_recv_t skt_recv;
    int ep;
    tt_fiber_t *cfb;

    *recvd = 0;
    *p_fev = NULL;
    *p_tmr = NULL;

    ep = __skt_ev_init(&skt_recv.io_ev, __SKT_RECV);
    cfb = skt_recv.io_ev.src;

    if (tt_fiber_recv(cfb, TT_FALSE, p_fev, p_tmr)) {
        return TT_SUCCESS;
    }

    skt_recv.skt = skt;
    skt_recv.buf = buf;
    skt_recv.len = len;
    skt_recv.recvd = recvd;

    skt_recv.result = TT_FAIL;
    skt_recv.ep = ep;
    skt_recv.done = TT_FALSE;

    tt_ep_read(ep, skt->s, &skt_recv.io_ev);

    cfb->recving = TT_TRUE;
    tt_fiber_suspend();
    cfb->recving = TT_FALSE;

    if (!skt_recv.done) {
        tt_ep_unread(ep, skt->s, &__s_null_io_ev);
    }

    if (tt_fiber_recv(cfb, TT_FALSE, p_fev, p_tmr)) {
        skt_recv.result = TT_SUCCESS;
    }

    return skt_recv.result;
}

tt_result_t tt_skt_join_mcast_ntv(IN tt_skt_ntv_t *skt,
                                  IN tt_net_family_t family,
                                  IN tt_sktaddr_ip_t *addr,
                                  IN const tt_char_t *itf)
{
    if (family == TT_NET_AF_INET) {
        struct ip_mreq mreq;
        if (!TT_OK(__addr_to_mreq(addr, itf, &mreq, skt->s))) {
            return TT_FAIL;
        }

        // join multicast group
        if (setsockopt(skt->s,
                       IPPROTO_IP,
                       IP_ADD_MEMBERSHIP,
                       &mreq,
                       (socklen_t)sizeof(struct ip_mreq)) == 0) {
            return TT_SUCCESS;
        } else {
            TT_ERROR_NTV("fail to join multicast group");
            return TT_FAIL;
        }
    } else {
        struct ipv6_mreq mreq;

        TT_ASSERT_SKT(family == TT_NET_AF_INET6);

        if (!TT_OK(__addr_to_mreq6(addr, itf, &mreq))) {
            return TT_FAIL;
        }

        // join multicast group
        if (setsockopt(skt->s,
                       IPPROTO_IPV6,
                       IPV6_ADD_MEMBERSHIP,
                       &mreq,
                       (socklen_t)sizeof(struct ipv6_mreq)) == 0) {
            return TT_SUCCESS;
        } else {
            TT_ERROR_NTV("fail to join multicast group");
            return TT_FAIL;
        }
    }
}

tt_result_t tt_skt_leave_mcast_ntv(IN tt_skt_ntv_t *skt,
                                   IN tt_net_family_t family,
                                   IN tt_sktaddr_ip_t *addr,
                                   IN const tt_char_t *itf)
{
    if (family == TT_NET_AF_INET) {
        struct ip_mreq mreq;
        if (!TT_OK(__addr_to_mreq(addr, itf, &mreq, skt->s))) {
            return TT_FAIL;
        }

        // leave multicast group
        if (setsockopt(skt->s,
                       IPPROTO_IP,
                       IP_DROP_MEMBERSHIP,
                       &mreq,
                       (socklen_t)sizeof(struct ip_mreq)) == 0) {
            return TT_SUCCESS;
        } else {
            TT_ERROR_NTV("fail to join multicast group");
            return TT_FAIL;
        }
    } else {
        struct ipv6_mreq mreq;

        TT_ASSERT_SKT(family == TT_NET_AF_INET6);

        if (!TT_OK(__addr_to_mreq6(addr, itf, &mreq))) {
            return TT_FAIL;
        }

        // leave multicast group
        if (setsockopt(skt->s,
                       IPPROTO_IPV6,
                       IPV6_DROP_MEMBERSHIP,
                       &mreq,
                       (socklen_t)sizeof(struct ipv6_mreq)) == 0) {
            return TT_SUCCESS;
        } else {
            TT_ERROR_NTV("fail to join multicast group");
            return TT_FAIL;
        }
    }
}

void tt_skt_worker_io(IN tt_io_ev_t *io_ev)
{
}

tt_bool_t tt_skt_poller_io(IN tt_io_ev_t *io_ev)
{
    return __skt_poller_io[io_ev->ev](io_ev);
}

int __skt_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev)
{
    io_ev->src = tt_current_fiber();
    io_ev->dst = NULL;
    tt_dnode_init(&io_ev->node);
    io_ev->io = TT_IO_SOCKET;
    io_ev->ev = ev;

    return io_ev->src->fs->thread->task->iop.sys_iop.ep;
}

tt_result_t __addr_to_mreq(IN tt_sktaddr_ip_t *addr,
                           IN const tt_char_t *itf,
                           OUT struct ip_mreq *mreq,
                           IN int skt)
{
    // address
    mreq->imr_multiaddr.s_addr = addr->a32.__u32;

    // interface
    if (itf != NULL) {
        struct ifreq ifr;

        ifr.ifr_addr.sa_family = AF_INET;
        strncpy(ifr.ifr_name, itf, IFNAMSIZ - 1);
        if (ioctl(skt, SIOCGIFADDR, &ifr) != 0) {
            TT_ERROR_NTV("fail to get address of %s", itf);
            return TT_FAIL;
        }

        mreq->imr_interface.s_addr =
            ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
    } else {
        mreq->imr_interface.s_addr = htonl(INADDR_ANY);
    }

    return TT_SUCCESS;
}

tt_result_t __addr_to_mreq6(IN tt_sktaddr_ip_t *addr,
                            IN const tt_char_t *itf,
                            OUT struct ipv6_mreq *mreq)
{
    // address
    tt_memcpy(mreq->ipv6mr_multiaddr.s6_addr, addr->a128.__u8, 16);

    // interface
    if (itf != NULL) {
        unsigned int if_idx;

        if_idx = if_nametoindex(itf);
        if (if_idx == 0) {
            TT_ERROR_NTV("fail to get if idx of %s", itf);
            return TT_FAIL;
        }

        mreq->ipv6mr_interface = if_idx;
    } else {
        mreq->ipv6mr_interface = 0;
    }

    return TT_SUCCESS;
}

tt_bool_t __do_null(IN tt_io_ev_t *io_ev)
{
    // no fiber to resume
    return TT_FALSE;
}

tt_bool_t __do_accept(IN tt_io_ev_t *io_ev)
{
    __skt_accept_t *skt_accept = (__skt_accept_t *)io_ev;

    socklen_t len = sizeof(struct sockaddr_storage);
    int s, flag;
    struct epoll_event event;

    // tell caller that kq returned
    skt_accept->done = TT_TRUE;

    skt_accept->new_skt = tt_malloc(sizeof(tt_skt_t));
    if (skt_accept->new_skt == NULL) {
        TT_ERROR("no mem for new skt");
        return TT_TRUE;
    }

again:
    s = accept(skt_accept->skt->s, (struct sockaddr *)skt_accept->addr, &len);
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

    event.events = EPOLLRDHUP | EPOLLONESHOT;
    event.data.ptr = &__s_null_io_ev;
    if (epoll_ctl(skt_accept->ep, EPOLL_CTL_ADD, s, &event) != 0) {
        TT_ERROR_NTV("fail to add skt to epoll");
        goto fail;
    }

    skt_accept->new_skt->sys_skt.s = s;

    return TT_TRUE;

fail:

    if (s != -1) {
        __RETRY_IF_EINTR(close(s));
    }

    return TT_TRUE;
}

tt_bool_t __do_connect(IN tt_io_ev_t *io_ev)
{
    __skt_connect_t *skt_connect = (__skt_connect_t *)io_ev;

    skt_connect->result = io_ev->io_result;
    return TT_TRUE;
}

tt_bool_t __do_send(IN tt_io_ev_t *io_ev)
{
    __skt_send_t *skt_send = (__skt_send_t *)io_ev;

    ssize_t n;

again:
    n = send(skt_send->skt->s,
             TT_PTR_INC(void, skt_send->buf, skt_send->pos),
             skt_send->len - skt_send->pos,
             skt_send->flags);
    if (n > 0) {
        skt_send->pos += n;
        TT_ASSERT_SKT(skt_send->pos <= skt_send->len);
        if (skt_send->pos < skt_send->len) {
            goto again;
        } else {
            TT_SAFE_ASSIGN(skt_send->sent, skt_send->pos);
            skt_send->result = TT_SUCCESS;
            return TT_TRUE;
        }
    } else if (errno == EINTR) {
        goto again;
    } else if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
        tt_ep_write(skt_send->ep, skt_send->skt->s, io_ev);
        return TT_FALSE;
    }

    // error
    if (skt_send->pos > 0) {
        TT_SAFE_ASSIGN(skt_send->sent, skt_send->pos);
        skt_send->result = TT_SUCCESS;
    } else if ((errno == ECONNRESET) || (errno == EPIPE)
               // || (errno == ENETDOWN)
               ) {
        skt_send->result = TT_E_END;
    } else {
        TT_ERROR_NTV("send failed");
        skt_send->result = TT_FAIL;
    }
    return TT_TRUE;
}

tt_bool_t __do_recv(IN tt_io_ev_t *io_ev)
{
    __skt_recv_t *skt_recv = (__skt_recv_t *)io_ev;

    ssize_t n;

again:
    n = recv(skt_recv->skt->s, skt_recv->buf, skt_recv->len, 0);
    if (n > 0) {
        TT_SAFE_ASSIGN(skt_recv->recvd, (tt_u32_t)n);
        skt_recv->result = TT_SUCCESS;
        skt_recv->done = TT_TRUE;
        return TT_TRUE;
    } else if (n == 0) {
        skt_recv->result = TT_E_END;
        skt_recv->done = TT_TRUE;
        return TT_TRUE;
    } else if (errno == EINTR) {
        goto again;
    } else if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
        tt_ep_read(skt_recv->ep, skt_recv->skt->s, io_ev);
        return TT_FALSE;
    }

    // error
    if (errno == ECONNRESET
        // || (errno == ENETDOWN)
        ) {
        skt_recv->result = TT_E_END;
    } else {
        TT_ERROR_NTV("recv failed");
        skt_recv->result = TT_FAIL;
    }
    skt_recv->done = TT_TRUE;
    return TT_TRUE;
}

tt_bool_t __do_sendto(IN tt_io_ev_t *io_ev)
{
    __skt_sendto_t *skt_sendto = (__skt_sendto_t *)io_ev;

    ssize_t n;

again:
    n = sendto(skt_sendto->skt->s,
               TT_PTR_INC(void, skt_sendto->buf, skt_sendto->pos),
               skt_sendto->len - skt_sendto->pos,
               0,
               (struct sockaddr *)skt_sendto->addr,
               sizeof(tt_sktaddr_t));
    if (n > 0) {
        skt_sendto->pos += n;
        TT_ASSERT_SKT(skt_sendto->pos <= skt_sendto->len);
        if (skt_sendto->pos < skt_sendto->len) {
            goto again;
        } else {
            TT_SAFE_ASSIGN(skt_sendto->sent, skt_sendto->pos);
            skt_sendto->result = TT_SUCCESS;
            return TT_TRUE;
        }
    } else if (errno == EINTR) {
        goto again;
    } else if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
        tt_ep_write(skt_sendto->ep, skt_sendto->skt->s, io_ev);
        return TT_FALSE;
    }

    // error
    if (skt_sendto->pos > 0) {
        TT_SAFE_ASSIGN(skt_sendto->sent, skt_sendto->pos);
        skt_sendto->result = TT_SUCCESS;
    } else if ((errno == ECONNRESET) || (errno == EPIPE)
               // || (errno == ENETDOWN)
               ) {
        skt_sendto->result = TT_E_END;
    } else {
        TT_ERROR_NTV("sendto failed");
        skt_sendto->result = TT_FAIL;
    }
    return TT_TRUE;
}

tt_bool_t __do_recvfrom(IN tt_io_ev_t *io_ev)
{
    __skt_recvfrom_t *skt_recvfrom = (__skt_recvfrom_t *)io_ev;

    ssize_t n;
    socklen_t addr_len = sizeof(tt_sktaddr_t);

again:
    n = recvfrom(skt_recvfrom->skt->s,
                 skt_recvfrom->buf,
                 skt_recvfrom->len,
                 0,
                 (struct sockaddr *)skt_recvfrom->addr,
                 &addr_len);
    if (n > 0) {
        TT_SAFE_ASSIGN(skt_recvfrom->recvd, (tt_u32_t)n);
        skt_recvfrom->result = TT_SUCCESS;
        skt_recvfrom->done = TT_TRUE;
        return TT_TRUE;
    } else if (n == 0) {
        skt_recvfrom->result = TT_E_END;
        skt_recvfrom->done = TT_TRUE;
        return TT_TRUE;
    } else if (errno == EINTR) {
        goto again;
    } else if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
        tt_ep_read(skt_recvfrom->ep, skt_recvfrom->skt->s, io_ev);
        return TT_FALSE;
    }

    // error
    if (errno == ECONNRESET
        // || (errno == ENETDOWN)
        ) {
        skt_recvfrom->result = TT_E_END;
    } else {
        TT_ERROR_NTV("recvfrom failed");
        skt_recvfrom->result = TT_FAIL;
    }
    skt_recvfrom->done = TT_TRUE;
    return TT_TRUE;
}

tt_bool_t __do_sendfile(IN tt_io_ev_t *io_ev)
{
    __skt_sendfile_t *skt_sendfile = (__skt_sendfile_t *)io_ev;

    off_t offset = (off_t)skt_sendfile->offset;
    size_t count = skt_sendfile->len;
    ssize_t n;

again:
    n = sendfile(skt_sendfile->skt->s,
                 skt_sendfile->f->sys_file.fd,
                 &offset,
                 count);
    if (n >= 0) {
        assert(n <= count);
        count -= n;
        if (count > 0) {
            // offset was updated
            goto again;
        } else {
            skt_sendfile->result = TT_SUCCESS;
        }
    } /*else if (errno == EINTR) {
        goto again;
    }*/ else {
        TT_ERROR_NTV("fail to sendfile to skt");
        skt_sendfile->result = TT_FAIL;
    }

    return TT_TRUE;
}

void __dump_socket_fdinfo(IN int s, IN tt_u32_t flag)
{
    int val;
    socklen_t len = sizeof(val);
    const char *type = "?";
    char local[128] = {0};
    char remote[128] = {0};

    if (getsockopt(s, SOL_SOCKET, SO_TYPE, &val, &len) != 0) {
        tt_printf("%s[fd: %d] ?\n",
                  TT_COND(flag & TT_SKT_STATUS_PREFIX, "<<Socket>> ", ""),
                  s);
    } else if (val == SOCK_STREAM) {
        struct sockaddr_storage a;
        socklen_t alen = sizeof(a);
        char local[128] = {0};
        char remote[128] = {0};

        if (getsockname(s, (struct sockaddr *)&a, &alen) != 0) {
            tt_snprintf(local, sizeof(local) - 1, "?|?");
        } else if (a.ss_family == AF_INET) {
            struct sockaddr_in *a4 = (struct sockaddr_in *)&a;
            tt_sktaddr_ip_t ip;
            char addr[64] = {0};

            ip.a32.__u32 = a4->sin_addr.s_addr;
            tt_sktaddr_ip_n2p(TT_NET_AF_INET, &ip, addr, sizeof(addr) - 1);
            tt_snprintf(local,
                        sizeof(local) - 1,
                        "%s|%d",
                        addr,
                        ntohs(a4->sin_port));
        } else if (a.ss_family == AF_INET6) {
            struct sockaddr_in6 *a6 = (struct sockaddr_in6 *)&a;
            tt_sktaddr_ip_t ip;
            char addr[64] = {0};

            tt_memcpy(ip.a128.__u8, a6->sin6_addr.s6_addr, 16);
            tt_sktaddr_ip_n2p(TT_NET_AF_INET6, &ip, addr, sizeof(addr) - 1);
            tt_snprintf(local,
                        sizeof(local) - 1,
                        "%s|%d",
                        addr,
                        ntohs(a6->sin6_port));
        } else if (a.ss_family == AF_LOCAL) {
            return;
        } else {
            tt_snprintf(local, sizeof(local) - 1, "?|?");
        }

        if (getpeername(s, (struct sockaddr *)&a, &alen) != 0) {
            tt_snprintf(remote, sizeof(remote) - 1, "?|?");
        } else if (a.ss_family == AF_INET) {
            struct sockaddr_in *a4 = (struct sockaddr_in *)&a;
            tt_sktaddr_ip_t ip;
            char addr[64] = {0};

            ip.a32.__u32 = a4->sin_addr.s_addr;
            tt_sktaddr_ip_n2p(TT_NET_AF_INET, &ip, addr, sizeof(addr) - 1);
            tt_snprintf(remote,
                        sizeof(remote) - 1,
                        "%s|%d",
                        addr,
                        ntohs(a4->sin_port));
        } else if (a.ss_family == AF_INET6) {
            struct sockaddr_in6 *a6 = (struct sockaddr_in6 *)&a;
            tt_sktaddr_ip_t ip;
            char addr[64] = {0};

            tt_memcpy(ip.a128.__u8, a6->sin6_addr.s6_addr, 16);
            tt_sktaddr_ip_n2p(TT_NET_AF_INET6, &ip, addr, sizeof(addr) - 1);
            tt_snprintf(remote,
                        sizeof(remote) - 1,
                        "%s|%d",
                        addr,
                        ntohs(a6->sin6_port));
        } else if (a.ss_family == AF_LOCAL) {
            return;
        } else {
            tt_snprintf(remote, sizeof(local) - 1, "?|?");
        }

        tt_printf("%s[fd: %d] tcp [%s --> %s]\n",
                  TT_COND(flag & TT_SKT_STATUS_PREFIX, "<<Socket>> ", ""),
                  s,
                  local,
                  remote);
    } else if (val == SOCK_DGRAM) {
        struct sockaddr_storage a;
        socklen_t alen = sizeof(a);
        char local[128] = {0};

        if (getsockname(s, (struct sockaddr *)&a, &alen) != 0) {
            tt_snprintf(local, sizeof(local) - 1, "?|?");
        } else if (a.ss_family == AF_INET) {
            struct sockaddr_in *a4 = (struct sockaddr_in *)&a;
            tt_sktaddr_ip_t ip;
            char addr[64] = {0};

            ip.a32.__u32 = a4->sin_addr.s_addr;
            tt_sktaddr_ip_n2p(TT_NET_AF_INET, &ip, addr, sizeof(addr) - 1);
            tt_snprintf(local,
                        sizeof(local) - 1,
                        "%s|%d",
                        addr,
                        ntohs(a4->sin_port));
        } else if (a.ss_family == AF_INET6) {
            struct sockaddr_in6 *a6 = (struct sockaddr_in6 *)&a;
            tt_sktaddr_ip_t ip;
            char addr[64] = {0};

            tt_memcpy(ip.a128.__u8, a6->sin6_addr.s6_addr, 16);
            tt_sktaddr_ip_n2p(TT_NET_AF_INET6, &ip, addr, sizeof(addr) - 1);
            tt_snprintf(local,
                        sizeof(local) - 1,
                        "%s|%d",
                        addr,
                        ntohs(a6->sin6_port));
        } else if (a.ss_family == AF_LOCAL) {
            return;
        } else {
            tt_snprintf(local, sizeof(local) - 1, "?|?");
        }

        tt_printf("%s[fd: %d] udp [%s]\n",
                  TT_COND(flag & TT_SKT_STATUS_PREFIX, "<<Socket>> ", ""),
                  s,
                  local);
    } /*else {
        tt_printf("%s[fd: %d] ?\n",
                  TT_COND(flag & TT_SKT_STATUS_PREFIX, "<<Socket>> ", ""),
                  s);
    }*/
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

#ifdef __SIMU_FAIL_SHUTDOWN
#undef shutdown
int __sf_shutdown(int socket, int how)

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
int __sf_accept(int socket, struct sockaddr *address, socklen_t *address_len)
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
    if ((tt_rand_u32() % 10) == 0) {
        return -1;
    } else {
        tt_u32_t n = tt_rand_u32() % length + 1;
        // TT_INFO("send %d < %d", n, length);
        return send(socket, buffer, n, flags);
    }
}
#endif

#ifdef __SIMU_FAIL_RECV
#undef recv
ssize_t __sf_recv(int socket, void *buffer, size_t length, int flags)
{
    if ((tt_rand_u32() % 10) == 0) {
        return -1;
    } else {
        tt_u32_t n = tt_rand_u32() % length + 1;
        // TT_INFO("recv %d < %d", n, length);
        return recv(socket, buffer, n, flags);
    }
}
#endif

#ifdef __SIMU_FAIL_SENDTO
#undef sendto
ssize_t __sf_sendto(int socket,
                    const void *buffer,
                    size_t length,
                    int flags,
                    const struct sockaddr *dest_addr,
                    socklen_t dest_len)
{
    if ((tt_rand_u32() % 10) == 0) {
        return -1;
    } else {
        return sendto(socket,
                      buffer,
                      tt_rand_u32() % length,
                      flags,
                      dest_addr,
                      dest_len);
    }
}
#endif

#ifdef __SIMU_FAIL_RECVFROM
#undef recvfrom
ssize_t __sf_recvfrom(int socket,
                      void *buffer,
                      size_t length,
                      int flags,
                      struct sockaddr *address,
                      socklen_t *address_len)
{
    if ((tt_rand_u32() % 10) == 0) {
        return -1;
    } else {
        return recvfrom(socket,
                        buffer,
                        tt_rand_u32() % length,
                        flags,
                        address,
                        address_len);
    }
}
#endif
