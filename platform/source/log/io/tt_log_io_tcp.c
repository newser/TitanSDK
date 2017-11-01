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

#include <log/io/tt_log_io_tcp.h>

#include <log/io/tt_log_io.h>
#include <os/tt_thread.h>

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

static void __lio_tcp_destroy(IN tt_logio_t *lio);

static void __lio_tcp_output(IN tt_logio_t *lio,
                             IN const tt_char_t *data,
                             IN tt_u32_t data_len);

static tt_logio_itf_t tt_s_logio_tcp_itf = {
    TT_LOGIO_TCP,

    NULL,
    __lio_tcp_destroy,
    __lio_tcp_output,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_logio_t *tt_logio_tcp_create(IN tt_net_family_t family,
                                IN tt_sktaddr_t *addr,
                                IN OPT tt_logio_tcp_attr_t *attr)
{
    tt_logio_tcp_attr_t __attr;
    tt_skt_t *skt;
    tt_logio_t *lio;

    if (attr == NULL) {
        tt_logio_tcp_attr_default(&__attr);
        attr = &__attr;
    }

    skt = tt_skt_create(family, TT_NET_PROTO_TCP, &attr->skt_attr);
    if (skt == NULL) {
        return NULL;
    }

    lio = tt_logio_tcp_create_skt(skt, addr);
    if (lio == NULL) {
        tt_skt_destroy(skt);
        return NULL;
    }

    return lio;
}

tt_logio_t *tt_logio_tcp_create_skt(IN TO tt_skt_t *skt, IN tt_sktaddr_t *addr)
{
    tt_logio_t *lio;
    tt_logio_tcp_t *lio_tcp;

    lio = tt_logio_create(sizeof(tt_logio_tcp_t), &tt_s_logio_tcp_itf);
    if (lio == NULL) {
        return NULL;
    }

    lio_tcp = TT_LOGIO_CAST(lio, tt_logio_tcp_t);

    lio_tcp->skt = skt;
    tt_memcpy(&lio_tcp->addr, addr, sizeof(tt_sktaddr_t));
    lio_tcp->state = __LC_INIT;

    return lio;
}

void tt_logio_tcp_attr_default(IN tt_logio_tcp_attr_t *attr)
{
    tt_skt_attr_default(&attr->skt_attr);
}

void __lio_tcp_destroy(IN tt_logio_t *lio)
{
    tt_logio_tcp_t *lio_tcp = TT_LOGIO_CAST(lio, tt_logio_tcp_t);

    // directly destroy it, no shutdown then receive then destroy as we
    // do not want to spend too much time on destroying a lio_tcp
    tt_skt_destroy(lio_tcp->skt);
}

void __lio_tcp_output(IN tt_logio_t *lio,
                      IN const tt_char_t *data,
                      IN tt_u32_t data_len)
{
    tt_logio_tcp_t *lio_tcp = TT_LOGIO_CAST(lio, tt_logio_tcp_t);
    tt_thread_t *t = tt_current_thread();
    tt_thread_log_t l;

    l = tt_thread_set_log(t, TT_THREAD_LOG_PRINTF);

    if (lio_tcp->state == __LC_INIT) {
        if (TT_OK(tt_skt_connect(lio_tcp->skt, &lio_tcp->addr))) {
            lio_tcp->state = __LC_CONNECTED;
        } else {
            TT_ERROR("connect failed, tcp log: %s", data);
            lio_tcp->state = __LC_DISCONNECTED;
        }
    }

    if (lio_tcp->state == __LC_CONNECTED) {
        if (!TT_OK(tt_skt_send_all(lio_tcp->skt, (tt_u8_t *)data, data_len))) {
            TT_ERROR("send failed, tcp log: %s", data);
            lio_tcp->state = __LC_DISCONNECTED;
        }
    } else {
        TT_ERROR("already disconnected, tcp log: %s", data);
    }

    tt_thread_set_log(t, l);
}
