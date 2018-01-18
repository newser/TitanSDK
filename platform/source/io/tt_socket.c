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

#include <io/tt_socket.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <io/tt_file_system.h>
#include <io/tt_socket_option.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_util.h>
#include <tt_cstd_api.h>

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

tt_atomic_s64_t tt_skt_stat_num;

tt_atomic_s64_t tt_skt_stat_peek;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __skt_component_init(IN tt_component_t *comp,
                                        IN tt_profile_t *profile);

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

tt_skt_t *tt_skt_create(IN tt_net_family_t family,
                        IN tt_net_protocol_t protocol,
                        IN OPT tt_skt_attr_t *attr)
{
    tt_skt_t *skt;
    tt_skt_attr_t __attr;

    TT_ASSERT(TT_NET_AF_VALID(family));
    TT_ASSERT(TT_NET_PROTO_VALID(protocol));

    skt = tt_malloc(sizeof(tt_skt_t));
    if (skt == NULL) {
        TT_ERROR("no mem for skt");
        return NULL;
    }

    if (attr != NULL) {
        tt_skt_attr_default(&__attr);
        attr = &__attr;
    }

    if (!TT_OK(tt_skt_create_ntv(&skt->sys_skt, family, protocol, attr))) {
        tt_free(skt);
        return NULL;
    }

    return skt;
}

void tt_skt_destroy(IN tt_skt_t *skt)
{
    TT_ASSERT(skt != NULL);

    tt_skt_destroy_ntv(&skt->sys_skt);

    tt_free(skt);
}

void tt_skt_attr_default(IN tt_skt_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_memset(attr, 0, sizeof(tt_skt_attr_t));
}

tt_result_t tt_skt_shutdown(IN tt_skt_t *skt, IN tt_skt_shut_t shut)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_SKT_SHUT_VALID(shut));

    return tt_skt_shutdown_ntv(&skt->sys_skt, shut);
}

tt_result_t tt_skt_bind(IN tt_skt_t *skt, IN tt_sktaddr_t *addr)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_bind_ntv(&skt->sys_skt, addr);
}

tt_result_t tt_skt_bind_n(IN tt_skt_t *skt,
                          IN tt_net_family_t family,
                          IN tt_sktaddr_ip_t *ip,
                          IN tt_u16_t port)
{
    tt_sktaddr_t addr;

    tt_sktaddr_init(&addr, family);
    tt_sktaddr_set_ip_n(&addr, ip);
    tt_sktaddr_set_port(&addr, port);

    return tt_skt_bind(skt, &addr);
}

tt_result_t tt_skt_bind_p(IN tt_skt_t *skt,
                          IN tt_net_family_t family,
                          IN const tt_char_t *ip,
                          IN tt_u16_t port)
{
    tt_sktaddr_t addr;

    tt_sktaddr_init(&addr, family);
    if (!TT_OK(tt_sktaddr_set_ip_p(&addr, ip))) {
        return TT_FAIL;
    }
    tt_sktaddr_set_port(&addr, port);

    return tt_skt_bind(skt, &addr);
}

tt_result_t tt_skt_listen(IN tt_skt_t *skt)
{
    TT_ASSERT(skt != NULL);

    return tt_skt_listen_ntv(&skt->sys_skt);
}

tt_skt_t *tt_skt_accept(IN tt_skt_t *skt,
                        IN OPT tt_skt_attr_t *new_attr,
                        IN OPT tt_sktaddr_t *addr,
                        OUT tt_fiber_ev_t **p_fev,
                        OUT struct tt_tmr_s **p_tmr)
{
    tt_sktaddr_t __addr;

    TT_ASSERT(skt != NULL);

    if (addr == NULL) {
        addr = &__addr;
    }

    return tt_skt_accept_ntv(&skt->sys_skt, addr, p_fev, p_tmr);
}

tt_result_t tt_skt_connect(IN tt_skt_t *skt, IN tt_sktaddr_t *addr)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(addr != NULL);

    return tt_skt_connect_ntv(&skt->sys_skt, addr);
}

tt_result_t tt_skt_connect_n(IN tt_skt_t *skt,
                             IN tt_net_family_t family,
                             IN tt_sktaddr_ip_t *ip,
                             IN tt_u16_t port)
{
    tt_sktaddr_t addr;

    tt_sktaddr_init(&addr, family);
    tt_sktaddr_set_ip_n(&addr, ip);
    tt_sktaddr_set_port(&addr, port);

    return tt_skt_connect(skt, &addr);
}

tt_result_t tt_skt_connect_p(IN tt_skt_t *skt,
                             IN tt_net_family_t family,
                             IN const tt_char_t *ip,
                             IN tt_u16_t port)
{
    tt_sktaddr_t addr;

    tt_sktaddr_init(&addr, family);
    if (!TT_OK(tt_sktaddr_set_ip_p(&addr, ip))) {
        return TT_FAIL;
    }
    tt_sktaddr_set_port(&addr, port);

    return tt_skt_connect(skt, &addr);
}

