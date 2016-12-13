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

#include <network/adns/tt_adns_resolver.h>

#include <algorithm/tt_buffer_format.h>
#include <event/tt_timer_event.h>
#include <io/tt_socket_aio.h>
#include <network/adns/tt_adns_domain_manager.h>
#include <network/adns/tt_adns_packet.h>
#include <timer/tt_timer.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __RSLVR_RECONN_DELAY (30 * 1000) // 30s

#define __RSLVR_INIT_BUF_SIZE 1000

#define __RSLVR_DATA_MIN_EXPAND_ORDER 10 // 1k
#define __RSLVR_DATA_MAX_EXPAND_ORDER 11 // 2k
#define __RSLVR_DATA_MAX_SIZE_ORDER 12 // 4k

#define TT_ASSERT_NS TT_ASSERT

#define __RSLVR_TMR_EV_RECONNECT 1
#define __RSLVR_TMR_EV_RECREATE 2

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

static tt_result_t __adr_udp_create(IN tt_adns_resolver_t *rslvr);
static void __adr_udp_destroy(IN tt_adns_resolver_t *rslvr);

static tt_result_t __adr_tcp_create(IN tt_adns_resolver_t *rslvr);
static void __adr_tcp_destroy(IN tt_adns_resolver_t *rslvr);

static void __adr_reconn(IN struct tt_tmr_s *timer,
                         IN void *param,
                         IN tt_u32_t reason);

static void __adr_skt_on_destroy(IN tt_skt_t *skt, IN void *cb_param);
static void __adr_skt_on_connect(IN tt_skt_t *skt,
                                 IN tt_sktaddr_t *remote_addr,
                                 IN tt_skt_aioctx_t *aioctx);
static void __adr_pkt_on_sendto(IN tt_skt_t *skt,
                                IN tt_blob_t *blob,
                                IN tt_u32_t blob_num,
                                IN tt_sktaddr_t *remote_addr,
                                IN tt_skt_aioctx_t *aioctx,
                                IN tt_u32_t send_len);
static void __adr_pkt_on_send(IN tt_skt_t *skt,
                              IN tt_blob_t *blob,
                              IN tt_u32_t blob_num,
                              IN tt_skt_aioctx_t *aioctx,
                              IN tt_u32_t send_len);
static void __adr_on_recvfrom(IN tt_skt_t *skt,
                              IN tt_blob_t *blob,
                              IN tt_u32_t blob_num,
                              IN tt_skt_aioctx_t *aioctx,
                              IN tt_u32_t recv_len,
                              IN tt_sktaddr_t *remote_addr);
static void __adr_on_recv(IN tt_skt_t *skt,
                          IN tt_blob_t *blob,
                          IN tt_u32_t blob_num,
                          IN tt_skt_aioctx_t *aioctx,
                          IN tt_u32_t recv_len);

