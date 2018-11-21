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

#include "tt_unit_test_case_config.h"
#include <unit_test/tt_unit_test.h>

#include <algorithm/tt_string.h>
#include <algorithm/tt_string_common.h>
#include <io/tt_file_system.h>
#include <io/tt_mac_addr.h>
#include <io/tt_socket.h>
#include <io/tt_socket_addr.h>
#include <io/tt_socket_option.h>
#include <memory/tt_memory_alloc.h>
#include <memory/tt_slab.h>
#include <network/netif/tt_netif.h>
#include <network/netif/tt_netif_addr.h>
#include <network/netif/tt_netif_group.h>
#include <os/tt_fiber_event.h>
#include <os/tt_process.h>
#include <os/tt_task.h>
#include <time/tt_time_reference.h>
#include <time/tt_timer.h>

// portlayer header files
#include <tt_cstd_api.h>

#include <time.h>
#if 1
////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

//#define __DBG_SKT_AIO_UT

#define __CHECK_IO

//#define __TCP_DETAIL

#if 0
#define __SKT_DETAIL TT_DEBUG
#else
#define __SKT_DETAIL(...)
#endif

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
TT_TEST_ROUTINE_DECLARE(case_sk_addr)
TT_TEST_ROUTINE_DECLARE(case_sk_opt)
TT_TEST_ROUTINE_DECLARE(case_bind_basic)
TT_TEST_ROUTINE_DECLARE(case_tcp_basic)
TT_TEST_ROUTINE_DECLARE(case_udp_basic)
TT_TEST_ROUTINE_DECLARE(case_tcp_oob)

TT_TEST_ROUTINE_DECLARE(case_tcp4_stress)
TT_TEST_ROUTINE_DECLARE(case_tcp6_close)
TT_TEST_ROUTINE_DECLARE(case_tcp4_sendfile)
    TT_TEST_ROUTINE_DECLARE(case_tcp_block)

TT_TEST_ROUTINE_DECLARE(case_tcp_event)
TT_TEST_ROUTINE_DECLARE(case_udp_event)

TT_TEST_ROUTINE_DECLARE(case_ab)
TT_TEST_ROUTINE_DECLARE(case_ab_nc)

TT_TEST_ROUTINE_DECLARE(case_mac_addr)

// =========================================

// sometimes local ip can not be retrieved in __ut_skt_enter, as
// the physical netif name is not listed in __ut_skt_enter
#if 0
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
static tt_char_t __ut_local_ifname[64];
static tt_char_t __ut_loopback_ifname[64];

static tt_string_t __wpath;