tt_result_t tt_skt_sendfile_path(IN tt_skt_t *skt, IN const tt_char_t *path)
{
    tt_file_t f;
    tt_result_t result;

    TT_ASSERT(path != NULL);

    if (!TT_OK(tt_fopen(&f, path, TT_FO_READ, NULL))) {
        return TT_FAIL;
    }

    result = tt_skt_sendfile(skt, &f);
    tt_fclose(&f);
    return result;
}

tt_result_t tt_skt_local_addr(IN tt_skt_t *skt, IN tt_sktaddr_t *addr)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(addr != NULL);

    return tt_skt_local_addr_ntv(&skt->sys_skt, addr);
}

tt_result_t tt_skt_remote_addr(IN tt_skt_t *skt, IN tt_sktaddr_t *addr)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(addr != NULL);

    return tt_skt_remote_addr_ntv(&skt->sys_skt, addr);
}

void tt_skt_stat_inc_num()
{
    tt_s64_t skt_num = tt_atomic_s64_inc(&tt_skt_stat_num);

    // the peek value is not accurate, just for reference
    if (skt_num > tt_atomic_s64_get(&tt_skt_stat_peek)) {
        tt_atomic_s64_set(&tt_skt_stat_peek, skt_num);
    }
}

void tt_skt_stat_dec_num()
{
    tt_atomic_s64_dec(&tt_skt_stat_num);
}

tt_result_t tt_skt_join_mcast(IN tt_skt_t *skt,
                              IN tt_net_family_t family,
                              IN tt_sktaddr_ip_t *ip,
                              IN OPT const tt_char_t *itf)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(family));
    TT_ASSERT(ip != NULL);

    return tt_skt_join_mcast_ntv(&skt->sys_skt, family, ip, itf);
}

tt_result_t tt_skt_leave_mcast(IN tt_skt_t *skt,
                               IN tt_net_family_t family,
                               IN tt_sktaddr_ip_t *ip,
                               IN OPT const tt_char_t *itf)
{
    TT_ASSERT(skt != NULL);
    TT_ASSERT(TT_NET_AF_VALID(family));
    TT_ASSERT(ip != NULL);

    return tt_skt_leave_mcast_ntv(&skt->sys_skt, family, ip, itf);
}

tt_skt_t *tt_tcp_server(IN tt_net_family_t family,
                        IN OPT tt_skt_attr_t *attr,
                        IN tt_sktaddr_t *addr)
{
    tt_skt_t *skt;

    skt = tt_skt_create(family, TT_NET_PROTO_TCP, attr);
    if (skt == NULL) {
        return NULL;
    }

    if (!TT_OK(tt_skt_bind(skt, addr)) || !TT_OK(tt_skt_listen(skt))) {
        tt_skt_destroy(skt);
        return NULL;
    }

    return skt;
}

tt_skt_t *tt_tcp_server_p(IN tt_net_family_t family,
                          IN OPT tt_skt_attr_t *attr,
                          IN const tt_char_t *ip,
                          IN tt_u16_t port)
{
    tt_sktaddr_t addr;

    tt_sktaddr_init(&addr, family);
    if (!TT_OK(tt_sktaddr_set_ip_p(&addr, ip))) {
        return NULL;
    }
    tt_sktaddr_set_port(&addr, port);

    return tt_tcp_server(family, attr, &addr);
}

tt_skt_t *tt_udp_server(IN tt_net_family_t family,
                        IN OPT tt_skt_attr_t *attr,
                        IN tt_sktaddr_t *addr)
{
    tt_skt_t *skt;

    skt = tt_skt_create(family, TT_NET_PROTO_UDP, attr);
    if (skt == NULL) {
        return NULL;
    }

    if (!TT_OK(tt_skt_bind(skt, addr))) {
        tt_skt_destroy(skt);
        return NULL;
    }

    return skt;
}

tt_skt_t *tt_udp_server_p(IN tt_net_family_t family,
                          IN OPT tt_skt_attr_t *attr,
                          IN const tt_char_t *ip,
                          IN tt_u16_t port)
{
    tt_sktaddr_t addr;

    tt_sktaddr_init(&addr, family);
    if (!TT_OK(tt_sktaddr_set_ip_p(&addr, ip))) {
        return NULL;
    }
    tt_sktaddr_set_port(&addr, port);

    return tt_udp_server(family, attr, &addr);
}

tt_result_t __skt_component_init(IN tt_component_t *comp,
                                 IN tt_profile_t *profile)
{
    // init low level socket system
    if (!TT_OK(tt_skt_component_init_ntv(profile))) {
        TT_ERROR("fail to initialize socket system native");
        return TT_FAIL;
    }

    tt_atomic_s64_set(&tt_skt_stat_num, 0);
    tt_atomic_s64_set(&tt_skt_stat_peek, 0);

    return TT_SUCCESS;
}