static tt_result_t __adr_pkt_handler(IN tt_adns_resolver_t *rslvr);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_adns_resolver_create(IN tt_adns_resolver_t *rslvr,
                                    IN struct tt_adns_dmgr_s *dmgr,
                                    IN tt_netaddr_t *ns_addr,
                                    IN OPT tt_adns_resolver_attr_t *attr)
{
    tt_u32_t __done = 0;
#define __NC_BUF (1 << 0)
#define __NC_UDP (1 << 1)
#define __NC_TCP (1 << 2)

    TT_ASSERT(rslvr != NULL);
    TT_ASSERT(dmgr != NULL);
    TT_ASSERT(ns_addr != NULL);

    if (attr == NULL) {
        tt_adns_resolver_attr_default(&rslvr->attr);
    } else {
        tt_memcpy(&rslvr->attr, attr, sizeof(tt_adns_resolver_attr_t));
    }
    attr = &rslvr->attr;

    tt_memcpy(&rslvr->ns_addr, ns_addr, sizeof(tt_netaddr_t));
    ns_addr = &rslvr->ns_addr;

    rslvr->dmgr = dmgr;

    if (!TT_OK(tt_buf_create(&rslvr->data_buf,
                             attr->init_buf_size,
                             &attr->buf_attr))) {
        TT_ERROR("fail to create rslvr data_buf");
        goto __nc_fail;
    }
    __done |= __NC_BUF;

    rslvr->reconn_tmr = NULL;

    // socket & timer
    if (ns_addr->protocol == TT_NET_PROTO_UDP) {
        if (!TT_OK(__adr_udp_create(rslvr))) {
            TT_ERROR("fail to create rslvr udp socket");
            goto __nc_fail;
        }
        __done |= __NC_UDP;
    } else {
        TT_ASSERT(ns_addr->protocol == TT_NET_PROTO_TCP);

        if (!TT_OK(__adr_tcp_create(rslvr))) {
            TT_ERROR("fail to create rslvr tcp socket");
            goto __nc_fail;
        }
        __done |= __NC_TCP;
    }

    return TT_SUCCESS;

__nc_fail:

    if (__done & __NC_TCP) {
        __adr_tcp_destroy(rslvr);
    }

    if (__done & __NC_UDP) {
        __adr_udp_destroy(rslvr);
    }

    if (__done & __NC_BUF) {
        tt_buf_destroy(&rslvr->data_buf);
    }

    return TT_FAIL;
}

void tt_adns_resolver_destroy(IN tt_adns_resolver_t *rslvr)
{
    TT_ASSERT(rslvr != NULL);

    rslvr->state = TT_ADRSLVR_DISCONNECTED;
    if (rslvr->ns_addr.protocol == TT_NET_PROTO_UDP) {
        __adr_udp_destroy(rslvr);
    } else {
        __adr_tcp_destroy(rslvr);
    }

    tt_buf_destroy(&rslvr->data_buf);
}

void tt_adns_resolver_attr_default(IN tt_adns_resolver_attr_t *attr)
{
    attr->reconn_delay = __RSLVR_RECONN_DELAY;

    attr->init_buf_size = __RSLVR_INIT_BUF_SIZE;

    tt_buf_attr_default(&attr->buf_attr);
    attr->buf_attr.min_expand_order = __RSLVR_DATA_MIN_EXPAND_ORDER;
    attr->buf_attr.max_expand_order = __RSLVR_DATA_MAX_EXPAND_ORDER;
    attr->buf_attr.max_size_order = __RSLVR_DATA_MAX_SIZE_ORDER;

    tt_memset(&attr->local_addr, 0, sizeof(tt_sktaddr_t));
    attr->has_local_addr = TT_FALSE;
}

void tt_adns_resolver_send(IN tt_adns_resolver_t *rslvr,
                           IN struct tt_adns_pkt_s *pkt)
{
    tt_blob_t blob = {0};

    TT_ASSERT(rslvr != NULL);
    TT_ASSERT(pkt != NULL);

    tt_buf_getptr_rpblob(&pkt->buf, &blob);
    if (rslvr->ns_addr.protocol == TT_NET_PROTO_UDP) {
        tt_adns_pkt_ref(pkt);
        if (!TT_OK(tt_skt_sendto_async(&rslvr->skt,
                                       &blob,
                                       1,
                                       &rslvr->ns_addr.sktaddr,
                                       __adr_pkt_on_sendto,
                                       pkt))) {
            tt_adns_pkt_release(pkt);
        }
    } else {
        TT_ASSERT(rslvr->ns_addr.protocol == TT_NET_PROTO_TCP);

        tt_adns_pkt_ref(pkt);
        if (!TT_OK(tt_skt_send_async(&rslvr->skt,
                                     &blob,
                                     1,
                                     __adr_pkt_on_send,
                                     pkt))) {
            tt_adns_pkt_release(pkt);
        }
    }
}

