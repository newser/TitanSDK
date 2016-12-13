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

#include <io/tt_socket.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <io/tt_socket_aio.h>
#include <io/tt_socket_option.h>
#include <misc/tt_util.h>

#include <tt_cstd_api.h>
#include <tt_socket_aio_native.h>
#include <tt_socket_native.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_atomic_s64_t tt_stat_socket_num;

tt_atomic_s64_t tt_stat_socket_peek;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __skt_component_init(IN tt_component_t *comp,
                                        IN tt_profile_t *profile);

tt_result_t __handle_skt_attr(IN tt_skt_t *skt);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_skt_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __skt_component_init,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_SOCKET, "Socket", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t tt_skt_create(IN tt_skt_t *skt,
                          IN tt_net_family_t family,
                          IN tt_net_protocol_t protocol,
                          IN OPT tt_skt_attr_t *attr)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(family));
    TT_ASSERT(TT_NET_PROTO_VALID(protocol));

    tt_memset(skt, 0, sizeof(tt_skt_t));

    if (attr != NULL) {
        tt_memcpy(&skt->attr, attr, sizeof(tt_skt_attr_t));
    } else {
        tt_skt_attr_default(&skt->attr);
    }

    if (!TT_OK(tt_skt_create_ntv(&skt->sys_socket,
                                 family,
                                 protocol,
                                 &skt->attr))) {
        return TT_FAIL;
    }

    skt->family = family;
    skt->protocol = protocol;

    if (!TT_OK(__handle_skt_attr(skt))) {
        tt_skt_destroy_ntv(&skt->sys_socket);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_skt_destroy(IN tt_skt_t *skt)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_destroy_ntv(&skt->sys_socket);
}

tt_result_t tt_skt_bind(IN tt_skt_t *skt, IN tt_sktaddr_t *local_addr)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(skt->family));
    TT_ASSERT(local_addr != NULL);

    return tt_skt_bind_ntv(&skt->sys_socket, local_addr);
}

tt_result_t tt_skt_bind_n(IN tt_skt_t *skt,
                          IN tt_net_family_t family,
                          IN tt_sktaddr_addr_t *addr,
                          IN tt_u16_t port)
{
    tt_sktaddr_t local_addr;

    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(family));

    tt_sktaddr_init(&local_addr, family);
    tt_sktaddr_set_addr_n(&local_addr, addr);
    tt_sktaddr_set_port(&local_addr, port);

    return tt_skt_bind(skt, &local_addr);
}

tt_result_t tt_skt_bind_p(IN tt_skt_t *skt,
                          IN tt_net_family_t family,
                          IN tt_char_t *addr,
                          IN tt_u16_t port)
{
    tt_sktaddr_t local_addr;

    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(family));
    TT_ASSERT(addr != NULL);

    tt_sktaddr_init(&local_addr, family);

    if (!TT_OK(tt_sktaddr_set_addr_p(&local_addr, addr))) {
        return TT_FAIL;
    }

    tt_sktaddr_set_port(&local_addr, port);

    return tt_skt_bind(skt, &local_addr);
}

tt_result_t tt_skt_listen(IN tt_skt_t *skt, IN tt_u32_t backlog)
{
    TT_ASSERT(skt != NULL);

    if (skt->protocol != TT_NET_PROTO_TCP) {
        TT_ERROR("not tcp socket to listen");
        return TT_FAIL;
    }

    return tt_skt_listen_ntv(&skt->sys_socket, backlog);
}

