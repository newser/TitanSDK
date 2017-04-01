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
#include <init/tt_profile.h>
#include <io/tt_socket.h>
#include <io/tt_socket_aio.h>
#include <log/tt_log.h>

#include <tt_wchar.h>

#include <ifdef.h>
#include <netioapi.h>
#include <ws2tcpip.h>

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

static tt_result_t __mcaddr_to_ipmreq(IN tt_sktaddr_ip_t *mc_addr,
                                      IN tt_char_t *mcast_itf,
                                      OUT struct ip_mreq *mreq);
static tt_result_t __mcaddr_to_ipv6mreq(IN tt_sktaddr_ip_t *mc_addr,
                                        IN tt_char_t *mcast_itf,
                                        OUT struct ipv6_mreq *mreq);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_skt_component_init_ntv(IN tt_profile_t *profile)
{
    return TT_SUCCESS;
}

tt_result_t tt_skt_create_ntv(OUT tt_skt_ntv_t *skt,
                              IN tt_net_family_t family,
                              IN tt_net_protocol_t protocol,
                              IN struct tt_skt_attr_s *attr)
{
    SOCKET __skt;
    int af;
    int type;
    int __protocol;

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
    if (__skt == INVALID_SOCKET) {
        TT_NET_ERROR_NTV("fail to create socket");
        return TT_FAIL;
    }

    skt->s = __skt;
    skt->evc = NULL;
    skt->ev_mark = TT_EVP_MARK_EV_END;
    skt->role = TT_SKT_ROLE_NUM;
    skt->aio_rd = NULL;
    skt->aio_wr = NULL;

    tt_list_init(&skt->read_q);
    tt_list_init(&skt->write_q);

    skt->ssl = NULL;

    return TT_SUCCESS;
}