tt_result_t __adr_udp_create(IN tt_adns_resolver_t *rslvr)
{
    tt_skt_attr_t skt_attr;
    tt_skt_exit_t skt_exit;
    tt_blob_t blob = {0};

    tt_net_family_t af = tt_sktaddr_get_family(&rslvr->ns_addr.sktaddr);
    tt_net_protocol_t protocol = rslvr->ns_addr.protocol;
    tt_sktaddr_t *local_addr = &rslvr->attr.local_addr;

    tt_u32_t __done = 0;
#define __NUC_SKT (1 << 0)

    TT_ASSERT(protocol == TT_NET_PROTO_UDP);

    tt_skt_attr_default(&skt_attr);

    if (!rslvr->attr.has_local_addr) {
        tt_sktaddr_init_any(local_addr, af);
    }

    skt_exit.on_destroy = __adr_skt_on_destroy;
    skt_exit.cb_param = rslvr;

    if (!TT_OK(tt_udp_server_async(&rslvr->skt,
                                   af,
                                   &skt_attr,
                                   local_addr,
                                   &skt_exit))) {
        TT_ERROR("fail to create rslvr udp socket");
        goto __nuc_fail;
    }
    __done |= __NUC_SKT;

    // the udp socket can be used directly
    rslvr->state = TT_ADRSLVR_CONNECTED;

    // start receiving
    tt_buf_getptr_wpblob(&rslvr->data_buf, &blob);
    if (!TT_OK(tt_skt_recvfrom_async(&rslvr->skt,
                                     &blob,
                                     1,
                                     __adr_on_recvfrom,
                                     rslvr))) {
        TT_ERROR("fail to start recvfrom");
        goto __nuc_fail;
    }

    return TT_SUCCESS;

__nuc_fail:

    if (__done & __NUC_SKT) {
        tt_async_skt_destroy(&rslvr->skt, TT_TRUE);
    }

    return TT_FAIL;
}

void __adr_udp_destroy(IN tt_adns_resolver_t *rslvr)
{
    TT_ASSERT(rslvr->reconn_tmr == NULL);

    tt_async_skt_destroy(&rslvr->skt, TT_TRUE);
}

tt_result_t __adr_tcp_create(IN tt_adns_resolver_t *rslvr)
{
    tt_result_t result;
    tt_netaddr_t *ns_addr = &rslvr->ns_addr;
    tt_skt_attr_t skt_attr;
    tt_skt_exit_t skt_exit;

    tt_u32_t __done = 0;
#define __NTC_SKT (1 << 0)
#define __NTC_TMR (1 << 1)

    TT_ASSERT(ns_addr->protocol == TT_NET_PROTO_TCP);

    tt_skt_attr_default(&skt_attr);

    skt_exit.on_destroy = __adr_skt_on_destroy;
    skt_exit.cb_param = rslvr;

    result = tt_async_skt_create(&rslvr->skt,
                                 tt_sktaddr_get_family(&ns_addr->sktaddr),
                                 ns_addr->protocol,
                                 TT_SKT_ROLE_TCP_CONNECT,
                                 &skt_attr,
                                 &skt_exit);
    if (!TT_OK(result)) {
        TT_ERROR("fail to create rslvr tcp socket");
        goto __ntc_fail;
    }
    __done |= __NTC_SKT;

    if (rslvr->reconn_tmr == NULL) {
        rslvr->reconn_tmr = tt_tmr_create(NULL,
                                          rslvr->attr.reconn_delay,
                                          rslvr,
                                          __adr_reconn,
                                          (void *)__RSLVR_TMR_EV_RECONNECT,
                                          0);
        if (rslvr->reconn_tmr == NULL) {
            TT_ERROR("fail to create rslvr reconn timer");
            goto __ntc_fail;
        }
        __done |= __NTC_TMR;
    } else {
        // stop timer, waiting for async connect result
        tt_tmr_stop(rslvr->reconn_tmr);
    }

    rslvr->state = TT_ADRSLVR_CONNECTING;

    // start connecting
    result = tt_skt_connect_async(&rslvr->skt,
                                  &rslvr->ns_addr.sktaddr,
                                  __adr_skt_on_connect,
                                  rslvr);
    if (!TT_OK(result)) {
        TT_ERROR("fail to start connect");
        goto __ntc_fail;
    }

    return TT_SUCCESS;

__ntc_fail:

    if (__done & __NTC_TMR) {
        tt_tmr_destroy(rslvr->reconn_tmr);
        rslvr->reconn_tmr = NULL;
    }

    if (__done & __NTC_SKT) {
        tt_async_skt_destroy(&rslvr->skt, TT_TRUE);
    }

    return TT_FAIL;
}

