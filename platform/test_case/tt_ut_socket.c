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

#include "tt_unit_test_case_config.h"
#include <unit_test/tt_unit_test.h>

#include <event/tt_event_center.h>
#include <event/tt_event_poller.h>
#include <io/tt_socket.h>
#include <io/tt_socket_addr.h>
#include <io/tt_socket_aio.h>
#include <io/tt_socket_option.h>
#include <memory/tt_memory_alloc.h>
#include <memory/tt_slab.h>
#include <network/tt_network_interface.h>

// portlayer header files
#include <tt_cstd_api.h>

#include <time.h>
#if 1
////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

//#define __DBG_SKT_AIO_UT

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// note system firewall may block some cases or impact
// performance test

// ipv6 cases can not run in ipv4-only environment

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_sk_addr)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_bind_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_tcp_server)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_tcp_server6)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_udp_server)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_udp_server6)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_udp_multicast)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_tcp_async_ipv4)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_tcp_async_ipv6)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_udp_async_ipv6)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_udp_async_ipv4)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_tcp_async_ipv4_st)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_tcp_async_ipv6_st)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_udp_async_ipv4_st)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_udp_async_ipv6_st)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_ab)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ab_nc)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_tcp_destroy_immd)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_udp_destroy_immd)

// =========================================

// sometimes local ip can not be retrieved in __ut_skt_enter, as
// the physical netif name is not listed in __ut_skt_enter
#if 1
#define __TLOCAL_IP __ut_skt_local_ip
#define __TLOCAL_IP6 __ut_skt_local_ip6
#define __TLOCAL_ITF __ut_skt_local_itf
#define __TLOCAL_IP6_MAPPED __ut_skt_local_ip6_mapped //"::ffff:"__TLOCAL_IP
#else
#define __TLOCAL_IP "127.0.0.1"
#define __TLOCAL_IP6 "::1"
#define __TLOCAL_ITF "127.0.0.1"
#define __TLOCAL_IP6_MAPPED __TLOCAL_IP6
#endif

static tt_bool_t __ut_skt_inited = TT_FALSE;
static tt_char_t __ut_skt_local_ip[40];
static tt_char_t __ut_skt_local_ip6[180];
static tt_char_t __ut_skt_local_itf[40];
static tt_char_t __ut_skt_local_ip6_mapped[180];

static void __ut_skt_enter(void *enter_param)
{
    tt_netif_group_t netif_group;
    tt_netif_t *nif = NULL;

    if (__ut_skt_inited) {
        return;
    }

    tt_netif_group_init(&netif_group, TT_NIFGRP_NO_IPV6_LINK_LOCAL);

#if TT_ENV_OS_IS_MACOS || TT_ENV_OS_IS_IOS
    tt_netif_group_add(&netif_group, "en0");
    tt_netif_group_add(&netif_group, "en1");
    tt_netif_group_add(&netif_group, "en2");
    tt_netif_group_add(&netif_group, "en3");
    tt_netif_group_add(&netif_group, "en4");
    tt_netif_group_add(&netif_group, "en5");
    tt_netif_group_add(&netif_group, "en6");
    tt_netif_group_add(&netif_group, "en7");
    tt_netif_group_add(&netif_group, "en8");
    tt_netif_group_add(&netif_group, "en9");
    tt_netif_group_add(&netif_group, "lo");
#elif TT_ENV_OS_IS_LINUX
    tt_netif_group_add(&netif_group, "eth0");
    tt_netif_group_add(&netif_group, "eth1");
    tt_netif_group_add(&netif_group, "eth2");
    tt_netif_group_add(&netif_group, "eth3");
    tt_netif_group_add(&netif_group, "eth4");
    tt_netif_group_add(&netif_group, "eth5");
    tt_netif_group_add(&netif_group, "eth6");
    tt_netif_group_add(&netif_group, "eth7");
    tt_netif_group_add(&netif_group, "eth8");
    tt_netif_group_add(&netif_group, "eth9");
    tt_netif_group_add(&netif_group, "lo");
#elif TT_ENV_OS_IS_WINDOWS
    tt_netif_group_add(&netif_group, "ethernet_0");
    tt_netif_group_add(&netif_group, "ethernet_1");
    tt_netif_group_add(&netif_group, "ethernet_2");
    tt_netif_group_add(&netif_group, "ethernet_3");
    tt_netif_group_add(&netif_group, "ethernet_4");
    tt_netif_group_add(&netif_group, "ethernet_5");
    tt_netif_group_add(&netif_group, "ethernet_6");
    tt_netif_group_add(&netif_group, "ethernet_7");
    tt_netif_group_add(&netif_group, "ethernet_8");
    tt_netif_group_add(&netif_group, "ethernet_9");
    tt_netif_group_add(&netif_group, "wireless_0");
    tt_netif_group_add(&netif_group, "wireless_1");
    tt_netif_group_add(&netif_group, "wireless_2");
    tt_netif_group_add(&netif_group, "wireless_3");
    tt_netif_group_add(&netif_group, "wireless_4");
    tt_netif_group_add(&netif_group, "wireless_5");
    tt_netif_group_add(&netif_group, "wireless_6");
    tt_netif_group_add(&netif_group, "wireless_7");
    tt_netif_group_add(&netif_group, "wireless_8");
    tt_netif_group_add(&netif_group, "wireless_9");
    tt_netif_group_add(&netif_group, "loopback_0");
    tt_netif_group_add(&netif_group, "loopback_1");
#else
#warn no netif added
#endif

    tt_netif_group_refresh_prepare(&netif_group);
    tt_netif_group_refresh(&netif_group, 0);
    tt_netif_group_refresh_done(&netif_group);

    while ((nif = tt_netif_group_next(&netif_group, nif)) != NULL) {
        if (nif->status == TT_NETIF_STATUS_ACTIVE) {
            tt_lnode_t *node = tt_list_head(&nif->addr_list);
            while (node != NULL) {
                tt_netif_addr_t *addr =
                    TT_CONTAINER(node, tt_netif_addr_t, node);
                tt_sktaddr_t *saddr = &addr->addr;

                if (tt_sktaddr_get_family(saddr) == TT_NET_AF_INET) {
                    tt_u32_t n = sizeof(sizeof("::ffff:")) - 1;

                    tt_sktaddr_get_addr_p(saddr,
                                          __ut_skt_local_ip,
                                          sizeof(__ut_skt_local_ip));

                    tt_memcpy(__ut_skt_local_ip6_mapped, "::ffff:", n);
                    tt_memcpy(&__ut_skt_local_ip6_mapped[n],
                              __ut_skt_local_ip,
                              (tt_u32_t)tt_strlen(__ut_skt_local_ip));
                } else {
                    TT_ASSERT(tt_sktaddr_get_family(saddr) == TT_NET_AF_INET6);

                    tt_sktaddr_get_addr_p(saddr,
                                          __ut_skt_local_ip6,
                                          sizeof(__ut_skt_local_ip6));
                }

                node = node->next;
            }

            tt_memcpy(__ut_skt_local_itf,
                      nif->name,
                      (tt_u32_t)tt_strlen(nif->name));

            __ut_skt_inited = TT_TRUE;
            break;
        }
    }

    TT_INFO("====== socket unit test addresses ======");
    if (!__ut_skt_inited) {
        TT_INFO("unable to get addresses");

        tt_strncpy(__ut_skt_local_ip,
                   "127.0.0.1",
                   sizeof(__ut_skt_local_ip) - 1);
        tt_strncpy(__ut_skt_local_ip6, "::1", sizeof(__ut_skt_local_ip6) - 1);
    }
    TT_INFO("ipv4: %s", __ut_skt_local_ip);
    if (__ut_skt_local_ip6[0] == 0) {
        tt_memcpy(__ut_skt_local_ip6,
                  __ut_skt_local_ip6_mapped,
                  sizeof(__ut_skt_local_ip6_mapped));
    }
    TT_INFO("ipv6: %s", __ut_skt_local_ip6);
    TT_INFO("ipv6 mapped: %s", __ut_skt_local_ip6_mapped);
    TT_INFO("interface: %s", __ut_skt_local_itf);
    TT_INFO("========================================");
}

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(sk_case)
#if 1
TT_TEST_CASE("tt_unit_test_sk_addr",
             "testing socket addr api",
             tt_unit_test_sk_addr,
             NULL,
             __ut_skt_enter,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_bind_basic",
                 "testing socket tcp api",
                 tt_unit_test_bind_basic,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_tcp_server",
                 "testing socket tcp api",
                 tt_unit_test_tcp_server,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_tcp_server6",
                 "testing socket tcp ipv6 api",
                 tt_unit_test_tcp_server6,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_udp_server",
                 "testing socket udp api",
                 tt_unit_test_udp_server,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_udp_server6",
                 "testing socket udp ipv6 api",
                 tt_unit_test_udp_server6,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 NULL,
                 NULL),
#endif

#if 0
    TT_TEST_CASE("tt_unit_test_udp_multicast", 
                 "testing socket udp multicast", 
                 tt_unit_test_udp_multicast, NULL, 
                 __ut_skt_enter, NULL,
                 NULL, NULL),
#endif

#if 1
    TT_TEST_CASE("tt_unit_test_udp_async_ipv4",
                 "testing socket udp async ipv4",
                 tt_unit_test_udp_async_ipv4,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_udp_async_ipv6",
                 "testing socket udp async ipv6",
                 tt_unit_test_udp_async_ipv6,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 NULL,
                 NULL),
#endif

#if 1
    TT_TEST_CASE("tt_unit_test_tcp_async_ipv4",
                 "testing socket tcp async ipv4",
                 tt_unit_test_tcp_async_ipv4,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_tcp_async_ipv6",
                 "testing socket tcp async ipv6",
                 tt_unit_test_tcp_async_ipv6,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_tcp_async_ipv4 ag",
                 "testing socket tcp async ipv4 again",
                 tt_unit_test_tcp_async_ipv4,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_tcp_async_ipv6 ag",
                 "testing socket tcp async ipv6 again",
                 tt_unit_test_tcp_async_ipv6,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 NULL,
                 NULL),
#endif

#if 0
    TT_TEST_CASE("tt_unit_test_udp_async_ipv4_st",
                 "testing socket udp async ipv4 stress",
                 tt_unit_test_udp_async_ipv4_st,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_udp_async_ipv6_st",
                 "testing socket udp async ipv6 stress",
                 tt_unit_test_udp_async_ipv6_st,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 NULL,
                 NULL),
#endif

#if 0
    TT_TEST_CASE("tt_unit_test_tcp_async_ipv4_st",
                 "testing socket tcp async ipv4 stress",
                 tt_unit_test_tcp_async_ipv4_st,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_tcp_async_ipv6_st",
                 "testing socket tcp async ipv6 stress",
                 tt_unit_test_tcp_async_ipv6_st,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 NULL,
                 NULL),
#endif

#if 0
    TT_TEST_CASE("tt_unit_test_ab", 
                 "testing socket test with apache benchmark", 
                 tt_unit_test_ab, NULL, 
                 __ut_skt_enter, NULL,
                 NULL, NULL),