static void __ut_skt_enter(void *enter_param)
{
    tt_netif_group_t netif_group;
    tt_netif_t *nif = NULL;

#if TT_ENV_OS_IS_IOS
#if (TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)
    tt_char_t *pwd = tt_current_path(TT_FALSE);
    tt_string_create(&__wpath, pwd, NULL);
    tt_free(pwd);

    tt_set_current_path("../tmp");
#else
    tt_char_t *s;

    s = getenv("HOME");
    if (s != NULL) {
        tt_string_init(&__wpath, NULL);
        tt_string_append(&__wpath, s);
        tt_string_append(&__wpath, "/Library/Caches");
        tt_set_current_path(tt_string_cstr(&__wpath));
        tt_string_destroy(&__wpath);
    }

    s = tt_current_path(TT_FALSE);
    tt_string_create(&__wpath, s, NULL);
    tt_free(s);
#endif

#elif TT_ENV_OS_IS_ANDROID
    tt_char_t *pwd = tt_current_path(TT_FALSE);
    tt_string_create(&__wpath, pwd, NULL);
    tt_free(pwd);

    tt_set_current_path("/data/data/com.titansdk.titansdkunittest/");
#endif

    if (__ut_skt_inited) {
        return;
    }

    tt_netif_group_create(&netif_group, TT_NIFGRP_NO_IPV6_LINK_LOCAL);

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
    tt_netif_group_add(&netif_group, "lo0");
#if 1
    tt_netif_group_add(&netif_group, "p2p0");
    tt_netif_group_add(&netif_group, "awdl0");
    tt_netif_group_add(&netif_group, "bridge0");
    tt_netif_group_add(&netif_group, "utun0");
    tt_netif_group_add(&netif_group, "utun1");
#endif
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
#elif TT_ENV_OS_IS_ANDROID
    tt_netif_group_add(&netif_group, "wlan0");
    tt_netif_group_add(&netif_group, "eth0");
    tt_netif_group_add(&netif_group, "lo");
    tt_netif_group_add(&netif_group, "lo0");
#else
#warn no netif added
#endif

    tt_netif_group_refresh_prepare(&netif_group);
    tt_netif_group_refresh(&netif_group, 0);
    tt_netif_group_refresh_done(&netif_group);
    tt_netif_group_dump(&netif_group);

    nif = NULL;
    while ((nif = tt_netif_group_next(&netif_group, nif)) != NULL) {
        if (nif->loopback && __ut_loopback_ifname[0] == 0) {
            tt_strncpy(__ut_loopback_ifname,
                       nif->name,
                       sizeof(__ut_loopback_ifname));
            break;
        }
    }

    nif = NULL;
    while ((nif = tt_netif_group_next(&netif_group, nif)) != NULL) {
        if (__ut_local_ifname[0] == 0) {
            tt_strncpy(__ut_local_ifname, nif->name, sizeof(__ut_local_ifname));
        }

        if (nif->status == TT_NETIF_STATUS_ACTIVE) {
            tt_lnode_t *node = tt_list_head(&nif->addr_list);
            while (node != NULL) {
                tt_netif_addr_t *addr =
                    TT_CONTAINER(node, tt_netif_addr_t, node);
                tt_sktaddr_t *saddr = &addr->addr;

                if (tt_sktaddr_get_family(saddr) == TT_NET_AF_INET) {
                    tt_u32_t n = sizeof("::ffff:") - 1;

                    tt_sktaddr_get_ip_p(saddr,
                                        __ut_skt_local_ip,
                                        sizeof(__ut_skt_local_ip));

                    tt_memcpy(__ut_skt_local_ip6_mapped, "::ffff:", n);
                    tt_memcpy(&__ut_skt_local_ip6_mapped[n],
                              __ut_skt_local_ip,
                              (tt_u32_t)tt_strlen(__ut_skt_local_ip));
                } else {
                    TT_ASSERT(tt_sktaddr_get_family(saddr) == TT_NET_AF_INET6);

                    tt_sktaddr_get_ip_p(saddr,
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

    tt_netif_group_destroy(&netif_group);
}

static void __ut_skt_exit(void *enter_param)
{
#if TT_ENV_OS_IS_IOS
#if (TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)
    tt_set_current_path(tt_string_cstr(&__wpath));
    tt_string_destroy(&__wpath);
#else
    tt_set_current_path(tt_string_cstr(&__wpath));
    tt_string_destroy(&__wpath);
#endif

#elif TT_ENV_OS_IS_ANDROID
    tt_set_current_path(tt_string_cstr(&__wpath));
    tt_string_destroy(&__wpath);
#endif
}

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(sk_case)

TT_TEST_CASE("case_mac_addr",
             "testing mac addr api",
             case_mac_addr,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_sk_addr",
                 "testing socket addr api",
                 case_sk_addr,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 __ut_skt_exit,
                 NULL),

    TT_TEST_CASE("case_sk_opt",
                 "testing socket option api",
                 case_sk_opt,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 __ut_skt_exit,
                 NULL),

    TT_TEST_CASE("case_bind_basic",
                 "testing socket bind api",
                 case_bind_basic,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 __ut_skt_exit,
                 NULL),

    TT_TEST_CASE("case_tcp_basic",
                 "testing socket tcp api",
                 case_tcp_basic,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 __ut_skt_exit,
                 NULL),
#if 1
    TT_TEST_CASE("case_tcp_block",
                 "testing socket tcp send block behavior",
                 case_tcp_block,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif
    TT_TEST_CASE("case_tcp_oob",
                 "testing socket tcp oob",
                 case_tcp_oob,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 __ut_skt_exit,
                 NULL),

    TT_TEST_CASE("case_udp_basic",
                 "testing socket udp api",
                 case_udp_basic,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 __ut_skt_exit,
                 NULL),

    TT_TEST_CASE("case_tcp6_close",
                 "testing socket tcp ipv6 close",
                 case_tcp6_close,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 __ut_skt_exit,
                 NULL),

    TT_TEST_CASE("case_tcp4_sendfile",
                 "testing socket tcp sendfile",
                 case_tcp4_sendfile,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 __ut_skt_exit,
                 NULL),

    TT_TEST_CASE("case_tcp4_stress",
                 "testing socket tcp ipv4 stress test",
                 case_tcp4_stress,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 __ut_skt_exit,
                 NULL),

    TT_TEST_CASE("case_tcp_event",
                 "testing socket tcp recv event",
                 case_tcp_event,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 __ut_skt_exit,
                 NULL),

    TT_TEST_CASE("case_udp_event",
                 "testing socket udp recv event",
                 case_udp_event,
                 NULL,
                 __ut_skt_enter,
                 NULL,
                 __ut_skt_exit,
                 NULL),

#if 0
    TT_TEST_CASE("case_ab",
                 "testing socket test with apache benchmark", 
                 case_ab, NULL, 
                 __ut_skt_enter, NULL,
                 __ut_skt_exit, NULL),
#endif

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
    TT_TEST_ROUTINE_DEFINE(case_mac_addr)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    static tt_s64_t __ut_skt_max_diff;
static tt_u32_t __skt_svr_tmr, __skt_cli_tmr;

TT_TEST_ROUTINE_DEFINE(case_sk_addr)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sktaddr_t sa;
    tt_sktaddr_ip_t saa;
    tt_result_t ret;

    tt_sktaddr_t sa6;
    tt_sktaddr_ip_t saa6;

    tt_char_t buf[100];
    // tt_char_t sc_buf[100];

    TT_TEST_CASE_ENTER()
    // test start

    tt_sktaddr_init(&sa, TT_NET_AF_INET);
    TT_UT_EQUAL(tt_sktaddr_get_family(&sa), TT_NET_AF_INET, "");

    ret = tt_sktaddr_ip_p2n(TT_NET_AF_INET, "1.2..4", &saa);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sktaddr_ip_p2n(TT_NET_AF_INET, "1.2.3.4", &saa);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_sktaddr_ip_n2p(TT_NET_AF_INET, &saa, buf, 7);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sktaddr_ip_n2p(TT_NET_AF_INET, &saa, buf, 8);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(buf[0], '1', "");
    TT_UT_EQUAL(buf[1], '.', "");
    TT_UT_EQUAL(buf[2], '2', "");
    TT_UT_EQUAL(buf[3], '.', "");
    TT_UT_EQUAL(buf[4], '3', "");
    TT_UT_EQUAL(buf[5], '.', "");
    TT_UT_EQUAL(buf[6], '4', "");
    TT_UT_EQUAL(buf[7], '\0', "");

    ret = tt_sktaddr_set_ip_p(&sa, "0.0.0");
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sktaddr_set_ip_p(&sa, "0.0.0.1");
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_sktaddr_get_ip_p(&sa, buf, 1);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sktaddr_get_ip_p(&sa, buf, 10);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(buf[0], '0', "");
    TT_UT_EQUAL(buf[1], '.', "");
    TT_UT_EQUAL(buf[2], '0', "");
    TT_UT_EQUAL(buf[3], '.', "");
    TT_UT_EQUAL(buf[4], '0', "");
    TT_UT_EQUAL(buf[5], '.', "");
    TT_UT_EQUAL(buf[6], '1', "");
    TT_UT_EQUAL(buf[7], '\0', "");

    tt_sktaddr_set_port(&sa, 1);
    TT_UT_EQUAL(tt_sktaddr_get_port(&sa), 1, "");

    ////////////////////// ipv6

    tt_sktaddr_init(&sa6, TT_NET_AF_INET6);
    TT_UT_EQUAL(tt_sktaddr_get_family(&sa6), TT_NET_AF_INET6, "");

    ret = tt_sktaddr_ip_p2n(TT_NET_AF_INET6, "1.2..4", &saa6);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sktaddr_ip_p2n(TT_NET_AF_INET6, "1::01", &saa6);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_sktaddr_ip_n2p(TT_NET_AF_INET6, &saa6, buf, 1);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sktaddr_ip_n2p(TT_NET_AF_INET6, &saa6, buf, 8);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(buf[0], '1', "");
    TT_UT_EQUAL(buf[1], ':', "");
    TT_UT_EQUAL(buf[2], ':', "");
    TT_UT_EQUAL(buf[3], '1', "");
    TT_UT_EQUAL(buf[4], '\0', "");

    ret = tt_sktaddr_set_ip_p(&sa6, "0.0.0");
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sktaddr_set_ip_p(&sa6, "1::01");
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_sktaddr_get_ip_p(&sa6, buf, 1);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_sktaddr_get_ip_p(&sa6, buf, 10);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(buf[0], '1', "");
    TT_UT_EQUAL(buf[1], ':', "");
    TT_UT_EQUAL(buf[2], ':', "");
    TT_UT_EQUAL(buf[3], '1', "");
    TT_UT_EQUAL(buf[4], '\0', "");

    tt_sktaddr_set_port(&sa6, 1);
    TT_UT_EQUAL(tt_sktaddr_get_port(&sa6), 1, "");

    //////////// ipv4 to ipv6

    ret = tt_sktaddr_ip_p2n(TT_NET_AF_INET, "1.2.3.4", &saa);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_sktaddr_init(&sa, TT_NET_AF_INET);
    tt_sktaddr_set_ip_n(&sa, &saa);
    tt_sktaddr_set_port(&sa, 1234);

    tt_sktaddr_map4to6(&sa, &sa6);

    TT_UT_EQUAL(tt_sktaddr_get_family(&sa6), TT_NET_AF_INET6, "");

    ret = tt_sktaddr_get_ip_p(&sa6, buf, 20);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(buf[0], ':', "");
    TT_UT_EQUAL(buf[1], ':', "");
    TT_UT_EQUAL(buf[2], 'f', "");
    TT_UT_EQUAL(buf[3], 'f', "");
    TT_UT_EQUAL(buf[4], 'f', "");
    TT_UT_EQUAL(buf[5], 'f', "");
    TT_UT_EQUAL(buf[6], ':', "");
    TT_UT_EQUAL(buf[7], '1', "");
    TT_UT_EQUAL(buf[8], '.', "");
    TT_UT_EQUAL(buf[9], '2', "");
    TT_UT_EQUAL(buf[10], '.', "");
    TT_UT_EQUAL(buf[11], '3', "");
    TT_UT_EQUAL(buf[12], '.', "");
    TT_UT_EQUAL(buf[13], '4', "");
    TT_UT_EQUAL(buf[14], '\0', "");

    TT_UT_EQUAL(tt_sktaddr_get_port(&sa6), 1234, "");

    tt_sktaddr_set_scope(&sa6, 12);
    TT_UT_EQUAL(tt_sktaddr_get_scope(&sa6), 12, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_sk_opt)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_skt_t *s;
    tt_result_t ret;
    tt_bool_t v;
    tt_u8_t v_u8;
    tt_u32_t ifidx;
    tt_u16_t v_u16;
    tt_u32_t v_u32;
    tt_char_t ifname[100];

    TT_TEST_CASE_ENTER()
    // test start

    // interface
    TT_UT_EXP(__ut_loopback_ifname[0] != 0, "");

    ret = tt_netif_name2idx(__ut_loopback_ifname, &ifidx);
    TT_UT_SUCCESS(ret, "");

    ret = tt_netif_idx2name(ifidx, ifname, sizeof(ifname));
    TT_UT_SUCCESS(ret, "");
    TT_UT_STREQ(ifname, __ut_loopback_ifname, "");
    ret = tt_netif_idx2name(ifidx, ifname, 1);
    TT_UT_EQUAL(ret, TT_E_NOSPC, "");
    ret = tt_netif_idx2name(~0, ifname, sizeof(ifname));
    TT_UT_FAIL(ret, "");

    // udp ipv4

    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_UDP, NULL);
    TT_UT_NOT_EQUAL(s, NULL, "");

    // nonblock
    ret = tt_skt_set_nonblock(s, TT_TRUE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_set_nonblock(s, TT_FALSE);
    TT_UT_SUCCESS(ret, "");

    // reuse addr
    ret = tt_skt_set_reuseaddr(s, TT_TRUE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_reuseaddr(s, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_TRUE, "");

    ret = tt_skt_set_reuseaddr(s, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_reuseaddr(s, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_FALSE, "");

    // broadcast
    ret = tt_skt_set_broadcast(s, TT_TRUE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_broadcast(s, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_TRUE, "");

    ret = tt_skt_set_broadcast(s, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_broadcast(s, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_FALSE, "");

    // ttl
    ret = tt_skt_set_ttl(s, TT_NET_AF_INET, 123);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_ttl(s, TT_NET_AF_INET, &v_u8);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v_u8, 123, "");

    // multicast loop
    ret = tt_skt_set_mcast_loop(s, TT_NET_AF_INET, TT_TRUE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_mcast_loop(s, TT_NET_AF_INET, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_TRUE, "");

    ret = tt_skt_set_mcast_loop(s, TT_NET_AF_INET, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_mcast_loop(s, TT_NET_AF_INET, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_FALSE, "");

    // multicast ttl
    ret = tt_skt_set_mcast_ttl(s, TT_NET_AF_INET, 199);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_mcast_ttl(s, TT_NET_AF_INET, &v_u8);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v_u8, 199, "");

    // multicast interface
    {
        tt_sktaddr_ip_t ip, ip2;
        tt_sktaddr_ip_p2n(TT_NET_AF_INET, "127.0.0.1", &ip);
        ret = tt_skt_set_mcast_if(s, &ip);
        TT_UT_SUCCESS(ret, "");
        ret = tt_skt_get_mcast_if(s, &ip2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_MEMEQ(&ip, &ip2, sizeof(ip.a32), "");
    }

// android can not set this opt for udp
#if !TT_ENV_OS_IS_ANDROID
    // reuse port
    ret = tt_skt_set_reuseport(s, TT_TRUE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_reuseport(s, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_TRUE, "");

#if !TT_ENV_OS_IS_WINDOWS
    ret = tt_skt_set_reuseport(s, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_reuseport(s, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_FALSE, "");
#endif
#endif

    tt_skt_destroy(s);

    // udp ipv6

    s = tt_skt_create(TT_NET_AF_INET6, TT_NET_PROTO_UDP, NULL);
    TT_UT_NOT_EQUAL(s, NULL, "");

    // ttl
    ret = tt_skt_set_ttl(s, TT_NET_AF_INET6, 145);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_ttl(s, TT_NET_AF_INET6, &v_u8);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v_u8, 145, "");

    // multicast loop
    ret = tt_skt_set_mcast_loop(s, TT_NET_AF_INET6, TT_TRUE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_mcast_loop(s, TT_NET_AF_INET6, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_TRUE, "");

    ret = tt_skt_set_mcast_loop(s, TT_NET_AF_INET6, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_mcast_loop(s, TT_NET_AF_INET6, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_FALSE, "");

    // multicast ttl
    ret = tt_skt_set_mcast_ttl(s, TT_NET_AF_INET6, 199);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_mcast_ttl(s, TT_NET_AF_INET6, &v_u8);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v_u8, 199, "");

    // multicast ttl
    {
        tt_u32_t idx, idx2;
        tt_netif_name2idx(__ut_loopback_ifname, &idx);
        ret = tt_skt_set_mcast_ifidx(s, idx);
        TT_UT_SUCCESS(ret, "");
        ret = tt_skt_get_mcast_ifidx(s, &idx2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(idx, idx2, "");
    }

    tt_skt_destroy(s);

    // tcp ipv6

    s = tt_skt_create(TT_NET_AF_INET6, TT_NET_PROTO_TCP, NULL);
    TT_UT_NOT_EQUAL(s, NULL, "");

    // keep alive
    ret = tt_skt_set_keepalive(s, TT_TRUE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_keepalive(s, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_TRUE, "");

    ret = tt_skt_set_keepalive(s, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_keepalive(s, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_FALSE, "");

    // oob
    ret = tt_skt_set_oobinline(s, TT_TRUE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_oobinline(s, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_TRUE, "");

    ret = tt_skt_set_oobinline(s, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_oobinline(s, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_FALSE, "");

    // sendbuf
    ret = tt_skt_set_sendbuf(s, 65432);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_sendbuf(s, &v_u32);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v_u32, 65432, "");

    // sendbuf
    ret = tt_skt_set_recvbuf(s, 23456);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_recvbuf(s, &v_u32);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v_u32, 23456, "");

    // sendtime
    ret = tt_skt_set_sendtime(s, 11000);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_sendtime(s, &v_u32);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v_u32, 11000, "");

    // recvtime
    ret = tt_skt_set_recvtime(s, 22000);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_recvtime(s, &v_u32);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v_u32, 22000, "");

    // linger
    ret = tt_skt_set_linger(s, TT_TRUE, 5);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_linger(s, &v, &v_u16);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_TRUE, "");
    TT_UT_EQUAL(v_u16, 5, "");

    // nonblock
    ret = tt_skt_set_nonblock(s, TT_TRUE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_set_nonblock(s, TT_FALSE);
    TT_UT_SUCCESS(ret, "");

    // reuse addr
    ret = tt_skt_set_reuseaddr(s, TT_TRUE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_reuseaddr(s, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_TRUE, "");

    ret = tt_skt_set_reuseaddr(s, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_reuseaddr(s, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_FALSE, "");

    // tcp nodelay
    ret = tt_skt_set_nodelay(s, TT_TRUE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_nodelay(s, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_TRUE, "");

    ret = tt_skt_set_nodelay(s, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_nodelay(s, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_FALSE, "");

    // ipv6 only
    ret = tt_skt_set_ipv6only(s, TT_TRUE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_ipv6only(s, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_TRUE, "");

    ret = tt_skt_set_ipv6only(s, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_ipv6only(s, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_FALSE, "");

#if !TT_ENV_OS_IS_ANDROID
    // reuse port
    ret = tt_skt_set_reuseport(s, TT_TRUE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_reuseport(s, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_TRUE, "");

#if !TT_ENV_OS_IS_WINDOWS
    ret = tt_skt_set_reuseport(s, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_get_reuseport(s, &v);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(v, TT_FALSE, "");
#endif
#endif

    // tcp linger
    ret = tt_skt_set_linger(s, TT_FALSE, ~0);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_set_linger(s, TT_TRUE, 0);
    TT_UT_SUCCESS(ret, "");
    ret = tt_skt_set_linger(s, TT_TRUE, 100);
    TT_UT_SUCCESS(ret, "");

    tt_skt_destroy(s);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_bind_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_skt_t *sk4;
    tt_skt_t *sk6;
    tt_result_t ret;
    tt_skt_attr_t attr;
    tt_sktaddr_t addr;

    TT_TEST_CASE_ENTER()
// test start

#define __TPORT 22450
#define __TPORT6 32350

    sk4 = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, &attr);
    TT_UT_NOT_EQUAL(sk4, NULL, "");

    if (TT_OK(tt_skt_local_addr(sk4, &addr))) {
        tt_sktaddr_ip_t addr_val;

        tt_sktaddr_get_ip_n(&addr, &addr_val);
        TT_UT_EQUAL(addr_val.a32.__u32, 0, "");

        TT_UT_EQUAL(tt_sktaddr_get_port(&addr), 0, "");
    }
    if (TT_OK(tt_skt_local_addr(sk4, &addr))) {
        tt_sktaddr_ip_t addr_val;

        tt_sktaddr_get_ip_n(&addr, &addr_val);
        TT_UT_EQUAL(addr_val.a32.__u32, 0, "");

        TT_UT_EQUAL(tt_sktaddr_get_port(&addr), 0, "");
    }

    // ret = tt_skt_listen(&sk4, TT_SKT_BACKLOG_DEFAULT);
    // TT_UT_EQUAL(ret, TT_FAIL, "");

    // invalid ipv4 addr
    ret = tt_skt_bind_p(sk4, TT_NET_AF_INET, "256.0.0.1", __TPORT);
    TT_UT_EQUAL(ret, TT_FAIL, "");
    ret = tt_skt_bind_p(sk4, TT_NET_AF_INET, "::127.0.0.1", __TPORT);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    ret = tt_skt_bind_p(sk4, TT_NET_AF_INET, "127.0.0.1", __TPORT);
    TT_UT_EQUAL(ret, TT_SUCCESS, "fail to bind to tcp test port");

    {
        tt_sktaddr_t sa1;
        tt_char_t buf[20];
        int i = 0;

        TT_UT_SUCCESS(tt_skt_local_addr(sk4, &sa1), "");
        TT_UT_EQUAL(tt_sktaddr_get_family(&sa1), TT_NET_AF_INET, "");

        TT_UT_EQUAL(tt_sktaddr_get_port(&sa1), __TPORT, "");

        tt_sktaddr_get_ip_p(&sa1, buf, 19);
        TT_UT_EQUAL(buf[i++], '1', "");
        TT_UT_EQUAL(buf[i++], '2', "");
        TT_UT_EQUAL(buf[i++], '7', "");
        TT_UT_EQUAL(buf[i++], '.', "");
        TT_UT_EQUAL(buf[i++], '0', "");
        TT_UT_EQUAL(buf[i++], '.', "");
        TT_UT_EQUAL(buf[i++], '0', "");
        TT_UT_EQUAL(buf[i++], '.', "");
        TT_UT_EQUAL(buf[i++], '1', "");
        TT_UT_EQUAL(buf[i++], '\0', "");

        // remote is still not valid
        TT_UT_FAIL(tt_skt_remote_addr(sk4, &sa1), "");
    }

    // can not rebind
    ret = tt_skt_bind_p(sk4, TT_NET_AF_INET, "127.0.0.1", __TPORT);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    ret = tt_skt_listen(sk4);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_skt_destroy(sk4);

    ///////////////////////////////////////////////////////////////////

    sk6 = tt_skt_create(TT_NET_AF_INET6, TT_NET_PROTO_TCP, &attr);
    TT_UT_NOT_EQUAL(sk6, NULL, "");

    ret = tt_skt_set_ipv6only(sk6, TT_FALSE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    if (TT_OK(tt_skt_local_addr(sk6, &addr))) {
        tt_sktaddr_ip_t addr_val;
        tt_sktaddr_ip_t cmp;

        tt_sktaddr_get_ip_n(&addr, &addr_val);
        memset(&cmp, 0, sizeof(cmp));
        TT_UT_EQUAL(memcmp(&addr_val, &cmp, sizeof(cmp)), 0, "");

        TT_UT_EQUAL(tt_sktaddr_get_port(&addr), 0, "");
    }
    if (TT_OK(tt_skt_local_addr(sk6, &addr))) {
        tt_sktaddr_ip_t addr_val;
        tt_sktaddr_ip_t cmp;

        tt_sktaddr_get_ip_n(&addr, &addr_val);
        memset(&cmp, 0, sizeof(cmp));
        TT_UT_EQUAL(memcmp(&addr_val, &cmp, sizeof(cmp)), 0, "");

        TT_UT_EQUAL(tt_sktaddr_get_port(&addr), 0, "");
    }

    // ret = tt_skt_listen(&sk6, TT_SKT_BACKLOG_DEFAULT);
    // TT_UT_EQUAL(ret, TT_FAIL, "");

    // invalid ipv6 address
    ret = tt_skt_bind_p(sk6, TT_NET_AF_INET6, "127.0.0.1", __TPORT6);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    if ((tt_strncmp(__TLOCAL_IP6, "fe80", 4) == 0) && (__TLOCAL_ITF != NULL)) {
        tt_sktaddr_t __a6;
        tt_sktaddr_init(&__a6, TT_NET_AF_INET6);
        tt_sktaddr_set_ip_p(&__a6, __TLOCAL_IP6);
        tt_sktaddr_set_port(&__a6, __TPORT6);
        tt_sktaddr_set_scope_p(&__a6, __TLOCAL_ITF);
        ret = tt_skt_bind(sk6, &__a6);
    } else {
        ret = tt_skt_bind_p(sk6, TT_NET_AF_INET6, __TLOCAL_IP6, __TPORT6);
    }
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    {
        tt_sktaddr_t sa1;
        tt_char_t buf[100];

        TT_UT_SUCCESS(tt_skt_local_addr(sk6, &sa1), "");
        // family has been converted
        TT_UT_EQUAL(tt_sktaddr_get_family(&sa1), TT_NET_AF_INET6, "");
        TT_UT_EQUAL(tt_sktaddr_get_port(&sa1), __TPORT6, "");

        ret = tt_sktaddr_get_ip_p(&sa1, buf, 99);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(tt_strncmp(buf, __TLOCAL_IP6, 99), 0, "");

        // remote is still not valid
        TT_UT_FAIL(tt_skt_remote_addr(sk6, &sa1), "");
    }

    // can not rebind
    ret = tt_skt_bind_p(sk6, TT_NET_AF_INET6, "::1", __TPORT);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    ret = tt_skt_listen(sk6);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_skt_destroy(sk6);

    /////// bind ipv6 addr

    sk6 = tt_skt_create(TT_NET_AF_INET6, TT_NET_PROTO_TCP, NULL);
    TT_UT_NOT_EQUAL(sk6, NULL, "");

    ret = tt_skt_bind_p(sk6, TT_NET_AF_INET6, "::1", __TPORT6);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    {
        tt_sktaddr_t sa1;
        tt_char_t buf[20];
        int i;

        TT_UT_SUCCESS(tt_skt_local_addr(sk6, &sa1), "");
        // family has been converted
        TT_UT_EQUAL(tt_sktaddr_get_family(&sa1), TT_NET_AF_INET6, "");
        TT_UT_EQUAL(tt_sktaddr_get_port(&sa1), __TPORT6, "");

        tt_sktaddr_get_ip_p(&sa1, buf, 19);

        i = 0;
        TT_UT_EQUAL(buf[i++], ':', "");
        TT_UT_EQUAL(buf[i++], ':', "");
        TT_UT_EQUAL(buf[i++], '1', "");
        TT_UT_EQUAL(buf[i++], '\0', "");

        // remote is still not valid
        TT_UT_FAIL(tt_skt_remote_addr(sk6, &sa1), "");
    }

    tt_skt_destroy(sk6);

    /////// ipv6 only

    sk6 = tt_skt_create(TT_NET_AF_INET6, TT_NET_PROTO_TCP, &attr);
    TT_UT_NOT_EQUAL(sk6, NULL, "");

    ret = tt_skt_set_ipv6only(sk6, TT_TRUE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_skt_bind_p(sk6, TT_NET_AF_INET, "127.0.0.1", __TPORT6);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    ret = tt_skt_bind_p(sk6, TT_NET_AF_INET6, "::ffff:127.0.0.1", __TPORT6);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    tt_skt_destroy(sk6);

    /////// not ipv6 only

    sk6 = tt_skt_create(TT_NET_AF_INET6, TT_NET_PROTO_TCP, &attr);
    TT_UT_NOT_EQUAL(sk6, NULL, "");

    ret = tt_skt_set_ipv6only(sk6, TT_FALSE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_skt_bind_p(sk6, TT_NET_AF_INET, "127.0.0.1", __TPORT6);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    ret = tt_skt_bind_p(sk6, TT_NET_AF_INET6, "::ffff:127.0.0.1", __TPORT6);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_skt_destroy(sk6);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u32_t __ut_skt_err_line;
static tt_atomic_s64_t __io_num;
static tt_u32_t __svr_sent, __svr_recvd, __cli_recvd, __cli_sent;

static tt_result_t __f_svr(IN void *param)
{
    tt_skt_t *s, *new_s;
    tt_u8_t buf[1 << 14] = "6789";
    tt_u32_t n;
    tt_result_t ret;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;

    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_bind_p(s, TT_NET_AF_INET, "127.0.0.1", 55556))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_listen(s))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    new_s = tt_skt_accept(s, NULL, NULL, &fev, &tmr);
    if (new_s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    while ((ret = tt_skt_recv(new_s, buf, sizeof(buf), &n, &fev, &tmr)) !=
           TT_E_END) {
        tt_u32_t total = n;
#ifdef __TCP_DETAIL
        if (n < sizeof(buf)) {
            TT_INFO("server recv %d", n);
        }
#endif
        /*
        it has to recv all data, otherwise, data are accumulated in recv
        buffer of new_s. if new_s recv buffer is full, client will be
        blocked in tt_skt_send()
        */
        while (total < sizeof(buf)) {
            if (!TT_OK(tt_skt_recv(new_s, buf, sizeof(buf), &n, &fev, &tmr))) {
                __ut_skt_err_line = __LINE__;
                return TT_FAIL;
            }
#ifdef __TCP_DETAIL
            if (n < sizeof(buf)) {
                TT_INFO("server recv %d", n);
            }
#endif
            __svr_recvd += n;

            total += n;
        }
        TT_ASSERT(total == sizeof(buf));
        tt_atomic_s64_add(&__io_num, total);

        if (!TT_OK(tt_skt_send(new_s, buf, sizeof(buf), &n))) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
#ifdef __TCP_DETAIL
        if (n < sizeof(buf)) {
            TT_INFO("server send %d", n);
        }
#endif
        __svr_sent += n;
        if (n != sizeof(buf)) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
    }

    if (!TT_OK(tt_skt_shutdown(new_s, TT_SKT_SHUT_WR))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }
#ifdef __TCP_DETAIL
    TT_INFO("server shutdown");
#endif

    if (tt_skt_recv(new_s, buf, sizeof(buf), &n, &fev, &tmr) != TT_E_END) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }
#ifdef __TCP_DETAIL
    TT_INFO("server recv end");
#endif

    tt_skt_destroy(new_s);
    tt_skt_destroy(s);

    return TT_SUCCESS;
}

#ifdef __UT_LITE__
#define TB_NUM 100
#else
#define TB_NUM 1000
#endif

static tt_result_t __f_cli(IN void *param)
{
    tt_skt_t *s;
    tt_u8_t buf[1 << 14] = "123";
    tt_u32_t n, loop;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;

    // invalid address, should fail
    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    if (TT_OK(tt_skt_connect_p(s, TT_NET_AF_INET, "127.0.0.1", 63333))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }
    tt_skt_destroy(s);

    // valid address
    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    tt_skt_set_reuseaddr(s, TT_TRUE);

    if (!TT_OK(tt_skt_connect_p(s, TT_NET_AF_INET, "127.0.0.1", 55556))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    loop = 0;
    while (loop++ < TB_NUM) {
        tt_u32_t total = 0;

        if (!TT_OK(tt_skt_send(s, buf, sizeof(buf), &n))) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
#ifdef __TCP_DETAIL
        if (n < sizeof(buf)) {
            TT_INFO("client sent %d", n);
        }
#endif
        __cli_sent += n;
        if (n != sizeof(buf)) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }

        total = 0;
        while (total < sizeof(buf)) {
            if (!TT_OK(tt_skt_recv(s, buf, sizeof(buf), &n, &fev, &tmr))) {
                __ut_skt_err_line = __LINE__;
                return TT_FAIL;
            }
#ifdef __TCP_DETAIL
            if (n < sizeof(buf)) {
                TT_INFO("client recv %d", n);
            }
#endif
            __cli_recvd += n;

            total += n;
        }
        TT_ASSERT(total == sizeof(buf));
    }

    if (!TT_OK(tt_skt_shutdown(s, TT_SKT_SHUT_WR))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }
#ifdef __TCP_DETAIL
    TT_INFO("client shutdown");
#endif

    while (tt_skt_recv(s, buf, sizeof(buf), &n, &fev, &tmr) != TT_E_END) {
    }
#ifdef __TCP_DETAIL
    TT_INFO("client recv end");
#endif

    tt_skt_destroy(s);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_tcp_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_task_t t;
    tt_s64_t start, end, dur;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_task_add_fiber(&t, NULL, __f_svr, NULL, NULL);
    tt_task_add_fiber(&t, NULL, __f_cli, NULL, NULL);

    __ut_skt_err_line = 0;
    tt_atomic_s64_set(&__io_num, 0);

    start = tt_time_ref();

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");

    tt_task_wait(&t);
    TT_UT_EQUAL(__ut_skt_err_line, 0, "");

    end = tt_time_ref();
    dur = tt_time_ref2ms(end - start);
    TT_RECORD_INFO("speed: %f MB/s",
                   (float)(tt_atomic_s64_get(&__io_num) >> 20) * 1000 / dur);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __f_svr_tcp6_close(IN void *param)
{
    tt_skt_t *s, *new_s;
    tt_u8_t buf[1 << 14] = "6789";
    tt_u32_t n;
    tt_result_t ret;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;

    s = tt_skt_create(TT_NET_AF_INET6, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    tt_skt_set_reuseaddr(s, TT_TRUE);
    if (!TT_OK(tt_skt_bind_p(s, TT_NET_AF_INET6, "::1", 55556))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_listen(s))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    new_s = tt_skt_accept(s, NULL, NULL, &fev, &tmr);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    while (((ret = tt_skt_recv(new_s, buf, sizeof(buf), &n, &fev, &tmr)) !=
            TT_E_END) &&
           TT_OK(tt_skt_send(new_s, buf, sizeof(buf), &n)) &&
           (tt_rand_u32() % 20 != 17)) {
    }

    tt_skt_destroy(new_s);
    tt_skt_destroy(s);

    return TT_SUCCESS;
}

static tt_result_t __f_cli_tcp6_close(IN void *param)
{
    tt_skt_t *s;
    tt_u8_t buf[1 << 14] = "123";
    tt_u32_t n;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;

    s = tt_skt_create(TT_NET_AF_INET6, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_connect_p(s, TT_NET_AF_INET6, "::1", 55556))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    while (TT_OK(tt_skt_send(s, buf, sizeof(buf), &n)) &&
           TT_OK(tt_skt_recv(s, buf, sizeof(buf), &n, &fev, &tmr)) &&
           (tt_rand_u32() % 20 != 19)) {
    }

    tt_skt_destroy(s);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_tcp6_close)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_task_t t;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_task_add_fiber(&t, NULL, __f_svr_tcp6_close, NULL, NULL);
    tt_task_add_fiber(&t, NULL, __f_cli_tcp6_close, NULL, NULL);

    __ut_skt_err_line = 0;

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");

    tt_task_wait(&t);
    TT_UT_EQUAL(__ut_skt_err_line, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

#define __SSF_F1 "ssf1"
#define __SSF_F2 "ssf2"

static tt_result_t __f_svr_tcp4_sendfile(IN void *param)
{
    tt_skt_t *s, *new_s;
    tt_u8_t buf[200];
    tt_u32_t n, len;
    tt_result_t ret;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;

    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        goto fail;
    }

    tt_skt_set_reuseaddr(s, TT_TRUE);
    if (!TT_OK(tt_skt_bind_p(s, TT_NET_AF_INET, "127.0.0.1", 56786))) {
        __ut_skt_err_line = __LINE__;
        goto fail;
    }

    if (!TT_OK(tt_skt_listen(s))) {
        __ut_skt_err_line = __LINE__;
        goto fail;
    }

    new_s = tt_skt_accept(s, NULL, NULL, &fev, &tmr);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        goto fail;
    }

    len = 0;
    while (((ret = tt_skt_recv(new_s, buf, sizeof(buf), &n, &fev, &tmr)) !=
            TT_E_END)) {
        len += n;
    }
    if (len != 110) {
        __ut_skt_err_line = __LINE__;
        goto fail;
    }
    for (n = 0; n < 110; n += 11) {
        if (tt_strncmp((tt_char_t *)buf + n, "0123456789", 11) != 0) {
            __ut_skt_err_line = __LINE__;
            goto fail;
        }
    }

    if (!TT_OK(tt_skt_shutdown(new_s, TT_SKT_SHUT_WR))) {
        __ut_skt_err_line = __LINE__;
        goto fail;
    }

    tt_skt_destroy(new_s);
    tt_skt_destroy(s);

    return TT_SUCCESS;

fail:
    tt_task_exit(NULL);
    return TT_FAIL;
}

static tt_result_t __f_cli_tcp4_sendfile(IN void *param)
{
    tt_skt_t *s;
    tt_u8_t buf[10];
    tt_u32_t n;
    tt_result_t ret;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;

    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        goto fail;
    }

    if (!TT_OK(tt_skt_connect_p(s, TT_NET_AF_INET, "127.0.0.1", 56786))) {
        __ut_skt_err_line = __LINE__;
        goto fail;
    }

    if (!TT_OK(tt_skt_sendfile_path(s, __SSF_F1))) {
        __ut_skt_err_line = __LINE__;
        goto fail;
    }
    if (!TT_OK(tt_skt_sendfile_path(s, __SSF_F2))) {
        __ut_skt_err_line = __LINE__;
        goto fail;
    }

    if (!TT_OK(tt_skt_shutdown(s, TT_SKT_SHUT_WR))) {
        __ut_skt_err_line = __LINE__;
        goto fail;
    }
    while (((ret = tt_skt_recv(s, buf, sizeof(buf), &n, &fev, &tmr)) !=
            TT_E_END)) {
    }

    tt_skt_destroy(s);

    return TT_SUCCESS;

fail:
    tt_task_exit(NULL);
    return TT_FAIL;
}

TT_TEST_ROUTINE_DEFINE(case_tcp4_sendfile)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_task_t t;
    tt_file_t f;
    tt_char_t buf[] = "0123456789";
    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    tt_fremove(__SSF_F1);
    tt_fremove(__SSF_F2);

    ret = tt_fcreate(__SSF_F1, NULL);
    TT_UT_SUCCESS(ret, "");
    ret = tt_fopen(&f, __SSF_F2, TT_FO_WRITE | TT_FO_CREAT, NULL);
    TT_UT_SUCCESS(ret, "");
    for (i = 0; i < 10; ++i) {
        TT_UT_SUCCESS(tt_fwrite_all(&f, (tt_u8_t *)buf, sizeof(buf)), "");
    }
    tt_fclose(&f);

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_task_add_fiber(&t, NULL, __f_svr_tcp4_sendfile, NULL, NULL);
    tt_task_add_fiber(&t, NULL, __f_cli_tcp4_sendfile, NULL, NULL);

    __ut_skt_err_line = 0;

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");

    tt_task_wait(&t);
    TT_UT_EQUAL(__ut_skt_err_line, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

#ifdef __UT_LITE__
#define UB_NUM 100
#else
#define UB_NUM 10000
#endif

static tt_result_t __f_svr_udp(IN void *param)
{
    tt_skt_t *s;
    tt_u8_t buf[1000] = "6789", buf2[1000];
    tt_u32_t n, i;
    tt_sktaddr_t addr;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;

    s = tt_skt_create(TT_NET_AF_INET6, TT_NET_PROTO_UDP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    tt_skt_set_reuseaddr(s, TT_TRUE);

    if (!TT_OK(tt_skt_bind_p(s, TT_NET_AF_INET6, "::1", 55557))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    i = 0;
    while (i++ < UB_NUM) {
        tt_u32_t len, k;
        tt_u8_t c;

        if (!TT_OK(tt_skt_recvfrom(s,
                                   buf2,
                                   sizeof(buf2),
                                   &n,
                                   &addr,
                                   &fev,
                                   &tmr))) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        if (tt_sktaddr_get_port(&addr) != 55558) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        tt_atomic_s64_add(&__io_num, n);

#if 0 //#ifdef __CHECK_IO
        c = buf2[0];
        for (k = 0; k < n; ++k) {
            if (buf2[k] != c++) {
                __ut_skt_err_line = __LINE__;
                return TT_FAIL;
            }
        }
#endif

        len = tt_rand_u32() % sizeof(buf2) + 1;
#ifdef __CHECK_IO
        c = tt_rand_u32();
        for (k = 0; k < len; ++k) {
            buf[k] = c++;
        }
#endif
        if (!TT_OK(tt_skt_sendto(s, buf, len, &n, &addr))) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        if (n != len) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
    }

    tt_skt_destroy(s);

    return TT_SUCCESS;
}

static tt_result_t __f_cli_udp(IN void *param)
{
    tt_skt_t *s;
    tt_u8_t buf[1000] = "123", buf2[1000];
    tt_u32_t n, i;
    tt_sktaddr_t addr;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;

    s = tt_skt_create(TT_NET_AF_INET6, TT_NET_PROTO_UDP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    tt_skt_set_reuseaddr(s, TT_TRUE);

    if (!TT_OK(tt_skt_bind_p(s, TT_NET_AF_INET6, "::1", 55558))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    tt_sktaddr_init(&addr, TT_NET_AF_INET6);
    tt_sktaddr_set_ip_p(&addr, "::1");
    tt_sktaddr_set_port(&addr, 55557);

    i = 0;
    while (i++ < UB_NUM) {
        tt_u32_t len, k;
        tt_u8_t c;

        len = tt_rand_u32() % sizeof(buf) + 1;
#ifdef __CHECK_IO
        c = tt_rand_u32();
        for (k = 0; k < len; ++k) {
            buf[k] = c++;
        }
#endif
        if (!TT_OK(tt_skt_sendto(s, buf, len, &n, &addr))) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        if (n != len) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }

        if (!TT_OK(tt_skt_recvfrom(s,
                                   buf2,
                                   sizeof(buf2),
                                   &n,
                                   &addr,
                                   &fev,
                                   &tmr))) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        if (tt_sktaddr_get_port(&addr) != 55557) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }

#if 0 //#ifdef __CHECK_IO
        c = buf2[0];
        for (k = 0; k < n; ++k) {
            if (buf2[k] != c++) {
                __ut_skt_err_line = __LINE__;
                return TT_FAIL;
            }
        }
#endif
    }

    tt_skt_destroy(s);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_udp_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_task_t t;
    tt_s64_t start, end, dur;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_task_add_fiber(&t, NULL, __f_svr_udp, NULL, NULL);
    tt_task_add_fiber(&t, NULL, __f_cli_udp, NULL, NULL);

    __ut_skt_err_line = 0;
    tt_atomic_s64_set(&__io_num, 0);

    start = tt_time_ref();
    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);
    end = tt_time_ref();
    TT_UT_EQUAL(__ut_skt_err_line, 0, "");

    dur = tt_time_ref2ms(end - start);
    TT_RECORD_INFO("udp: %f MB/s, cli sent/recv: %d/%d, svr sent/recv: %d/%d",
                   ((float)tt_atomic_s64_get(&__io_num) / (1 << 20)) * 1000 /
                       dur,
                   __cli_sent,
                   __cli_recvd,
                   __svr_sent,
                   __svr_recvd);

    // test end
    TT_TEST_CASE_LEAVE()
}

#ifdef __UT_LITE__
#define __CON_PER_TASK 100
#else
#define __CON_PER_TASK 1000
#endif

//#define __TC_STRESS_DETAIL
#ifdef __TC_STRESS_DETAIL
#define __S TT_INFO
#else
#define __S(...)
#endif

static tt_result_t __f_svr_acc_t4(IN void *param)
{
    tt_skt_t *new_s = (tt_skt_t *)param;
    tt_u8_t buf2[1000], c;
    tt_u32_t n, i, len;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;

    if (!TT_OK(tt_skt_recv(new_s, buf2, sizeof(buf2), &n, &fev, &tmr))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }
    __S("acc recv: %d", n);

#ifdef __CHECK_IO
    c = buf2[0];
    for (i = 0; i < n; ++i) {
        if (buf2[i] != (c++)) {
            __ut_skt_err_line = __LINE__;
            break;
        }
    }
#endif

    len = n;
    if (!TT_OK(tt_skt_send(new_s, buf2, len, &n))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }
    __S("acc send: %d", n);
    if (n != len) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_shutdown(new_s, TT_SKT_SHUT_WR))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    if (tt_skt_recv(new_s, buf2, sizeof(buf2), &n, &fev, &tmr) != TT_E_END) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    __S("acc destroy");
    tt_skt_destroy(new_s);

    tt_atomic_s64_add(&__io_num, n);

    return TT_SUCCESS;
}

static tt_result_t __f_svr_t4(IN void *param)
{
    tt_skt_t *s, *new_s;
    tt_u32_t n, i = (tt_u32_t)(tt_uintptr_t)param;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;

    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    tt_skt_set_reuseaddr(s, TT_TRUE);

    if (!TT_OK(
            tt_skt_bind_p(s, TT_NET_AF_INET, __ut_skt_local_ip, __TPORT + i))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_listen(s))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    n = 0;
    while (n++ < __CON_PER_TASK) {
        tt_fiber_t *fb;

        new_s = tt_skt_accept(s, NULL, NULL, &fev, &tmr);
        if (new_s == NULL) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        __S("new conn: %d", n);
        tt_skt_set_linger(new_s, TT_TRUE, 0);

        fb = tt_fiber_create(NULL, __f_svr_acc_t4, new_s, NULL);
        if (fb == NULL) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        tt_fiber_resume(fb, TT_FALSE);
    }

    tt_skt_destroy(s);

    return TT_SUCCESS;
}

static tt_result_t __f_cli_t4(IN void *param)
{
    tt_skt_t *s;
    tt_u8_t buf[1000], c;
    tt_u32_t n, len, num, i;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;

    num = 0;
    while (num++ < __CON_PER_TASK) {
        s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
        if (s == NULL) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }

        tt_skt_set_reuseaddr(s, TT_TRUE);
        tt_skt_set_linger(s, TT_TRUE, 0);

        if (!TT_OK(tt_skt_connect_p(s,
                                    TT_NET_AF_INET,
                                    __ut_skt_local_ip,
                                    __TPORT + (tt_u32_t)(tt_uintptr_t)param))) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        __S("connected: %d", num);

        len = tt_rand_u32() % sizeof(buf) + 1;
#ifdef __CHECK_IO
        c = tt_rand_u32();
        for (i = 0; i < len; ++i) {
            buf[i] = (c++);
        }
#endif

        if (!TT_OK(tt_skt_send(s, buf, len, &n))) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        __S("con send: %d", n);

        if (!TT_OK(tt_skt_shutdown(s, TT_SKT_SHUT_WR))) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }

        if (!TT_OK(tt_skt_recv(s, buf, sizeof(buf), &n, &fev, &tmr))) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        if (n != len) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        __S("con recv: %d", n);

#ifdef __CHECK_IO
        c = buf[0];
        for (i = 0; i < n; ++i) {
            if (buf[i] != (c++)) {
                __ut_skt_err_line = __LINE__;
                break;
            }
        }
#endif

        if (tt_skt_recv(s, buf, sizeof(buf), &n, &fev, &tmr) != TT_E_END) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }

        __S("con destroy");
        tt_skt_destroy(s);
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_tcp4_stress)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_task_t t[1];
    tt_s64_t start, end, dur;
    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    for (i = 0; i < sizeof(t) / sizeof(t[0]); ++i) {
        ret = tt_task_create(&t[i], NULL);
        TT_UT_SUCCESS(ret, "");

        tt_task_add_fiber(&t[i],
                          NULL,
                          __f_svr_t4,
                          (void *)(tt_uintptr_t)i,
                          NULL);
        tt_task_add_fiber(&t[i],
                          NULL,
                          __f_cli_t4,
                          (void *)(tt_uintptr_t)i,
                          NULL);
    }

    __ut_skt_err_line = 0;
    tt_atomic_s64_set(&__io_num, 0);

    start = tt_time_ref();
    for (i = 0; i < sizeof(t) / sizeof(t[0]); ++i) {
        ret = tt_task_run(&t[i]);
        TT_UT_SUCCESS(ret, "");
    }
    for (i = 0; i < sizeof(t) / sizeof(t[0]); ++i) {
        tt_task_wait(&t[i]);
    }
    end = tt_time_ref();
    dur = tt_time_ref2ms(end - start);
    TT_RECORD_INFO("speed: %f MB/s",
                   ((float)tt_atomic_s64_get(&__io_num) / dur) / 1000);

    TT_UT_EQUAL(__ut_skt_err_line, 0, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u32_t __ut_ev_snd, __ut_ev_rcv;

static tt_result_t __f_svr_ev(IN void *param)
{
    tt_skt_t *s, *new_s;
    tt_u8_t buf[1 << 14] = "6789";
    tt_u32_t n;
    tt_result_t ret;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr, *e_tmr;

    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_bind_p(s, TT_NET_AF_INET, "127.0.0.1", 56556))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_listen(s))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    while ((new_s = tt_skt_accept(s, NULL, NULL, &fev, &tmr)) == NULL) {
        if (fev != NULL) {
            __SKT_DETAIL("=> svr acc recv ev");
            if ((fev->src == NULL) && (fev->ev != 0x87654321)) {
                __ut_skt_err_line = __LINE__;
            }
            if ((fev->src != NULL) && (fev->ev != 0x12345678)) {
                __ut_skt_err_line = __LINE__;
            }
            ++__ut_ev_rcv;
            tt_fiber_finish(fev);
        } else {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
    }

    __SKT_DETAIL("=> svr recv");
    while ((ret = tt_skt_recv(new_s, buf, sizeof(buf), &n, &fev, &e_tmr)) !=
           TT_E_END) {
        tt_u32_t total = n;
        __SKT_DETAIL("<= svr recv %d", n);
#ifdef __TCP_DETAIL
        if (n < sizeof(buf) && n != 0) {
            TT_INFO("server recv %d", n);
        }
        if (!TT_OK(ret)) {
            TT_INFO("server recv ret %x", ret);
        }
#endif
        __svr_recvd += n;
        if (fev != NULL) {
            if ((fev->src == NULL) && (fev->ev != 0x87654321)) {
                __ut_skt_err_line = __LINE__;
            }
            if ((fev->src != NULL) && (fev->ev != 0x12345678)) {
                __ut_skt_err_line = __LINE__;
            }
            ++__ut_ev_rcv;
            tt_fiber_finish(fev);
        }

        if (e_tmr != NULL) {
            tt_s64_t now = tt_time_ref();
            now -= (tt_s64_t)(tt_uintptr_t)e_tmr->param;
            now = labs((long)now);
            now = tt_time_ref2ms(now);
            if (now > __ut_skt_max_diff) {
                __ut_skt_max_diff = now;
            }

            now = tt_rand_u32() % 3;
            if (now == 0) {
                tt_tmr_destroy(e_tmr);
            } else if (now == 1) {
                tt_tmr_stop(e_tmr);
            } else {
                tt_tmr_set_delay(e_tmr, tt_rand_u32() % 5 + 5);
                tt_tmr_set_param(e_tmr, (void *)(tt_uintptr_t)tt_time_ref());
                tt_tmr_start(e_tmr);
            }
        }

        if (tt_rand_u32() % 100 == 0) {
            tmr = tt_tmr_create(tt_rand_u32() % 5 + 5,
                                0,
                                (void *)(tt_uintptr_t)tt_time_ref());
            if (tmr == NULL) {
                __ut_skt_err_line = __LINE__;
                return TT_FAIL;
            }
            tt_tmr_start(tmr);
        }

        /*
        it has to recv all data, otherwise, data are accumulated in recv
        buffer of new_s. if new_s recv buffer is full, client will be
        blocked in tt_skt_send()
        */
        while (total < sizeof(buf)) {
            __SKT_DETAIL("=> svr recv");
            ret = tt_skt_recv(new_s, buf, sizeof(buf), &n, &fev, &e_tmr);
            __SKT_DETAIL("<= svr recv %d", n);
            if (!TT_OK(ret)) {
                if (ret == TT_E_END) {
                    break;
                } else {
                    __ut_skt_err_line = __LINE__;
                    return TT_FAIL;
                }
            }
#ifdef __TCP_DETAIL
            if (n < sizeof(buf) && n != 0) {
                TT_INFO("server recv %d", n);
            }
#endif
            __svr_recvd += n;

            if (fev != NULL) {
                if ((fev->src == NULL) && (fev->ev != 0x87654321)) {
                    __ut_skt_err_line = __LINE__;
                }
                if ((fev->src != NULL) && (fev->ev != 0x12345678)) {
                    __ut_skt_err_line = __LINE__;
                }
                ++__ut_ev_rcv;
                tt_fiber_finish(fev);
            }

            if (e_tmr != NULL) {
                tt_s64_t now = tt_time_ref();
                now -= (tt_s64_t)(tt_uintptr_t)e_tmr->param;
                now = labs((long)now);
                now = tt_time_ref2ms(now);
                if (now > __ut_skt_max_diff) {
                    __ut_skt_max_diff = now;
                }

                now = tt_rand_u32() % 3;
                if (now == 0) {
                    tt_tmr_destroy(e_tmr);
                } else if (now == 1) {
                    tt_tmr_stop(e_tmr);
                } else {
                    tt_tmr_set_delay(e_tmr, tt_rand_u32() % 5 + 5);
                    tt_tmr_set_param(e_tmr,
                                     (void *)(tt_uintptr_t)tt_time_ref());
                    tt_tmr_start(e_tmr);
                }
            }

            if (tt_rand_u32() % 100 == 0) {
                tmr = tt_tmr_create(tt_rand_u32() % 5 + 5,
                                    0,
                                    (void *)(tt_uintptr_t)tt_time_ref());
                if (tmr == NULL) {
                    __ut_skt_err_line = __LINE__;
                    return TT_FAIL;
                }
                tt_tmr_start(tmr);
            }

            total += n;
        }
        TT_ASSERT(total == 0 || total == sizeof(buf));
        tt_atomic_s64_add(&__io_num, total);

        __SKT_DETAIL("=> svr send");
        if (!TT_OK(tt_skt_send(new_s, buf, sizeof(buf), &n))) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        __SKT_DETAIL("<= svr send %d", n);
#ifdef __TCP_DETAIL
        if (n < sizeof(buf)) {
            TT_INFO("server send %d", n);
        }
#endif
        __svr_sent += n;
        if (n != sizeof(buf)) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }

        __SKT_DETAIL("=> svr recv");
    }

    if (!TT_OK(tt_skt_shutdown(new_s, TT_SKT_SHUT_WR))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }
#ifdef __TCP_DETAIL
    TT_INFO("server shutdown");
#endif

    while (
        TT_OK(ret = tt_skt_recv(new_s, buf, sizeof(buf), &n, &fev, &e_tmr))) {
    }
    if (ret != TT_E_END) {
        __ut_skt_err_line = __LINE__;
    }
#ifdef __TCP_DETAIL
    TT_INFO("server recv end");
#endif

    tt_skt_destroy(new_s);
    tt_skt_destroy(s);

    return TT_SUCCESS;
}

#ifdef __UT_LITE__
#define TE_NUM 100
#else
#define TE_NUM 1000
#endif

static tt_result_t __f_cli_ev(IN void *param)
{
    tt_skt_t *s;
    tt_u8_t buf[1 << 14] = "123";
    tt_u32_t n, loop;
    tt_fiber_t *svr = tt_fiber_find("svr");
    tt_tmr_t *tmr, *e_tmr;
    tt_fiber_ev_t *fev;

    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    tt_skt_set_reuseaddr(s, TT_TRUE);

    n = 0;
    while (n++ < 20) {
        if (tt_rand_u32() % 5 == 0) {
            tt_u32_t r = tt_rand_u32() % 2;
            if (r == 0) {
                tt_fiber_ev_t e;
                tt_fiber_ev_init(&e, 0x12345678);
                __SKT_DETAIL("=> cli send ev wait");
                tt_fiber_send_ev(svr, &e, TT_TRUE);
                __SKT_DETAIL("<= cli send ev wait");
            } else {
                tt_fiber_ev_t *e = tt_fiber_ev_create(0x87654321, 0);
                __SKT_DETAIL("=> cli send ev");
                tt_fiber_send_ev(svr, e, TT_FALSE);
                __SKT_DETAIL("<= cli send ev");
            }
            ++__ut_ev_snd;
        }
    }

    if (!TT_OK(tt_skt_connect_p(s, TT_NET_AF_INET, "127.0.0.1", 56556))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    loop = 0;
    while (loop++ < TE_NUM) {
        tt_u32_t total = 0;

        __SKT_DETAIL("=> cli send");
        if (!TT_OK(tt_skt_send(s, buf, sizeof(buf), &n))) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        __SKT_DETAIL("<= cli send %d", n);
#ifdef __TCP_DETAIL
        if (n < sizeof(buf)) {
            TT_INFO("client sent %d", n);
        }
#endif
        __cli_sent += n;
        if (n != sizeof(buf)) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }

        if (tt_rand_u32() % 5 == 0) {
            tt_u32_t r = tt_rand_u32() % 2;
            if (r == 0) {
                tt_fiber_ev_t e;
                tt_fiber_ev_init(&e, 0x12345678);
                __SKT_DETAIL("=> cli send ev wait");
                tt_fiber_send_ev(svr, &e, TT_TRUE);
                __SKT_DETAIL("<= cli send ev wait");
            } else {
                tt_fiber_ev_t *e = tt_fiber_ev_create(0x87654321, 0);
                __SKT_DETAIL("=> cli send ev");
                tt_fiber_send_ev(svr, e, TT_FALSE);
                __SKT_DETAIL("<= cli send ev");
            }
            ++__ut_ev_snd;
        }

        total = 0;
        while (total < sizeof(buf)) {
            __SKT_DETAIL("=> cli recv");
            if (!TT_OK(tt_skt_recv(s, buf, sizeof(buf), &n, &fev, &e_tmr))) {
                __ut_skt_err_line = __LINE__;
                return TT_FAIL;
            }
            __SKT_DETAIL("<= cli recv %d", n);
#ifdef __TCP_DETAIL
            if (n < sizeof(buf) && n != 0) {
                TT_INFO("client recv %d", n);
            }
#endif
            __cli_recvd += n;

            if (e_tmr != NULL) {
                tt_s64_t now = tt_time_ref();
                now -= (tt_s64_t)(tt_uintptr_t)e_tmr->param;
                now = labs((long)now);
                now = tt_time_ref2ms(now);
                if (now > __ut_skt_max_diff) {
                    __ut_skt_max_diff = now;
                }

                now = tt_rand_u32() % 3;
                if (now == 0) {
                    tt_tmr_destroy(e_tmr);
                } else if (now == 1) {
                    tt_tmr_stop(e_tmr);
                } else {
                    tt_tmr_set_delay(e_tmr, tt_rand_u32() % 5 + 5);
                    tt_tmr_set_param(e_tmr,
                                     (void *)(tt_uintptr_t)tt_time_ref());
                    tt_tmr_start(e_tmr);
                }
            }

            if (tt_rand_u32() % 100 == 0) {
                tmr = tt_tmr_create(tt_rand_u32() % 5 + 5,
                                    0,
                                    (void *)(tt_uintptr_t)tt_time_ref());
                if (tmr == NULL) {
                    __ut_skt_err_line = __LINE__;
                    return TT_FAIL;
                }
                tt_tmr_start(tmr);
            }

            total += n;
        }
        TT_ASSERT(total == sizeof(buf));
    }

    if (!TT_OK(tt_skt_shutdown(s, TT_SKT_SHUT_WR))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }
#ifdef __TCP_DETAIL
    TT_INFO("client shutdown");
#endif

    while (TT_OK(tt_skt_recv(s, buf, sizeof(buf), &n, &fev, &e_tmr))) {
    }
#ifdef __TCP_DETAIL
    TT_INFO("client recv end");
#endif

    tt_skt_destroy(s);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_tcp_event)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_task_t t;
    tt_s64_t start, end, dur;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_task_add_fiber(&t, "svr", __f_svr_ev, NULL, NULL);
    tt_task_add_fiber(&t, "cli", __f_cli_ev, NULL, NULL);

    __ut_skt_err_line = 0;
    __ut_ev_rcv = 0;
    __ut_ev_snd = 0;
    __svr_sent = 0;
    __svr_recvd = 0;
    __cli_sent = 0;
    __cli_recvd = 0;
    tt_atomic_s64_set(&__io_num, 0);
    __ut_skt_max_diff = 0;

    start = tt_time_ref();

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");

    tt_task_wait(&t);
    if (__ut_skt_err_line != 0) {
        TT_RECORD_INFO("error line: %d", __ut_skt_err_line);
    }
    TT_UT_EQUAL(__ut_skt_err_line, 0, "");
    TT_UT_EQUAL(__ut_ev_rcv, __ut_ev_snd, "");

    end = tt_time_ref();
    dur = tt_time_ref2ms(end - start);
    TT_RECORD_INFO(
        "udp: %f MB/s, cli sent/recv: %d/%d, svr sent/recv: %d/%d, time diff: "
        "%d",
        ((float)tt_atomic_s64_get(&__io_num) / (1 << 20)) * 1000 / dur,
        __cli_sent,
        __cli_recvd,
        __svr_sent,
        __svr_recvd,
        (tt_s32_t)__ut_skt_max_diff);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_u32_t __svr_r_num, __svr_s_num;
static tt_u32_t __cli_r_num, __cli_s_num;

#ifdef __UT_LITE__
#define UE_NUM 100
#else
#define UE_NUM 10000
#endif

// when use buf size of 100 or 1000, we see udp packet lost
// this may be caused by udp packet is not received out of
// skt recv buffer as caller is processing fiber event, using
// 10bytes we can see almost no udp packet loss
static tt_result_t __f_svr_udp_ev(IN void *param)
{
    tt_skt_t *s;
    tt_u8_t buf[10] = "6789", buf2[10];
    tt_u32_t n, i;
    tt_sktaddr_t addr;
    tt_fiber_t *cli = tt_fiber_find("cli");
    tt_tmr_t *tmr, *e_tmr;
    tt_fiber_ev_t *fev;

    s = tt_skt_create(TT_NET_AF_INET6, TT_NET_PROTO_UDP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    tt_skt_set_reuseaddr(s, TT_TRUE);

    if (!TT_OK(tt_skt_bind_p(s, TT_NET_AF_INET6, "::1", 56557))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    tmr = tt_tmr_create(5, ~0, (void *)(tt_uintptr_t)tt_time_ref());
    if (tmr == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }
    tt_tmr_start(tmr);

    i = 0;
    while (i++ < UE_NUM) {
        tt_u32_t len, k;
        tt_u8_t c;

        if (!TT_OK(tt_skt_recvfrom(s,
                                   buf2,
                                   sizeof(buf2),
                                   &n,
                                   &addr,
                                   &fev,
                                   &e_tmr))) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        // on some plat: if recvfrom is interrupted by fiber ev and return
        // all data in addr are cleared...
        if ((n != 0) && (tt_sktaddr_get_port(&addr) != 56558)) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        tt_atomic_s64_add(&__io_num, n);
        __svr_recvd += n;
        // TT_INFO("svr recv %d, total: %d", n, __svr_recvd);
        if (n != 0) {
            __svr_r_num++;
        }

        if (e_tmr != NULL) {
            tt_s64_t now = tt_time_ref();
            now -= (tt_s64_t)(tt_uintptr_t)e_tmr->param;
            now = labs((long)now);
            now = tt_time_ref2ms(now);
            if (now > __ut_skt_max_diff) {
                __ut_skt_max_diff = now;
            }

            if (e_tmr->ev == ~0) {
                tt_tmr_set_param(e_tmr, (void *)(tt_uintptr_t)tt_time_ref());
                tt_tmr_start(e_tmr);
            } else {
                now = tt_rand_u32() % 3;
                if (now == 0) {
                    --__skt_svr_tmr;
                    tt_tmr_destroy(e_tmr);
                } else if (now == 1) {
                    --__skt_svr_tmr;
                    tt_tmr_stop(e_tmr);
                } else {
                    tt_tmr_set_delay(e_tmr, tt_rand_u32() % 5 + 5);
                    tt_tmr_set_param(e_tmr,
                                     (void *)(tt_uintptr_t)tt_time_ref());
                    tt_tmr_start(e_tmr);
                }
            }
        }

        if (tt_rand_u32() % 100 == 0) {
            tmr = tt_tmr_create(tt_rand_u32() % 5 + 5,
                                i,
                                (void *)(tt_uintptr_t)tt_time_ref());
            if (tmr == NULL) {
                __ut_skt_err_line = __LINE__;
                return TT_FAIL;
            }
            tt_tmr_start(tmr);
            ++__skt_svr_tmr;
        }

#if 1
        if (tt_rand_u32() % 5 == 0) {
            tt_u32_t r = tt_rand_u32() % 2;
            if (r == 0) {
                tt_fiber_ev_t e;
                tt_fiber_ev_init(&e, 0x12345678);
                tt_fiber_send_ev(cli, &e, TT_TRUE);
            } else {
                tt_fiber_ev_t *e = tt_fiber_ev_create(0x87654321, 0);
                tt_fiber_send_ev(cli, e, TT_FALSE);
            }
            ++__ut_ev_snd;
        }
#endif

#if 0 //#ifdef __CHECK_IO
        c = buf2[0];
        for (k = 0; k < n; ++k) {
            if (buf2[k] != c++) {
                __ut_skt_err_line = __LINE__;
                return TT_FAIL;
            }
        }
#endif

        len = tt_rand_u32() % sizeof(buf2) + 1;
#ifdef __CHECK_IO
        c = tt_rand_u32();
        for (k = 0; k < len; ++k) {
            buf[k] = c++;
        }
#endif
        if (!TT_OK(tt_skt_sendto(s, buf, len, &n, &addr))) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        if (n != len) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        __svr_sent += n;
        // TT_INFO("svr send %d/%d", n, len);
    }
    TT_INFO("server done");

#if 0 // enable it to check all data sent from cli and recvd by svr
    while (TT_OK(tt_skt_recvfrom(s, buf2, sizeof(buf2), &n, &addr, NULL))) {
        __svr_recvd += n;
        //TT_INFO("svr recv %d, total: %d", n, __svr_recvd);
        if (n != 0) {
            __svr_r_num++;
        }
    }
#endif

    {
        tt_fiber_ev_t *e = tt_fiber_ev_create(0x11112222, 0);
        tt_fiber_send_ev(cli, e, TT_FALSE);
        __ut_ev_snd += 1;
    }

    tt_skt_destroy(s);

    return TT_SUCCESS;
}

static tt_result_t __f_cli_udp_ev(IN void *param)
{
    tt_skt_t *s;
    tt_u8_t buf[10] = "123", buf2[10];
    tt_u32_t n, i;
    tt_sktaddr_t addr, addr2;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr, *e_tmr;

    s = tt_skt_create(TT_NET_AF_INET6, TT_NET_PROTO_UDP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    tt_skt_set_reuseaddr(s, TT_TRUE);

    if (!TT_OK(tt_skt_bind_p(s, TT_NET_AF_INET6, "::1", 56558))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    tt_sktaddr_init(&addr, TT_NET_AF_INET6);
    tt_sktaddr_set_ip_p(&addr, "::1");
    tt_sktaddr_set_port(&addr, 56557);

    // this timer is used to make recvfrom return, as udp packet may be lost
    tmr = tt_tmr_create(5, ~0, (void *)(tt_uintptr_t)tt_time_ref());
    if (tmr == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }
    tt_tmr_start(tmr);

    i = 0;
    while (1 || i++ < UE_NUM) {
        tt_u32_t len, k;
        tt_u8_t c;

        len = tt_rand_u32() % sizeof(buf) + 1;
#ifdef __CHECK_IO
        c = tt_rand_u32();
        for (k = 0; k < len; ++k) {
            buf[k] = c++;
        }
#endif
        if (!TT_OK(tt_skt_sendto(s, buf, len, &n, &addr))) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        if (n != len) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        __cli_sent += n;
        ++__cli_s_num;
        /*TT_INFO("                            cli send %d/%d, total: %d",
                n, len, __cli_sent);*/

        if (tt_rand_u32() % 100 == 0) {
            tmr = tt_tmr_create(tt_rand_u32() % 5 + 5,
                                i,
                                (void *)(tt_uintptr_t)tt_time_ref());
            if (tmr == NULL) {
                __ut_skt_err_line = __LINE__;
                return TT_FAIL;
            }
            tt_tmr_start(tmr);
            ++__skt_cli_tmr;
        }

        if (!TT_OK(tt_skt_recvfrom(s,
                                   buf2,
                                   sizeof(buf2),
                                   &n,
                                   &addr2,
                                   &fev,
                                   &e_tmr))) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        // on some plat: if recvfrom is interrupted by fiber ev and return
        // all data in addr are cleared...
        if ((n != 0) && (tt_sktaddr_get_port(&addr2) != 56557)) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        __cli_recvd += n;
        // TT_INFO("                  cli recv %d", n);

        if (fev != NULL) {
            tt_bool_t done = TT_FALSE;
            if (fev->ev == 0x11112222) {
                done = TT_TRUE;
            } else {
                if ((fev->src == NULL) && (fev->ev != 0x87654321)) {
                    __ut_skt_err_line = __LINE__;
                }
                if ((fev->src != NULL) && (fev->ev != 0x12345678)) {
                    __ut_skt_err_line = __LINE__;
                }
            }
            ++__ut_ev_rcv;

            tt_fiber_finish(fev);
            if (done) {
                break;
            }
        }

        if (e_tmr != NULL) {
            tt_s64_t now = tt_time_ref();
            now -= (tt_s64_t)(tt_uintptr_t)e_tmr->param;
            now = labs((long)now);
            now = tt_time_ref2ms(now);
            if (now > __ut_skt_max_diff) {
                __ut_skt_max_diff = now;
            }

            if (e_tmr->ev == ~0) {
                tt_tmr_set_param(e_tmr, (void *)(tt_uintptr_t)tt_time_ref());
                tt_tmr_start(e_tmr);
            } else {
                now = tt_rand_u32() % 3;
                if (now == 0) {
                    --__skt_cli_tmr;
                    tt_tmr_destroy(e_tmr);
                } else if (now == 1) {
                    --__skt_cli_tmr;
                    tt_tmr_stop(e_tmr);
                } else {
                    tt_tmr_set_delay(e_tmr, tt_rand_u32() % 5 + 5);
                    tt_tmr_set_param(e_tmr,
                                     (void *)(tt_uintptr_t)tt_time_ref());
                    tt_tmr_start(e_tmr);
                }
            }
        }

#if 0 //#ifdef __CHECK_IO
        c = buf2[0];
        for (k = 0; k < n; ++k) {
            if (buf2[k] != c++) {
                __ut_skt_err_line = __LINE__;
                return TT_FAIL;
            }
        }
#endif
    }
    TT_INFO("client done");

    tt_skt_destroy(s);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_udp_event)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_task_t t;
    tt_s64_t start, end, dur;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_task_add_fiber(&t, "svr", __f_svr_udp_ev, NULL, NULL);
    tt_task_add_fiber(&t, "cli", __f_cli_udp_ev, NULL, NULL);

    __ut_skt_err_line = 0;
    __ut_ev_rcv = 0;
    __ut_ev_snd = 0;
    __cli_sent = 0;
    __cli_recvd = 0;
    __svr_sent = 0;
    __svr_recvd = 0;
    tt_atomic_s64_set(&__io_num, 0);
    __ut_skt_max_diff = 0;
    __skt_svr_tmr = 0;
    __skt_cli_tmr = 0;

    start = tt_time_ref();
    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");
    tt_task_wait(&t);
    end = tt_time_ref();
    TT_UT_EQUAL(__ut_skt_err_line, 0, "");
    TT_UT_EQUAL(__ut_ev_rcv, __ut_ev_snd, "");

    dur = tt_time_ref2ms(end - start);
    TT_RECORD_INFO(
        "udp: %f MB/s, cli sent/recv: %d/%d, svr sent/recv: %d/%d, time diff: "
        "%d",
        ((float)tt_atomic_s64_get(&__io_num) / (1 << 20)) * 1000 / dur,
        __cli_sent,
        __cli_recvd,
        __svr_sent,
        __svr_recvd,
        (tt_u32_t)__ut_skt_max_diff);

    // test end
    TT_TEST_CASE_LEAVE()
}

#endif

TT_TEST_ROUTINE_DEFINE(case_mac_addr)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_macaddr_t ma;
    tt_result_t ret;
    tt_u8_t a[6] = {1, 2, 3, 4, 5, 6};
    tt_char_t buf[20];

    TT_TEST_CASE_ENTER()
    // test start

    tt_macaddr_init(&ma, NULL);
    TT_UT_EQUAL(ma.addr[0], 0, "");
    TT_UT_EQUAL(ma.addr[5], 0, "");

    tt_macaddr_init(&ma, a);
    TT_UT_EQUAL(ma.addr[0], 1, "");
    TT_UT_EQUAL(ma.addr[5], 6, "");

    ret = tt_macaddr_n2p(&ma, buf, 2, 0);
    TT_UT_EQUAL(ret, TT_E_NOSPC, "");
    ret = tt_macaddr_n2p(&ma, buf, 18, 0);
    TT_UT_SUCCESS(ret, "");
    TT_UT_STREQ(buf, "01:02:03:04:05:06", "");
    ret = tt_macaddr_n2p(&ma, buf, 20, 0);
    TT_UT_SUCCESS(ret, "");
    TT_UT_STREQ(buf, "01:02:03:04:05:06", "");

    ret = tt_macaddr_p2n(&ma, "");
    TT_UT_FAIL(ret, "");
    ret = tt_macaddr_p2n(&ma, "12");
    TT_UT_FAIL(ret, "");
    ret = tt_macaddr_p2n(&ma, "123");
    TT_UT_FAIL(ret, "");
    ret = tt_macaddr_p2n(&ma, "12:");
    TT_UT_FAIL(ret, "");
    ret = tt_macaddr_p2n(&ma, "12:34:56:78:90:aa:bb:cc");
    TT_UT_FAIL(ret, "");

    tt_strncpy(buf, "12:34:56:78:90:ab", sizeof(buf));
    ret = tt_macaddr_p2n(&ma, buf);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(ma.addr[0], 0x12, "");
    TT_UT_EQUAL(ma.addr[5], 0xab, "");

    tt_strncpy(buf, "1:34:05:78:90:a", sizeof(buf));
    ret = tt_macaddr_p2n(&ma, buf);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(ma.addr[0], 0x1, "");
    TT_UT_EQUAL(ma.addr[2], 0x5, "");
    TT_UT_EQUAL(ma.addr[4], 0x90, "");
    TT_UT_EQUAL(ma.addr[5], 0xa, "");

    buf[0] = 1;
    ret = tt_macaddr_p2n(&ma, buf);
    TT_UT_FAIL(ret, "");
    buf[0] = '1';

    buf[2] = 1;
    ret = tt_macaddr_p2n(&ma, buf);
    TT_UT_FAIL(ret, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_bool_t __oob_recvd;

static tt_result_t __f_svr_oob(IN void *param)
{
    tt_skt_t *s, *new_s;
    tt_u8_t buf[100];
    tt_u32_t n;
    tt_result_t ret;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;

    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    tt_skt_set_reuseaddr(s, TT_TRUE);

    if (!TT_OK(tt_skt_bind_p(s, TT_NET_AF_INET, "127.0.0.1", 44556))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_listen(s))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    new_s = tt_skt_accept(s, NULL, NULL, &fev, &tmr);
    if (new_s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

#if 1
    if (!TT_OK(tt_skt_set_oobinline(new_s, TT_TRUE))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }
#endif

    while ((ret = tt_skt_recv(new_s, buf, sizeof(buf), &n, &fev, &tmr)) !=
           TT_E_END) {
        tt_u32_t i;
        for (i = 0; i < n; ++i) {
            if (buf[i] == '2') {
                __oob_recvd = TT_TRUE;
                goto done;
            }
        }
    }

done:

    tt_skt_destroy(new_s);
    tt_skt_destroy(s);

    return TT_SUCCESS;
}

static tt_result_t __f_cli_oob(IN void *param)
{
    tt_skt_t *s;
    tt_u8_t buf[100];
    tt_u32_t n;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;

    // valid address
    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    tt_skt_set_reuseaddr(s, TT_TRUE);
    tt_skt_set_oobinline(s, TT_TRUE);

    if (!TT_OK(tt_skt_connect_p(s, TT_NET_AF_INET, "127.0.0.1", 44556))) {
        __ut_skt_err_line = __LINE__;
        goto fail;
    }

    if (!TT_OK(tt_skt_send(s, (tt_u8_t *)"111", 3, &n))) {
        __ut_skt_err_line = __LINE__;
        goto fail;
    }

    if (!TT_OK(tt_skt_send_oob(s, (tt_u8_t)'2'))) {
        __ut_skt_err_line = __LINE__;
        goto fail;
    }

    if (!TT_OK(tt_skt_send(s, (tt_u8_t *)"3333", 4, &n))) {
        __ut_skt_err_line = __LINE__;
        goto fail;
    }

    // shutdown may fail as server may already closed
    if (TT_OK(tt_skt_shutdown(s, TT_SKT_SHUT_WR))) {
        while (tt_skt_recv(s, buf, sizeof(buf), &n, &fev, &tmr) != TT_E_END) {
        }
    }


    tt_skt_destroy(s);

    return TT_SUCCESS;

fail:
    tt_task_exit(NULL);
    return TT_FAIL;
}

TT_TEST_ROUTINE_DEFINE(case_tcp_oob)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_task_t t;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    __oob_recvd = TT_FALSE;
    tt_task_add_fiber(&t, NULL, __f_svr_oob, NULL, NULL);
    tt_task_add_fiber(&t, NULL, __f_cli_oob, NULL, NULL);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");

    tt_task_wait(&t);
    TT_UT_EQUAL(__ut_skt_err_line, 0, "");
    TT_UT_EQUAL(__oob_recvd, TT_TRUE, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_result_t __f_svr_block(IN void *param)
{
    tt_skt_t *s, *new_s;
    tt_u8_t buf[2345] = "6789";
    tt_u32_t n, loop, ev_num = 0;
    tt_result_t ret;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;

    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_bind_p(s, TT_NET_AF_INET, "127.0.0.1", 56565))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    if (!TT_OK(tt_skt_listen(s))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

        while ((new_s = tt_skt_accept(s, NULL, NULL, &fev, &tmr)) == NULL) {
            if (fev != NULL) {
                __SKT_DETAIL("=> svr acc recv ev");
                if ((fev->src == NULL) && (fev->ev != 0x87654321)) {
                    __ut_skt_err_line = __LINE__;
                }
                if ((fev->src != NULL) && (fev->ev != 0x12345678)) {
                    __ut_skt_err_line = __LINE__;
                }
                ++__ut_ev_rcv;
                tt_fiber_finish(fev);
            } else {
                __ut_skt_err_line = __LINE__;
                return TT_FAIL;
            }
        }

// only send but no receive
    loop = 0;
    while (loop++ < TB_NUM) {
        tt_u32_t total = 0;

        if (!TT_OK(tt_skt_send(new_s, buf, sizeof(buf), &n))) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        __svr_sent += n;
#if 1 // #ifdef __TCP_DETAIL
            TT_INFO("server sent %d => %d", n, __svr_sent);
#endif

        if (n == 0) {
            TT_INFO("tcp blocking");
            ev_num++;
            if (ev_num < 10) { 
                tt_fiber_yield();
            //tt_sleep(500); // sleep thread
                } else {break;}
        }
    }
    tt_skt_shutdown(new_s, TT_SKT_SHUT_WR);
    
    while ((ret = tt_skt_recv(new_s, buf, sizeof(buf), &n, &fev, &tmr)) !=
           TT_E_END) {
           __svr_recvd += n;
           TT_INFO("server recv %d => %d", n, __svr_recvd);
    }
                
        tt_skt_destroy(new_s);
        tt_skt_destroy(s);
    
        return TT_SUCCESS;
    }

static tt_result_t __f_cli_block(IN void *param)
{
    tt_skt_t *s;
    tt_u8_t buf[1234] = "123";
    tt_u32_t n, loop, ev_num = 0;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr, *p_tmr;
    tt_result_t ret;
    
    // invalid address, should fail
    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    if (TT_OK(tt_skt_connect_p(s, TT_NET_AF_INET, "127.0.0.1", 63333))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }
    tt_skt_destroy(s);

    // valid address
    s = tt_skt_create(TT_NET_AF_INET, TT_NET_PROTO_TCP, NULL);
    if (s == NULL) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    tt_skt_set_reuseaddr(s, TT_TRUE);

    if (!TT_OK(tt_skt_connect_p(s, TT_NET_AF_INET, "127.0.0.1", 56565))) {
        __ut_skt_err_line = __LINE__;
        return TT_FAIL;
    }

    loop = 0;
    while (loop++ < TB_NUM) {
        tt_u32_t total = 0;

        tmr = tt_tmr_create(3000, 0, NULL);
        tt_tmr_start(tmr);
        
        if (!TT_OK(tt_skt_send(s, buf, sizeof(buf), &n))) {
            __ut_skt_err_line = __LINE__;
            return TT_FAIL;
        }
        __cli_sent += n;
#if 1 // #ifdef __TCP_DETAIL
            TT_INFO("client sent %d => %d", n, __cli_sent);
#endif
        tt_tmr_stop(tmr);

        if (n == 0) {
            TT_INFO("tcp blocking");
            ev_num++;
            if (ev_num < 10) { 
                tt_fiber_yield();
            //tt_sleep(500); // sleep thread
                } else {break;}
        }
    }
    tt_skt_shutdown(s, TT_SKT_SHUT_WR);

    while ((ret = tt_skt_recv(s, buf, sizeof(buf), &n, &fev, &tmr)) !=
           TT_E_END) {
           __cli_recvd += n;
           TT_INFO("client recv %d => %d", n, __cli_recvd);
    }

    tt_skt_destroy(s);

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(case_tcp_block)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_task_t t;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_task_create(&t, NULL);
    TT_UT_SUCCESS(ret, "");

    __cli_sent = 0;
    __cli_recvd = 0;
    __svr_sent = 0;
    __svr_recvd = 0;

    tt_task_add_fiber(&t, "s", __f_svr_block, NULL, NULL);
    tt_task_add_fiber(&t, "c", __f_cli_block, NULL, NULL);

    __ut_skt_err_line = 0;
    tt_atomic_s64_set(&__io_num, 0);

    ret = tt_task_run(&t);
    TT_UT_SUCCESS(ret, "");

    tt_task_wait(&t);
    TT_UT_EQUAL(__ut_skt_err_line, 0, "");
    TT_UT_EQUAL(__cli_sent, __cli_recvd, "");
    TT_UT_EQUAL(__svr_sent, __cli_recvd, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