tt_result_t tt_skt_accept(IN tt_skt_t *skt,
                          OUT tt_skt_t *new_skt,
                          IN OPT tt_skt_attr_t *new_skt_attr)
{
    tt_sktaddr_t remote_addr;
    tt_sktaddr_t local_addr;
    tt_net_family_t af;

    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(skt->family));
    TT_ASSERT(new_skt != NULL);

    if (skt->protocol != TT_NET_PROTO_TCP) {
        TT_ERROR("not tcp socket to accept");
        return TT_FAIL;
    }

    tt_memset(new_skt, 0, sizeof(tt_skt_t));

    if (!TT_OK(tt_skt_accept_ntv(&skt->sys_socket,
                                 &new_skt->sys_socket,
                                 &remote_addr))) {
        return TT_FAIL;
    }

    // get family of the new socket
    if (!TT_OK(tt_skt_local_addr(new_skt, &local_addr))) {
        tt_skt_destroy_ntv(&new_skt->sys_socket);
        return TT_FAIL;
    }

    af = tt_sktaddr_get_family(&local_addr);
    if (!TT_NET_AF_VALID(af)) {
        return TT_FAIL;
    }
    new_skt->family = af;

    new_skt->protocol = TT_NET_PROTO_TCP;

    if (new_skt_attr != NULL) {
        tt_memcpy(&new_skt->attr, new_skt_attr, sizeof(tt_skt_attr_t));
    } else {
        tt_skt_attr_default(&new_skt->attr);
    }

    if (!TT_OK(__handle_skt_attr(new_skt))) {
        tt_skt_destroy_ntv(&new_skt->sys_socket);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_skt_connect(IN tt_skt_t *skt, IN tt_sktaddr_t *remote_addr)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(skt->family));
    TT_ASSERT(remote_addr != NULL);

    return tt_skt_connect_ntv(&skt->sys_socket, remote_addr);
}

tt_result_t tt_skt_connect_n(IN tt_skt_t *skt,
                             IN tt_net_family_t family,
                             IN tt_sktaddr_addr_t *addr,
                             IN tt_u16_t port)
{
    tt_sktaddr_t remote_addr;

    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(family));
    TT_ASSERT(addr != NULL);

    tt_sktaddr_init(&remote_addr, family);
    tt_sktaddr_set_addr_n(&remote_addr, addr);
    tt_sktaddr_set_port(&remote_addr, port);

    return tt_skt_connect(skt, &remote_addr);
}


tt_result_t tt_skt_connect_p(IN tt_skt_t *skt,
                             IN tt_net_family_t family,
                             IN tt_char_t *addr,
                             IN tt_u16_t port)
{
    tt_sktaddr_t remote_addr;

    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(family));
    TT_ASSERT(addr != NULL);

    tt_sktaddr_init(&remote_addr, family);

    if (!TT_OK(tt_sktaddr_set_addr_p(&remote_addr, addr))) {
        return TT_FAIL;
    }

    tt_sktaddr_set_port(&remote_addr, port);

    return tt_skt_connect(skt, &remote_addr);
}