void __adr_tcp_destroy(IN tt_adns_resolver_t *rslvr)
{
    tt_tmr_destroy(rslvr->reconn_tmr);
    rslvr->reconn_tmr = NULL;

    tt_async_skt_destroy(&rslvr->skt, TT_TRUE);
}

void __adr_skt_on_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
    tt_adns_resolver_t *rslvr = (tt_adns_resolver_t *)cb_param;
    tt_netaddr_t *ns_addr = &rslvr->ns_addr;

    if (rslvr->state == TT_ADRSLVR_DISCONNECTED) {
        return;
    }

    if (ns_addr->protocol == TT_NET_PROTO_UDP) {
        rslvr->state = TT_ADRSLVR_CONNECTING;
    } else {
        tt_tmr_t *reconn_tmr = rslvr->reconn_tmr;

        rslvr->state = TT_ADRSLVR_CONNECTING;

        // this happen only when connection of rslvr is down, so
        // rslvr is still working and we can access its member

        TT_ASSERT(reconn_tmr != NULL);
        tt_tmr_set_delay(reconn_tmr, 0);
        tt_tmr_set_cbparam(reconn_tmr, (void *)__RSLVR_TMR_EV_RECREATE);
        tt_tmr_start(reconn_tmr);
    }
}

void __adr_skt_on_connect(IN tt_skt_t *skt,
                          IN tt_sktaddr_t *remote_addr,
                          IN tt_skt_aioctx_t *aioctx)
{
    tt_adns_resolver_t *rslvr = (tt_adns_resolver_t *)aioctx->cb_param;
    tt_blob_t blob = {0};

    if (!TT_OK(aioctx->result)) {
        if (aioctx->result != TT_CANCELLED) {
            rslvr->state = TT_ADRSLVR_CONNECTING;
            tt_tmr_start(rslvr->reconn_tmr);
        }
        // else TT_CANCELLED means skt is being destroyed and it should
        // not retry connecting
        return;
    }

    rslvr->state = TT_ADRSLVR_CONNECTED;

    tt_buf_getptr_wpblob(&rslvr->data_buf, &blob);
    tt_skt_recv_async(&rslvr->skt, &blob, 1, __adr_on_recv, rslvr);
}

void __adr_reconn(IN struct tt_tmr_s *timer, IN void *param, IN tt_u32_t reason)
{
    tt_adns_resolver_t *rslvr = (tt_adns_resolver_t *)timer->opaque;
    tt_u32_t ev = (tt_u32_t)(tt_uintptr_t)param;

    if (ev == __RSLVR_TMR_EV_RECONNECT) {
        tt_skt_connect_async(&rslvr->skt,
                             &rslvr->ns_addr.sktaddr,
                             __adr_skt_on_connect,
                             rslvr);
    } else if (ev == __RSLVR_TMR_EV_RECREATE) {
        tt_tmr_destroy(rslvr->reconn_tmr);
        rslvr->reconn_tmr = NULL;
        // skt has already been destroyed
        if (!TT_OK(__adr_tcp_create(rslvr))) {
            TT_FATAL("fail to recreated rslvr tcp socket");
        }
    } else {
        TT_FATAL("unknown rslvr timer event: %d", ev);
    }
}

