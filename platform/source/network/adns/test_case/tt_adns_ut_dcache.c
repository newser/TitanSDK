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

#include <event/tt_event_center.h>
#include <network/adns/tt_adns_domain_manager.h>
#include <network/adns/tt_adns_packet.h>
#include <network/adns/tt_adns_rr.h>
#include <network/adns/tt_adns_rr_a_in.h>
#include <network/adns/tt_adns_rr_aaaa_in.h>
#include <network/tt_network_def.h>

#include <unit_test/tt_unit_test.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __ADNS_UT_LOG TT_DEBUG
//#define __ADNS_UT_LOG(...)

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_adns_ut_dc_basic)
TT_TEST_ROUTINE_DECLARE(tt_adns_ut_dc_excep)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(adns_dc_case)

#if 0
    TT_TEST_CASE("tt_adns_ut_dc_basic",
                 "adns domain basic",
                 tt_adns_ut_dc_basic, NULL,
                 NULL, NULL,
                 NULL, NULL),
#endif

#if 1
TT_TEST_CASE("tt_adns_ut_dc_excep",
             "adns domain exceptional cases",
             tt_adns_ut_dc_excep,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,
#endif

    TT_TEST_CASE_LIST_DEFINE_END(adns_dc_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(ADNS_UT_DCACHE, 0, adns_dc_case)

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

    static struct __ns_input_t
{
    const tt_char_t *ns_name;
    tt_net_family_t af;
    tt_net_protocol_t proto;
    tt_u16_t port;
} __test_ns[] = {
    {"10.74.51.20", TT_NET_AF_INET, TT_NET_PROTO_UDP, 53},
    {"10.74.51.20", TT_NET_AF_INET, TT_NET_PROTO_TCP, 53},
    {"64.104.123.144", TT_NET_AF_INET, TT_NET_PROTO_UDP, 53},
};

static struct
{
    const tt_char_t *name;
    tt_bool_t queried;
    tt_dlist_t rrlist;
} __test_set[] = {
    {"aBcDeFg.com"}, {"abc.sipurash.com"}, {"sipurash.com"},
    //{ "www.apple.com" }, // this entry has short ttl
};

static tt_atomic_s32_t q_cnt;
static tt_result_t case_ret;
static tt_u32_t case_err_line;
void ___on_query1(IN tt_char_t *name,
                  IN tt_adns_rr_type_t type,
                  IN tt_u32_t flag,
                  IN tt_adns_qryctx_t *qryctx)
{
    tt_u32_t i = (tt_u32_t)(tt_uintptr_t)qryctx->cb_param;

    if (!__test_set[i].queried) {
        tt_dlist_init(&__test_set[i].rrlist);

        tt_adns_rrlist_copy(&__test_set[i].rrlist,
                            qryctx->rrlist,
                            TT_ADNS_RRLIST_COPY_ALL);
        tt_adns_rrlist_dump(&__test_set[i].rrlist);

        __test_set[i].queried = TT_TRUE;
    } else {
        // compare rr num for simplicity
        if (tt_dlist_count(&__test_set[i].rrlist) !=
            tt_dlist_count(qryctx->rrlist)) {
            case_ret = TT_FAIL;
            case_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }

        tt_adns_rrlist_dump(qryctx->rrlist);
    }

    if (tt_atomic_s32_dec(&q_cnt) == 0) {
        for (i = 0; i < sizeof(__test_set) / sizeof(__test_set[0]); ++i) {
            tt_result_t ret;
            tt_adns_qryctx_t q_ret;
            ret = tt_adns_query(__test_set[i].name,
                                TT_ADNS_RR_A_IN,
                                TT_ADNS_QUERY_ALL | TT_ADNS_QUERY_NEW,
                                &q_ret);

            if (TT_OK(ret)) {
                if (tt_dlist_count(&__test_set[i].rrlist) !=
                    tt_dlist_count(q_ret.rrlist)) {
                    case_ret = TT_FAIL;
                    case_err_line = __LINE__;
                    tt_evc_exit(TT_LOCAL_EVC);
                    return;
                }
            } else {
                if (tt_dlist_count(&__test_set[i].rrlist) != 0) {
                    case_ret = TT_FAIL;
                    case_err_line = __LINE__;
                    tt_evc_exit(TT_LOCAL_EVC);
                    return;
                }
            }
        }

        // the adcache->evc is not the central evc
        tt_evc_exit(TT_LOCAL_EVC);
    }
}

tt_result_t __dc_basic_on_init(IN struct tt_evcenter_s *evc,
                               IN void *on_init_param)
{
    tt_u32_t i;
    tt_result_t ret;

    q_cnt = 2 * sizeof(__test_set) / sizeof(__test_set[0]);

    for (i = 0; i < sizeof(__test_set) / sizeof(__test_set[0]); ++i) {
        ret = tt_adns_query_async(__test_set[i].name,
                                  TT_ADNS_RR_A_IN,
                                  TT_ADNS_QUERY_ALL | TT_ADNS_QUERY_NEW,
                                  ___on_query1,
                                  (void *)(tt_uintptr_t)i);
        if (ret != TT_SUCCESS) {
            case_ret = TT_FAIL;
            case_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return TT_FAIL;
        }
    }
    for (i = 0; i < sizeof(__test_set) / sizeof(__test_set[0]); ++i) {
        ret = tt_adns_query_async(__test_set[i].name,
                                  TT_ADNS_RR_A_IN,
                                  TT_ADNS_QUERY_ALL | TT_ADNS_QUERY_NEW,
                                  ___on_query1,
                                  (void *)(tt_uintptr_t)i);
        if (ret != TT_SUCCESS) {
            case_ret = TT_FAIL;
            case_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_adns_ut_dc_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_evcenter_t evc;
    tt_evc_attr_t evc_attr;
    tt_result_t ret;

    tt_netaddr_t ns_addr[sizeof(__test_ns) / sizeof(struct __ns_input_t)];
    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    case_ret = TT_SUCCESS;
    case_err_line = 0;

    for (i = 0; i < sizeof(__test_ns) / sizeof(struct __ns_input_t); ++i) {
        ns_addr[i].protocol = __test_ns[i].proto;
        tt_sktaddr_init(&ns_addr[i].sktaddr, __test_ns[i].af);
        tt_sktaddr_set_ip_p(&ns_addr[i].sktaddr,
                            (tt_char_t *)__test_ns[i].ns_name);
        tt_sktaddr_set_port(&ns_addr[i].sktaddr, __test_ns[i].port);
    }

    for (i = 0; i < sizeof(__test_set) / sizeof(__test_set[0]); ++i) {
        tt_dlist_init(&__test_set[i].rrlist);
    }

    tt_evc_attr_default(&evc_attr);
    evc_attr.adns_ev_attr.enable = TT_TRUE;
    evc_attr.adns_ev_attr.name_server = ns_addr;
    evc_attr.adns_ev_attr.name_server_num =
        sizeof(__test_ns) / sizeof(struct __ns_input_t);

    evc_attr.on_init = __dc_basic_on_init;

    ret = tt_evc_create(&evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(case_ret, TT_SUCCESS, "");

    for (i = 0; i < sizeof(__test_set) / sizeof(__test_set[0]); ++i) {
        tt_adns_rrlist_destroy(&__test_set[i].rrlist);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

#define __ad2_ns_port 50111
#define __ad2_ns_num 4

static tt_result_t __ad2_ret;
static tt_u32_t __ad2_err_line;
#define __ad2_fail(...)                                                        \
    do {                                                                       \
        __ad2_ret = TT_FAIL;                                                   \
        __ad2_err_line = __LINE__;                                             \
    } while (0)
#define __ad2_fail_exit(...)                                                   \
    do {                                                                       \
        __ad2_ret = TT_FAIL;                                                   \
        __ad2_err_line = __LINE__;                                             \
        tt_evc_exit(TT_LOCAL_EVC);                                             \
    } while (0)

static tt_skt_t __ad2_ns_skt[__ad2_ns_num];
static tt_skt_t __ad2_acc_skt[__ad2_ns_num];
static tt_u8_t __ad2_buf[__ad2_ns_num][4000];
static tt_u32_t __ad2_cnt[__ad2_ns_num];

struct __ns_input_t __ad2_ns_input[__ad2_ns_num] = {
    {
        "127.0.0.1", TT_NET_AF_INET, TT_NET_PROTO_UDP, __ad2_ns_port,
    },
    {
        "127.0.0.1", TT_NET_AF_INET, TT_NET_PROTO_TCP, __ad2_ns_port + 2,
    },
    {
        "::1", TT_NET_AF_INET6, TT_NET_PROTO_UDP, __ad2_ns_port + 1,
    },
    {
        "::1", TT_NET_AF_INET6, TT_NET_PROTO_TCP, __ad2_ns_port + 3,
    },
};

#define __ad2_q_name "test_ad2"
#define __ad2_q_ip "1.2.3.4"

void __ad2_on_ns_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
}

void __ad2_on_accept(IN tt_skt_t *listening_skt,
                     IN tt_skt_t *new_skt,
                     IN tt_skt_aioctx_t *aioctx);
void __ad2_on_acc_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
    tt_result_t ret;
    tt_u32_t idx = (tt_u32_t)(tt_uintptr_t)cb_param;
    tt_skt_exit_t de = {__ad2_on_acc_destroy, (void *)(tt_uintptr_t)idx};

    TT_ASSERT(idx < __ad2_ns_num);

    ret = tt_skt_accept_async(&__ad2_ns_skt[idx],
                              &__ad2_acc_skt[idx],
                              NULL,
                              &de,
                              __ad2_on_accept,
                              (void *)(tt_uintptr_t)idx);
    if (ret != TT_SUCCESS) {
        //__ad2_fail_exit();
        return;
    }
}

tt_adns_pkt_t *__ad2_pkt_proc(tt_adns_pkt_t *pkt, tt_u32_t idx)
{
    tt_u32_t n;
    tt_adns_pkt_t *resp = NULL;

    ++__ad2_cnt[idx];
    n = __ad2_cnt[idx];

    if (n < 10) {
        // ignore
        return NULL;
    } else {
        if (n == 10) {
            // not a response
            resp = tt_adns_pkt_create(pkt->__id, 0, 0, 0);
        } else {
            resp = tt_adns_pkt_create(pkt->__id, 0, 0, __ADNS_PKT_RESP);
        }

        if (resp != NULL) {
            tt_adns_rr_t *ans = NULL;
            tt_adrr_a_t rd;

            tt_sktaddr_ip_p2n(TT_NET_AF_INET, __ad2_q_ip, &rd.addr);

            if (n % 10 == 1) {
                // ttl 0
                ans = tt_adrr_a_create(__ad2_q_name,
                                       TT_ADNS_RR_COPY_NAME,
                                       0,
                                       &rd);
            } else if (n % 10 == 2) {
                // ttl 1
                ans = tt_adrr_a_create(__ad2_q_name,
                                       TT_ADNS_RR_COPY_NAME,
                                       1,
                                       &rd);
            } else if (n % 10 == 3) {
                // ttl 10
                ans = tt_adrr_a_create("wront_name",
                                       TT_ADNS_RR_COPY_NAME,
                                       10,
                                       &rd);
            } else {
                ans = tt_adrr_a_create(__ad2_q_name,
                                       TT_ADNS_RR_COPY_NAME,
                                       3600,
                                       &rd);
            }

            if (ans != NULL) {
                tt_adns_pkt_add_answer(resp, ans);
            }
        }

        tt_adns_pkt_generate(resp);

        __ADNS_UT_LOG("======>> n: %d, idx: %d", n, idx);
        if (n < 20 || idx != 3) {
            // change packet content
            tt_u8_t *pos = TT_BUF_RPOS(&resp->buf);
            tt_u32_t data_len = TT_BUF_RLEN(&resp->buf);
            tt_u8_t *end = pos + data_len;
            tt_u32_t len = tt_rand_u32() % data_len + 1;
            while ((pos < end) && (len > 0)) {
                *pos = tt_rand_u32() & 0xff;
                ++pos;
                --len;
            }
        }
        return resp;
    }
}

void __ad2_on_sendto(IN tt_skt_t *skt,
                     IN tt_blob_t *blob_array,
                     IN tt_u32_t blob_num,
                     IN tt_sktaddr_t *remote_addr,
                     IN tt_skt_aioctx_t *aioctx,
                     IN tt_u32_t send_len)
{
    tt_adns_pkt_t *pkt = (tt_adns_pkt_t *)aioctx->cb_param;
    tt_adns_pkt_release(pkt);
}
void __ad2_on_recvfrom(IN tt_skt_t *skt,
                       IN tt_blob_t *buf_array,
                       IN tt_u32_t buf_num,
                       IN tt_skt_aioctx_t *aioctx,
                       IN tt_u32_t recv_len,
                       IN tt_sktaddr_t *remote_addr)
{
    tt_u32_t idx = (tt_u32_t)(tt_uintptr_t)aioctx->cb_param;
    tt_blob_t __bf = {__ad2_buf[idx], sizeof(__ad2_buf[idx])};
    tt_buf_t tds;
    tt_adns_pkt_t *pkt;

    if (!TT_OK(aioctx->result)) {
        if (aioctx->result == TT_END) {
            tt_async_skt_shutdown(skt, TT_SKT_SHUT_RD);
        }
        return;
    }

    __ADNS_UT_LOG("======>> ns[%d] recvfrom request, %d bytes", idx, recv_len);

    tt_buf_create_nocopy(&tds, buf_array->addr, buf_array->len, NULL);
    pkt = tt_adns_pkt_parse(&tds, TT_ADNS_PKT_PARSE_ALL);
    if (pkt != NULL) {
        tt_adns_pkt_t *resp = __ad2_pkt_proc(pkt, idx);
        if (resp != NULL) {
            tt_blob_t sbuf;
            tt_buf_get_rblob(&resp->buf, &sbuf);
            if (sbuf.len > 1)
                --sbuf.len; // to let tcp send valid result
            if (tt_skt_sendto_async(skt,
                                    &sbuf,
                                    1,
                                    remote_addr,
                                    __ad2_on_sendto,
                                    resp) != TT_SUCCESS) {
                __ad2_fail_exit();
                return;
            }
        }
        tt_adns_pkt_release(pkt);
    }

    if (tt_skt_recvfrom_async(skt,
                              &__bf,
                              1,
                              __ad2_on_recvfrom,
                              (void *)(tt_uintptr_t)idx) != TT_SUCCESS) {
        __ad2_fail_exit();
        return;
    }
}

void __ad2_on_send(IN tt_skt_t *skt,
                   IN tt_blob_t *buf_array,
                   IN tt_u32_t buf_num,
                   IN tt_skt_aioctx_t *aioctx,
                   IN tt_u32_t send_len)
{
    tt_adns_pkt_t *pkt = (tt_adns_pkt_t *)aioctx->cb_param;
    tt_adns_pkt_release(pkt);
}
void __ad2_on_recv(IN tt_skt_t *skt,
                   IN tt_blob_t *buf_array,
                   IN tt_u32_t buf_num,
                   IN tt_skt_aioctx_t *aioctx,
                   IN tt_u32_t recv_len)
{
    tt_u32_t idx = (tt_u32_t)(tt_uintptr_t)aioctx->cb_param;
    tt_blob_t __bf = {__ad2_buf[idx], sizeof(__ad2_buf[idx])};
    tt_buf_t tds;
    tt_adns_pkt_t *pkt;

    if (!TT_OK(aioctx->result)) {
        if (aioctx->result == TT_END) {
            tt_async_skt_shutdown(skt, TT_SKT_SHUT_RD);
        }
        return;
    }

    __ADNS_UT_LOG("======>> ns[%d] recv request, %d bytes", idx, recv_len);

    tt_buf_create_nocopy(&tds, buf_array->addr, buf_array->len, NULL);
    pkt = tt_adns_pkt_parse(&tds, TT_ADNS_PKT_PARSE_ALL);
    if (pkt != NULL) {
        tt_adns_pkt_t *resp = __ad2_pkt_proc(pkt, idx);
        if (resp != NULL) {
            int n = tt_rand_u32() % 10 + 1;
            int seg = TT_BUF_RLEN(&resp->buf) / n;
            tt_u8_t *pos;
            tt_blob_t tmp_blob;

            pos = TT_BUF_RPOS(&resp->buf);
            n = TT_BUF_RLEN(&resp->buf);
            while (n >= seg) {
                tmp_blob.addr = pos;
                tmp_blob.len = seg;

                pos += seg;
                n -= seg;

                tt_adns_pkt_ref(resp);
                __ADNS_UT_LOG("<<====== ns[%d] sent response", idx);
                if (tt_skt_send_async(skt, &tmp_blob, 1, __ad2_on_send, resp) !=
                    TT_SUCCESS) {
                    __ad2_fail_exit();
                    return;
                }
            }
            if (n > 0) {
                tmp_blob.addr = pos;
                tmp_blob.len = n;
                tt_adns_pkt_ref(resp);
                __ADNS_UT_LOG("<<====== ns[%d] sent response again", idx);
                if (tt_skt_send_async(skt, &tmp_blob, 1, __ad2_on_send, resp) !=
                    TT_SUCCESS) {
                    __ad2_fail_exit();
                    return;
                }
            }

            tt_adns_pkt_release(resp);
        }
        tt_adns_pkt_release(pkt);
    }

    if (idx > 1 && __ad2_cnt[idx] % 5 == 0) {
        // test tcp disconnect
        tt_async_skt_destroy(skt, TT_FALSE);
    } else if (tt_skt_recv_async(skt,
                                 &__bf,
                                 1,
                                 __ad2_on_recv,
                                 (void *)(tt_uintptr_t)idx) != TT_SUCCESS) {
        //__ad2_fail_exit(); // may fail if skt is shutdown
        return;
    }
}

void __ad2_on_accept(IN tt_skt_t *listening_skt,
                     IN tt_skt_t *new_skt,
                     IN tt_skt_aioctx_t *aioctx)
{
    tt_u32_t idx = (tt_u32_t)(tt_uintptr_t)aioctx->cb_param;
    tt_blob_t __bf = {__ad2_buf[idx], sizeof(__ad2_buf[idx])};
    tt_result_t ret;

    if (!TT_OK(aioctx->result)) {
        return;
    }

    ret = tt_skt_recv_async(new_skt,
                            &__bf,
                            1,
                            __ad2_on_recv,
                            (void *)(tt_uintptr_t)idx);

    if (idx != 3) {
        TT_INFO("idx 3 recv ongoing");
    }

    if (ret != TT_SUCCESS) {
        __ad2_fail_exit();
    }
}

tt_result_t __ad2_ns_on_init(IN struct tt_evcenter_s *evc,
                             IN void *on_init_param)
{
    tt_u32_t idx = (tt_u32_t)(tt_uintptr_t)on_init_param;
    struct __ns_input_t *ni = &__ad2_ns_input[idx];
    tt_sktaddr_t sa;
    tt_result_t ret;
    tt_skt_exit_t de = {__ad2_on_ns_destroy, (void *)(tt_uintptr_t)idx};
    tt_skt_attr_t sat;

    tt_sktaddr_init(&sa, ni->af);
    tt_sktaddr_set_ip_p(&sa, (tt_char_t *)ni->ns_name);
    tt_sktaddr_set_port(&sa, ni->port);

    tt_skt_attr_default(&sat);
    tt_skt_attr_set_reuseaddr(&sat, TT_TRUE);
    tt_skt_attr_set_reuseport(&sat, TT_TRUE);

    if (ni->proto == TT_NET_PROTO_UDP) {
        tt_blob_t __bf = {__ad2_buf[idx], sizeof(__ad2_buf[idx])};

        ret = tt_udp_server_async(&__ad2_ns_skt[idx], ni->af, &sat, &sa, &de);
        if (!TT_OK(ret)) {
            __ad2_fail();
            return TT_FAIL;
        }

        ret = tt_skt_recvfrom_async(&__ad2_ns_skt[idx],
                                    &__bf,
                                    1,
                                    __ad2_on_recvfrom,
                                    (void *)(tt_uintptr_t)idx);
        if (ret != TT_SUCCESS) {
            __ad2_fail();
            return TT_FAIL;
        }
    } else {
        ret = tt_tcp_server_async(&__ad2_ns_skt[idx],
                                  ni->af,
                                  &sat,
                                  &sa,
                                  TT_SKT_BACKLOG_DEFAULT,
                                  &de);
        if (!TT_OK(ret)) {
            __ad2_fail();
            return TT_FAIL;
        }

        de.on_destroy = __ad2_on_acc_destroy;
        ret = tt_skt_accept_async(&__ad2_ns_skt[idx],
                                  &__ad2_acc_skt[idx],
                                  NULL,
                                  &de,
                                  __ad2_on_accept,
                                  (void *)(tt_uintptr_t)idx);
        if (ret != TT_SUCCESS) {
            __ad2_fail();
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}

tt_result_t __ad2_ns_on_exit(IN struct tt_evcenter_s *evc,
                             IN void *on_init_param)
{
    tt_u32_t idx = (tt_u32_t)(tt_uintptr_t)on_init_param;
    struct __ns_input_t *ni = &__ad2_ns_input[idx];

    if (ni->proto == TT_NET_PROTO_UDP) {
        tt_async_skt_destroy(&__ad2_ns_skt[idx], TT_TRUE);
    } else {
        tt_async_skt_destroy(&__ad2_acc_skt[idx], TT_TRUE);
        tt_async_skt_destroy(&__ad2_ns_skt[idx], TT_TRUE);
    }

    return TT_SUCCESS;
}

void __ad2_on_query(IN tt_char_t *name,
                    IN tt_adns_rr_type_t type,
                    IN tt_u32_t flag,
                    IN tt_adns_qryctx_t *qryctx)
{
    tt_adns_rr_t *rr;
    tt_adrr_a_t *rd;
    tt_adrr_a_t corrent_rd;

    if (qryctx->result != TT_SUCCESS) {
        if (tt_adns_query_async(__ad2_q_name,
                                TT_ADNS_RR_A_IN,
                                TT_ADNS_QUERY_ALL | TT_ADNS_QUERY_NEW,
                                __ad2_on_query,
                                NULL) != TT_SUCCESS) {
            __ad2_fail_exit();
        }
        return;
    }

    if (tt_dlist_count(qryctx->rrlist) != 1) {
        __ad2_fail_exit();
        return;
    }

    rr = TT_CONTAINER(tt_dlist_head(qryctx->rrlist), tt_adns_rr_t, node);
    rd = TT_ADRR_CAST(rr, tt_adrr_a_t);
    tt_sktaddr_ip_p2n(TT_NET_AF_INET, __ad2_q_ip, &corrent_rd.addr);
    if (rd->addr.a32.__u32 != corrent_rd.addr.a32.__u32) {
        //__ad2_fail_exit(); // may due to random modification
        TT_INFO("correct: %x, received: %x",
                corrent_rd.addr.a32.__u32,
                rd->addr.a32.__u32);

        if (tt_adns_query_async(__ad2_q_name,
                                TT_ADNS_RR_A_IN,
                                TT_ADNS_QUERY_ALL | TT_ADNS_QUERY_NEW,
                                __ad2_on_query,
                                NULL) != TT_SUCCESS) {
            __ad2_fail_exit();
        }
        return;
    }

    tt_adns_rrlist_dump(qryctx->rrlist);
    __ad2_ret = TT_SUCCESS;
    tt_evc_exit(TT_LOCAL_EVC);
}

tt_result_t __dc_excep_on_init(IN struct tt_evcenter_s *evc,
                               IN void *on_init_param)
{
    tt_result_t ret = tt_adns_query_async(__ad2_q_name,
                                          TT_ADNS_RR_A_IN,
                                          TT_ADNS_QUERY_ALL | TT_ADNS_QUERY_NEW,
                                          __ad2_on_query,
                                          NULL);
    if (ret != TT_SUCCESS) {
        __ad2_fail();
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_adns_ut_dc_excep)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_evcenter_t evc;
    tt_evc_attr_t evc_attr;
    tt_result_t ret;
    tt_evcenter_t ns_evc[__ad2_ns_num];
    tt_netaddr_t ns_addr[__ad2_ns_num];
    tt_u32_t i;

    TT_TEST_CASE_ENTER()
    // test start

    // start name server
    for (i = 0; i < __ad2_ns_num; ++i) {
        tt_evc_attr_default(&evc_attr);
        evc_attr.on_init = __ad2_ns_on_init;
        evc_attr.on_init_param = (void *)(tt_uintptr_t)i;
        evc_attr.on_exit = __ad2_ns_on_exit;
        evc_attr.on_exit_param = (void *)(tt_uintptr_t)i;

        ret = tt_evc_create(&ns_evc[i], TT_FALSE, &evc_attr);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    }

    // start evc
    for (i = 0; i < __ad2_ns_num; ++i) {
        ns_addr[i].protocol = __ad2_ns_input[i].proto;
        tt_sktaddr_init(&ns_addr[i].sktaddr, __ad2_ns_input[i].af);
        tt_sktaddr_set_ip_p(&ns_addr[i].sktaddr,
                            (tt_char_t *)__ad2_ns_input[i].ns_name);
        tt_sktaddr_set_port(&ns_addr[i].sktaddr, __ad2_ns_input[i].port);
    }

    tt_evc_attr_default(&evc_attr);
    evc_attr.adns_ev_attr.enable = TT_TRUE;
    evc_attr.adns_ev_attr.name_server = ns_addr;
    evc_attr.adns_ev_attr.name_server_num = __ad2_ns_num;

    evc_attr.on_init = __dc_excep_on_init;

    ret = tt_evc_create(&evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    for (i = 0; i < __ad2_ns_num; ++i) {
        tt_evc_exit(&ns_evc[i]);
        ret = tt_evc_wait(&ns_evc[i]);
        if (ret != TT_SUCCESS) {
            TT_ERROR("fail to wati ns evc[%d]", i);
        }
    }

    TT_TEST_CHECK_EQUAL(__ad2_ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
