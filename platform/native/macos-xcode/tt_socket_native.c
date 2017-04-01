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

#include <tt_socket_native.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <io/tt_io_event.h>
#include <io/tt_socket.h>
#include <log/tt_log.h>
#include <os/tt_task.h>

#include <tt_cstd_api.h>
#include <tt_util_native.h>

#include <net/if.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <unistd.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_SKT TT_ASSERT

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    __SKT_ACCEPT,
    __SKT_CONNECT,
    __SKT_SEND,
    __SKT_RECV,
    __SKT_SENDTO,
    __SKT_RECVFROM,

    __SKT_EV_NUM,
};

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    tt_skt_ntv_t *new_skt;
    tt_sktaddr_t *addr;

    tt_result_t result;
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
    tt_u32_t len;
    tt_u32_t *sent;

    tt_result_t result;
} __skt_send_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    tt_u8_t *buf;
    tt_u32_t len;
    tt_u32_t *recvd;

    tt_result_t result;
} __skt_recv_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    tt_u8_t *buf;
    tt_u32_t len;
    tt_u32_t *sent;
    tt_sktaddr_t *addr;

    tt_result_t result;
} __skt_sendto_t;

typedef struct
{
    tt_io_ev_t io_ev;

    tt_skt_ntv_t *skt;
    tt_u8_t *buf;
    tt_u32_t len;
    tt_u32_t *recvd;
    tt_sktaddr_t *addr;

    tt_result_t result;
} __skt_recvfrom_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static int __skt_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev);

static tt_result_t __addr_to_mreq(IN tt_sktaddr_ip_t *addr,
                                  IN const tt_char_t *itf,
                                  OUT struct ip_mreq *mreq,
                                  IN int skt);
static tt_result_t __mcaddr_to_mreq6(IN tt_sktaddr_ip_t *addr,
                                     IN const tt_char_t *itf,
                                     OUT struct ipv6_mreq *mreq);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_skt_component_init_ntv(IN tt_profile_t *profile)
{
    return TT_SUCCESS;
}