tt_result_t tt_skt_destroy_ntv(IN tt_skt_ntv_t *skt)
{
    if (skt->evc != NULL) {
        TT_ERROR("can not destroy an async socket synchronously");
        return TT_FAIL;
    }

    if (closesocket(skt->s) == 0) {
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("fail to destroy skt");
        return TT_FAIL;
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
        TT_NET_ERROR_NTV("listening fail");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_accept_ntv(IN tt_skt_ntv_t *skt,
                              OUT tt_skt_ntv_t *new_socket,
                              OUT tt_sktaddr_ntv_t *remote_addr)
{
    SOCKET __skt;
    SOCKADDR_STORAGE *addr = remote_addr;
    int addr_len = sizeof(SOCKADDR_STORAGE);

    if (skt->evc != NULL) {
        TT_ERROR("can not accept an async socket synchronously");
        return TT_FAIL;
    }

    __skt = accept(skt->s, (SOCKADDR *)addr, &addr_len);
    if (__skt == INVALID_SOCKET) {
        TT_NET_ERROR_NTV("accept fail");
        return TT_FAIL;
    }

    // need check ??
    if (addr->ss_family == AF_INET) {
        if (addr_len < sizeof(SOCKADDR_IN)) {
            TT_ERROR("length of address[%d] is less than sockaddr_in[%d]",
                     addr_len,
                     sizeof(SOCKADDR_IN));

            closesocket(__skt);
            return TT_FAIL;
        }
    } else if (addr->ss_family == AF_INET6) {
        if (addr_len < sizeof(SOCKADDR_IN6)) {
            TT_ERROR("length of address[%d] is less than sockaddr_in[%d]",
                     addr_len,
                     sizeof(SOCKADDR_IN6));

            closesocket(__skt);
            return TT_FAIL;
        }
    } else {
        TT_ERROR("unsupported family %d", addr->ss_family);

        closesocket(__skt);
        return TT_FAIL;
    }

    new_socket->s = __skt;
    new_socket->evc = NULL;
    new_socket->ev_mark = TT_EVP_MARK_EV_END;
    new_socket->role = TT_SKT_ROLE_NUM;

    tt_list_init(&new_socket->read_q);
    tt_list_init(&new_socket->write_q);

    skt->ssl = NULL;

    return TT_SUCCESS;
}

tt_result_t tt_skt_connect_ntv(IN tt_skt_ntv_t *skt,
                               IN tt_sktaddr_ntv_t *remote_addr)
{
    if (skt->evc != NULL) {
        TT_ERROR("can not connect an async socket synchronously");
        return TT_FAIL;
    }

    if (connect(skt->s, (SOCKADDR *)remote_addr, sizeof(SOCKADDR_STORAGE)) ==
        0) {
        return TT_SUCCESS;
    } else {
        TT_NET_ERROR_NTV("connect fail");
        return TT_FAIL;
    }
}

tt_result_t tt_skt_local_addr_ntv(IN tt_skt_ntv_t *skt,
                                  OUT tt_sktaddr_ntv_t *local_addr)
{
    SOCKADDR_STORAGE *addr = local_addr;
    int addr_len = sizeof(SOCKADDR_STORAGE);

    if (getsockname(skt->s, (SOCKADDR *)addr, &addr_len) != 0) {
        TT_NET_ERROR_NTV("fail to get local address");
        return TT_FAIL;
    }

    // need check ??
    if (addr->ss_family == AF_INET) {
        if (addr_len < sizeof(SOCKADDR_IN)) {
            TT_ERROR("length of address[%d] is less than sockaddr_in[%d]",
                     addr_len,
                     sizeof(SOCKADDR_IN));
            return TT_FAIL;
        }
    } else if (addr->ss_family == AF_INET6) {
        if (addr_len < sizeof(SOCKADDR_IN6)) {
            TT_ERROR("length of address[%d] is less than sockaddr_in[%d]",
                     addr_len,
                     sizeof(SOCKADDR_IN6));
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
    SOCKADDR_STORAGE *addr = remote_addr;
    int addr_len = sizeof(SOCKADDR_STORAGE);

    if (getpeername(skt->s, (SOCKADDR *)addr, &addr_len) != 0) {
        TT_NET_ERROR_NTV("fail to get remote address");
        return TT_FAIL;
    }

    // need check ??
    if (addr->ss_family == AF_INET) {
        if (addr_len < sizeof(SOCKADDR_IN)) {
            TT_ERROR("length of address[%d] is less than sockaddr_in[%d]",
                     addr_len,
                     sizeof(SOCKADDR_IN));
            return TT_FAIL;
        }
    } else if (addr->ss_family == AF_INET6) {
        if (addr_len < sizeof(SOCKADDR_IN6)) {
            TT_ERROR("length of address[%d] is less than sockaddr_in[%d]",
                     addr_len,
                     sizeof(SOCKADDR_IN6));
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
    int __recv_len = 0;
    SOCKADDR_STORAGE *addr = remote_addr;
    int addr_len = (int)sizeof(SOCKADDR_STORAGE);

    if (skt->evc != NULL) {
        TT_ERROR("can not recvfrom an async socket synchronously");
        return TT_FAIL;
    }

    if (addr != NULL) {
        __recv_len =
            recvfrom(skt->s, buf, buf_len, 0, (SOCKADDR *)addr, &addr_len);
    } else {
        __recv_len = recvfrom(skt->s, buf, buf_len, 0, NULL, NULL);
    }
    if (__recv_len == SOCKET_ERROR) {
        TT_NET_ERROR_NTV("recvfrom fail");
        return TT_FAIL;
    } else if (__recv_len == 0) {
        return TT_END;
    } else {
        *recv_len = __recv_len;
        return TT_SUCCESS;
    }
}

tt_result_t tt_skt_sendto_ntv(IN tt_skt_ntv_t *skt,
                              IN tt_u8_t *buf,
                              IN tt_u32_t buf_len,
                              OUT tt_u32_t *send_len,
                              IN tt_sktaddr_ntv_t *remote_addr)
{
    int __send_len = 0;
    SOCKADDR_STORAGE *addr = remote_addr;
    int addr_len = sizeof(SOCKADDR_STORAGE);

    if (skt->evc != NULL) {
        TT_ERROR("can not sendto an async socket synchronously");
        return TT_FAIL;
    }

    __send_len = sendto(skt->s,
                        (const char *)buf,
                        buf_len,
                        0,
                        (SOCKADDR *)addr,
                        addr_len);
    if (__send_len == SOCKET_ERROR) {
        TT_NET_ERROR_NTV("send fail");
        return TT_FAIL;
    } else if (__send_len == 0) {
        TT_ERROR_NTV("send 0 bytes");
        return TT_FAIL;
    }

    *send_len = __send_len;
    return TT_SUCCESS;
}

tt_result_t tt_skt_send_ntv(IN tt_skt_ntv_t *skt,
                            OUT tt_u8_t *buf,
                            IN tt_u32_t buf_len,
                            OUT tt_u32_t *send_len)
{
    int __send_len = 0;

    if (skt->evc != NULL) {
        TT_ERROR("can not send an async socket synchronously");
        return TT_FAIL;
    }

    __send_len = send(skt->s, buf, buf_len, 0);
    if (__send_len == SOCKET_ERROR) {
        TT_NET_ERROR_NTV("send fail");
        return TT_FAIL;
    } else if (__send_len == 0) {
        TT_ERROR_NTV("send 0 bytes");
        return TT_FAIL;
    }

    *send_len = __send_len;
    return TT_SUCCESS;
}

tt_result_t tt_skt_recv_ntv(IN tt_skt_ntv_t *skt,
                            IN tt_u8_t *buf,
                            IN tt_u32_t buf_len,
                            OUT tt_u32_t *recv_len)
{
    int __recv_len = 0;

    if (skt->evc != NULL) {
        TT_ERROR("can not recv an async socket synchronously");
        return TT_FAIL;
    }

    __recv_len = recv(skt->s, buf, buf_len, 0);
    if (__recv_len == SOCKET_ERROR) {
        TT_NET_ERROR_NTV("send fail");
        return TT_FAIL;
    } else if (__recv_len == 0) {
        return TT_END;
    } else {
        *recv_len = __recv_len;
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
        if (!TT_OK(__mcaddr_to_ipmreq(mc_addr, mcast_itf, &mreq))) {
            return TT_FAIL;
        }

        // join multicast group
        if (setsockopt(skt->s,
                       IPPROTO_IP,
                       IP_ADD_MEMBERSHIP,
                       (char *)&mreq,
                       (int)sizeof(struct ip_mreq)) == 0) {
            return TT_SUCCESS;
        } else {
            TT_NET_ERROR_NTV("fail to join multicast group");
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
                       IPV6_ADD_MEMBERSHIP,
                       (char *)&mreq,
                       (int)sizeof(struct ipv6_mreq)) == 0) {
            return TT_SUCCESS;
        } else {
            TT_NET_ERROR_NTV("fail to join multicast group");
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
        if (!TT_OK(__mcaddr_to_ipmreq(mc_addr, mcast_itf, &mreq))) {
            return TT_FAIL;
        }

        // leave multicast group
        if (setsockopt(skt->s,
                       IPPROTO_IP,
                       IP_DROP_MEMBERSHIP,
                       (char *)&mreq,
                       (int)sizeof(struct ip_mreq)) == 0) {
            return TT_SUCCESS;
        } else {
            TT_NET_ERROR_NTV("fail to join multicast group");
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
                       IPV6_DROP_MEMBERSHIP,
                       (char *)&mreq,
                       (int)sizeof(struct ipv6_mreq)) == 0) {
            return TT_SUCCESS;
        } else {
            TT_NET_ERROR_NTV("fail to join multicast group");
            return TT_FAIL;
        }
    } else {
        TT_ERROR("invalid family: %d", family);
        return TT_FAIL;
    }
}

tt_result_t __mcaddr_to_ipmreq(IN tt_sktaddr_ip_t *mc_addr,
                               IN tt_char_t *mcast_itf,
                               OUT struct ip_mreq *mreq)
{
    // multicast address
    mreq->imr_multiaddr.s_addr = mc_addr->a32.__u32;

    // local interface
    if (mcast_itf != NULL) {
        WCHAR *InterfaceName;
        NET_LUID luid;
        NET_IFINDEX ifidx;
        NETIO_STATUS ns;

        InterfaceName = tt_wchar_create(mcast_itf, NULL);
        if (InterfaceName == NULL) {
            return TT_FAIL;
        }

        // get network interface index
        ns = ConvertInterfaceNameToLuidW(InterfaceName, &luid);
        tt_wchar_destroy(InterfaceName);
        if (ns != 0) {
            TT_ERROR_NTV("can not get luid of %s", mcast_itf);
            return TT_FAIL;
        }
        if (ConvertInterfaceLuidToIndex(&luid, &ifidx) != NO_ERROR) {
            TT_ERROR_NTV("can not get index from luid of %s", mcast_itf);
            return TT_FAIL;
        }

        // windows accept setting imr_interface to address like
        // "0.x.x.x" where "x.x.x" represent the interface index
        if (ifidx >= (1 << 24)) {
            TT_ERROR("invalid interface index: %x", ifidx);
            return TT_FAIL;
        }
        mreq->imr_interface.s_addr = htonl(ifidx);
    } else {
        // use default interface
        memset(&mreq->imr_interface, 0, sizeof(struct in_addr));
    }

    return TT_SUCCESS;
}

tt_result_t __mcaddr_to_ipv6mreq(IN tt_sktaddr_ip_t *mc_addr,
                                 IN tt_char_t *mcast_itf,
                                 OUT struct ipv6_mreq *mreq)
{
    // multicast address
    tt_memcpy(mreq->ipv6mr_multiaddr.s6_bytes, mc_addr->a128.__u8, 16);

    // local interface
    if (mcast_itf != NULL) {
        WCHAR *InterfaceName;
        NET_LUID luid;
        NET_IFINDEX ifidx;
        NETIO_STATUS ns;

        InterfaceName = tt_wchar_create(mcast_itf, NULL);
        if (InterfaceName == NULL) {
            return TT_FAIL;
        }

        // get network interface index
        ns = ConvertInterfaceNameToLuidW(InterfaceName, &luid);
        tt_wchar_destroy(InterfaceName);
        if (ns != 0) {
            TT_ERROR_NTV("can not get luid of %s", mcast_itf);
            return TT_FAIL;
        }
        if (ConvertInterfaceLuidToIndex(&luid, &ifidx) != NO_ERROR) {
            TT_ERROR_NTV("can not get index from luid of %s", mcast_itf);
            return TT_FAIL;
        }
        mreq->ipv6mr_interface = ifidx;
    } else {
        // use default interface
        mreq->ipv6mr_interface = 0;
    }

    return TT_SUCCESS;
}