tt_result_t tt_skt_send(IN tt_skt_t *skt,
                        OUT tt_u8_t *buf,
                        IN tt_u32_t buf_len,
                        OUT OPT tt_u32_t *send_len)
{
    tt_u32_t pos = 0;

    TT_ASSERT(skt != NULL);
    TT_ASSERT(buf != NULL);

    if (skt->protocol != TT_NET_PROTO_TCP) {
        TT_ERROR("can only be called by tcp socket");
        return TT_FAIL;
    }

    if (buf_len == 0) {
        TT_SAFE_ASSIGN(send_len, 0);
        return TT_SUCCESS;
    }

    while (pos < buf_len) {
        tt_u32_t len = 0;

        if (TT_OK(tt_skt_send_ntv(&skt->sys_socket,
                                  buf + pos,
                                  buf_len - pos,
                                  &len))) {
            pos += len;
        } else {
            break;
        }
    }
    TT_ASSERT(pos <= buf_len);

    if (pos > 0) {
        TT_SAFE_ASSIGN(send_len, pos);
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_skt_recv(IN tt_skt_t *skt,
                        IN tt_u8_t *buf,
                        IN tt_u32_t buf_len,
                        OUT OPT tt_u32_t *recv_len)
{
    tt_u32_t pos = 0;
    tt_result_t result = TT_SUCCESS;

    TT_ASSERT(skt != NULL);
    TT_ASSERT(buf != NULL);

    if (skt->protocol != TT_NET_PROTO_TCP) {
        TT_ERROR("can only be called by tcp socket");
        return TT_FAIL;
    }

    if (buf_len == 0) {
        TT_SAFE_ASSIGN(recv_len, 0);
        return TT_SUCCESS;
    }

    while (pos < buf_len) {
        tt_u32_t len = 0;

        result =
            tt_skt_recv_ntv(&skt->sys_socket, buf + pos, buf_len - pos, &len);
        if (TT_OK(result)) {
            pos += len;
        } else {
            break;
        }
    }
    TT_ASSERT(pos <= buf_len);

    if (pos > 0) {
        TT_SAFE_ASSIGN(recv_len, pos);
        return TT_SUCCESS;
    } else if (result == TT_END) {
        return TT_END;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_skt_recvfrom(IN tt_skt_t *skt,
                            OUT tt_u8_t *buf,
                            IN tt_u32_t buf_len,
                            OUT OPT tt_u32_t *recv_len,
                            OUT OPT tt_sktaddr_t *remote_addr)
{
    tt_u32_t len;
    tt_sktaddr_t addr;

    TT_ASSERT(skt != NULL);
    TT_ASSERT(buf != NULL);

    if (skt->protocol != TT_NET_PROTO_UDP) {
        TT_ERROR("can only be called by udp socket");
        return TT_FAIL;
    }

    if (buf_len == 0) {
        TT_SAFE_ASSIGN(recv_len, 0);
        return TT_SUCCESS;
    }

    if (remote_addr == NULL) {
        remote_addr = &addr;
    }

    if (TT_OK(tt_skt_recvfrom_ntv(&skt->sys_socket,
                                  buf,
                                  buf_len,
                                  &len,
                                  remote_addr))) {
        TT_SAFE_ASSIGN(recv_len, len);
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_skt_sendto(IN tt_skt_t *skt,
                          IN tt_u8_t *buf,
                          IN tt_u32_t buf_len,
                          OUT OPT tt_u32_t *send_len,
                          IN tt_sktaddr_t *remote_addr)
{
    tt_u32_t len;

    TT_ASSERT(skt != NULL);
    TT_ASSERT(buf != NULL);
    TT_ASSERT(remote_addr != NULL);

    if (skt->protocol != TT_NET_PROTO_UDP) {
        TT_ERROR("can only be called by udp socket");
        return TT_FAIL;
    }

    if (buf_len == 0) {
        TT_SAFE_ASSIGN(send_len, 0);
        return TT_SUCCESS;
    }

    if (TT_OK(tt_skt_sendto_ntv(&skt->sys_socket,
                                buf,
                                buf_len,
                                &len,
                                remote_addr))) {
        TT_SAFE_ASSIGN(send_len, len);
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_skt_local_addr(IN tt_skt_t *skt, IN tt_sktaddr_t *addr)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(addr != NULL);

    return tt_skt_local_addr_ntv(&skt->sys_socket, addr);
}

tt_result_t tt_skt_remote_addr(IN tt_skt_t *skt, IN tt_sktaddr_t *addr)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(addr != NULL);

    return tt_skt_remote_addr_ntv(&skt->sys_socket, addr);
}

void tt_skt_stat_show(IN tt_u32_t flag)
{
    TT_INFO("-------------------------------------");
    TT_INFO("[SOCKET] statistic");

    TT_INFO("- socket:");
    TT_INFO("    current socket: %ld", tt_atomic_s64_get(&tt_stat_socket_num));
    TT_INFO("    peak socket num: %ld",
            tt_atomic_s64_get(&tt_stat_socket_peek));

    TT_INFO("-------------------------------------");
}

void tt_skt_stat_inc_num()
{
    tt_s64_t skt_num = tt_atomic_s64_inc(&tt_stat_socket_num);

    // the peek value is not accurate, just for reference
    if (skt_num > tt_atomic_s64_get(&tt_stat_socket_peek)) {
        tt_atomic_s64_set(&tt_stat_socket_peek, skt_num);
    }
}

void tt_skt_stat_dec_num()
{
    tt_atomic_s64_dec(&tt_stat_socket_num);
}

tt_result_t tt_skt_join_mcast(IN tt_skt_t *skt,
                              IN tt_sktaddr_addr_t *mc_addr,
                              IN OPT tt_char_t *mcast_itf)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(mc_addr != NULL);

    if (skt->protocol != TT_NET_PROTO_UDP) {
        TT_ERROR("only support udp multicast");
        return TT_FAIL;
    }

    return tt_skt_join_mcast_ntv(&skt->sys_socket,
                                 skt->family,
                                 mc_addr,
                                 mcast_itf);
}

tt_result_t tt_skt_leave_mcast(IN tt_skt_t *skt,
                               IN tt_sktaddr_addr_t *mc_addr,
                               IN OPT tt_char_t *mcast_itf)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(mc_addr != NULL);

    if (skt->protocol != TT_NET_PROTO_UDP) {
        TT_ERROR("only support udp multicast");
        return TT_FAIL;
    }

    return tt_skt_leave_mcast_ntv(&skt->sys_socket,
                                  skt->family,
                                  mc_addr,
                                  mcast_itf);
}

tt_result_t tt_tcp_server(OUT tt_skt_t *skt,
                          IN tt_net_family_t family,
                          IN tt_skt_attr_t *attr,
                          IN tt_sktaddr_t *local_addr,
                          IN tt_u32_t backlog)
{
    if (!TT_OK(tt_skt_create(skt, family, TT_NET_PROTO_TCP, attr))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_bind(skt, local_addr))) {
        tt_skt_destroy(skt);
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_listen(skt, backlog))) {
        tt_skt_destroy(skt);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_udp_server(OUT tt_skt_t *skt,
                          IN tt_net_family_t family,
                          IN tt_skt_attr_t *attr,
                          IN tt_sktaddr_t *local_addr)
{
    if (!TT_OK(tt_skt_create(skt, family, TT_NET_PROTO_UDP, attr))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_bind(skt, local_addr))) {
        tt_skt_destroy(skt);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_udp_server_mcast(OUT tt_skt_t *skt,
                                IN tt_net_family_t family,
                                IN tt_skt_attr_t *attr,
                                IN tt_sktaddr_t *local_addr,
                                IN OPT tt_sktaddr_addr_t *mcast_addr,
                                IN OPT tt_char_t *mcast_itf)
{
    if (!TT_OK(tt_skt_create(skt, family, TT_NET_PROTO_UDP, attr))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_bind(skt, local_addr))) {
        tt_skt_destroy(skt);
        return TT_FAIL;
    }

    if ((mcast_addr != NULL) &&
        !TT_OK(tt_skt_join_mcast(skt, mcast_addr, mcast_itf))) {
        tt_skt_destroy(skt);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_skt_attr_default(IN tt_skt_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_memset(attr, 0, sizeof(tt_skt_attr_t));
}

tt_result_t __skt_component_init(IN tt_component_t *comp,
                                 IN tt_profile_t *profile)
{
    tt_result_t result;

    // init low level socket system
    result = tt_skt_component_init_ntv(profile);
    if (!TT_OK(result)) {
        TT_ERROR("fail to initialize socket system native");
        return TT_FAIL;
    }

    // init async io
    result = tt_skt_aio_component_init(profile);
    if (!TT_OK(result)) {
        TT_ERROR("fail to initialize socket system native");
        return TT_FAIL;
    }

    tt_atomic_s64_init(&tt_stat_socket_num, 0);
    tt_atomic_s64_init(&tt_stat_socket_peek, 0);

    return TT_SUCCESS;
}

tt_result_t __handle_skt_attr(IN tt_skt_t *skt)
{
    tt_skt_attr_t *attr = &skt->attr;

    // ipv6 only
    if (attr->config_ipv6only) {
        if ((skt->family == TT_NET_AF_INET6) &&
            !TT_OK(tt_skt_set_ipv6only(skt, attr->ipv6only))) {
            return TT_FAIL;
        }
    }

    // reuse addr
    if (attr->config_reuse_addr &&
        !TT_OK(tt_skt_set_reuseaddr(skt, attr->reuse_addr))) {
        return TT_FAIL;
    }

    // reuse port
    if (attr->config_reuse_port &&
        !TT_OK(tt_skt_set_reuseport(skt, attr->reuse_port))) {
        return TT_FAIL;
    }

    // reuse addr
    if (attr->config_tcp_nodelay &&
        !TT_OK(tt_skt_set_tcp_nodelay(skt, attr->tcp_nodelay))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