void __adr_pkt_on_sendto(IN tt_skt_t *skt,
                         IN tt_blob_t *blob,
                         IN tt_u32_t blob_num,
                         IN tt_sktaddr_t *remote_addr,
                         IN tt_skt_aioctx_t *aioctx,
                         IN tt_u32_t send_len)
{
    tt_adns_pkt_release((tt_adns_pkt_t *)aioctx->cb_param);
}

void __adr_pkt_on_send(IN tt_skt_t *skt,
                       IN tt_blob_t *blob,
                       IN tt_u32_t blob_num,
                       IN tt_skt_aioctx_t *aioctx,
                       IN tt_u32_t send_len)
{
    tt_adns_pkt_release((tt_adns_pkt_t *)aioctx->cb_param);
}

void __adr_on_recvfrom(IN tt_skt_t *skt,
                       IN tt_blob_t *blob,
                       IN tt_u32_t blob_num,
                       IN tt_skt_aioctx_t *aioctx,
                       IN tt_u32_t recv_len,
                       IN tt_sktaddr_t *remote_addr)
{
    tt_adns_resolver_t *rslvr = (tt_adns_resolver_t *)aioctx->cb_param;
    tt_buf_t *data_buf = &rslvr->data_buf;
    tt_result_t result;
    tt_blob_t b;

    if (!TT_OK(aioctx->result)) {
        if (aioctx->result == TT_END) {
            // server closed the connection
            tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_RDWR);
        } else if (aioctx->result == TT_CANCELLED) {
            // do nothing
        } else {
            TT_FATAL("adns stops receiving udp data");
        }
        return;
    }

    tt_buf_inc_wp(data_buf, recv_len);

    result = __adr_pkt_handler(rslvr);
    if (TT_OK(result)) {
        // it should have handled all dns packets when returing TT_SUCCESS,
        // so reset the recv buffer
        if (TT_BUF_RLEN(data_buf) > 0) {
            TT_WARN("%d bytes are discarded", TT_BUF_RLEN(data_buf));
        }
        tt_buf_reset_rwp(data_buf);
    } else if (result == TT_BUFFER_INCOMPLETE) {
        // the left data in the buffer belongs to a incomplete dns packet
        // so we should try to make more space in buffer by either refining
        // or expanding

        if (TT_BUF_REFINABLE(data_buf) > 600) {
            tt_buf_refine(data_buf);
        }

        if ((TT_BUF_WLEN(data_buf) < 600) &&
            !TT_OK(tt_buf_reserve(data_buf, 600))) {
            TT_FATAL("fail to reserver more space, resolver may not work");
        }
    } else {
        // discard all data in recv buffer as it has no way to distingusith
        // where the next packet is
        if (TT_BUF_RLEN(data_buf) > 0) {
            TT_WARN("%d bytes are discarded due to error",
                    TT_BUF_RLEN(data_buf));
        }
        tt_buf_reset_rwp(data_buf);
    }

    tt_buf_getptr_wpblob(data_buf, &b);
    tt_skt_recvfrom_async(&rslvr->skt, &b, 1, __adr_on_recvfrom, rslvr);
}

