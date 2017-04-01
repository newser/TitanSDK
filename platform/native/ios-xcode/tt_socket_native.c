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

#include <event/tt_event_center.h>
#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <io/tt_socket.h>
#include <io/tt_socket_aio.h>
#include <log/tt_log.h>

#include <tt_cstd_api.h>

#include <net/if.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <unistd.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __SOCKET_BACKLOG_DEFAULT SOMAXCONN

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#define close __close_socket
static int __close_socket(int skt);

static tt_result_t __mcaddr_to_ipmreq(IN tt_sktaddr_ip_t *mc_addr,
                                      IN tt_char_t *mcast_itf,
                                      OUT struct ip_mreq *mreq,
                                      IN int skt);
static tt_result_t __mcaddr_to_ipv6mreq(IN tt_sktaddr_ip_t *mc_addr,
                                        IN tt_char_t *mcast_itf,
                                        OUT struct ipv6_mreq *mreq);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_skt_component_init_ntv(IN tt_profile_t *profile)
{
    tt_result_t result = TT_FAIL;

    result = tt_skt_aio_component_init(profile);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_skt_create_ntv(OUT tt_skt_ntv_t *skt,
                              IN tt_net_family_t family,
                              IN tt_net_protocol_t protocol,
                              IN struct tt_skt_attr_s *attr)
{
    int __skt;
    int af;
    int type;
    int __protocol;
    int nosigpipe = 1;

    // convert family
    if (family == TT_NET_AF_INET) {
        af = AF_INET;
    } else if (family == TT_NET_AF_INET6) {
        af = AF_INET6;
    } else {
        TT_ERROR("invalid family: %d", family);
        return TT_FAIL;
    }

    // convert type
    if (protocol == TT_NET_PROTO_TCP) {
        type = SOCK_STREAM;
        __protocol = IPPROTO_TCP;
    } else if (protocol == TT_NET_PROTO_UDP) {
        type = SOCK_DGRAM;
        __protocol = IPPROTO_UDP;
    } else {
        TT_ERROR("invalid protocol: %d", protocol);
        return TT_FAIL;
    }

    // create socket
    __skt = socket(af, type, __protocol);
    if (__skt < 0) {
        TT_ERROR_NTV("fail to create socket");
        return TT_FAIL;
    }
    tt_skt_stat_inc_num();

    // never use SIGPIPE
    if (setsockopt(__skt,
                   SOL_SOCKET,
                   SO_NOSIGPIPE,
                   &nosigpipe,
                   sizeof(nosigpipe)) != 0) {
        TT_ERROR_NTV("fail to set SO_NOSIGPIPE");

        while ((close(__skt) != 0) && (errno == EINTR))
            ;
        return TT_FAIL;
    }

    skt->s = __skt;
    skt->evc = NULL;
    skt->ev_mark_rd = TT_SKT_MARK_KQ_RD;
    skt->ev_mark_wr = TT_SKT_MARK_KQ_WR;
    skt->role = TT_SKT_ROLE_NUM;

    tt_list_init(&skt->read_q);
    tt_list_init(&skt->write_q);

    // left securty 0
    skt->ssl = NULL;

    return TT_SUCCESS;
}

tt_result_t tt_skt_destroy_ntv(IN tt_skt_ntv_t *skt)
{
#if 0 // we decide to expose a way for user to force a socket closing
    if (skt->evc != NULL)
    {
        TT_ERROR("can not destroy an async socket synchronously");
        return TT_FAIL;
    }
#endif

__retry:
    if (close(skt->s) == 0) {
        return TT_SUCCESS;
    } else {
        if (errno == EINTR) {
            goto __retry;
        } else {
            TT_ERROR_NTV("fail to destroy skt");
            return TT_FAIL;
        }
    }
}

tt_result_t tt_skt_listen_ntv(IN tt_skt_ntv_t *skt, IN tt_u32_t backlog)
{
    int sys_backlog = backlog;

    if (sys_backlog == TT_SKT_BACKLOG_DEFAULT) {
        sys_backlog = __SOCKET_BACKLOG_DEFAULT;
    }

    if (listen(skt->s, sys_backlog) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("listening fail");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_accept_ntv(IN tt_skt_ntv_t *skt,
                              OUT tt_skt_ntv_t *new_socket,
                              OUT tt_sktaddr_ntv_t *remote_addr)
{
    int __skt;
    struct sockaddr_storage *addr = remote_addr;
    socklen_t addr_len = sizeof(struct sockaddr_storage);

    if (skt->evc != NULL) {
        TT_ERROR("can not accept an async socket synchronously");
        return TT_FAIL;
    }

__retry:
    __skt = accept(skt->s, (struct sockaddr *)addr, &addr_len);
    if (__skt < 0) {
        if (errno == EINTR) {
            goto __retry;
        } else {
            TT_ERROR_NTV("accept fail");
            return TT_FAIL;
        }
    }
    tt_skt_stat_inc_num();

    // need check ??
    if (addr->ss_family == AF_INET) {
        if (addr_len < sizeof(struct sockaddr_in)) {
            TT_ERROR("length of address[%d] is less than sockaddr_in[%d]",
                     addr_len,
                     sizeof(struct sockaddr_in));

            close(__skt);
            return TT_FAIL;
        }
    } else if (addr->ss_family == AF_INET6) {
        if (addr_len < sizeof(struct sockaddr_in6)) {
            TT_ERROR("length of address[%d] is less than sockaddr_in[%d]",
                     addr_len,
                     sizeof(struct sockaddr_in6));

            close(__skt);
            return TT_FAIL;
        }
    } else {
        TT_ERROR("unsupported family %d", addr->ss_family);

        close(__skt);
        return TT_FAIL;
    }

    new_socket->s = __skt;
    new_socket->evc = NULL;
    new_socket->ev_mark_rd = TT_SKT_MARK_KQ_RD;
    new_socket->ev_mark_wr = TT_SKT_MARK_KQ_WR;
    new_socket->role = TT_SKT_ROLE_NUM;

    tt_list_init(&new_socket->read_q);
    tt_list_init(&new_socket->write_q);

    // left securty 0
    new_socket->ssl = NULL;

    return TT_SUCCESS;
}

tt_result_t tt_skt_connect_ntv(IN tt_skt_ntv_t *skt,
                               IN tt_sktaddr_ntv_t *remote_addr)
{
    if (skt->evc != NULL) {
        TT_ERROR("can not connect an async socket synchronously");
        return TT_FAIL;
    }

    if (connect(skt->s, (struct sockaddr *)remote_addr, remote_addr->ss_len) ==
        0) {
        return TT_SUCCESS;
    } else {
        if (errno == EINTR) {
            // can not retry connect as SYN has been sent
            struct pollfd pfd;
            int skt_err = 0;
            socklen_t skt_err_len = sizeof(skt_err);

            // poll connect result
            pfd.fd = skt->s;
            pfd.events = POLLOUT;
            while (poll(&pfd, 1, -1) < 0) {
                if (errno != EINTR) {
                    TT_ERROR_NTV("polling connect fail");
                    return TT_FAIL;
                }
                // continue if (errno == EINTR)
            }
            // polling done

            // check connect result
            if (getsockopt(skt->s,
                           SOL_SOCKET,
                           SO_ERROR,
                           &skt_err,
                           &skt_err_len) != 0) {
                TT_ERROR_NTV("fail to get connect result");
                return TT_FAIL;
            }

            if (skt_err != 0) {
                TT_ERROR("connect fail");
                return TT_FAIL;
            }

            // connect done
            return TT_SUCCESS;
        } else {
            TT_ERROR_NTV("connect fail");
            return TT_FAIL;
        }
    }
}

tt_result_t tt_skt_local_addr_ntv(IN tt_skt_ntv_t *skt,
                                  OUT tt_sktaddr_ntv_t *local_addr)
{
    struct sockaddr_storage *addr = local_addr;
    socklen_t addr_len = sizeof(struct sockaddr_storage);

    if (getsockname(skt->s, (struct sockaddr *)addr, &addr_len) != 0) {
        TT_ERROR_NTV("fail to get local address");
        return TT_FAIL;
    }

    // need check ??
    if (addr->ss_family == AF_INET) {
        if (addr_len < sizeof(struct sockaddr_in)) {
            TT_ERROR("length of address[%d] is less than sockaddr_in[%d]",
                     addr_len,
                     sizeof(struct sockaddr_in));
            return TT_FAIL;
        }
    } else if (addr->ss_family == AF_INET6) {
        if (addr_len < sizeof(struct sockaddr_in6)) {
            TT_ERROR("length of address[%d] is less than sockaddr_in[%d]",
                     addr_len,
                     sizeof(struct sockaddr_in6));
            return TT_FAIL;
        }
    } else {
        TT_ERROR("unsupported family %d", addr->ss_family);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_skt_remote_addr_ntv(IN tt_skt_ntv_t *skt,
                                   OUT tt_sktaddr_ntv_t *remote_addr)
{
    struct sockaddr_storage *addr = remote_addr;
    socklen_t addr_len = sizeof(struct sockaddr_storage);

    if (getpeername(skt->s, (struct sockaddr *)addr, &addr_len) != 0) {
        TT_ERROR_NTV("fail to get remote address");
        return TT_FAIL;
    }

    // need check ??
    if (addr->ss_family == AF_INET) {
        if (addr_len < sizeof(struct sockaddr_in)) {
            TT_ERROR("length of address[%d] is less than sockaddr_in[%d]",
                     addr_len,
                     sizeof(struct sockaddr_in));
            return TT_FAIL;
        }
    } else if (addr->ss_family == AF_INET6) {
        if (addr_len < sizeof(struct sockaddr_in6)) {
            TT_ERROR("length of address[%d] is less than sockaddr_in[%d]",
                     addr_len,
                     sizeof(struct sockaddr_in6));
            return TT_FAIL;
        }
    } else {
        TT_ERROR("unsupported family %d", addr->ss_family);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_skt_recvfrom_ntv(IN tt_skt_ntv_t *skt,
                                OUT tt_u8_t *buf,
                                IN tt_u32_t buf_len,
                                OUT tt_u32_t *recv_len,
                                OUT tt_sktaddr_ntv_t *remote_addr)
{
    ssize_t __recv_len = 0;
    struct sockaddr_storage *addr = remote_addr;
    socklen_t addr_len = (int)sizeof(struct sockaddr_storage);

    if (skt->evc != NULL) {
        TT_ERROR("can not recvfrom an async socket synchronously");
        return TT_FAIL;
    }

__retry:
    if (addr != NULL) {
        __recv_len = recvfrom(skt->s,
                              buf,
                              buf_len,
                              0,
                              (struct sockaddr *)addr,
                              &addr_len);
    } else {
        __recv_len = recvfrom(skt->s, buf, buf_len, 0, NULL, NULL);
    }
    if (__recv_len < 0) {
        if (errno == EINTR) {
            goto __retry;
        } else {
            TT_ERROR_NTV("recvfrom fail");
            return TT_FAIL;
        }
    } else if (__recv_len == 0) {
        return TT_END;
    } else {
        *recv_len = (tt_u32_t)__recv_len;
        return TT_SUCCESS;
    }
}

tt_result_t tt_skt_sendto_ntv(IN tt_skt_ntv_t *skt,
                              IN tt_u8_t *buf,
                              IN tt_u32_t buf_len,
                              OUT tt_u32_t *send_len,
                              IN tt_sktaddr_ntv_t *remote_addr)
{
    ssize_t __send_len = 0;
    struct sockaddr_storage *addr = remote_addr;
    socklen_t addr_len = remote_addr->ss_len;

    if (skt->evc != NULL) {
        TT_ERROR("can not sendto an async socket synchronously");
        return TT_FAIL;
    }

__retry:
    __send_len = sendto(skt->s,
                        (const char *)buf,
                        buf_len,
                        0,
                        (struct sockaddr *)addr,
                        addr_len);
    if (__send_len < 0) {
        if (errno == EINTR) {
            goto __retry;
        } else {
            TT_ERROR_NTV("send fail");
            return TT_FAIL;
        }
    } else if (__send_len == 0) {
        TT_ERROR("send 0 byte");
        return TT_FAIL;
    } else {
        *send_len = (tt_u32_t)__send_len;
        return TT_SUCCESS;
    }
}

tt_result_t tt_skt_send_ntv(IN tt_skt_ntv_t *skt,
                            OUT tt_u8_t *buf,
                            IN tt_u32_t buf_len,
                            OUT tt_u32_t *send_len)
{
    ssize_t __send_len = 0;

    if (skt->evc != NULL) {
        TT_ERROR("can not send an async socket synchronously");
        return TT_FAIL;
    }

__retry:
    __send_len = send(skt->s, buf, buf_len, 0);
    if (__send_len < 0) {
        if (errno == EINTR) {
            goto __retry;
        } else {
            TT_ERROR_NTV("send fail");
            return TT_FAIL;
        }
    } else if (__send_len == 0) {
        TT_ERROR("send 0 byte");
        return TT_FAIL;
    } else {
        *send_len = (tt_u32_t)__send_len;
        return TT_SUCCESS;
    }
}

tt_result_t tt_skt_recv_ntv(IN tt_skt_ntv_t *skt,
                            IN tt_u8_t *buf,
                            IN tt_u32_t buf_len,
                            OUT tt_u32_t *recv_len)
{
    ssize_t __recv_len = 0;

    if (skt->evc != NULL) {
        TT_ERROR("can not recv an async socket synchronously");
        return TT_FAIL;
    }

__retry:
    __recv_len = recv(skt->s, buf, buf_len, 0);
    if (__recv_len < 0) {
        if (errno == EINTR) {
            goto __retry;
        } else {
            TT_ERROR_NTV("send fail");
            return TT_FAIL;
        }
    } else if (__recv_len == 0) {
        return TT_END;
    } else {
        *recv_len = (tt_u32_t)__recv_len;
        return TT_SUCCESS;
    }
}

tt_result_t tt_skt_join_mcast_ntv(IN tt_skt_ntv_t *skt,
                                  IN tt_net_family_t family,
                                  IN tt_sktaddr_ip_t *mc_addr,
                                  IN tt_char_t *mcast_itf)
{
    if (family == TT_NET_AF_INET) {
        struct ip_mreq mreq;
        if (!TT_OK(__mcaddr_to_ipmreq(mc_addr, mcast_itf, &mreq, skt->s))) {
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
    } else if (family == TT_NET_AF_INET6) {
        struct ipv6_mreq mreq;
        if (!TT_OK(__mcaddr_to_ipv6mreq(mc_addr, mcast_itf, &mreq))) {
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
    } else {
        TT_ERROR("invalid family: %d", family);
        return TT_FAIL;
    }
}

tt_result_t tt_skt_leave_mcast_ntv(IN tt_skt_ntv_t *skt,
                                   IN tt_net_family_t family,
                                   IN tt_sktaddr_ip_t *mc_addr,
                                   IN tt_char_t *mcast_itf)
{
    if (family == TT_NET_AF_INET) {
        struct ip_mreq mreq;
        if (!TT_OK(__mcaddr_to_ipmreq(mc_addr, mcast_itf, &mreq, skt->s))) {
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
        if (!TT_OK(__mcaddr_to_ipv6mreq(mc_addr, mcast_itf, &mreq))) {
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

tt_result_t __mcaddr_to_ipmreq(IN tt_sktaddr_ip_t *mc_addr,
                               IN tt_char_t *mcast_itf,
                               OUT struct ip_mreq *mreq,
                               IN int skt)
{
    // multicast address
    mreq->imr_multiaddr.s_addr = mc_addr->a32.__u32;

    // local interface
    if (mcast_itf != NULL) {
        struct ifreq ifr;

        // set family
        ifr.ifr_addr.sa_family = AF_INET;

        // if name
        strncpy(ifr.ifr_name, mcast_itf, IFNAMSIZ - 1);

        // get if address
        if (ioctl(skt, SIOCGIFADDR, &ifr) != 0) {
            TT_ERROR_NTV("fail to get address of %s", mcast_itf);
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

tt_result_t __mcaddr_to_ipv6mreq(IN tt_sktaddr_ip_t *mc_addr,
                                 IN tt_char_t *mcast_itf,
                                 OUT struct ipv6_mreq *mreq)
{
    // multicast address
    tt_memcpy(mreq->ipv6mr_multiaddr.s6_addr, mc_addr->a128.__u8, 16);

    // local interface
    if (mcast_itf != NULL) {
        unsigned int if_idx = if_nametoindex(mcast_itf);

        // get if index
        if (if_idx == 0) {
            TT_ERROR_NTV("fail to get if idx of %s", mcast_itf);
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

#undef close
int __close_socket(int skt)
{
    if (skt >= 0) {
        tt_atomic_s64_dec(&tt_skt_stat_num);
    }
    return close(skt);
}