tt_result_t tt_skt_create_ntv(IN tt_skt_ntv_t *skt,
                              IN tt_net_family_t family,
                              IN tt_net_protocol_t protocol,
                              IN OPT tt_skt_attr_t *attr)
{
    int af, type, proto, s, flag, kq;
    int nosigpipe = 1;
    struct linger linger = {0};

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

    if (setsockopt(s,
                   SOL_SOCKET,
                   SO_NOSIGPIPE,
                   &nosigpipe,
                   sizeof(nosigpipe)) != 0) {
        TT_ERROR_NTV("fail to set SO_NOSIGPIPE");
        goto fail;
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

    if (setsockopt(s, SOL_SOCKET, SO_LINGER, &linger, sizeof(struct linger)) !=
        0) {
        TT_ERROR_NTV("fail to set SO_LINGER");
        goto fail;
    }

    skt->s = s;

    tt_skt_stat_inc_num();
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
    // mac need precise value of the 3rd argument of bind()
    // addr->ss_len should have been set in tt_sktaddr_init()
    if (bind(skt->s, (struct sockaddr *)addr, addr->ss_len) == 0) {
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

tt_result_t tt_skt_accept_ntv(IN tt_skt_ntv_t *skt,
                              OUT tt_skt_ntv_t *new_skt,
                              OUT tt_sktaddr_t *addr)
{
    __skt_accept_t skt_accept;
    int kq;

    kq = __skt_ev_init(&skt_accept.io_ev, __SKT_ACCEPT);

    skt_accept.skt = skt;
    skt_accept.new_skt = new_skt;
    skt_accept.addr = addr;

    skt_accept.result = TT_FAIL;

    tt_kq_read(kq, skt->s, &skt_accept.io_ev);
    tt_fiber_yield();
    return skt_accept.result;
}

tt_result_t tt_skt_connect_ntv(IN tt_skt_ntv_t *skt, IN tt_sktaddr_t *addr)
{
    __skt_connect_t skt_connect;
    int kq;

    kq = __skt_ev_init(&skt_connect.io_ev, __SKT_CONNECT);

    skt_connect.skt = skt;
    skt_connect.addr = addr;

    skt_connect.result = TT_FAIL;

    tt_kq_write(kq, skt->s, &skt_connect.io_ev);
    tt_fiber_yield();
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
                                OUT OPT tt_sktaddr_t *addr)
{
    __skt_recvfrom_t skt_recvfrom;
    int kq;

    kq = __skt_ev_init(&skt_recvfrom.io_ev, __SKT_RECVFROM);

    skt_recvfrom.skt = skt;
    skt_recvfrom.buf = buf;
    skt_recvfrom.len = len;
    skt_recvfrom.recvd = recvd;
    skt_recvfrom.addr = addr;

    skt_recvfrom.result = TT_FAIL;

    tt_kq_read(kq, skt->s, &skt_recvfrom.io_ev);
    tt_fiber_yield();
    return skt_recvfrom.result;
}

tt_result_t tt_skt_sendto_ntv(IN tt_skt_ntv_t *skt,
                              IN tt_u8_t *buf,
                              IN tt_u32_t len,
                              OUT OPT tt_u32_t *sent,
                              IN tt_sktaddr_t *addr)
{
    __skt_sendto_t skt_sendto;
    int kq;

    kq = __skt_ev_init(&skt_sendto.io_ev, __SKT_SENDTO);

    skt_sendto.skt = skt;
    skt_sendto.buf = buf;
    skt_sendto.len = len;
    skt_sendto.sent = sent;
    skt_sendto.addr = addr;

    skt_sendto.result = TT_FAIL;

    tt_kq_write(kq, skt->s, &skt_sendto.io_ev);
    tt_fiber_yield();
    return skt_sendto.result;
}

tt_result_t tt_skt_send_ntv(IN tt_skt_ntv_t *skt,
                            IN tt_u8_t *buf,
                            IN tt_u32_t len,
                            OUT OPT tt_u32_t *sent)
{
    __skt_send_t skt_send;
    int kq;

    kq = __skt_ev_init(&skt_send.io_ev, __SKT_SEND);

    skt_send.skt = skt;
    skt_send.buf = buf;
    skt_send.len = len;
    skt_send.sent = sent;

    skt_send.result = TT_FAIL;

    tt_kq_write(kq, skt->s, &skt_send.io_ev);
    tt_fiber_yield();
    return skt_send.result;
}

tt_result_t tt_skt_recv_ntv(IN tt_skt_ntv_t *skt,
                            OUT tt_u8_t *buf,
                            IN tt_u32_t len,
                            OUT OPT tt_u32_t *recvd)
{
    __skt_recv_t __skt_recv_t;
    int kq;

    kq = __skt_ev_init(&__skt_recv_t.io_ev, __SKT_RECV);

    __skt_recv_t.skt = skt;
    __skt_recv_t.buf = buf;
    __skt_recv_t.len = len;
    __skt_recv_t.recvd = recvd;

    __skt_recv_t.result = TT_FAIL;

    tt_kq_read(kq, skt->s, &__skt_recv_t.io_ev);
    tt_fiber_yield();
    return __skt_recv_t.result;
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

        if (!TT_OK(__mcaddr_to_mreq6(addr, itf, &mreq))) {
            return TT_FAIL;
        }

        // join multicast group
        if (setsockopt(skt->s,
                       IPPROTO_IPV6,
                       IPV6_JOIN_GROUP,
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
    } else if (family == TT_NET_AF_INET6) {
        struct ipv6_mreq mreq;
        if (!TT_OK(__mcaddr_to_mreq6(addr, itf, &mreq))) {
            return TT_FAIL;
        }

        // leave multicast group
        if (setsockopt(skt->s,
                       IPPROTO_IPV6,
                       IPV6_LEAVE_GROUP,
                       &mreq,
                       (socklen_t)sizeof(struct ipv6_mreq)) == 0) {
            return TT_SUCCESS;
        } else {
            TT_ERROR_NTV("fail to join multicast group");
            return TT_FAIL;
        }
    } else {
        TT_ERROR("invalid family: %d", family);
        return TT_FAIL;
    }
}

int __skt_ev_init(IN tt_io_ev_t *io_ev, IN tt_u32_t ev)
{
    io_ev->src = tt_current_fiber();
    io_ev->dst = NULL;
    tt_dnode_init(&io_ev->node);
    io_ev->io = TT_IO_SOCKET;
    io_ev->ev = ev;

    return io_ev->src->fs->thread->task->iop.sys_iop.kq;
}

tt_result_t __addr_to_mreq(IN tt_sktaddr_ip_t *addr,
                           IN const tt_char_t *itf,
                           OUT struct ip_mreq *mreq,
                           IN int skt)
{
    // multicast address
    mreq->imr_multiaddr.s_addr = addr->a32.__u32;

    // local interface
    if (itf != NULL) {
        struct ifreq ifr;

        // set family
        ifr.ifr_addr.sa_family = AF_INET;

        // if name
        strncpy(ifr.ifr_name, itf, IFNAMSIZ - 1);

        // get if address
        if (ioctl(skt, SIOCGIFADDR, &ifr) != 0) {
            TT_ERROR_NTV("fail to get address of %s", itf);
            return TT_FAIL;
        }

        // return if address
        mreq->imr_interface.s_addr =
            ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
    } else {
        // use default interface
        mreq->imr_interface.s_addr = htonl(INADDR_ANY);
    }

    return TT_SUCCESS;
}

tt_result_t __mcaddr_to_mreq6(IN tt_sktaddr_ip_t *addr,
                              IN const tt_char_t *itf,
                              OUT struct ipv6_mreq *mreq)
{
    // multicast address
    tt_memcpy(mreq->ipv6mr_multiaddr.s6_addr, addr->a128.__u8, 16);

    // local interface
    if (itf != NULL) {
        unsigned int if_idx = if_nametoindex(itf);

        // get if index
        if (if_idx == 0) {
            TT_ERROR_NTV("fail to get if idx of %s", itf);
            return TT_FAIL;
        }

        // return if address
        mreq->ipv6mr_interface = if_idx;
    } else {
        // use default interface
        mreq->ipv6mr_interface = 0;
    }

    return TT_SUCCESS;
}