void __adr_on_recv(IN tt_skt_t *skt,
                   IN tt_blob_t *blob,
                   IN tt_u32_t blob_num,
                   IN tt_skt_aioctx_t *aioctx,
                   IN tt_u32_t recv_len)
{
    tt_adns_resolver_t *rslvr = (tt_adns_resolver_t *)aioctx->cb_param;
    tt_buf_t *data_buf = &rslvr->data_buf;
    tt_result_t result;
    tt_blob_t b;

    if (!TT_OK(aioctx->result)) {
        if (aioctx->result == TT_END) {
            // server closed the connection
            tt_async_skt_shutdown(skt, TT_SKT_SHUTDOWN_RDWR);
        } else if (aioctx->result == TT_CANCELLED) {
            // do nothing
        } else {
            TT_FATAL("adns stops receiving udp data");
        }
        return;
    }

    tt_buf_inc_wp(data_buf, recv_len);

    result = __adr_pkt_handler(rslvr);
    if (TT_OK(result)) {
        // it should have handled all dns packets when returing TT_SUCCESS,
        // so reset the recv buffer
        if (TT_BUF_RLEN(data_buf) > 0) {
            TT_WARN("%d bytes are discarded", TT_BUF_RLEN(data_buf));
        }
        tt_buf_reset_rwp(data_buf);
    } else if (result == TT_BUFFER_INCOMPLETE) {
        // the left data in the buffer belongs to a incomplete dns packet
        // so we should try to make more space in buffer by either refining
        // or expanding

        if (TT_BUF_REFINABLE(data_buf) > 600) {
            tt_buf_refine(data_buf);
        }

        if ((TT_BUF_WLEN(data_buf) < 600) &&
            !TT_OK(tt_buf_reserve(data_buf, 600))) {
            TT_FATAL("fail to reserver more space, resolver may not work");
        }
    } else {
        // discard all data in recv buffer as it has no way to distingusith
        // where the next packet is
        if (TT_BUF_RLEN(data_buf) > 0) {
            TT_WARN("%d bytes are discarded due to error",
                    TT_BUF_RLEN(data_buf));
        }
        tt_buf_reset_rwp(data_buf);
    }

    tt_buf_getptr_wpblob(data_buf, &b);
    tt_skt_recv_async(&rslvr->skt, &b, 1, __adr_on_recv, rslvr);
}

tt_result_t __adr_pkt_handler(IN tt_adns_resolver_t *rslvr)
{
    tt_buf_t *data_buf = &rslvr->data_buf;
    tt_u32_t parse_flag = 0;

    parse_flag |= TT_ADNS_PKT_PARSE_ANSWER;
    // parse_flag |= TT_ADNS_PKT_PARSE_ADDITIONAL;

    while (TT_BUF_RLEN(data_buf) > 0) {
        tt_u32_t bak_rp, bak_wp;
        tt_u32_t next_rp, next_wp;
        tt_result_t result;
        tt_adns_pkt_t *pkt;

        TT_BUF_CHECK(data_buf);

        // this is a typical case to parse a packet:
        //  - parse_prepare should do:
        //    - return success and update rd pos when it found a complete pkt;
        //    - return incomplete if not found complete pkt but no error;
        //    - return error;
        //  - if parse_prepare return success, then record data_buf rd pos and
        //    continue parsing packet; if parse_prepare return incomplete,
        //    then return; otherwise, discard all data and return;
        //    note data_buf rd pos does not change whatever parse_prepare return
        //  - parse_pkt should do:
        //    - return pkt and update rd pos when it parsed a complete pkt;
        //    - return error;
        //  - if parse_pkt succeed, update rd_pos and continue;otherwise,
        //    update rd_pos and continue; so whatever it returns, rd pos
        //    would be set to beginning address of next pkt

        tt_buf_backup_rwp(data_buf, &bak_rp, &bak_wp);
        result = tt_adns_pkt_parse_prepare(data_buf);
        tt_buf_backup_rwp(data_buf, &next_rp, &next_wp);
        tt_buf_restore_rwp(data_buf, &bak_rp, &bak_wp);

        if (TT_OK(result)) {
            pkt = tt_adns_pkt_parse(data_buf, parse_flag);
            if (pkt != NULL) {
                tt_adns_dmgr_pkt_handler(rslvr->dmgr, pkt);
                tt_adns_pkt_release(pkt);
            }
            tt_buf_restore_rwp(data_buf, &next_rp, &next_wp);
        } else if (result == TT_BUFFER_INCOMPLETE) {
            return TT_BUFFER_INCOMPLETE;
        } else {
            tt_buf_reset_rwp(data_buf);
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}