#endif

    TT_TEST_CASE("tt_unit_test_tcp_destroy_immd",
                 "testing socket tcp destroy immediately",
                 tt_unit_test_tcp_destroy_immd,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_udp_destroy_immd",
                 "testing socket udp destroy immediately",
                 tt_unit_test_udp_destroy_immd,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(sk_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_SOCKET, 0, sk_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(name)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_sk_addr)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sktaddr_t sa;
    tt_sktaddr_addr_t saa;
    tt_result_t ret;

    tt_sktaddr_t sa6;
    tt_sktaddr_addr_t saa6;
    tt_u16_t port;

    tt_char_t buf[100];
    // tt_char_t sc_buf[100];

    TT_TEST_CASE_ENTER()
    // test start

    tt_sktaddr_init(&sa, TT_NET_AF_INET);
    TT_TEST_CHECK_EQUAL(tt_sktaddr_get_family(&sa), TT_NET_AF_INET, "");

    ret = tt_sktaddr_addr_p2n(TT_NET_AF_INET, "1.2..4", &saa);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    ret = tt_sktaddr_addr_p2n(TT_NET_AF_INET, "1.2.3.4", &saa);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_sktaddr_addr_n2p(TT_NET_AF_INET, &saa, buf, 7);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    ret = tt_sktaddr_addr_n2p(TT_NET_AF_INET, &saa, buf, 8);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(buf[0], '1', "");
    TT_TEST_CHECK_EQUAL(buf[1], '.', "");
    TT_TEST_CHECK_EQUAL(buf[2], '2', "");
    TT_TEST_CHECK_EQUAL(buf[3], '.', "");
    TT_TEST_CHECK_EQUAL(buf[4], '3', "");
    TT_TEST_CHECK_EQUAL(buf[5], '.', "");
    TT_TEST_CHECK_EQUAL(buf[6], '4', "");
    TT_TEST_CHECK_EQUAL(buf[7], '\0', "");

    ret = tt_sktaddr_set_addr_p(&sa, "0.0.0");
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    ret = tt_sktaddr_set_addr_p(&sa, "0.0.0.1");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_sktaddr_get_addr_p(&sa, buf, 1);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    ret = tt_sktaddr_get_addr_p(&sa, buf, 10);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(buf[0], '0', "");
    TT_TEST_CHECK_EQUAL(buf[1], '.', "");
    TT_TEST_CHECK_EQUAL(buf[2], '0', "");
    TT_TEST_CHECK_EQUAL(buf[3], '.', "");
    TT_TEST_CHECK_EQUAL(buf[4], '0', "");
    TT_TEST_CHECK_EQUAL(buf[5], '.', "");
    TT_TEST_CHECK_EQUAL(buf[6], '1', "");
    TT_TEST_CHECK_EQUAL(buf[7], '\0', "");

    tt_sktaddr_set_port(&sa, 1);
    TT_TEST_CHECK_EQUAL(tt_sktaddr_get_port(&sa, &port), TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(port, 1, "");

    ////////////////////// ipv6

    tt_sktaddr_init(&sa6, TT_NET_AF_INET6);
    TT_TEST_CHECK_EQUAL(tt_sktaddr_get_family(&sa6), TT_NET_AF_INET6, "");

    ret = tt_sktaddr_addr_p2n(TT_NET_AF_INET6, "1.2..4", &saa6);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    ret = tt_sktaddr_addr_p2n(TT_NET_AF_INET6, "1::01", &saa6);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_sktaddr_addr_n2p(TT_NET_AF_INET6, &saa6, buf, 1);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    ret = tt_sktaddr_addr_n2p(TT_NET_AF_INET6, &saa6, buf, 8);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(buf[0], '1', "");
    TT_TEST_CHECK_EQUAL(buf[1], ':', "");
    TT_TEST_CHECK_EQUAL(buf[2], ':', "");
    TT_TEST_CHECK_EQUAL(buf[3], '1', "");
    TT_TEST_CHECK_EQUAL(buf[4], '\0', "");

    ret = tt_sktaddr_set_addr_p(&sa6, "0.0.0");
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    ret = tt_sktaddr_set_addr_p(&sa6, "1::01");
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_sktaddr_get_addr_p(&sa6, buf, 1);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    ret = tt_sktaddr_get_addr_p(&sa6, buf, 10);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(buf[0], '1', "");
    TT_TEST_CHECK_EQUAL(buf[1], ':', "");
    TT_TEST_CHECK_EQUAL(buf[2], ':', "");
    TT_TEST_CHECK_EQUAL(buf[3], '1', "");
    TT_TEST_CHECK_EQUAL(buf[4], '\0', "");

    tt_sktaddr_set_port(&sa6, 1);
    TT_TEST_CHECK_EQUAL(tt_sktaddr_get_port(&sa6, &port), TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(port, 1, "");

    //////////// ipv4 to ipv6

    ret = tt_sktaddr_addr_p2n(TT_NET_AF_INET, "1.2.3.4", &saa);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_sktaddr_init(&sa, TT_NET_AF_INET);
    tt_sktaddr_set_addr_n(&sa, &saa);
    tt_sktaddr_set_port(&sa, 1234);

    tt_sktaddr_map4to6(&sa, &sa6);

    TT_TEST_CHECK_EQUAL(tt_sktaddr_get_family(&sa6), TT_NET_AF_INET6, "");

    ret = tt_sktaddr_get_addr_p(&sa6, buf, 20);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(buf[0], ':', "");
    TT_TEST_CHECK_EQUAL(buf[1], ':', "");
    TT_TEST_CHECK_EQUAL(buf[2], 'f', "");
    TT_TEST_CHECK_EQUAL(buf[3], 'f', "");
    TT_TEST_CHECK_EQUAL(buf[4], 'f', "");
    TT_TEST_CHECK_EQUAL(buf[5], 'f', "");
    TT_TEST_CHECK_EQUAL(buf[6], ':', "");
    TT_TEST_CHECK_EQUAL(buf[7], '1', "");
    TT_TEST_CHECK_EQUAL(buf[8], '.', "");
    TT_TEST_CHECK_EQUAL(buf[9], '2', "");
    TT_TEST_CHECK_EQUAL(buf[10], '.', "");
    TT_TEST_CHECK_EQUAL(buf[11], '3', "");
    TT_TEST_CHECK_EQUAL(buf[12], '.', "");
    TT_TEST_CHECK_EQUAL(buf[13], '4', "");
    TT_TEST_CHECK_EQUAL(buf[14], '\0', "");

    TT_TEST_CHECK_EQUAL(tt_sktaddr_get_port(&sa6, &port), TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(port, 1234, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_bind_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_skt_t sk4;
    tt_skt_t sk6;
    tt_result_t ret;
    tt_skt_attr_t attr;
    tt_sktaddr_t addr;
    tt_u16_t port;

    TT_TEST_CASE_ENTER()
// test start

#define __TPORT 22450
#define __TPORT6 32350

    tt_skt_attr_default(&attr);
    attr.config_reuse_addr = TT_TRUE;
    attr.reuse_addr = TT_TRUE;

    ret = tt_skt_create(&sk4, TT_NET_AF_INET, TT_NET_PROTO_TCP, &attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    if (TT_OK(tt_skt_local_addr(&sk4, &addr))) {
        tt_sktaddr_addr_t addr_val;

        tt_sktaddr_get_addr_n(&addr, &addr_val);
        TT_TEST_CHECK_EQUAL(addr_val.addr32.__u32, 0, "");

        TT_TEST_CHECK_EQUAL(tt_sktaddr_get_port(&addr, &port), TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(port, 0, "");
    }
    if (TT_OK(tt_skt_local_addr(&sk4, &addr))) {
        tt_sktaddr_addr_t addr_val;

        tt_sktaddr_get_addr_n(&addr, &addr_val);
        TT_TEST_CHECK_EQUAL(addr_val.addr32.__u32, 0, "");

        TT_TEST_CHECK_EQUAL(tt_sktaddr_get_port(&addr, &port), TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(port, 0, "");
    }

    // ret = tt_skt_listen(&sk4, TT_SKT_BACKLOG_DEFAULT);
    // TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    // ipv4 can not bind ipv6
    ret = tt_skt_bind_p(&sk4, TT_NET_AF_INET6, "::1", __TPORT);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    // invalid ipv4 addr
    ret = tt_skt_bind_p(&sk4, TT_NET_AF_INET, "256.0.0.1", __TPORT);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    ret = tt_skt_bind_p(&sk4, TT_NET_AF_INET, "::127.0.0.1", __TPORT);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    ret = tt_skt_bind_p(&sk4, TT_NET_AF_INET, "127.0.0.1", __TPORT);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "fail to bind to tcp test port");

    {
        tt_sktaddr_t sa1;
        tt_char_t buf[20];
        int i = 0;

        TT_TEST_CHECK_SUCCESS(tt_skt_local_addr(&sk4, &sa1), "");
        TT_TEST_CHECK_EQUAL(tt_sktaddr_get_family(&sa1), TT_NET_AF_INET, "");

        TT_TEST_CHECK_EQUAL(tt_sktaddr_get_port(&sa1, &port), TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(port, __TPORT, "");

        tt_sktaddr_get_addr_p(&sa1, buf, 19);
        TT_TEST_CHECK_EQUAL(buf[i++], '1', "");
        TT_TEST_CHECK_EQUAL(buf[i++], '2', "");
        TT_TEST_CHECK_EQUAL(buf[i++], '7', "");
        TT_TEST_CHECK_EQUAL(buf[i++], '.', "");
        TT_TEST_CHECK_EQUAL(buf[i++], '0', "");
        TT_TEST_CHECK_EQUAL(buf[i++], '.', "");
        TT_TEST_CHECK_EQUAL(buf[i++], '0', "");
        TT_TEST_CHECK_EQUAL(buf[i++], '.', "");
        TT_TEST_CHECK_EQUAL(buf[i++], '1', "");
        TT_TEST_CHECK_EQUAL(buf[i++], '\0', "");

        // remote is still not valid
        TT_TEST_CHECK_FAIL(tt_skt_remote_addr(&sk4, &sa1), "");
    }

    // can not rebind
    ret = tt_skt_bind_p(&sk4, TT_NET_AF_INET, "127.0.0.1", __TPORT);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    ret = tt_skt_listen(&sk4, TT_SKT_BACKLOG_DEFAULT);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_skt_destroy(&sk4);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ///////////////////////////////////////////////////////////////////

    tt_skt_attr_default(&attr);
    attr.config_reuse_addr = TT_TRUE;
    attr.reuse_addr = TT_FALSE;

    tt_skt_attr_set_ipv6only(&attr, TT_FALSE);

    ret = tt_skt_create(&sk6, TT_NET_AF_INET6, TT_NET_PROTO_TCP, &attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    if (TT_OK(tt_skt_local_addr(&sk6, &addr))) {
        tt_sktaddr_addr_t addr_val;
        tt_sktaddr_addr_t cmp;

        tt_sktaddr_get_addr_n(&addr, &addr_val);
        memset(&cmp, 0, sizeof(cmp));
        TT_TEST_CHECK_EQUAL(memcmp(&addr_val, &cmp, sizeof(cmp)), 0, "");

        TT_TEST_CHECK_EQUAL(tt_sktaddr_get_port(&addr, &port), TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(port, 0, "");
    }
    if (TT_OK(tt_skt_local_addr(&sk6, &addr))) {
        tt_sktaddr_addr_t addr_val;
        tt_sktaddr_addr_t cmp;

        tt_sktaddr_get_addr_n(&addr, &addr_val);
        memset(&cmp, 0, sizeof(cmp));
        TT_TEST_CHECK_EQUAL(memcmp(&addr_val, &cmp, sizeof(cmp)), 0, "");

        TT_TEST_CHECK_EQUAL(tt_sktaddr_get_port(&addr, &port), TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(port, 0, "");
    }

    // ret = tt_skt_listen(&sk6, TT_SKT_BACKLOG_DEFAULT);
    // TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    // invalid ipv6 address
    ret = tt_skt_bind_p(&sk6, TT_NET_AF_INET6, "127.0.0.1", __TPORT6);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    if ((tt_strncmp(__TLOCAL_IP6, "fe80", 4) == 0) && (__TLOCAL_ITF != NULL)) {
        tt_sktaddr_t __a6;
        tt_sktaddr_init(&__a6, TT_NET_AF_INET6);
        tt_sktaddr_set_addr_p(&__a6, __TLOCAL_IP6);
        tt_sktaddr_set_port(&__a6, __TPORT6);
        tt_sktaddr_set_scope_p(&__a6, __TLOCAL_ITF);
        ret = tt_skt_bind(&sk6, &__a6);
    } else {
        ret = tt_skt_bind_p(&sk6, TT_NET_AF_INET6, __TLOCAL_IP6, __TPORT6);
    }
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    {
        tt_sktaddr_t sa1;
        tt_char_t buf[100];

        TT_TEST_CHECK_SUCCESS(tt_skt_local_addr(&sk6, &sa1), "");
        // family has been converted
        TT_TEST_CHECK_EQUAL(tt_sktaddr_get_family(&sa1), TT_NET_AF_INET6, "");
        TT_TEST_CHECK_EQUAL(tt_sktaddr_get_port(&sa1, &port), TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(port, __TPORT6, "");

        ret = tt_sktaddr_get_addr_p(&sa1, buf, 99);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(tt_strncmp(buf, __TLOCAL_IP6, 99), 0, "");

        // remote is still not valid
        TT_TEST_CHECK_FAIL(tt_skt_remote_addr(&sk6, &sa1), "");
    }

    // can not rebind
    ret = tt_skt_bind_p(&sk6, TT_NET_AF_INET6, "::1", __TPORT);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    ret = tt_skt_listen(&sk6, TT_SKT_BACKLOG_DEFAULT);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_skt_destroy(&sk6);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    /////// bind ipv6 addr

    ret = tt_skt_create(&sk6, TT_NET_AF_INET6, TT_NET_PROTO_TCP, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_skt_bind_p(&sk6, TT_NET_AF_INET6, "::1", __TPORT6);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    {
        tt_sktaddr_t sa1;
        tt_char_t buf[20];
        int i;

        TT_TEST_CHECK_SUCCESS(tt_skt_local_addr(&sk6, &sa1), "");
        // family has been converted
        TT_TEST_CHECK_EQUAL(tt_sktaddr_get_family(&sa1), TT_NET_AF_INET6, "");
        TT_TEST_CHECK_EQUAL(tt_sktaddr_get_port(&sa1, &port), TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(port, __TPORT6, "");

        tt_sktaddr_get_addr_p(&sa1, buf, 19);

        i = 0;
        TT_TEST_CHECK_EQUAL(buf[i++], ':', "");
        TT_TEST_CHECK_EQUAL(buf[i++], ':', "");
        TT_TEST_CHECK_EQUAL(buf[i++], '1', "");
        TT_TEST_CHECK_EQUAL(buf[i++], '\0', "");

        // remote is still not valid
        TT_TEST_CHECK_FAIL(tt_skt_remote_addr(&sk6, &sa1), "");
    }

    ret = tt_skt_destroy(&sk6);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    /////// ipv6 only

    attr.config_ipv6only = TT_TRUE;
    attr.ipv6only = TT_TRUE;

    ret = tt_skt_create(&sk6, TT_NET_AF_INET6, TT_NET_PROTO_TCP, &attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_skt_bind_p(&sk6, TT_NET_AF_INET, "127.0.0.1", __TPORT6);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    ret = tt_skt_bind_p(&sk6, TT_NET_AF_INET6, "::ffff:127.0.0.1", __TPORT6);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    ret = tt_skt_destroy(&sk6);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

#define __echo_buf_len 100

static tt_result_t __echo_cli(tt_skt_t *cli_skt)
{
    char buf[__echo_buf_len];
    char recv_buf[__echo_buf_len];
    int i;
    int times = 0;
    tt_result_t ret;
    tt_u32_t len;

    while (times++ < 10) {
        srand((tt_u32_t)time(NULL));
        for (i = 0; i < __echo_buf_len; ++i)
            buf[i] = (char)(rand() % 26 + 'a');

        ret = tt_skt_send(cli_skt, (tt_u8_t *)buf, __echo_buf_len, &len);
        TT_ASSERT(TT_OK(ret));
        TT_ASSERT(len == __echo_buf_len);

        ret = tt_skt_send(cli_skt, (tt_u8_t *)buf, 0, &len);
        TT_ASSERT(TT_OK(ret));
        TT_ASSERT(len == 0);
        ret = tt_skt_send(cli_skt, (tt_u8_t *)buf, 0, &len);
        TT_ASSERT(TT_OK(ret));
        TT_ASSERT(len == 0);

        ret = tt_skt_recv(cli_skt, (tt_u8_t *)recv_buf, __echo_buf_len, &len);
        TT_ASSERT(TT_OK(ret));
        TT_ASSERT(len == __echo_buf_len);

        ret = tt_skt_recv(cli_skt, (tt_u8_t *)recv_buf, 0, &len);
        TT_ASSERT(TT_OK(ret));
        TT_ASSERT(len == 0);
        ret = tt_skt_recv(cli_skt, (tt_u8_t *)recv_buf, 0, &len);
        TT_ASSERT(TT_OK(ret));
        TT_ASSERT(len == 0);

        // all received
        if (memcmp(buf, recv_buf, __echo_buf_len) != 0) {
            TT_ERROR("recv and send are different");
            return TT_FAIL;
        }
    }
    return TT_SUCCESS;
}

static tt_result_t __echo_svr(tt_skt_t *svr_skt)
{
    char buf[__echo_buf_len];
    tt_u32_t len;
    tt_result_t ret;

    while (1) {
        ret = tt_skt_recv(svr_skt, (tt_u8_t *)buf, __echo_buf_len, &len);
        if (!TT_OK(ret)) {
            if (ret == TT_END)
                return TT_SUCCESS;
            else
                return TT_FAIL;
        }
        if (len != __echo_buf_len)
            return TT_FAIL;

        ret = tt_skt_send(svr_skt, (tt_u8_t *)buf, __echo_buf_len, &len);
        if (!TT_OK(ret))
            return TT_FAIL;
        if (len != __echo_buf_len)
            return TT_FAIL;
    }
    return TT_SUCCESS;
}

static tt_result_t __tcp_thread_c(IN tt_thread_t *thread, IN void *param)
{
    tt_skt_t sk4;
    tt_u16_t port = (tt_u16_t)(tt_ptrdiff_t)param;
    tt_sktaddr_t server_addr;
    tt_sktaddr_t server_addr6;
    tt_result_t ret;

    tt_sktaddr_init(&server_addr, TT_NET_AF_INET);
    ret = tt_sktaddr_set_addr_p(&server_addr, "127.0.0.1");
    tt_sktaddr_set_port(&server_addr, port);

    ret = tt_skt_create(&sk4, TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    {
        tt_sktaddr_init(&server_addr6, TT_NET_AF_INET6);
        ret = tt_sktaddr_set_addr_p(&server_addr6, "::1");
        tt_sktaddr_set_port(&server_addr6, port);

        ret = tt_skt_connect(&sk4, &server_addr6);
        TT_ASSERT(!TT_OK(ret));
    }

    ret = tt_skt_connect(&sk4, &server_addr);
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = __echo_cli(&sk4);
    if (!TT_OK(ret))
        return TT_FAIL;

    tt_skt_destroy(&sk4);
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_tcp_server)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_skt_t sk4;
    tt_skt_t sk4_1;
    tt_result_t ret;
    tt_skt_attr_t attr;

    tt_sktaddr_t server_addr;
    tt_skt_t new_skt;
    tt_thread_t *tthd;

    TT_TEST_CASE_ENTER()
    // test start

    tt_skt_attr_default(&attr);
    attr.config_reuse_addr = TT_TRUE;
    attr.reuse_addr = TT_TRUE;

    tt_sktaddr_init(&server_addr, TT_NET_AF_INET);
    ret = tt_sktaddr_set_addr_p(&server_addr, TT_SKTADDR_ANY);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    tt_sktaddr_set_port(&server_addr, __TPORT);

    // server socket
    ret = tt_tcp_server(&sk4,
                        TT_NET_AF_INET,
                        &attr,
                        &server_addr,
                        TT_SKT_BACKLOG_DEFAULT);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_skt_create(&sk4_1, TT_NET_AF_INET, TT_NET_PROTO_UDP, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_skt_accept(&sk4_1, &new_skt, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    sk4_1.protocol = TT_NET_PROTO_TCP;
    ret = tt_skt_accept(&sk4_1, &new_skt, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    ////////////////////

    tthd = tt_thread_create("", __tcp_thread_c, (void *)__TPORT, NULL);
    TT_TEST_CHECK_NOT_EQUAL(tthd, NULL, "");

    // block untill new connection comes
    ret = tt_skt_accept(&sk4, &new_skt, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // an echo server
    ret = __echo_svr(&new_skt);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_thread_wait(tthd);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_skt_destroy(&new_skt);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    /////////////////////

    //

    ////////////////////

    ret = tt_skt_destroy(&sk4);

    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __tcp_thread_c6(IN tt_thread_t *thread, IN void *param)
{
    tt_skt_t sk6;
    tt_u16_t port = (tt_u16_t)(tt_ptrdiff_t)param;
    tt_sktaddr_t server_addr;
    tt_result_t ret;

    tt_sktaddr_init(&server_addr, TT_NET_AF_INET6);
    ret = tt_sktaddr_set_addr_p(&server_addr, "::1");
    tt_sktaddr_set_port(&server_addr, port);

    ret = tt_skt_create(&sk6, TT_NET_AF_INET6, TT_NET_PROTO_TCP, NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = tt_skt_connect(&sk6, &server_addr);
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = __echo_cli(&sk6);
    if (!TT_OK(ret))
        return TT_FAIL;

    tt_skt_destroy(&sk6);
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_tcp_server6)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_skt_t sk6;
    tt_result_t ret;
    tt_skt_attr_t attr;

    tt_sktaddr_t server_addr;
    tt_skt_t new_skt;
    tt_thread_t *tthd;

    TT_TEST_CASE_ENTER()
    // test start

    tt_skt_attr_default(&attr);
    attr.config_reuse_addr = TT_TRUE;
    attr.reuse_addr = TT_TRUE;

    tt_skt_attr_set_ipv6only(&attr, TT_FALSE);

    tt_sktaddr_init(&server_addr, TT_NET_AF_INET6);
    ret = tt_sktaddr_set_addr_p(&server_addr, TT_SKTADDR_ANY);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    tt_sktaddr_set_port(&server_addr, __TPORT + 1);

    // server socket
    ret = tt_tcp_server(&sk6,
                        TT_NET_AF_INET6,
                        &attr,
                        &server_addr,
                        TT_SKT_BACKLOG_DEFAULT);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ////////////////////

    tthd = tt_thread_create("", __tcp_thread_c6, (void *)(__TPORT + 1), NULL);
    TT_TEST_CHECK_NOT_EQUAL(tthd, NULL, "");

    // block untill new connection comes
    ret = tt_skt_accept(&sk6, &new_skt, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // an echo server
    ret = __echo_svr(&new_skt);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_thread_wait(tthd);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_skt_destroy(&new_skt);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    /////////////////////

    // ipv4 client, ipv6 server

    tthd = tt_thread_create("", __tcp_thread_c, (void *)(__TPORT + 1), NULL);
    TT_TEST_CHECK_NOT_EQUAL(tthd, NULL, "");

    // block untill new connection comes
    ret = tt_skt_accept(&sk6, &new_skt, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // an echo server
    ret = __echo_svr(&new_skt);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_thread_wait(tthd);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_skt_destroy(&new_skt);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ////////////////////

    ret = tt_skt_destroy(&sk6);

    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __echo_cli_udp(tt_skt_t *cli_skt, tt_sktaddr_t *svr_addr)
{
    char buf[__echo_buf_len];
    char recv_buf[__echo_buf_len];
    int pos = 0, i;
    int times = 0;
    tt_result_t ret;
    // tt_sktaddr_t remote_addr;
    tt_u32_t len;

    while (times++ < 10) {
        srand((tt_u32_t)time(NULL));
        for (i = 0; i < __echo_buf_len; ++i)
            buf[i] = (char)(rand() % 26 + 'a');

    send_ag:
        ret = tt_skt_sendto(cli_skt,
                            (tt_u8_t *)buf + pos,
                            __echo_buf_len - pos,
                            &len,
                            svr_addr);
        if (!TT_OK(ret))
            return TT_FAIL;
        pos += len;
        if (pos < __echo_buf_len)
            goto send_ag;

    recv_ag:
        pos = 0;
        ret = tt_skt_recvfrom(cli_skt,
                              (tt_u8_t *)recv_buf + pos,
                              __echo_buf_len - pos,
                              &len,
                              NULL);
        pos += len;
        if (pos < __echo_buf_len)
            goto recv_ag;

        // all received
        if (memcmp(buf, recv_buf, __echo_buf_len) != 0) {
            TT_ERROR("recv and send are different");
            return TT_FAIL;
        }

        pos = 0;
    }
    return TT_SUCCESS;
}

static tt_result_t __echo_svr_udp(tt_skt_t *svr_skt)
{
    char buf[__echo_buf_len];
    tt_u32_t len;
    int pos = 0;
    tt_result_t ret;
    tt_sktaddr_t remote_addr;
    int times = 0;

    while (times++ < 10) {
        ret = tt_skt_recvfrom(svr_skt,
                              (tt_u8_t *)buf + pos,
                              __echo_buf_len - pos,
                              &len,
                              &remote_addr);

        pos += len;
        if (pos < __echo_buf_len)
            continue;

        ret = tt_skt_recvfrom(svr_skt,
                              (tt_u8_t *)buf + pos,
                              0,
                              &len,
                              &remote_addr);
        TT_ASSERT(TT_OK(ret));
        TT_ASSERT(len == 0);

        // until buffer is full
        pos = 0;
    sag:
        ret = tt_skt_sendto(svr_skt,
                            (tt_u8_t *)buf + pos,
                            __echo_buf_len - pos,
                            &len,
                            &remote_addr);
        if (ret != TT_SUCCESS)
            return TT_FAIL;
        pos += len;
        if (pos < __echo_buf_len)
            goto sag;

        ret =
            tt_skt_sendto(svr_skt, (tt_u8_t *)buf + pos, 0, &len, &remote_addr);
        TT_ASSERT(TT_OK(ret));
        TT_ASSERT(len == 0);

        // receive
        pos = 0;
    }
    return TT_SUCCESS;
}

static tt_result_t __udp_thread_c(IN tt_thread_t *thread, IN void *param)
{
    tt_skt_t sk4;
    tt_u16_t port = (tt_u16_t)(tt_ptrdiff_t)param;
    tt_sktaddr_t server_addr;
    // tt_sktaddr_t server_addr6;
    tt_result_t ret;

    tt_sktaddr_init(&server_addr, TT_NET_AF_INET);
    ret = tt_sktaddr_set_addr_p(&server_addr, "127.0.0.1");
    tt_sktaddr_set_port(&server_addr, port);

    ret = tt_skt_create(&sk4, TT_NET_AF_INET, TT_NET_PROTO_UDP, NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    // ret = tt_skt_connect(&sk4,&server_addr);

    ret = __echo_cli_udp(&sk4, &server_addr);
    if (!TT_OK(ret))
        return TT_FAIL;

    tt_skt_destroy(&sk4);
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_udp_server)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_skt_t sk4;
    tt_result_t ret;
    tt_skt_attr_t attr;

    tt_sktaddr_t server_addr;
    tt_skt_t new_skt;
    tt_thread_t *tthd;

    TT_TEST_CASE_ENTER()
    // test start

    tt_skt_attr_default(&attr);
    attr.config_reuse_addr = TT_TRUE;
    attr.reuse_addr = TT_TRUE;

    // linux+udp_ipv4+ipv6_only = fail
    // tt_skt_attr_set_ipv6only(&attr, TT_FALSE);

    tt_sktaddr_init(&server_addr, TT_NET_AF_INET);
    ret = tt_sktaddr_set_addr_p(&server_addr, TT_SKTADDR_ANY);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    tt_sktaddr_set_port(&server_addr, __TPORT + 2);

    // server socket
    ret = tt_udp_server(&sk4, TT_NET_AF_INET, &attr, &server_addr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_skt_listen(&sk4, TT_SKT_BACKLOG_DEFAULT);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    ret = tt_skt_accept(&sk4, &new_skt, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    ////////////////////

    tthd = tt_thread_create("", __udp_thread_c, (void *)(__TPORT + 2), NULL);
    TT_TEST_CHECK_NOT_EQUAL(tthd, NULL, "");

    // an echo server
    ret = __echo_svr_udp(&sk4);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_thread_wait(tthd);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    /////////////////////

    //

    ////////////////////

    ret = tt_skt_destroy(&sk4);

    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __udp_thread_c6(IN tt_thread_t *thread, IN void *param)
{
    tt_skt_t sk6;
    tt_u16_t port = (tt_u16_t)(tt_ptrdiff_t)param;
    tt_sktaddr_t server_addr;
    // tt_sktaddr_t server_addr6;
    tt_result_t ret;

    tt_sktaddr_init(&server_addr, TT_NET_AF_INET6);
    ret = tt_sktaddr_set_addr_p(&server_addr, "::1");
    tt_sktaddr_set_port(&server_addr, port);

    ret = tt_skt_create(&sk6, TT_NET_AF_INET6, TT_NET_PROTO_UDP, NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    // ret = tt_skt_connect(&sk4,&server_addr);

    ret = __echo_cli_udp(&sk6, &server_addr);
    if (!TT_OK(ret))
        return TT_FAIL;

    tt_skt_destroy(&sk6);
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_udp_server6)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_skt_t sk6;
    tt_result_t ret;
    tt_skt_attr_t attr;

    tt_sktaddr_t server_addr;
    tt_skt_t new_skt;
    tt_thread_t *tthd;

    TT_TEST_CASE_ENTER()
    // test start

    tt_skt_attr_default(&attr);
    attr.config_reuse_addr = TT_TRUE;
    attr.reuse_addr = TT_TRUE;

    tt_skt_attr_set_ipv6only(&attr, TT_FALSE);

    tt_sktaddr_init(&server_addr, TT_NET_AF_INET6);
    ret = tt_sktaddr_set_addr_p(&server_addr, TT_SKTADDR_ANY);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    tt_sktaddr_set_port(&server_addr, __TPORT + 3);

    // server socket
    ret = tt_udp_server(&sk6, TT_NET_AF_INET6, &attr, &server_addr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_skt_listen(&sk6, TT_SKT_BACKLOG_DEFAULT);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");
    ret = tt_skt_accept(&sk6, &new_skt, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    ////////////////////

    // ipv4 client
    tthd = tt_thread_create("", __udp_thread_c, (void *)(__TPORT + 3), NULL);
    TT_TEST_CHECK_NOT_EQUAL(tthd, NULL, "");

    // an echo server
    ret = __echo_svr_udp(&sk6);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_thread_wait(tthd);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    /////////////////////

    // ipv6 client
    tthd = tt_thread_create("", __udp_thread_c6, (void *)(__TPORT + 3), NULL);
    TT_TEST_CHECK_NOT_EQUAL(tthd, NULL, "");

    // an echo server
    ret = __echo_svr_udp(&sk6);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_thread_wait(tthd);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ////////////////////

    ret = tt_skt_destroy(&sk6);

    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

////////////////////////////////////////////////////////////////////

static tt_sktaddr_addr_t __mc_addr;
#define ___mulc_addr4 "224.0.0.100"
#define ___mulc_port4 12344
#define ___mulc_port4_cli 12444
#define ___mulc_content "multicast packet"

static tt_sktaddr_addr_t __mc_addr6;
#define ___mulc_addr6 "ff02::1"
#define ___mulc_port6 12366
#define ___mulc_port6_cli 12466
#define ___mulc_content6 "multicast packet ipv6"

#if 1
//#define ___mcast_itf "wireless_0")
//#define ___mcast_itf "eth0")
#define ___mcast_itf __TLOCAL_ITF
#else
#define ___mcast_itf NULL
#endif

static tt_result_t __mc_thread_4(IN tt_thread_t *thread, IN void *param)
{
    tt_skt_t sk4;
    tt_u16_t port = (tt_u16_t)(tt_ptrdiff_t)param;
    tt_sktaddr_t server_addr;
    tt_sktaddr_t client_addr;
    // tt_sktaddr_t server_addr6;
    tt_result_t ret;

    tt_u32_t len;
    tt_u8_t buf[100] = {0};

    tt_sktaddr_init(&server_addr, TT_NET_AF_INET);
    tt_sktaddr_set_addr_n(&server_addr, &__mc_addr);
    tt_sktaddr_set_port(&server_addr, port);

    ret = tt_skt_create(&sk4, TT_NET_AF_INET, TT_NET_PROTO_UDP, NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    tt_sktaddr_init(&client_addr, TT_NET_AF_INET);
    ret = tt_sktaddr_set_addr_p(&client_addr, TT_SKTADDR_ANY);
    if (!TT_OK(ret))
        return TT_FAIL;
    tt_sktaddr_set_port(&client_addr, ___mulc_port4_cli);

    ret = tt_skt_bind(&sk4, &client_addr);
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = tt_skt_join_mcast(&sk4, &__mc_addr, ___mcast_itf);
    if (!TT_OK(ret))
        return TT_FAIL;

    // send multicast
    ret = tt_skt_sendto(&sk4,
                        (tt_u8_t *)___mulc_content,
                        sizeof(___mulc_content),
                        &len,
                        &server_addr);
    if (!TT_OK(ret))
        return TT_FAIL;
    if (len != sizeof(___mulc_content))
        return TT_FAIL;

    // recv multicast
    ret = tt_skt_recvfrom(&sk4, (tt_u8_t *)buf, sizeof(buf), &len, NULL);
    if (!TT_OK(ret))
        return TT_FAIL;
    if (tt_strncmp((const char *)buf, ___mulc_content, sizeof(buf)) != 0)
        return TT_FAIL;

    tt_skt_destroy(&sk4);
    return TT_SUCCESS;
}

static tt_result_t __mc_thread_6(IN tt_thread_t *thread, IN void *param)
{
    tt_skt_t sk6;
    tt_u16_t port = (tt_u16_t)(tt_ptrdiff_t)param;
    tt_sktaddr_t server_addr;
    tt_sktaddr_t client_addr;
    // tt_sktaddr_t server_addr6;
    tt_result_t ret;

    tt_u32_t len;
    tt_u8_t buf[100] = {0};

    tt_sktaddr_init(&server_addr, TT_NET_AF_INET6);
    tt_sktaddr_set_addr_n(&server_addr, &__mc_addr6);
    tt_sktaddr_set_port(&server_addr, port);

    ret = tt_skt_create(&sk6, TT_NET_AF_INET6, TT_NET_PROTO_UDP, NULL);
    if (!TT_OK(ret))
        return TT_FAIL;

    tt_sktaddr_init(&client_addr, TT_NET_AF_INET6);
    ret = tt_sktaddr_set_addr_p(&client_addr, TT_SKTADDR_ANY);
    if (!TT_OK(ret))
        return TT_FAIL;
    tt_sktaddr_set_port(&client_addr, ___mulc_port6_cli);
    if (1 || ((tt_strncmp(__TLOCAL_IP6, "fe80", 4) == 0) &&
              (__TLOCAL_ITF != NULL))) {
        tt_sktaddr_set_scope_p(&client_addr, __TLOCAL_ITF);
    }

    ret = tt_skt_bind(&sk6, &client_addr);
    if (!TT_OK(ret))
        return TT_FAIL;

    ret = tt_skt_join_mcast(&sk6, &__mc_addr6, ___mcast_itf);
    if (!TT_OK(ret))
        return TT_FAIL;

    // send multicast
    ret = tt_skt_sendto(&sk6,
                        (tt_u8_t *)___mulc_content6,
                        sizeof(___mulc_content6),
                        &len,
                        &server_addr);
    if (!TT_OK(ret))
        return TT_FAIL;
    if (len != sizeof(___mulc_content6))
        return TT_FAIL;

    // recv multicast
    ret = tt_skt_recvfrom(&sk6, (tt_u8_t *)buf, sizeof(buf), &len, NULL);
    if (!TT_OK(ret))
        return TT_FAIL;
    if (tt_strncmp((const char *)buf, ___mulc_content6, sizeof(buf)) != 0)
        return TT_FAIL;

    tt_skt_destroy(&sk6);
    return TT_SUCCESS;
}


TT_TEST_ROUTINE_DEFINE(tt_unit_test_udp_multicast)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_skt_t sk4;
    tt_sktaddr_t server_addr4;
    tt_sktaddr_t remote_addr4;

    tt_skt_t sk6;
    tt_sktaddr_t server_addr6;
    tt_sktaddr_t remote_addr6;

    tt_result_t ret;
    tt_thread_t *tthd;
    tt_u8_t buf[100] = {0};
    tt_u32_t recv_len;

    TT_TEST_CASE_ENTER()
    // test start

    // init mc addr
    ret = tt_sktaddr_addr_p2n(TT_NET_AF_INET, ___mulc_addr4, &__mc_addr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ////////////////////////////////////////////////////////
    // ipv4
    ////////////////////////////////////////////////////////

    // create server addr
    tt_sktaddr_init(&server_addr4, TT_NET_AF_INET);
    ret = tt_sktaddr_set_addr_p(&server_addr4, TT_SKTADDR_ANY);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    tt_sktaddr_set_port(&server_addr4, ___mulc_port4);

    // create server skt
    ret = tt_udp_server(&sk4, TT_NET_AF_INET, NULL, &server_addr4);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_skt_join_mcast(&sk4, &__mc_addr, ___mcast_itf);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // create client thread to send multicast packet
    tthd = tt_thread_create("", __mc_thread_4, (void *)___mulc_port4, NULL);
    TT_TEST_CHECK_NOT_EQUAL(tthd, NULL, "");

    // recv
    ret = tt_skt_recvfrom(&sk4, buf, sizeof(buf), &recv_len, &remote_addr4);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(tt_strncmp((const char *)buf,
                                   ___mulc_content,
                                   sizeof(buf)),
                        0,
                        "");

    // send
    tt_sktaddr_init(&remote_addr4, TT_NET_AF_INET);
    tt_sktaddr_set_addr_n(&remote_addr4, &__mc_addr);
    tt_sktaddr_set_port(&remote_addr4, ___mulc_port4_cli);

    ret = tt_skt_sendto(&sk4,
                        buf,
                        sizeof(___mulc_content),
                        &recv_len,
                        &remote_addr4);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(tt_strncmp((const char *)buf,
                                   ___mulc_content,
                                   sizeof(buf)),
                        0,
                        "");

    // wait client end
    ret = tt_thread_wait(tthd);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");


    tt_skt_destroy(&sk4);

    ////////////////////////////////////////////////////////
    // ipv6
    ////////////////////////////////////////////////////////

    // init mc addr
    ret = tt_sktaddr_addr_p2n(TT_NET_AF_INET6, ___mulc_addr6, &__mc_addr6);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // create server addr
    tt_sktaddr_init(&server_addr6, TT_NET_AF_INET6);
    ret = tt_sktaddr_set_addr_p(&server_addr6, TT_SKTADDR_ANY);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    tt_sktaddr_set_port(&server_addr6, ___mulc_port6);
    if (1 || ((tt_strncmp(__TLOCAL_IP6, "fe80", 4) == 0) &&
              (__TLOCAL_ITF != NULL))) {
        tt_sktaddr_set_scope_p(&server_addr6, __TLOCAL_ITF);
    }

    // create server skt
    ret = tt_udp_server(&sk6, TT_NET_AF_INET6, NULL, &server_addr6);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_skt_join_mcast(&sk6, &__mc_addr6, ___mcast_itf);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // create client thread to send multicast packet
    tthd = tt_thread_create("", __mc_thread_6, (void *)___mulc_port6, NULL);
    TT_TEST_CHECK_NOT_EQUAL(tthd, NULL, "");

    // recv
    ret = tt_skt_recvfrom(&sk6, buf, sizeof(buf), &recv_len, &remote_addr6);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(tt_strncmp((const char *)buf,
                                   ___mulc_content6,
                                   sizeof(buf)),
                        0,
                        "");

    // send
    tt_sktaddr_init(&remote_addr6, TT_NET_AF_INET6);
    tt_sktaddr_set_addr_n(&remote_addr6, &__mc_addr6);
    tt_sktaddr_set_port(&remote_addr6, ___mulc_port6_cli);

    ret = tt_skt_sendto(&sk6,
                        buf,
                        sizeof(___mulc_content6),
                        &recv_len,
                        &remote_addr6);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(tt_strncmp((const char *)buf,
                                   ___mulc_content6,
                                   sizeof(buf)),
                        0,
                        "");

    // wait client end
    ret = tt_thread_wait(tthd);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");


    tt_skt_destroy(&sk6);

    // test end
    TT_TEST_CASE_LEAVE()
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

#if 1
static tt_evcenter_t __a1_server_evc;
static tt_evcenter_t __a1_client_evc;

static tt_result_t __a1_ret;

#define __a1_ev_done 0x1234a

#define __a1_hs_num 10
#define __a1_buf_num 10
#define __a1_buf_size 100
static tt_u8_t __a1_sample_bufs[__a1_buf_num][__a1_buf_size];
static tt_u8_t __a1_svr_bufs[__a1_hs_num][__a1_buf_num][__a1_buf_size];
static tt_u8_t __a1_cli_bufs[__a1_hs_num][__a1_buf_num][__a1_buf_size];
static tt_u32_t __a1_buf_n[__a1_hs_num + 1];

static tt_skt_t __a1_server;
static tt_skt_t __a1_server_acc;
static tt_skt_t __a1_server_acc2;
static tt_skt_t __a1_client;

static tt_u32_t __a1_error_line;

static int __a1_cli_cnt;
static int __a1_svr_cnt;

static int __a1_svr_port_shift;
static int __a1_first_run = 3;

void __a1_on_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
    tt_result_t ret;

#ifdef __DBG_SKT_AIO_UT
    TT_DETAIL("%p, server: %p, client: %p", skt, &__a1_server, &__a1_client);
#endif

    if ((tt_evc_current() != &__a1_client_evc) &&
        (tt_evc_current() != &__a1_server_evc)) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    // can not do more destroy
    ret = tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_RD);
    if (TT_OK(ret)) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
    ret = tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_RDWR);
    if (TT_OK(ret)) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if (skt == &__a1_client || skt == &__a1_server) {
        tt_evc_exit(TT_LOCAL_EVC);
    } else {
        // this is an accepted socket
        tt_async_skt_destroy(&__a1_server, TT_FALSE);
    }
}

void __a1_on_send(IN tt_skt_t *skt,
                  IN tt_blob_t *buf_array,
                  IN tt_u32_t buf_num,
                  IN tt_skt_aioctx_t *aioctx,
                  IN tt_u32_t send_len)
{
    tt_result_t ret;

#ifdef __DBG_SKT_AIO_UT
    TT_DETAIL("%d", (int)(tt_ptrdiff_t)aioctx->cb_param);
#endif

    if (aioctx->result != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if ((tt_evc_current() != &__a1_client_evc) &&
        (tt_evc_current() != &__a1_server_evc)) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if ((int)(tt_ptrdiff_t)aioctx->cb_param == (__a1_hs_num - 1)) {
        ret = tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_WR);
        if (ret != TT_SUCCESS) {
            __a1_ret = TT_FAIL;
            __a1_error_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }

        ret = tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_WR);
        if (TT_OK(ret)) {
            __a1_ret = TT_FAIL;
            __a1_error_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    }
}

void __a1_on_recv(IN tt_skt_t *skt,
                  IN tt_blob_t *buf_array,
                  IN tt_u32_t buf_num,
                  IN tt_skt_aioctx_t *aioctx,
                  IN tt_u32_t recv_len)
{
    tt_result_t ret;
    int i, j;

#ifdef __DBG_SKT_AIO_UT
    TT_DETAIL("%d", (int)(tt_ptrdiff_t)aioctx->cb_param);
#endif

    if (__a1_svr_cnt != (int)(tt_ptrdiff_t)aioctx->cb_param) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
    ++__a1_svr_cnt;

    if ((tt_evc_current() != &__a1_client_evc) &&
        (tt_evc_current() != &__a1_server_evc)) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if (aioctx->result == TT_END) {
        if (recv_len != 0) {
            __a1_ret = TT_FAIL;
            __a1_error_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }

        tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_RD);
        // wait for on_destroy
        return;
    }

    if (aioctx->result != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
    if (recv_len % __a1_buf_size != 0) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
    // n = recv_len / __a1_buf_size;

    for (i = 0; i < (int)buf_num; ++i) {
        for (j = 0; j < __a1_buf_size; ++j) {
            if (buf_array[i].addr[j] != __a1_sample_bufs[i][j]) {
                __a1_ret = TT_FAIL;
                __a1_error_line = __LINE__;
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }
        }
    }

    ret = tt_skt_send_async(skt,
                            buf_array,
                            buf_num,
                            __a1_on_send,
                            aioctx->cb_param);
    if (ret != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

void __a1_on_accept(IN tt_skt_t *listening_skt,
                    IN tt_skt_t *new_skt,
                    IN tt_skt_aioctx_t *aioctx)
{
    tt_result_t ret;
    tt_blob_t bufs[__a1_buf_num];
    int i, j;

#ifdef __DBG_SKT_AIO_UT
    TT_DETAIL("accept");
#endif

    if (aioctx->result != TT_SUCCESS) {
        return;
    }

    if ((tt_evc_current() != &__a1_server_evc)) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    for (j = 0; j < __a1_hs_num + 1; ++j) {
        for (i = 0; i < __a1_buf_num; ++i) {
            bufs[i].addr = __a1_svr_bufs[j][i];
            bufs[i].len = __a1_buf_size;
        }

        ret = tt_skt_recv_async(new_skt,
                                bufs,
                                __a1_buf_n[j],
                                __a1_on_recv,
                                (void *)(tt_ptrdiff_t)j);
        if (ret != TT_SUCCESS) {
            __a1_ret = TT_FAIL;
            __a1_error_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    }
}

tt_result_t __a1_server_on_init(IN struct tt_evcenter_s *evc,
                                IN void *on_init_param)
{
    tt_result_t ret;
    tt_sktaddr_t _svr_addr;
    tt_skt_exit_t de;
    tt_skt_attr_t _svr_attr;
    tt_skt_attr_t _acc_attr;

    tt_net_family_t af;
    int close_svr = 0;

    tt_skt_attr_default(&_svr_attr);
    tt_skt_attr_set_reuseaddr(&_svr_attr, TT_TRUE);
    tt_skt_attr_set_reuseport(&_svr_attr, TT_TRUE);

    tt_skt_attr_default(&_acc_attr);

    de.on_destroy = __a1_on_destroy;
    de.cb_param = NULL;

    if (on_init_param == NULL) {
        af = TT_NET_AF_INET;

        tt_sktaddr_init(&_svr_addr, TT_NET_AF_INET);
        tt_sktaddr_set_addr_p(&_svr_addr, __TLOCAL_IP);
        tt_sktaddr_set_port(&_svr_addr, 62250 + __a1_svr_port_shift);
    } else {
        af = TT_NET_AF_INET6;

        tt_sktaddr_init(&_svr_addr, TT_NET_AF_INET6);
        tt_sktaddr_set_addr_p(&_svr_addr, __TLOCAL_IP6_MAPPED);
        tt_sktaddr_set_port(&_svr_addr, 62250 + __a1_svr_port_shift);

        tt_skt_attr_set_ipv6only(&_svr_attr, TT_FALSE);
        // tt_skt_attr_set_ipv6only(&_acc_attr, TT_FALSE);
    }

    // create socket
    ret = tt_tcp_server_async(&__a1_server,
                              af,
                              &_svr_attr,
                              &_svr_addr,
                              TT_SKT_BACKLOG_DEFAULT,
                              &de);
    if (ret != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        return TT_FAIL;
    }

    // start 2 accept
    if (__a1_first_run)
        ret = tt_skt_accept_async(&__a1_server,
                                  &__a1_server_acc2,
                                  &_acc_attr,
                                  &de,
                                  __a1_on_accept,
                                  NULL);
    else
        ret = tt_skt_accept_async(&__a1_server,
                                  &__a1_server_acc2,
                                  NULL,
                                  &de,
                                  __a1_on_accept,
                                  NULL);
    if (ret != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        return TT_FAIL;
    }

    if (__a1_first_run)
        ret = tt_skt_accept_async(&__a1_server,
                                  &__a1_server_acc,
                                  &_acc_attr,
                                  &de,
                                  __a1_on_accept,
                                  NULL);
    else
        ret = tt_skt_accept_async(&__a1_server,
                                  &__a1_server_acc,
                                  NULL,
                                  &de,
                                  __a1_on_accept,
                                  NULL);
    if (ret != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void __a1_on_cli_recv(IN tt_skt_t *skt,
                      IN tt_blob_t *buf_array,
                      IN tt_u32_t buf_num,
                      IN tt_skt_aioctx_t *aioctx,
                      IN tt_u32_t recv_len)
{
    // tt_result_t ret;
    int i, j, n;

#ifdef __DBG_SKT_AIO_UT
    TT_DETAIL("%d", (int)(tt_ptrdiff_t)aioctx->cb_param);
#endif

    if (aioctx->result == TT_END) {
        if (recv_len != 0) {
            __a1_ret = TT_FAIL;
            __a1_error_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }

        tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_RD);

        // can not recv after closing
        do {
            tt_blob_t __bf = {(tt_u8_t *)&i, sizeof(i)};
            tt_result_t ret =
                tt_skt_recv_async(skt, &__bf, 1, __a1_on_cli_recv, NULL);
            if (ret == TT_SUCCESS) {
                __a1_ret = TT_FAIL;
                __a1_error_line = __LINE__;
                return;
            }
        } while (0);

        // wait for on_destroy
        return;
    }

    if ((tt_evc_current() != &__a1_client_evc)) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if (aioctx->result != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
    if (recv_len % __a1_buf_size != 0) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
    n = recv_len / __a1_buf_size;

    for (i = 0; i < n; ++i) {
        for (j = 0; j < __a1_buf_size; ++j) {
            if (buf_array[i].addr[j] != __a1_sample_bufs[i][j]) {
                __a1_ret = TT_FAIL;
                __a1_error_line = __LINE__;
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }
        }
    }
}

void __a1_on_cli_send(IN tt_skt_t *skt,
                      IN tt_blob_t *buf_array,
                      IN tt_u32_t buf_num,
                      IN tt_skt_aioctx_t *aioctx,
                      IN tt_u32_t send_len)
{
    tt_result_t ret;
    tt_blob_t bufs[__a1_buf_num];
    int i;

#ifdef __DBG_SKT_AIO_UT
    TT_DETAIL("%d", (int)(tt_ptrdiff_t)aioctx->cb_param);
#endif

    if (__a1_cli_cnt != (int)(tt_ptrdiff_t)aioctx->cb_param) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
    ++__a1_cli_cnt;

    if ((tt_evc_current() != &__a1_client_evc)) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if (aioctx->result != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    for (i = 0; i < __a1_buf_num; ++i) {
        bufs[i].addr = __a1_cli_bufs[(int)(tt_ptrdiff_t)aioctx->cb_param][i];
        bufs[i].len = __a1_buf_size;
    }

    // recv
    ret = tt_skt_recv_async(skt,
                            bufs,
                            __a1_buf_n[(int)(tt_ptrdiff_t)aioctx->cb_param],
                            __a1_on_cli_recv,
                            aioctx->cb_param);
    if (ret != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if ((int)(tt_ptrdiff_t)aioctx->cb_param == (__a1_hs_num - 1)) {
        // it's expected no more recv, this final recv is to detect io end event
        ret = tt_skt_recv_async(skt,
                                bufs,
                                __a1_buf_n[(int)(tt_ptrdiff_t)aioctx->cb_param],
                                __a1_on_cli_recv,
                                aioctx->cb_param);
        if (ret != TT_SUCCESS) {
            __a1_ret = TT_FAIL;
            __a1_error_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    }
}

void __a1_on_connect(IN tt_skt_t *skt,
                     IN tt_sktaddr_t *remote_addr,
                     IN tt_skt_aioctx_t *aioctx)
{
    tt_blob_t bufs[__a1_buf_num];
    int i;
    tt_result_t ret;

#ifdef __DBG_SKT_AIO_UT
    TT_DETAIL("%d", (int)(tt_ptrdiff_t)aioctx->cb_param);
#endif

    if (aioctx->result != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if ((tt_evc_current() != &__a1_client_evc)) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    for (i = 0; i < __a1_buf_num; ++i) {
        bufs[i].addr = __a1_sample_bufs[i];
        bufs[i].len = __a1_buf_size;
    }

    for (i = 0; i < __a1_hs_num; ++i) {
        ret = tt_skt_send_async(skt,
                                bufs,
                                __a1_buf_n[i],
                                __a1_on_cli_send,
                                (void *)(tt_ptrdiff_t)i);
        if (ret != TT_SUCCESS) {
            __a1_ret = TT_FAIL;
            __a1_error_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    }

    // all sent
    ret = tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_WR);
    if (ret != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    ret = tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_WR);
    if (ret == TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    // can not connect after sd write
    do {
        tt_result_t ret =
            tt_skt_connect_async(skt, remote_addr, __a1_on_connect, NULL);
        if (ret == TT_SUCCESS) {
            __a1_ret = TT_FAIL;
            __a1_error_line = __LINE__;
            return;
        }
    } while (0);
    // can not write after closing
    do {
        tt_blob_t __bf = {(tt_u8_t *)&i, sizeof(i)};
        tt_result_t ret =
            tt_skt_send_async(skt, &__bf, 1, __a1_on_cli_send, NULL);
        if (ret == TT_SUCCESS) {
            __a1_ret = TT_FAIL;
            __a1_error_line = __LINE__;
            return;
        }
    } while (0);
}

// tt_result_t __a1_client_thread_func(IN void *param, IN struct tt_thread_s
// *thread)
tt_result_t __a1_client_on_init(IN struct tt_evcenter_s *evc,
                                IN void *on_init_param)
{
    tt_result_t ret;
    tt_sktaddr_t _svr_addr;
    tt_skt_exit_t de;
    tt_skt_attr_t _cli_attr;

    tt_net_family_t af;

    tt_skt_attr_default(&_cli_attr);
    tt_skt_attr_set_reuseaddr(&_cli_attr, TT_TRUE);
    tt_skt_attr_set_reuseport(&_cli_attr, TT_TRUE);

    if (on_init_param == NULL)
        af = TT_NET_AF_INET;
    else {
        af = TT_NET_AF_INET6;
        tt_skt_attr_set_ipv6only(&_cli_attr, TT_FALSE);
    }

    de.on_destroy = __a1_on_destroy;
    de.cb_param = NULL;

    ret = tt_async_skt_create(&__a1_client,
                              af,
                              TT_NET_PROTO_TCP,
                              TT_SKT_ROLE_TCP_CONNECT,
                              &_cli_attr,
                              &de);

    if (ret != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        return TT_FAIL;
    }

    tt_sktaddr_init(&_svr_addr, TT_NET_AF_INET);
    tt_sktaddr_set_addr_p(&_svr_addr, __TLOCAL_IP);
    tt_sktaddr_set_port(&_svr_addr, 62250 + __a1_svr_port_shift);

    if (af == TT_NET_AF_INET6) {
        tt_sktaddr_map4to6(&_svr_addr, &_svr_addr);
    }

    ret = tt_skt_connect_async(&__a1_client, &_svr_addr, __a1_on_connect, NULL);
    if (ret != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        return TT_FAIL;
    }

    // init done
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_tcp_async_ipv4)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t ret;
    int i, j;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    __a1_ret = TT_SUCCESS;

    srand((tt_u32_t)time(NULL));
    for (i = 0; i < __a1_buf_num; ++i) {
        for (j = 0; j < __a1_buf_size; ++j) {
            __a1_sample_bufs[i][j] = (tt_u8_t)(((i + j) % 25) + 'a');
        }
    }

    if (__a1_first_run)
        --__a1_first_run;
    __a1_cli_cnt = 0;
    __a1_svr_cnt = 0;
    ++__a1_svr_port_shift;

    for (i = 0; i <= __a1_hs_num; ++i) {
        __a1_buf_n[i] = rand() % __a1_buf_num + 1;
    }

    tt_evc_attr_default(&evc_attr);

    // server
    // ret = tt_thread_create(&__a1_server_thread, "",
    //                       __a1_server_thread_func, NULL, NULL, &ta);
    evc_attr.on_init = __a1_server_on_init;
    ret = tt_evc_create(&__a1_server_evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // client
    evc_attr.on_init = __a1_client_on_init;
    ret = tt_evc_create(&__a1_client_evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // wait
    ret = tt_evc_wait(&__a1_client_evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_evc_wait(&__a1_server_evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__a1_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_tcp_async_ipv6)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t ret;
    int i, j;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    __a1_ret = TT_SUCCESS;

    srand((tt_u32_t)time(NULL));
    for (i = 0; i < __a1_buf_num; ++i) {
        for (j = 0; j < __a1_buf_size; ++j) {
            __a1_sample_bufs[i][j] = (tt_u8_t)rand();
        }
    }

    if (__a1_first_run)
        --__a1_first_run;
    __a1_cli_cnt = 0;
    __a1_svr_cnt = 0;
    ++__a1_svr_port_shift;

    for (i = 0; i <= __a1_hs_num; ++i) {
        __a1_buf_n[i] = rand() % __a1_buf_num + 1;
    }

    tt_evc_attr_default(&evc_attr);

    // server
    // ret = tt_thread_create(&__a1_server_thread, "",
    //                       __a1_server_thread_func, NULL, NULL, &ta);
    evc_attr.on_init = __a1_server_on_init;
    evc_attr.on_init_param = (void *)1;
    ret = tt_evc_create(&__a1_server_evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // client
    evc_attr.on_init = __a1_client_on_init;
    evc_attr.on_init_param = (void *)1;
    ret = tt_evc_create(&__a1_client_evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // wait
    ret = tt_evc_wait(&__a1_client_evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_evc_wait(&__a1_server_evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__a1_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

void __a2_on_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
#ifdef __DBG_SKT_AIO_UT
    TT_DETAIL("__a2_on_destroy: %p, cli[%p], svr[%p]",
              skt,
              &__a1_client,
              &__a1_server);
#endif

    tt_evc_exit(TT_LOCAL_EVC);
}

void __a2_svr_on_sendto(IN tt_skt_t *skt,
                        IN tt_blob_t *buf_array,
                        IN tt_u32_t buf_num,
                        IN tt_sktaddr_t *remote_addr,
                        IN tt_skt_aioctx_t *aioctx,
                        IN tt_u32_t send_len)
{
// int i,j, n;

#ifdef __DBG_SKT_AIO_UT
    TT_DETAIL("%d", (int)(tt_ptrdiff_t)aioctx->cb_param);
#endif

    if (aioctx->result != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
    if (send_len != buf_num * __a1_buf_size) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if ((tt_evc_current() != &__a1_client_evc) &&
        (tt_evc_current() != &__a1_server_evc)) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if ((int)(tt_ptrdiff_t)aioctx->cb_param == (__a1_hs_num - 1)) {
        tt_async_skt_destroy(skt, TT_FALSE);
    }
}

void __a2_svr_on_recvfrom(IN tt_skt_t *skt,
                          IN tt_blob_t *buf_array,
                          IN tt_u32_t buf_num,
                          IN tt_skt_aioctx_t *aioctx,
                          IN tt_u32_t recv_len,
                          IN tt_sktaddr_t *remote_addr)
{
    tt_result_t ret;
    int i, j, n;

#ifdef __DBG_SKT_AIO_UT
    TT_DETAIL("%d", (int)(tt_ptrdiff_t)aioctx->cb_param);
#endif

    if ((tt_evc_current() != &__a1_client_evc) &&
        (tt_evc_current() != &__a1_server_evc)) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if (__a1_svr_cnt != (int)(tt_ptrdiff_t)aioctx->cb_param) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
    ++__a1_svr_cnt;

    if (aioctx->result != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
    // if (recv_len != __a1_buf_n[(int)aioctx->cb_param] * __a1_buf_size) {
    if (recv_len % __a1_buf_size != 0) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    n = recv_len / __a1_buf_size;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < __a1_buf_size; ++j) {
            if (buf_array[i].addr[j] != __a1_sample_bufs[i][j]) {
                __a1_ret = TT_FAIL;
                __a1_error_line = __LINE__;
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }
        }
    }

    if (skt->family == TT_NET_AF_INET6) {
        ret = tt_skt_sendto_async(skt,
                                  buf_array,
                                  n,
                                  tt_sktaddr_map4to6(remote_addr, remote_addr),
                                  __a2_svr_on_sendto,
                                  aioctx->cb_param);

    } else {
        ret = tt_skt_sendto_async(skt,
                                  buf_array,
                                  n,
                                  remote_addr,
                                  __a2_svr_on_sendto,
                                  aioctx->cb_param);
    }
    if (ret != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

tt_result_t __a2_recv_on_init(IN struct tt_evcenter_s *evc,
                              IN void *on_init_param)
{
    tt_result_t ret;
    tt_sktaddr_t _svr_addr;
    tt_skt_exit_t de;
    tt_skt_attr_t _svr_attr;
    int i, j;

    tt_net_family_t af;
    int close_svr = 0;

    tt_blob_t bufs[__a1_buf_num];

    tt_skt_attr_default(&_svr_attr);
    tt_skt_attr_set_reuseaddr(&_svr_attr, TT_TRUE);
    tt_skt_attr_set_reuseport(&_svr_attr, TT_TRUE);

    de.on_destroy = __a2_on_destroy;
    de.cb_param = NULL;

    if (on_init_param != NULL) {
        af = TT_NET_AF_INET6;

        tt_sktaddr_init(&_svr_addr, TT_NET_AF_INET6);
        tt_sktaddr_set_addr_p(&_svr_addr, __TLOCAL_IP6_MAPPED);
        tt_sktaddr_set_port(&_svr_addr, 52250);

        tt_skt_attr_set_ipv6only(&_svr_attr, TT_FALSE);
    } else {
        af = TT_NET_AF_INET;

        tt_sktaddr_init(&_svr_addr, TT_NET_AF_INET);
        tt_sktaddr_set_addr_p(&_svr_addr, __TLOCAL_IP);
        tt_sktaddr_set_port(&_svr_addr, 52250);
    }


    // create socket
    ret = tt_udp_server_async(&__a1_server, af, &_svr_attr, &_svr_addr, &de);
    if (ret != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        return TT_FAIL;
    }

    // submit recvfrom
    for (j = 0; j < __a1_hs_num; ++j) {
        for (i = 0; i < __a1_buf_num; ++i) {
            bufs[i].addr = __a1_svr_bufs[j][i];
            bufs[i].len = __a1_buf_size;
        }

        ret = tt_skt_recvfrom_async(&__a1_server,
                                    bufs,
                                    __a1_buf_num,
                                    __a2_svr_on_recvfrom,
                                    (void *)(tt_ptrdiff_t)j);
        if (ret != TT_SUCCESS) {
            __a1_ret = TT_FAIL;
            __a1_error_line = __LINE__;
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}

static void __a2_cli_on_sendto(IN tt_skt_t *skt,
                               IN tt_blob_t *buf_array,
                               IN tt_u32_t buf_num,
                               IN tt_sktaddr_t *remote_addr,
                               IN tt_skt_aioctx_t *aioctx,
                               IN tt_u32_t send_len);

static void __a2_cli_on_recvfrom(IN tt_skt_t *skt,
                                 IN tt_blob_t *buf_array,
                                 IN tt_u32_t buf_num,
                                 IN tt_skt_aioctx_t *aioctx,
                                 IN tt_u32_t recv_len,
                                 IN tt_sktaddr_t *remote_addr)
{
    int i, j, n;

#ifdef __DBG_SKT_AIO_UT
    TT_DETAIL("%d", (int)(tt_ptrdiff_t)aioctx->cb_param);
#endif

    if ((tt_evc_current() != &__a1_client_evc) &&
        (tt_evc_current() != &__a1_server_evc)) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    // at least [0, __a1_hs_num-1] must succeed
    if ((aioctx->result != TT_SUCCESS) &&
        ((tt_ptrdiff_t)aioctx->cb_param < __a1_hs_num)) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
    // if (recv_len != __a1_buf_n[(int)aioctx->cb_param] * __a1_buf_size) {
    if (recv_len % __a1_buf_size != 0) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    n = recv_len / __a1_buf_size;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < __a1_buf_size; ++j) {
            if (buf_array[i].addr[j] != __a1_sample_bufs[i][j]) {
                __a1_ret = TT_FAIL;
                __a1_error_line = __LINE__;
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }
        }
    }

#ifdef __DBG_SKT_AIO_UT
// TT_DETAIL("%d", (int)(tt_ptrdiff_t)aioctx->cb_param);
#endif
    if ((int)(tt_ptrdiff_t)aioctx->cb_param == (__a1_hs_num - 1)) {
        tt_async_skt_destroy(skt, TT_FALSE);

        // can not sendto after closing
        do {
            tt_blob_t __bf = {(tt_u8_t *)&i, sizeof(i)};
            tt_result_t ret = tt_skt_sendto_async(&__a1_client,
                                                  &__bf,
                                                  1,
                                                  remote_addr,
                                                  __a2_cli_on_sendto,
                                                  NULL);
            if (ret == TT_SUCCESS) {
                __a1_ret = TT_FAIL;
                __a1_error_line = __LINE__;
                return;
            }
        } while (0);
        // can not recvfrom after closing
        do {
            tt_blob_t __bf = {(tt_u8_t *)&i, sizeof(i)};
            tt_result_t ret = tt_skt_recvfrom_async(&__a1_client,
                                                    &__bf,
                                                    1,
                                                    __a2_cli_on_recvfrom,
                                                    NULL);
            if (ret == TT_SUCCESS) {
                __a1_ret = TT_FAIL;
                __a1_error_line = __LINE__;
                return;
            }
        } while (0);
    }
}

static void __a2_cli_on_sendto(IN tt_skt_t *skt,
                               IN tt_blob_t *buf_array,
                               IN tt_u32_t buf_num,
                               IN tt_sktaddr_t *remote_addr,
                               IN tt_skt_aioctx_t *aioctx,
                               IN tt_u32_t send_len)
{
    tt_result_t ret;
    int i, j;
    tt_blob_t bufs[__a1_buf_num];

#ifdef __DBG_SKT_AIO_UT
    TT_DETAIL("%d", (int)(tt_ptrdiff_t)aioctx->cb_param);
#endif

    if (__a1_cli_cnt != (int)(tt_ptrdiff_t)aioctx->cb_param) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
    ++__a1_cli_cnt;

    if ((tt_evc_current() != &__a1_client_evc)) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if (aioctx->result != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
    if (send_len !=
        __a1_buf_n[(int)(tt_ptrdiff_t)aioctx->cb_param] * __a1_buf_size) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    j = (int)(tt_ptrdiff_t)aioctx->cb_param;
    for (i = 0; i < __a1_buf_num; ++i) {
        bufs[i].addr = __a1_cli_bufs[j][i];
        bufs[i].len = __a1_buf_size;
    }

    ret = tt_skt_recvfrom_async(skt,
                                bufs,
                                __a1_buf_num,
                                __a2_cli_on_recvfrom,
                                (void *)(tt_ptrdiff_t)(2 * j));
    /*
    // possible fail, if skt is destroyed in __a2_cli_on_recvfrom
    if (ret != TT_SUCCESS) {
        __a1_ret = TT_FAIL;__a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
    */
    (void)ret;

    ret = tt_skt_recvfrom_async(skt,
                                bufs,
                                __a1_buf_num,
                                __a2_cli_on_recvfrom,
                                (void *)(tt_ptrdiff_t)(2 * j + 1));
    /*
    // possible fail, if skt is destroyed in __a2_cli_on_recvfrom
    if (ret != TT_SUCCESS) {
        __a1_ret = TT_FAIL;__a1_error_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
    */
}

tt_result_t __a2_send_on_init(IN struct tt_evcenter_s *evc,
                              IN void *on_init_param)
{
    tt_result_t ret;
    tt_sktaddr_t _cli_addr;
    tt_skt_exit_t de;
    tt_skt_attr_t _cli_attr;
    int i, j;

    tt_sktaddr_t _remote_addr;

    tt_net_family_t af;
    int close_svr = 0;

    tt_blob_t bufs[__a1_buf_num];

    tt_skt_attr_default(&_cli_attr);
    tt_skt_attr_set_reuseaddr(&_cli_attr, TT_TRUE);
    tt_skt_attr_set_reuseport(&_cli_attr, TT_TRUE);

    de.on_destroy = __a2_on_destroy;
    de.cb_param = NULL;

    if (on_init_param != NULL) {
        af = TT_NET_AF_INET6;

        tt_sktaddr_init(&_cli_addr, TT_NET_AF_INET6);
        tt_sktaddr_set_addr_p(&_cli_addr, __TLOCAL_IP6_MAPPED);
        tt_sktaddr_set_port(&_cli_addr, 21224);

        tt_sktaddr_init(&_remote_addr, TT_NET_AF_INET6);
        tt_sktaddr_set_addr_p(&_remote_addr, __TLOCAL_IP6_MAPPED);
        tt_sktaddr_set_port(&_remote_addr, 52250);

        tt_skt_attr_set_ipv6only(&_cli_attr, TT_FALSE);
    } else {
        af = TT_NET_AF_INET;

        tt_sktaddr_init(&_cli_addr, TT_NET_AF_INET);
        tt_sktaddr_set_addr_p(&_cli_addr, __TLOCAL_IP);
        tt_sktaddr_set_port(&_cli_addr, 21224);

        tt_sktaddr_init(&_remote_addr, TT_NET_AF_INET);
        tt_sktaddr_set_addr_p(&_remote_addr, __TLOCAL_IP);
        tt_sktaddr_set_port(&_remote_addr, 52250);
    }

    // create socket
    ret = tt_udp_server_async(&__a1_client, af, &_cli_attr, &_cli_addr, &de);
    if (ret != TT_SUCCESS) {
        __a1_ret = TT_FAIL;
        __a1_error_line = __LINE__;
        return TT_FAIL;
    }


    // submit sendto
    for (i = 0; i < __a1_buf_num; ++i) {
        bufs[i].addr = __a1_sample_bufs[i];
        bufs[i].len = __a1_buf_size;
    }
    for (j = 0; j < __a1_hs_num + 1; ++j) {
        ret = tt_skt_sendto_async(&__a1_client,
                                  bufs,
                                  __a1_buf_n[j],
                                  &_remote_addr,
                                  __a2_cli_on_sendto,
                                  (void *)(tt_ptrdiff_t)j);
        if (ret != TT_SUCCESS) {
            __a1_ret = TT_FAIL;
            __a1_error_line = __LINE__;
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_udp_async_ipv4)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    int i;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    __a1_cli_cnt = 0;
    __a1_svr_cnt = 0;
    __a1_ret = TT_SUCCESS;

    for (i = 0; i <= __a1_hs_num; ++i) {
        __a1_buf_n[i] = rand() % __a1_buf_num + 1;
    }

    tt_evc_attr_default(&evc_attr);

    // server
    // ret = tt_thread_create(&__a1_server_thread, "",
    //                       __a1_server_thread_func, NULL, NULL, &ta);
    evc_attr.on_init = __a2_recv_on_init;
    ret = tt_evc_create(&__a1_server_evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // client
    evc_attr.on_init = __a2_send_on_init;
    ret = tt_evc_create(&__a1_client_evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&__a1_server_evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_evc_wait(&__a1_client_evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__a1_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_udp_async_ipv6)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    int i;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    __a1_cli_cnt = 0;
    __a1_svr_cnt = 0;
    __a1_ret = TT_SUCCESS;

    for (i = 0; i <= __a1_hs_num; ++i) {
        __a1_buf_n[i] = rand() % __a1_buf_num + 1;
    }

    tt_evc_attr_default(&evc_attr);

    // server
    // ret = tt_thread_create(&__a1_server_thread, "",
    //                       __a1_server_thread_func, NULL, NULL, &ta);
    evc_attr.on_init = __a2_recv_on_init;
    evc_attr.on_init_param = (void *)1;
    ret = tt_evc_create(&__a1_server_evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // client
    evc_attr.on_init = __a2_send_on_init;
    evc_attr.on_init_param = (void *)1;
    ret = tt_evc_create(&__a1_client_evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&__a1_server_evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_evc_wait(&__a1_client_evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__a1_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

static tt_bool_t __st1_end;
static tt_net_family_t __st1_af;

#define __st1_buf_num 10

static tt_evcenter_t __st1_client;
static tt_u8_t __st1_cli_buf[__st1_buf_num][100];
static tt_blob_t __st1_cli_tsbuf[__st1_buf_num];

static tt_evcenter_t __st1_server;
static tt_u8_t __st1_svr_buf[__st1_buf_num][100];
static tt_blob_t __st1_svr_tsbuf[__st1_buf_num];
static tt_sktaddr_t __st1_svr_addr;

static tt_skt_t __st1_cli_skt;
static tt_skt_t __st1_svr_skt;
static tt_skt_t __st1_acc_skt[10];

static tt_skt_exit_t __st1_acc_de;
static tt_skt_exit_t __st1_cli_de;

static tt_result_t __st1_ret;
static tt_u32_t __st1_error_line;

static tt_atomic_s32_t __st1_acc_num;
static tt_atomic_s32_t __st1_rcv_num;
static tt_atomic_s32_t __st1_snd_num;

#define __SF1_PORT 20202
#define __SF1_PORT6 20212

#define __ST1_RAND_CLOSE TT_FALSE

static tt_atomic_s32_t acc_ref;

void __st1_cli_on_recv(IN tt_skt_t *skt,
                       IN tt_blob_t *buf_array,
                       IN tt_u32_t buf_num,
                       IN tt_skt_aioctx_t *aioctx,
                       IN tt_u32_t recv_len)
{
    tt_result_t ret;

#ifdef __DBG_SKT_AIO_UT
    TT_INFO("__st1_cli_on_recv: %d", recv_len);
#endif

    if (aioctx->result != TT_SUCCESS)
        TT_ASSERT(recv_len == 0);
    else
        TT_ASSERT(recv_len > 0);

    if (aioctx->result != TT_SUCCESS) {
        if (aioctx->result == TT_END)
            tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_RD);
        else {
            while (1) {
                ret = tt_skt_recv_async(skt,
                                        buf_array,
                                        buf_num,
                                        __st1_cli_on_recv,
                                        NULL);
                if (ret != TT_SUCCESS) {
                    if (ret == TT_END)
                        tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_WR);
                    else {
#ifdef __SIMULATE_SKT_AIO_FAIL
                        if (__st1_end)
                            break;
                        continue;
#else
                        __st1_ret = TT_FAIL;
                        __st1_error_line = __LINE__;
                        break;
#endif
                    }
                } else
                    break;
            }
        }
    } else {
        // tt_async_skt_shutdown(skt,TT_SKT_SHUTDOWN_RDWR,
        //                aioctx->worker);

        tt_s32_t num = tt_atomic_s32_get(&__st1_rcv_num);
        while (!TT_OK(tt_atomic_s32_cas(&__st1_rcv_num, num, num + recv_len)))
            ;

        // keep receiving until io_end, then socket is destroyed
        while (1) {
            ret = tt_skt_recv_async(skt,
                                    buf_array,
                                    buf_num,
                                    __st1_cli_on_recv,
                                    NULL);
            if (ret != TT_SUCCESS) {
#ifdef __SIMULATE_SKT_AIO_FAIL
                if (__st1_end)
                    break;
                continue;
#else
                __st1_ret = TT_FAIL;
                __st1_error_line = __LINE__;
                break;
#endif
            } else
                break;
        }
    }
}

void __st1_cli_on_send(IN tt_skt_t *skt,
                       IN tt_blob_t *buf_array,
                       IN tt_u32_t buf_num,
                       IN tt_skt_aioctx_t *aioctx,
                       IN tt_u32_t send_len)
{
    tt_result_t ret;

#ifdef __DBG_SKT_AIO_UT
    TT_INFO("__st1_cli_on_send: %d", send_len);
#endif

    if (aioctx->result != TT_SUCCESS)
        TT_ASSERT(send_len == 0);
    else
        TT_ASSERT(send_len > 0);

    if (aioctx->result != TT_SUCCESS) {
        if (aioctx->result == TT_END)
            tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_RDWR);
        else {
            while (1) {
                ret = tt_skt_send_async(skt,
                                        buf_array,
                                        buf_num,
                                        __st1_cli_on_send,
                                        NULL);
                if (ret != TT_SUCCESS) {
                    if (ret == TT_END)
                        tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_RDWR);
                    else {
#ifdef __SIMULATE_SKT_AIO_FAIL
                        if (__st1_end)
                            break;
                        continue;
#else
                        __st1_ret = TT_FAIL;
                        __st1_error_line = __LINE__;
                        break;
#endif
                    }
                } else
                    break;
            }
        }
    } else {
        tt_s32_t num = tt_atomic_s32_get(&__st1_snd_num);
        while (!TT_OK(tt_atomic_s32_cas(&__st1_snd_num, num, num + send_len)))
            ;

        // all sent
        // tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_WR);

        while (1) {
            ret = tt_skt_recv_async(skt,
                                    buf_array,
                                    buf_num,
                                    __st1_cli_on_recv,
                                    NULL);
            if (ret != TT_SUCCESS) {
#ifdef __SIMULATE_SKT_AIO_FAIL
                if (__st1_end)
                    break;
                continue;
#else
                __st1_ret = TT_FAIL;
                __st1_error_line = __LINE__;
                break;
#endif
            } else
                break;
        }
    }
}

void __st1_on_connect(IN tt_skt_t *skt,
                      IN tt_sktaddr_t *remote_addr,
                      IN tt_skt_aioctx_t *aioctx)
{
    tt_result_t ret;

#ifdef __DBG_SKT_AIO_UT
    TT_INFO("__st1_on_connect");
#endif

    if (__st1_end) {
        return;
    }

    if (aioctx->result != TT_SUCCESS) {
        while (1) {
            ret = tt_skt_connect_async(&__st1_cli_skt,
                                       &__st1_svr_addr,
                                       __st1_on_connect,
                                       NULL);
            if (ret != TT_SUCCESS) {
#ifdef __SIMULATE_SKT_AIO_FAIL
                if (__st1_end)
                    break;
                continue;
#else
                __st1_ret = TT_FAIL;
                __st1_error_line = __LINE__;
                break;
#endif
            } else
                break;
        }
    } else {
        while (1) {
            ret = tt_skt_send_async(skt,
                                    __st1_cli_tsbuf,
                                    rand() % (__st1_buf_num - 1) + 1,
                                    __st1_cli_on_send,
                                    NULL);
            if (ret != TT_SUCCESS) {
                if (ret == TT_END)
                    tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_RDWR);
                else {
#ifdef __SIMULATE_SKT_AIO_FAIL
                    if (__st1_end)
                        break;
                    continue;
#else
                    __st1_ret = TT_FAIL;
                    __st1_error_line = __LINE__;
                    break;
#endif
                }
            } else {
                tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_WR);
                break;
            }
        }
    }
}

static tt_atomic_s32_t __st1_svr_ready;
static tt_result_t __st1_cli_on_init(IN tt_evcenter_t *evc,
                                     IN void *on_init_param)
{
    tt_result_t ret;
    tt_skt_attr_t attr;

    srand((tt_u32_t)time(NULL));

    tt_skt_attr_default(&attr);
    tt_skt_attr_set_reuseaddr(&attr, TT_TRUE);
    tt_skt_attr_set_reuseport(&attr, TT_TRUE);
    tt_skt_attr_set_ipv6only(&attr, TT_FALSE);

    ret = tt_async_skt_create(&__st1_cli_skt,
                              __st1_af,
                              TT_NET_PROTO_TCP,
                              TT_SKT_ROLE_TCP_CONNECT,
                              &attr,
                              &__st1_cli_de);
    if (!TT_OK(ret)) {
        __st1_ret = TT_FAIL;
        __st1_error_line = __LINE__;
        return TT_FAIL;
    }
    while (tt_atomic_s32_get(&__st1_svr_ready) == 0)
        ;

    // submit an connect until succeed
    while (1) {
        ret = tt_skt_connect_async(&__st1_cli_skt,
                                   &__st1_svr_addr,
                                   __st1_on_connect,
                                   NULL);
        if (ret != TT_SUCCESS) {
#ifdef __SIMULATE_SKT_AIO_FAIL
            if (__st1_end)
                break;
            continue;
#else
            __st1_ret = TT_FAIL;
            __st1_error_line = __LINE__;
            break;
#endif
        } else
            break;
    }

    return TT_SUCCESS;
}

void __st1_cli_on_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
    tt_result_t ret;

#ifdef __DBG_SKT_AIO_UT
    TT_INFO("__st1_cli_on_destroy, __st1_end: %d", __st1_end);
#endif

    TT_ASSERT(skt == &__st1_cli_skt);
    // TT_ASSERT(aioctx->result == TT_SUCCESS);

    if (__st1_end) {
        // current evc may not be st1_server
        tt_evc_exit(&__st1_client);
    } else {
        ret = tt_async_skt_create(&__st1_cli_skt,
                                  __st1_af,
                                  TT_NET_PROTO_TCP,
                                  TT_SKT_ROLE_TCP_CONNECT,
                                  NULL,
                                  &__st1_cli_de);
        if (!TT_OK(ret)) {
            __st1_ret = TT_FAIL;
            __st1_error_line = __LINE__;
            return;
        }

        while (1) {
            ret = tt_skt_connect_async(&__st1_cli_skt,
                                       &__st1_svr_addr,
                                       __st1_on_connect,
                                       NULL);
            if (ret != TT_SUCCESS) {
#ifdef __SIMULATE_SKT_AIO_FAIL
                if (__st1_end)
                    break;
                continue;
#else
                __st1_ret = TT_FAIL;
                __st1_error_line = __LINE__;
                break;
#endif
            } else
                break;
        }
    }
}

void __st1_svr_on_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
#ifdef __DBG_SKT_AIO_UT
    TT_INFO("__st1_svr_on_destroy");
#endif

    TT_ASSERT(skt == &__st1_svr_skt);

    // current evc may not be st1_server
    tt_evc_exit(&__st1_server);
}

static void __st1_on_accept(IN tt_skt_t *listening_skt,
                            IN tt_skt_t *new_skt,
                            IN tt_skt_aioctx_t *aioctx);

void __st1_acc_on_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
    tt_result_t ret;

#ifdef __DBG_SKT_AIO_UT
    TT_INFO("__st1_acc_on_destroy");
#endif

    tt_atomic_s32_inc(&__st1_acc_num);

    if (__st1_end) {
        tt_atomic_s32_dec(&acc_ref);
        return;
    }

    {
        // to accept new
        while (1) {
            ret = tt_skt_accept_async(&__st1_svr_skt,
                                      skt,
                                      NULL,
                                      &__st1_acc_de,
                                      __st1_on_accept,
                                      NULL);
            if (ret != TT_SUCCESS) {
                if (__st1_end) {
                    tt_atomic_s32_dec(&acc_ref);
                    return;
                } else {
#ifdef __SIMULATE_SKT_AIO_FAIL
                    if (__st1_end) {
                        tt_atomic_s32_dec(&acc_ref);
                        return;
                    }
                    continue;
#else
                    __st1_ret = TT_FAIL;
                    __st1_error_line = __LINE__;
                    break;
#endif
                }
            } else
                break;
        }
    }
}

void __st1_svr_on_recv(IN tt_skt_t *skt,
                       IN tt_blob_t *buf_array,
                       IN tt_u32_t buf_num,
                       IN tt_skt_aioctx_t *aioctx,
                       IN tt_u32_t recv_len);

void __st1_svr_on_send(IN tt_skt_t *skt,
                       IN tt_blob_t *buf_array,
                       IN tt_u32_t buf_num,
                       IN tt_skt_aioctx_t *aioctx,
                       IN tt_u32_t send_len)
{
    tt_result_t ret;

#ifdef __DBG_SKT_AIO_UT
    TT_INFO("__st1_svr_on_send: %d", send_len);
#endif

    if (aioctx->result != TT_SUCCESS)
        TT_ASSERT(send_len == 0);
    else
        TT_ASSERT(send_len > 0);

    if (aioctx->result != TT_SUCCESS) {
        if (aioctx->result == TT_END)
            tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_RDWR);
        else {
            while (1) {
                ret = tt_skt_send_async(skt,
                                        buf_array,
                                        buf_num,
                                        __st1_svr_on_send,
                                        NULL);
                if (ret != TT_SUCCESS) {
                    if (ret == TT_END)
                        tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_RDWR);
                    else {
#ifdef __SIMULATE_SKT_AIO_FAIL
                        if (__st1_end)
                            break;
                        continue;
#else
                        __st1_ret = TT_FAIL;
                        __st1_error_line = __LINE__;
                        break;
#endif
                    }
                } else
                    break;
            }
        }
    } else {
        // keep recv until io_end, then socket is destroyed
        while (1) {
            ret = tt_skt_recv_async(skt,
                                    buf_array,
                                    buf_num,
                                    __st1_svr_on_recv,
                                    NULL);
            if (ret != TT_SUCCESS) {
#ifdef __SIMULATE_SKT_AIO_FAIL
                if (__st1_end)
                    break;
                continue;
#else
                __st1_ret = TT_FAIL;
                __st1_error_line = __LINE__;
                break;
#endif
            } else
                break;
        }
    }
}

void __st1_svr_on_recv(IN tt_skt_t *skt,
                       IN tt_blob_t *buf_array,
                       IN tt_u32_t buf_num,
                       IN tt_skt_aioctx_t *aioctx,
                       IN tt_u32_t recv_len)
{
    tt_result_t ret;

#ifdef __DBG_SKT_AIO_UT
    TT_INFO("__st1_svr_on_recv: %d", recv_len);
#endif

    if (aioctx->result != TT_SUCCESS)
        TT_ASSERT(recv_len == 0);
    else
        TT_ASSERT(recv_len > 0);

    if (aioctx->result != TT_SUCCESS) {
        if (aioctx->result == TT_END)
            tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_RD);
        else {
            while (1) {
                ret = tt_skt_recv_async(skt,
                                        buf_array,
                                        buf_num,
                                        __st1_svr_on_recv,
                                        NULL);
                if (ret != TT_SUCCESS) {
                    if (ret == TT_END)
                        tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_WR);
                    else {
#ifdef __SIMULATE_SKT_AIO_FAIL
                        if (__st1_end)
                            break;
                        continue;
#else
                        __st1_ret = TT_FAIL;
                        __st1_error_line = __LINE__;
                        break;
#endif
                    }
                } else
                    break;
            }
        }
    } else {
        while (1) {
            ret = tt_skt_send_async(skt,
                                    buf_array,
                                    buf_num,
                                    __st1_svr_on_send,
                                    NULL);
            if (ret != TT_SUCCESS) {
                if (ret == TT_END)
                    tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_RDWR);
                else {
#ifdef __SIMULATE_SKT_AIO_FAIL
                    if (__st1_end)
                        break;
                    continue;
#else
                    __st1_ret = TT_FAIL;
                    __st1_error_line = __LINE__;
                    break;
#endif
                }
            } else {
                tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_WR);
                break;
            }
        }
    }
}

void __st1_on_accept(IN tt_skt_t *listening_skt,
                     IN tt_skt_t *new_skt,
                     IN tt_skt_aioctx_t *aioctx)
{
    tt_result_t ret;

#ifdef __DBG_SKT_AIO_UT
    TT_INFO("__st1_on_accept");
#endif

    if (__st1_end)
        return;

    if (aioctx->result != TT_SUCCESS) {
        while (1) {
            ret = tt_skt_accept_async(&__st1_svr_skt,
                                      new_skt,
                                      NULL,
                                      &__st1_acc_de,
                                      __st1_on_accept,
                                      NULL);
            if (ret != TT_SUCCESS) {
#ifdef __SIMULATE_SKT_AIO_FAIL
                if (__st1_end)
                    break;
                continue;
#else
                __st1_ret = TT_FAIL;
                __st1_error_line = __LINE__;
                break;
#endif
            } else
                break;
        }
    } else {
        // try read
        while (1) {
            ret = tt_skt_recv_async(new_skt,
                                    __st1_svr_tsbuf,
                                    __st1_buf_num,
                                    __st1_svr_on_recv,
                                    NULL);
            if (ret != TT_SUCCESS) {
                if (ret == TT_END)
                    tt_async_skt_shutdown(new_skt, TT_SKT_SHUTDOWN_RDWR);
                else {
#ifdef __SIMULATE_SKT_AIO_FAIL
                    if (__st1_end)
                        break;
                    continue;
#else
                    __st1_ret = TT_FAIL;
                    __st1_error_line = __LINE__;
                    break;
#endif
                }
            } else
                break;
        }
    }
}

tt_result_t __st1_svr_on_init(IN struct tt_evcenter_s *evc,
                              IN void *on_init_param)
{
    tt_result_t ret;
    int i;
    tt_skt_attr_t svr_attr;

    tt_skt_exit_t svr_de = {__st1_svr_on_destroy, NULL};

    srand((tt_u32_t)time(NULL));

    tt_skt_attr_default(&svr_attr);
    tt_skt_attr_set_ipv6only(&svr_attr, TT_FALSE);
    tt_skt_attr_set_reuseaddr(&svr_attr, TT_TRUE);

    ret = tt_tcp_server_async(&__st1_svr_skt,
                              __st1_af,
                              &svr_attr,
                              &__st1_svr_addr,
                              TT_SKT_BACKLOG_DEFAULT,
                              &svr_de);
    if (!TT_OK(ret)) {
        __st1_ret = TT_FAIL;
        __st1_error_line = __LINE__;
        return TT_FAIL;
    }
    tt_atomic_s32_set(&__st1_svr_ready, 1);

    tt_atomic_s32_set(&acc_ref, sizeof(__st1_acc_skt) / sizeof(tt_skt_t));

#if 1
    for (i = 0; i < sizeof(__st1_acc_skt) / sizeof(tt_skt_t); ++i) {
        // submit an accept until succeed
        while (1) {
            ret = tt_skt_accept_async(&__st1_svr_skt,
                                      &__st1_acc_skt[i],
                                      NULL,
                                      &__st1_acc_de,
                                      __st1_on_accept,
                                      NULL);
            if (ret != TT_SUCCESS) {
#ifdef __SIMULATE_SKT_AIO_FAIL
                if (__st1_end)
                    break;
                continue;
#else
                __st1_ret = TT_FAIL;
                __st1_error_line = __LINE__;
                break;
#endif
            } else
                break;
        }
    }
#endif

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_tcp_async_ipv4_st)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t ret;
    int i;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    srand((tt_u32_t)time(NULL));

    __st1_ret = TT_SUCCESS;
    __a1_error_line = 0;
    __st1_svr_ready = 0;

    __st1_acc_num = 0;
    __st1_rcv_num = 0;
    __st1_snd_num = 0;

    __st1_acc_de.on_destroy = __st1_acc_on_destroy;
    __st1_acc_de.cb_param = NULL;

    __st1_cli_de.on_destroy = __st1_cli_on_destroy;
    __st1_cli_de.cb_param = NULL;

    __st1_af = TT_NET_AF_INET;
    __st1_end = TT_FALSE;

    for (i = 0; i < __st1_buf_num; ++i) {
        __st1_svr_tsbuf[i].addr = __st1_svr_buf[i];
        __st1_svr_tsbuf[i].len = sizeof(__st1_svr_buf[i]);
    }
    for (i = 0; i < __st1_buf_num; ++i) {
        __st1_cli_tsbuf[i].addr = __st1_cli_buf[i];
        __st1_cli_tsbuf[i].len = sizeof(__st1_cli_buf[i]);
    }

    tt_sktaddr_init(&__st1_svr_addr, TT_NET_AF_INET);
    tt_sktaddr_set_addr_p(&__st1_svr_addr, __TLOCAL_IP);
    tt_sktaddr_set_port(&__st1_svr_addr, __SF1_PORT);

    tt_evc_attr_default(&evc_attr);

    // server first
    evc_attr.on_init = __st1_svr_on_init;
    ret = tt_evc_create(&__st1_server, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // client
    evc_attr.on_init = __st1_cli_on_init;
    ret = tt_evc_create(&__st1_client, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_sleep(10 * 1000); // 10s
    // tt_sleep(1000 * 1000); // 10s

    __st1_end = TT_TRUE;
    tt_evc_exit(&__st1_server);
    tt_evc_exit(&__st1_client);
    // wait them
    ret = tt_evc_wait(&__st1_server);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_evc_wait(&__st1_client);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_async_skt_destroy(&__st1_svr_skt, TT_TRUE);
    tt_async_skt_destroy(&__st1_cli_skt, TT_TRUE);

    TT_RECORD_INFO("acc: %d, rcv: %d, snd: %d",
                   __st1_acc_num,
                   __st1_rcv_num,
                   __st1_snd_num);

    // acc skt may still be referenced by central evc/evp
    while (tt_atomic_s32_dec(&acc_ref) != 0) {
        tt_sleep(1000);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

TT_TEST_ROUTINE_DEFINE(tt_unit_test_tcp_async_ipv6_st)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t ret;
    int i;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    srand((tt_u32_t)time(NULL));

    __st1_ret = TT_SUCCESS;
    __a1_error_line = 0;
    __st1_svr_ready = 0;

    __st1_acc_num = 0;
    __st1_rcv_num = 0;
    __st1_snd_num = 0;

    __st1_acc_de.on_destroy = __st1_acc_on_destroy;
    __st1_acc_de.cb_param = NULL;

    __st1_cli_de.on_destroy = __st1_cli_on_destroy;
    __st1_cli_de.cb_param = NULL;

    __st1_af = TT_NET_AF_INET6;
    __st1_end = TT_FALSE;

    for (i = 0; i < __st1_buf_num; ++i) {
        __st1_svr_tsbuf[i].addr = __st1_svr_buf[i];
        __st1_svr_tsbuf[i].len = sizeof(__st1_svr_buf[i]);
    }
    for (i = 0; i < __st1_buf_num; ++i) {
        __st1_cli_tsbuf[i].addr = __st1_cli_buf[i];
        __st1_cli_tsbuf[i].len = sizeof(__st1_cli_buf[i]);
    }

    tt_sktaddr_init(&__st1_svr_addr, TT_NET_AF_INET6);
    tt_sktaddr_set_addr_p(&__st1_svr_addr, __TLOCAL_IP6);
    tt_sktaddr_set_port(&__st1_svr_addr, __SF1_PORT6);
    if ((tt_strncmp(__TLOCAL_IP6, "fe80", 4) == 0) && (__TLOCAL_ITF != NULL)) {
        tt_sktaddr_set_scope_p(&__st1_svr_addr, __TLOCAL_ITF);
    }

    tt_evc_attr_default(&evc_attr);

    // server first
    evc_attr.on_init = __st1_svr_on_init;
    ret = tt_evc_create(&__st1_server, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // client
    evc_attr.on_init = __st1_cli_on_init;
    ret = tt_evc_create(&__st1_client, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_sleep(10 * 1000); // 10s

    __st1_end = TT_TRUE;
    tt_evc_exit(&__st1_server);
    tt_evc_exit(&__st1_client);
    // wait them
    ret = tt_evc_wait(&__st1_server);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_evc_wait(&__st1_client);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_async_skt_destroy(&__st1_svr_skt, TT_TRUE);
    tt_async_skt_destroy(&__st1_cli_skt, TT_TRUE);

    TT_RECORD_INFO("acc: %d, rcv: %d, snd: %d",
                   __st1_acc_num,
                   __st1_rcv_num,
                   __st1_snd_num);

    // acc skt may still be referenced by central evc/evp
    while (tt_atomic_s32_dec(&acc_ref) != 0) {
        tt_sleep(1000);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

static tt_sktaddr_t __st1_svr_addr2;
#define __SF1_PORT2 21292

#define __st2_buf_num 10
static tt_u8_t __st2_cli_buf[__st2_buf_num][100];
static tt_u8_t __st2_svr_buf[__st2_buf_num][100];

static tt_u32_t __st2_sd_cnt;
static tt_u32_t __st2_rcv_cnt;

void __st2_on_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
    tt_result_t ret = TT_FAIL;

    if (skt == &__st1_cli_skt)
        tt_evc_exit(&__st1_client);
    else if (skt == &__st1_svr_skt)
        tt_evc_exit(&__st1_server);
    else
        TT_ASSERT(0);
}

static void __st1_on_recvfrom(IN tt_skt_t *skt,
                              IN tt_blob_t *buf_array,
                              IN tt_u32_t buf_num,
                              IN tt_skt_aioctx_t *aioctx,
                              IN tt_u32_t recv_len,
                              IN tt_sktaddr_t *remote_addr);

static tt_u32_t cli_s_n;
static tt_u32_t svr_s_n;
static tt_u32_t cli_r_n;
static tt_u32_t svr_r_n;
static tt_atomic_s32_t rs_lock;

void __st1_on_sendto(IN tt_skt_t *skt,
                     IN tt_blob_t *buf_array,
                     IN tt_u32_t buf_num,
                     IN tt_sktaddr_t *remote_addr,
                     IN tt_skt_aioctx_t *aioctx,
                     IN tt_u32_t send_len)
{
    tt_result_t ret;

    if (aioctx->result != TT_SUCCESS)
        TT_ASSERT(send_len == 0);
    else
        TT_ASSERT(send_len > 0);

    if (__st1_end) {
        return;
    }

    TT_ASSERT(buf_num == 1);
#if 0
    TT_ASSERT((buf_array[0].addr == __st2_cli_buf[(int)aioctx->cb_param]) ||
              (buf_array[0].addr == __st2_svr_buf[(int)aioctx->cb_param]));
#else
    if ((buf_array[0].addr !=
         __st2_cli_buf[(int)(tt_ptrdiff_t)aioctx->cb_param]) &&
        (buf_array[0].addr !=
         __st2_svr_buf[(int)(tt_ptrdiff_t)aioctx->cb_param])) {
        int stop_to_check = 0;
        ++stop_to_check;
    }
#endif
#ifndef __SIMULATE_SKT_AIO_FAIL
    TT_ASSERT(buf_array[0].len == 100);
#endif

    while (tt_atomic_s32_cas(&rs_lock, 0, 1) == 0)
        ;
    if ((skt == &__st1_cli_skt) && (TT_OK(aioctx->result))) {
        cli_s_n += send_len;
    }
    if ((skt == &__st1_svr_skt) && (TT_OK(aioctx->result))) {
        svr_s_n += send_len;
    }
    tt_atomic_s32_cas(&rs_lock, 1, 0);

    ++__st2_sd_cnt;
    if (aioctx->result != TT_SUCCESS) {
        while (1) {
            ret = tt_skt_sendto_async(skt,
                                      buf_array,
                                      buf_num,
                                      remote_addr,
                                      __st1_on_sendto,
                                      aioctx->cb_param);
            if (ret != TT_SUCCESS) {
#ifdef __SIMULATE_SKT_AIO_FAIL
                if (__st1_end)
                    break;
                continue;
#else
                __st1_ret = TT_FAIL;
                __st1_error_line = __LINE__;
                break;
#endif
            } else
                break;
        }
    } else {
        while (1) {
            ret = tt_skt_recvfrom_async(skt,
                                        buf_array,
                                        buf_num,
                                        __st1_on_recvfrom,
                                        aioctx->cb_param);
            if (ret != TT_SUCCESS) {
#ifdef __SIMULATE_SKT_AIO_FAIL
                if (__st1_end)
                    break;
                continue;
#else
                __st1_ret = TT_FAIL;
                __st1_error_line = __LINE__;
                break;
#endif
            } else
                break;
        }
    }
}

static void __st1_on_recvfrom(IN tt_skt_t *skt,
                              IN tt_blob_t *buf_array,
                              IN tt_u32_t buf_num,
                              IN tt_skt_aioctx_t *aioctx,
                              IN tt_u32_t recv_len,
                              IN tt_sktaddr_t *remote_addr)
{
    tt_result_t ret;

    if (aioctx->result != TT_SUCCESS)
        TT_ASSERT(recv_len == 0);
    else
        TT_ASSERT(recv_len > 0);

    if (__st1_end) {
        return;
    }

    TT_ASSERT(buf_num == 1);
#if 0
    TT_ASSERT((buf_array[0].addr == __st2_cli_buf[(int)aioctx->cb_param]) ||
              (buf_array[0].addr == __st2_svr_buf[(int)aioctx->cb_param]));
#else
    if ((buf_array[0].addr !=
         __st2_cli_buf[(int)(tt_ptrdiff_t)aioctx->cb_param]) &&
        (buf_array[0].addr !=
         __st2_svr_buf[(int)(tt_ptrdiff_t)aioctx->cb_param])) {
        int stop_to_check = 0;
        ++stop_to_check;
    }
#endif
#ifndef __SIMULATE_SKT_AIO_FAIL
    TT_ASSERT(buf_array[0].len == 100);
#endif

    while (tt_atomic_s32_cas(&rs_lock, 0, 1) == 0)
        ;
    if ((skt == &__st1_cli_skt) && (TT_OK(aioctx->result))) {
        cli_r_n += recv_len;
    }
    if ((skt == &__st1_svr_skt) && (TT_OK(aioctx->result))) {
        svr_r_n += recv_len;
    }
    tt_atomic_s32_cas(&rs_lock, 1, 0);

    ++__st2_rcv_cnt;
    if (aioctx->result == TT_SUCCESS) {
        while (1) {
            ret = tt_skt_sendto_async(skt,
                                      buf_array,
                                      buf_num,
                                      remote_addr,
                                      __st1_on_sendto,
                                      aioctx->cb_param);
            if (ret != TT_SUCCESS) {
#ifdef __SIMULATE_SKT_AIO_FAIL
                if (__st1_end)
                    break;
                continue;
#else
                __st1_ret = TT_FAIL;
                __st1_error_line = __LINE__;
                break;
#endif
            } else
                break;
        }
    } else {
        while (1) {
            ret = tt_skt_recvfrom_async(skt,
                                        buf_array,
                                        buf_num,
                                        __st1_on_recvfrom,
                                        aioctx->cb_param);
            if (ret != TT_SUCCESS) {
#ifdef __SIMULATE_SKT_AIO_FAIL
                if (__st1_end)
                    break;
                continue;
#else
                __st1_ret = TT_FAIL;
                __st1_error_line = __LINE__;
                break;
#endif
            } else
                break;
        }
    }
}

static tt_result_t __st1_udp_cli_on_init(IN struct tt_evcenter_s *evc,
                                         IN void *on_init_param)
{
    tt_result_t ret;
    tt_skt_attr_t attr;
    tt_blob_t buf;
    int i;

    tt_skt_attr_default(&attr);
    tt_skt_attr_set_reuseaddr(&attr, TT_TRUE);
    tt_skt_attr_set_reuseport(&attr, TT_TRUE);
    tt_skt_attr_set_ipv6only(&attr, TT_FALSE);

    ret = tt_udp_server_async(&__st1_cli_skt,
                              __st1_af,
                              &attr,
                              &__st1_svr_addr2,
                              &__st1_cli_de);
    if (!TT_OK(ret)) {
        __st1_ret = TT_FAIL;
        __st1_error_line = __LINE__;
        return TT_FAIL;
    }

    // submit some sendto aio
    i = 0;
    while (i < __st2_buf_num) {
        buf.addr = __st2_cli_buf[i];
        buf.len = sizeof(__st2_cli_buf[i]);

        while (1) {
            ret = tt_skt_sendto_async(&__st1_cli_skt,
                                      &buf,
                                      1,
                                      &__st1_svr_addr,
                                      __st1_on_sendto,
                                      (void *)(tt_ptrdiff_t)i);
            if (ret != TT_SUCCESS) {
#ifdef __SIMULATE_SKT_AIO_FAIL
                continue;
#else
                __st1_ret = TT_FAIL;
                __st1_error_line = __LINE__;
                break;
#endif
            } else
                break;
        }

        ++i;
    }

    return TT_SUCCESS;
}

static tt_result_t __st1_udp_svr_on_init(IN struct tt_evcenter_s *evc,
                                         IN void *on_init_param)
{
    tt_result_t ret;
    int i;
    tt_blob_t buf;
    tt_skt_attr_t svr_attr;

    tt_skt_attr_default(&svr_attr);
    tt_skt_attr_set_ipv6only(&svr_attr, TT_FALSE);

    ret = tt_udp_server_async(&__st1_svr_skt,
                              __st1_af,
                              &svr_attr,
                              &__st1_svr_addr,
                              &__st1_acc_de);
    if (!TT_OK(ret)) {
        __st1_ret = TT_FAIL;
        __st1_error_line = __LINE__;
        return TT_FAIL;
    }

    i = 0;
    while (i < __st2_buf_num) {
        buf.addr = __st2_svr_buf[i];
        buf.len = sizeof(__st2_svr_buf[i]);

        while (1) {
            ret = tt_skt_recvfrom_async(&__st1_svr_skt,
                                        &buf,
                                        (tt_ptrdiff_t)1,
                                        __st1_on_recvfrom,
                                        (void *)(tt_ptrdiff_t)i);
            if (ret != TT_SUCCESS) {
#ifdef __SIMULATE_SKT_AIO_FAIL
                continue;
#else
                __st1_error_line = __LINE__;
#endif
            } else
                break;
        }
        ++i;
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_udp_async_ipv4_st)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t ret;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    srand((tt_u32_t)time(NULL));

    __st1_ret = TT_SUCCESS;
    __a1_error_line = 0;

    __st1_acc_de.on_destroy = __st2_on_destroy;
    __st1_acc_de.cb_param = NULL;

    __st1_cli_de.on_destroy = __st2_on_destroy;
    __st1_cli_de.cb_param = NULL;

    __st1_af = TT_NET_AF_INET;
    __st1_end = TT_FALSE;

    rs_lock = 0;
    cli_s_n = 0;
    svr_s_n = 0;
    cli_r_n = 0;
    svr_r_n = 0;

    tt_sktaddr_init(&__st1_svr_addr, TT_NET_AF_INET);
    tt_sktaddr_set_addr_p(&__st1_svr_addr, __TLOCAL_IP);
    tt_sktaddr_set_port(&__st1_svr_addr, __SF1_PORT2);

    tt_sktaddr_init(&__st1_svr_addr2, TT_NET_AF_INET);
    tt_sktaddr_set_addr_p(&__st1_svr_addr2, __TLOCAL_IP);
    tt_sktaddr_set_port(&__st1_svr_addr2, __SF1_PORT2 + 1);

    tt_evc_attr_default(&evc_attr);

    // server first
    evc_attr.on_init = __st1_udp_svr_on_init;
    ret = tt_evc_create(&__st1_server, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_sleep(1000);

    // client
    evc_attr.on_init = __st1_udp_cli_on_init;
    ret = tt_evc_create(&__st1_client, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_sleep(10 * 1000); // 10s

    __st1_end = TT_TRUE;
    tt_evc_exit(&__st1_server);
    tt_evc_exit(&__st1_client);
    // wait them
    ret = tt_evc_wait(&__st1_server);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_evc_wait(&__st1_client);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_async_skt_destroy(&__st1_svr_skt, TT_TRUE);
    tt_async_skt_destroy(&__st1_cli_skt, TT_TRUE);

    TT_TEST_CHECK_EQUAL(__st1_ret, TT_SUCCESS, "");
    TT_RECORD_INFO("throughput: %d bytes", cli_s_n);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_udp_async_ipv6_st)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t ret;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    srand((tt_u32_t)time(NULL));

    __st1_ret = TT_SUCCESS;
    __a1_error_line = 0;

    __st1_acc_de.on_destroy = __st2_on_destroy;
    __st1_acc_de.cb_param = NULL;

    __st1_cli_de.on_destroy = __st2_on_destroy;
    __st1_cli_de.cb_param = NULL;

    __st1_af = TT_NET_AF_INET6;
    __st1_end = TT_FALSE;

    rs_lock = 0;
    cli_s_n = 0;
    svr_s_n = 0;
    cli_r_n = 0;
    svr_r_n = 0;

    tt_sktaddr_init(&__st1_svr_addr, TT_NET_AF_INET6);
    tt_sktaddr_set_addr_p(&__st1_svr_addr, __TLOCAL_IP6);
    tt_sktaddr_set_port(&__st1_svr_addr, __SF1_PORT2 + 10);
    if ((tt_strncmp(__TLOCAL_IP6, "fe80", 4) == 0) && (__TLOCAL_ITF != NULL)) {
        tt_sktaddr_set_scope_p(&__st1_svr_addr, __TLOCAL_ITF);
    }

    tt_sktaddr_init(&__st1_svr_addr2, TT_NET_AF_INET6);
    tt_sktaddr_set_addr_p(&__st1_svr_addr2, __TLOCAL_IP6);
    tt_sktaddr_set_port(&__st1_svr_addr2, __SF1_PORT2 + 11);
    if ((tt_strncmp(__TLOCAL_IP6, "fe80", 4) == 0) && (__TLOCAL_ITF != NULL)) {
        tt_sktaddr_set_scope_p(&__st1_svr_addr2, __TLOCAL_ITF);
    }

    tt_evc_attr_default(&evc_attr);

    // server first
    evc_attr.on_init = __st1_udp_svr_on_init;
    ret = tt_evc_create(&__st1_server, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_sleep(1000);

    // client
    evc_attr.on_init = __st1_udp_cli_on_init;
    ret = tt_evc_create(&__st1_client, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_sleep(10 * 1000); // 10s

    __st1_end = TT_TRUE;
    tt_evc_exit(&__st1_server);
    tt_evc_exit(&__st1_client);
    // wait them
    ret = tt_evc_wait(&__st1_server);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_evc_wait(&__st1_client);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    tt_async_skt_destroy(&__st1_svr_skt, TT_TRUE);
    tt_async_skt_destroy(&__st1_cli_skt, TT_TRUE);

    TT_TEST_CHECK_EQUAL(__st1_ret, TT_SUCCESS, "");
    TT_RECORD_INFO("throughput: %d bytes", cli_s_n);

    // test end
    TT_TEST_CASE_LEAVE()
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

static tt_skt_t __ab_skt;
static tt_net_family_t __ab_af;
static tt_sktaddr_t __ab_addr;
#define __AB_PORT 30123

static tt_bool_t __ab_long_conn = TT_FALSE;

static tt_evpoller_t __ab_evtm;
static tt_slab_t __ab_skt_slab;

static tt_u32_t __ab_fail_line;

#define __ab_buf_num 10
static tt_u8_t __ab_buf[__ab_buf_num][1000];

static tt_char_t __ab_200[1000] =
    "HTTP/1.1 200 OK\r\n"
    "Server: TitanSDK\r\n"
    "X-Powered-By: ASP.NET\r\n"
    "Date: Fri, 03 Mar 2006 06:34:03 GMT\r\n"
    "Content-Type: text/html\r\n"
    "Accept-Ranges: bytes\r\n"
    "Last-Modified: Fri, 03 Mar 2006 06:33:18 GMT\r\n"
    "ETag: \"5ca4f75b8c3ec61:9ee\"\r\n"
    "Content-Length: 600\r\n"
    "\r\n"
    "<html><body>hello world</body></html>\r\n";

static tt_atomic_s32_t __ab_skt_num;
static tt_atomic_s32_t __ab_sys_skt_num;
static tt_atomic_s32_t __ab_peek_num;

static tt_skt_attr_t __acc_skt_attr;

static void __ab_on_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
    TT_ASSERT(skt == &__ab_skt);
}

static void __ab_acc_on_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
    tt_atomic_s32_dec(&__ab_sys_skt_num);

    memset(skt, 0xDD, sizeof(tt_skt_t));
    tt_slab_free(skt);
    tt_atomic_s32_dec(&__ab_skt_num);
}

void __ab_on_send(IN tt_skt_t *skt,
                  IN tt_blob_t *buf_array,
                  IN tt_u32_t buf_num,
                  IN tt_skt_aioctx_t *aioctx,
                  IN tt_u32_t send_len)
{
    if (aioctx->result != TT_SUCCESS) {
        if (aioctx->result == TT_END)
            tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_RD);
        else {
            __ab_fail_line = __LINE__;
            return;
        }
    }
}

void __ab_on_recv_end(IN tt_skt_t *skt,
                      IN tt_blob_t *buf_array,
                      IN tt_u32_t buf_num,
                      IN tt_skt_aioctx_t *aioctx,
                      IN tt_u32_t recv_len)
{
    if (aioctx->result != TT_SUCCESS) {
        if (aioctx->result == TT_END)
            return;
        else {
            //__ab_fail_line = __LINE__;
            // force closing
            tt_async_skt_destroy(skt, TT_FALSE);
            return;
        }
    } else {
        tt_result_t ret =
            tt_skt_recv_async(skt, buf_array, buf_num, __ab_on_recv_end, NULL);
        if (ret != TT_SUCCESS) {
            if (ret != TT_END) {
                __ab_fail_line = __LINE__;
                return;
            }
        }
    }
}

void __ab_on_recv(IN tt_skt_t *skt,
                  IN tt_blob_t *buf_array,
                  IN tt_u32_t buf_num,
                  IN tt_skt_aioctx_t *aioctx,
                  IN tt_u32_t recv_len)
{
    tt_result_t ret;

    if (aioctx->result != TT_SUCCESS) {
        if (aioctx->result == TT_END)
            tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_WR);
        else {
            __ab_fail_line = __LINE__;
            return;
        }
    } else {
        // tt_blob_t sbuf = { (tt_u8_t*)__ab_200, sizeof(__ab_200)-1 };
        tt_blob_t sbuf = {(tt_u8_t *)__ab_200, 700};
        ret = tt_skt_send_async(skt, &sbuf, 1, __ab_on_send, NULL);
        if (ret != TT_SUCCESS) {
            if (ret == TT_END)
                tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_RD);
            else {
                __ab_fail_line = __LINE__;
                return;
            }
        }

        if (__ab_long_conn) {
            ret =
                tt_skt_recv_async(skt, buf_array, buf_num, __ab_on_recv, NULL);
            if (ret != TT_SUCCESS) {
                if (ret == TT_END)
                    tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_WR);
                else {
                    __ab_fail_line = __LINE__;
                    return;
                }
            }
        } else {
            // nothing more to be sent
            ret = tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_WR);
            if (ret != TT_SUCCESS) {
                __ab_fail_line = __LINE__;
                return;
            }

            ret = tt_skt_recv_async(skt,
                                    buf_array,
                                    buf_num,
                                    __ab_on_recv_end,
                                    NULL);
            if (ret != TT_SUCCESS) {
                if (ret != TT_END) {
                    __ab_fail_line = __LINE__;
                    return;
                }
            }
        }
    }
}

static void __ab_on_accept(IN tt_skt_t *listening_skt,
                           IN tt_skt_t *new_skt,
                           IN tt_skt_aioctx_t *aioctx)
{
    tt_result_t ret;
    tt_s32_t n;
    tt_skt_t *acc_skt = NULL;
    tt_skt_exit_t acc_de = {__ab_acc_on_destroy, NULL};
    tt_blob_t buf = {__ab_buf[(int)(uintptr_t)aioctx->cb_param],
                     sizeof(__ab_buf[(int)(uintptr_t)aioctx->cb_param])};

    if (!TT_OK(aioctx->result)) {
        tt_slab_free(new_skt);
        __ab_fail_line = __LINE__;
        return;
    }
    n = tt_atomic_s32_inc(&__ab_sys_skt_num);
    if (n > tt_atomic_s32_get(&__ab_peek_num)) {
        // may be not accurate enough
        tt_atomic_s32_set(&__ab_peek_num, n);
    }

    acc_skt = (tt_skt_t *)tt_slab_alloc(&__ab_skt_slab);
    if (acc_skt == NULL) {
        __ab_fail_line = __LINE__;
        return;
    }
    tt_atomic_s32_inc(&__ab_skt_num);

    // to accept new
    ret = tt_skt_accept_async(listening_skt,
                              acc_skt,
                              NULL, //&__acc_skt_attr,
                              &acc_de,
                              __ab_on_accept,
                              aioctx->cb_param);
    if (ret != TT_SUCCESS) {
        __ab_fail_line = __LINE__;
        return;
    }

    // try read
    ret = tt_skt_recv_async(new_skt, &buf, 1, __ab_on_recv, NULL);
    if (ret != TT_SUCCESS) {
        if (ret == TT_END)
            tt_async_skt_shutdown(new_skt, TT_SKT_SHUTDOWN_WR);
        else {
            __ab_fail_line = __LINE__;
            return;
        }
    }
}

static tt_result_t __ab_start(IN struct tt_evcenter_s *evc,
                              IN void *on_init_param)
{
    tt_result_t ret;
    tt_u32_t i;
    tt_skt_attr_t attr;

    tt_skt_exit_t svr_de = {__ab_on_destroy, NULL};
    tt_skt_exit_t acc_de = {__ab_acc_on_destroy, NULL};

    tt_skt_attr_default(&attr);
    tt_skt_attr_set_reuseaddr(&attr, TT_TRUE);
    tt_skt_attr_set_reuseport(&attr, TT_TRUE);

    if (on_init_param != NULL) {
    }

    ret = tt_tcp_server_async(&__ab_skt,
                              __ab_af,
                              &attr,
                              &__ab_addr,
                              TT_SKT_BACKLOG_DEFAULT,
                              &svr_de);
    if (!TT_OK(ret)) {
        __ab_fail_line = __LINE__;
        return TT_FAIL;
    }

    for (i = 0; i < __ab_buf_num; ++i) {
        tt_skt_t *acc_skt = (tt_skt_t *)tt_slab_alloc(&__ab_skt_slab);
        if (acc_skt == NULL) {
            __ab_fail_line = __LINE__;
            return TT_FAIL;
        }
        tt_atomic_s32_inc(&__ab_skt_num);

        ret = tt_skt_accept_async(&__ab_skt,
                                  acc_skt,
                                  NULL, //&__acc_skt_attr,
                                  &acc_de,
                                  __ab_on_accept,
                                  (void *)(tt_uintptr_t)i);
        if (ret != TT_SUCCESS) {
            __ab_fail_line = __LINE__;
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ab)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t ret;
    tt_thread_t svr;
    tt_thread_attr_t ta;
    tt_evcenter_t evc;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    tt_thread_attr_default(&ta);

    __ab_af = TT_NET_AF_INET;
    //__ab_long_conn = TT_TRUE;

    tt_sktaddr_init(&__ab_addr, __ab_af);
    tt_sktaddr_set_addr_p(&__ab_addr, __TLOCAL_IP);
    tt_sktaddr_set_port(&__ab_addr, __AB_PORT);

    ret = tt_slab_create(&__ab_skt_slab, sizeof(tt_skt_t), NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");


    tt_skt_attr_default(&__acc_skt_attr);
    tt_skt_attr_set_nodelay(&__acc_skt_attr, TT_TRUE);

    // create svr thread
    tt_evc_attr_default(&evc_attr);
    evc_attr.on_init = __ab_start;

    ret = tt_evc_create(&evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    while (1) {
        tt_sleep(10 * 1000);
        TT_DETAIL("socket num: now: %d, peek: %d, sys: %d. fail position: %d",
                  tt_atomic_s32_get(&__ab_skt_num),
                  tt_atomic_s32_get(&__ab_peek_num),
                  tt_atomic_s32_get(&__ab_sys_skt_num),
                  __ab_fail_line);
    }

    // wait svr thread
    ret = tt_thread_wait(&svr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_DETAIL("server thread exited");

    // test end
    TT_TEST_CASE_LEAVE()
}

#endif

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

static tt_result_t __dimm_ret;
static tt_u32_t __dimm_err_line = 0;
static tt_u32_t __dimm_cli_cnt = 0;
static tt_u32_t __dimm_svr_cnt = 0;
static tt_skt_t __dimm_cli;
static tt_skt_t __dimm_svr;
static tt_sktaddr_t __dimm_svr_addr;

void __dimm_on_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
    tt_u32_t *n = (tt_u32_t *)cb_param;
    *n -= 1;
    TT_DETAIL("left aio: %d", *n);
}

void __dimm_on_connect(IN struct tt_skt_s *skt,
                       IN tt_sktaddr_t *remote_addr,
                       IN tt_skt_aioctx_t *aioctx)
{
    if (aioctx->result != TT_CANCELLED) {
        __st1_ret = TT_FAIL;
        __st1_error_line = __LINE__;
        return;
    }

    --__dimm_cli_cnt;
    TT_DETAIL("left client aio: %d", __dimm_cli_cnt);
}

void __dimm_on_accept(IN tt_skt_t *listening_skt,
                      IN tt_skt_t *new_skt,
                      IN tt_skt_aioctx_t *aioctx)
{
    if (aioctx->result != TT_CANCELLED) {
        __st1_ret = TT_FAIL;
        __st1_error_line = __LINE__;
        return;
    }

    --__dimm_svr_cnt;
    TT_DETAIL("left server aio: %d", __dimm_svr_cnt);
}

static tt_result_t __dimm_server_on_init(IN tt_evcenter_t *evc,
                                         IN void *on_init_param)
{
    tt_result_t ret;
    int i;
    tt_skt_attr_t svr_attr;
    tt_blob_t blob = {(tt_u8_t *)&i, sizeof(i)};

    tt_skt_exit_t svr_de = {__dimm_on_destroy, &__dimm_svr_cnt};

    tt_skt_attr_default(&svr_attr);
    tt_skt_attr_set_ipv6only(&svr_attr, TT_FALSE);
    tt_skt_attr_set_reuseaddr(&svr_attr, TT_TRUE);

    ret = tt_tcp_server_async(&__dimm_svr,
                              TT_NET_AF_INET,
                              &svr_attr,
                              &__dimm_svr_addr,
                              TT_SKT_BACKLOG_DEFAULT,
                              &svr_de);
    if (!TT_OK(ret)) {
        __dimm_ret = TT_FAIL;
        __dimm_err_line = __LINE__;
        return TT_FAIL;
    }

    // can not connect
    ret = tt_skt_connect_async(&__dimm_svr,
                               &__dimm_svr_addr,
                               __dimm_on_connect,
                               NULL);
    if (TT_OK(ret)) {
        __dimm_ret = TT_FAIL;
        __dimm_err_line = __LINE__;
        return TT_FAIL;
    }

    // can not send
    ret = tt_skt_send_async(&__dimm_svr,
                            &blob,
                            1,
                            __ab_on_send, // won't trigger
                            NULL);
    if (TT_OK(ret)) {
        __dimm_ret = TT_FAIL;
        __dimm_err_line = __LINE__;
        return TT_FAIL;
    }

    // can not recv
    ret = tt_skt_recv_async(&__dimm_svr,
                            &blob,
                            1,
                            __ab_on_recv, // won't trigger
                            NULL);
    if (TT_OK(ret)) {
        __dimm_ret = TT_FAIL;
        __dimm_err_line = __LINE__;
        return TT_FAIL;
    }

    // can not sendto
    ret = tt_skt_sendto_async(&__dimm_svr,
                              &blob,
                              1,
                              &__dimm_svr_addr,
                              __st1_on_sendto, // won't trigger
                              NULL);
    if (TT_OK(ret)) {
        __dimm_ret = TT_FAIL;
        __dimm_err_line = __LINE__;
        return TT_FAIL;
    }

    // can not recvfrom
    ret = tt_skt_recvfrom_async(&__dimm_svr,
                                &blob,
                                1,
                                __st1_on_recvfrom, // won't trigger
                                NULL);
    if (TT_OK(ret)) {
        __dimm_ret = TT_FAIL;
        __dimm_err_line = __LINE__;
        return TT_FAIL;
    }

    // can accept
    for (i = 0; i < sizeof(__st1_acc_skt) / sizeof(tt_skt_t); ++i) {
        ret = tt_skt_accept_async(&__dimm_svr,
                                  &__st1_acc_skt[i],
                                  NULL,
                                  &svr_de, // won't trigger
                                  __dimm_on_accept,
                                  NULL);
        if (ret != TT_SUCCESS) {
            __st1_ret = TT_FAIL;
            __st1_error_line = __LINE__;
            break;
        }
    }

    __dimm_svr_cnt = 1 + sizeof(__st1_acc_skt) / sizeof(tt_skt_t);

    return TT_FAIL;
}

static tt_result_t __dimm_server_on_exit(IN tt_evcenter_t *evc,
                                         IN void *on_init_param)
{
    tt_async_skt_destroy(&__dimm_svr, TT_TRUE);
    return TT_SUCCESS;
}

static tt_result_t __dimm_client_on_init(IN tt_evcenter_t *evc,
                                         IN void *on_init_param)
{
    tt_result_t ret;
    tt_skt_attr_t attr;
    tt_skt_exit_t se = {__dimm_on_destroy, &__dimm_cli_cnt};
    tt_blob_t blob = {(tt_u8_t *)&ret, sizeof(ret)};

    tt_skt_attr_default(&attr);
    tt_skt_attr_set_reuseaddr(&attr, TT_TRUE);
    tt_skt_attr_set_reuseport(&attr, TT_TRUE);
    tt_skt_attr_set_ipv6only(&attr, TT_FALSE);

    ret = tt_async_skt_create(&__dimm_cli,
                              TT_NET_AF_INET,
                              TT_NET_PROTO_TCP,
                              TT_SKT_ROLE_TCP_CONNECT,
                              &attr,
                              &se);
    if (!TT_OK(ret)) {
        __dimm_ret = TT_FAIL;
        __dimm_err_line = __LINE__;
        return TT_FAIL;
    }

    // submit an connect until succeed
    ret = tt_skt_connect_async(&__dimm_cli,
                               &__dimm_svr_addr,
                               __dimm_on_connect,
                               NULL);
    if (ret != TT_SUCCESS) {
        __dimm_ret = TT_FAIL;
        __dimm_err_line = __LINE__;
        return TT_FAIL;
    }

    // next connect would fail
    ret = tt_skt_connect_async(&__dimm_cli,
                               &__dimm_svr_addr,
                               __dimm_on_connect,
                               NULL);
    if (ret == TT_SUCCESS) {
        __dimm_ret = TT_FAIL;
        __dimm_err_line = __LINE__;
        return TT_FAIL;
    }

    // can not send
    ret = tt_skt_send_async(&__dimm_cli,
                            &blob,
                            1,
                            __ab_on_send, // won't trigger
                            NULL);
    if (TT_OK(ret)) {
        __dimm_ret = TT_FAIL;
        __dimm_err_line = __LINE__;
        return TT_FAIL;
    }

    // can not recv
    ret = tt_skt_recv_async(&__dimm_cli,
                            &blob,
                            1,
                            __ab_on_recv, // won't trigger
                            NULL);
    if (TT_OK(ret)) {
        __dimm_ret = TT_FAIL;
        __dimm_err_line = __LINE__;
        return TT_FAIL;
    }

    // can not sendto
    ret = tt_skt_sendto_async(&__dimm_cli,
                              &blob,
                              1,
                              &__dimm_svr_addr,
                              __st1_on_sendto, // won't trigger
                              NULL);
    if (TT_OK(ret)) {
        __dimm_ret = TT_FAIL;
        __dimm_err_line = __LINE__;
        return TT_FAIL;
    }

    // can not recvfrom
    ret = tt_skt_recvfrom_async(&__dimm_cli,
                                &blob,
                                1,
                                __st1_on_recvfrom, // won't trigger
                                NULL);
    if (TT_OK(ret)) {
        __dimm_ret = TT_FAIL;
        __dimm_err_line = __LINE__;
        return TT_FAIL;
    }

    // can not accept
    ret = tt_skt_accept_async(&__dimm_cli,
                              &__st1_acc_skt[0],
                              NULL,
                              &se, // won't trigger
                              __dimm_on_accept,
                              NULL);
    if (ret == TT_SUCCESS) {
        __st1_ret = TT_FAIL;
        __st1_error_line = __LINE__;
        return TT_FAIL;
    }

    __dimm_cli_cnt = 2;
    tt_async_skt_destroy(&__dimm_cli, TT_TRUE);

    // let it stop
    return TT_FAIL;
}

static tt_result_t __dimm_client_on_exit(IN tt_evcenter_t *evc,
                                         IN void *on_exit_param)
{
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_tcp_destroy_immd)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t ret;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    __dimm_ret = TT_SUCCESS;
    __dimm_cli_cnt = 0;
    __dimm_svr_cnt = 0;

    tt_sktaddr_init(&__dimm_svr_addr, TT_NET_AF_INET);
    tt_sktaddr_set_addr_p(&__dimm_svr_addr, __TLOCAL_IP);
    tt_sktaddr_set_port(&__dimm_svr_addr, 60000);

    tt_evc_attr_default(&evc_attr);

    // run server first
    evc_attr.on_init = __dimm_server_on_init;
    evc_attr.on_exit = __dimm_server_on_exit;
    ret = tt_evc_create(&__a1_server_evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // client
    evc_attr.on_init = __dimm_client_on_init;
    evc_attr.on_exit = __dimm_client_on_exit;
    ret = tt_evc_create(&__a1_client_evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__dimm_ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(__dimm_cli_cnt, 0, "");

    // exit then wait server
    tt_evc_exit(&__a1_server_evc);
    ret = tt_evc_wait(&__a1_server_evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__dimm_ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(__dimm_svr_cnt, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

void __dimm_on_recvfrom(IN struct tt_skt_s *skt,
                        IN tt_blob_t *blob,
                        IN tt_u32_t blob_num,
                        IN tt_skt_aioctx_t *aioctx,
                        IN tt_u32_t recv_len,
                        IN tt_sktaddr_t *remote_addr)
{
    if (aioctx->result != TT_CANCELLED) {
        __st1_ret = TT_FAIL;
        __st1_error_line = __LINE__;
        return;
    }

    --__dimm_cli_cnt;
    TT_INFO("left udp aio: %d", __dimm_cli_cnt);
}

void __dimm_on_sendto(IN struct tt_skt_s *skt,
                      IN tt_blob_t *blob,
                      IN tt_u32_t blob_num,
                      IN tt_sktaddr_t *remote_addr,
                      IN tt_skt_aioctx_t *aioctx,
                      IN tt_u32_t send_len)
{
    if (aioctx->result != TT_CANCELLED) {
        __st1_ret = TT_FAIL;
        __st1_error_line = __LINE__;
        return;
    }

    --__dimm_cli_cnt;
    TT_INFO("left udp aio: %d", __dimm_cli_cnt);
}

static tt_result_t __dimm_udp_on_init(IN tt_evcenter_t *evc,
                                      IN void *on_init_param)
{
    tt_result_t ret;
    tt_skt_attr_t attr;
    tt_skt_exit_t se = {__dimm_on_destroy, &__dimm_cli_cnt};
    tt_blob_t blob = {(tt_u8_t *)&ret, sizeof(ret)};
    tt_u32_t i;

    tt_skt_attr_default(&attr);

    ret = tt_async_skt_create(&__dimm_cli,
                              TT_NET_AF_INET,
                              TT_NET_PROTO_UDP,
                              TT_SKT_ROLE_UDP,
                              &attr,
                              &se);
    if (!TT_OK(ret)) {
        __dimm_ret = TT_FAIL;
        __dimm_err_line = __LINE__;
        return TT_FAIL;
    }
    __dimm_cli_cnt = 1;

    // can not connect would fail
    ret = tt_skt_connect_async(&__dimm_cli,
                               &__dimm_svr_addr,
                               __dimm_on_connect,
                               NULL);
    if (ret == TT_SUCCESS) {
        __dimm_ret = TT_FAIL;
        __dimm_err_line = __LINE__;
        return TT_FAIL;
    }

    // can not send
    ret = tt_skt_send_async(&__dimm_cli,
                            &blob,
                            1,
                            __ab_on_send, // won't trigger
                            NULL);
    if (TT_OK(ret)) {
        __dimm_ret = TT_FAIL;
        __dimm_err_line = __LINE__;
        return TT_FAIL;
    }

    // can not recv
    ret = tt_skt_recv_async(&__dimm_cli,
                            &blob,
                            1,
                            __ab_on_recv, // won't trigger
                            NULL);
    if (TT_OK(ret)) {
        __dimm_ret = TT_FAIL;
        __dimm_err_line = __LINE__;
        return TT_FAIL;
    }

    // submit some sendto
    for (i = 0; i < 8; ++i) {
        ret = tt_skt_sendto_async(&__dimm_cli,
                                  &blob,
                                  1,
                                  &__dimm_svr_addr,
                                  __dimm_on_sendto,
                                  NULL);
        if (TT_OK(!ret)) {
            __dimm_ret = TT_FAIL;
            __dimm_err_line = __LINE__;
            return TT_FAIL;
        }
    }
    __dimm_cli_cnt += 8;

    // some recvfrom
    for (i = 0; i < 10; ++i) {
        ret = tt_skt_recvfrom_async(&__dimm_cli,
                                    &blob,
                                    1,
                                    __dimm_on_recvfrom,
                                    NULL);
        if (!TT_OK(ret)) {
            __dimm_ret = TT_FAIL;
            __dimm_err_line = __LINE__;
            return TT_FAIL;
        }
    }
    __dimm_cli_cnt += 10;

    // can not accept
    ret = tt_skt_accept_async(&__dimm_cli,
                              &__st1_acc_skt[0],
                              NULL,
                              &se, // won't trigger
                              __dimm_on_accept,
                              NULL);
    if (ret == TT_SUCCESS) {
        __st1_ret = TT_FAIL;
        __st1_error_line = __LINE__;
        return TT_FAIL;
    }

    tt_async_skt_destroy(&__dimm_cli, TT_TRUE);

    // let it stop
    return TT_FAIL;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_udp_destroy_immd)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t ret;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    __dimm_ret = TT_SUCCESS;
    __dimm_cli_cnt = 0;

    tt_evc_attr_default(&evc_attr);

    // client
    evc_attr.on_init = __dimm_udp_on_init;
    ret = tt_evc_create(&__a1_client_evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&__a1_client_evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__dimm_ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(__dimm_cli_cnt, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

#endif
