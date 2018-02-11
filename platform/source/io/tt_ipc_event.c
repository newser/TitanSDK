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

#include <io/tt_ipc_event.h>

#include <io/tt_ipc.h>
#include <io/tt_socket.h>
#include <memory/tt_memory_alloc.h>
#include <os/tt_fiber_event.h>
#include <time/tt_timer.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#if 1
#define TT_ASSERT_IPC TT_ASSERT
#else
#define TT_ASSERT_IPC(...)
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

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_ipc_ev_t *__parse_ipc_ev(IN tt_buf_t *buf);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_ipc_ev_t *tt_ipc_ev_create(IN tt_u32_t ev, IN tt_u32_t size)
{
    tt_ipc_ev_t *pev;

    pev = tt_malloc(sizeof(tt_ipc_ev_t) + size);
    if (pev != NULL) {
        pev->ev = ev;
        pev->size = size;
    }

    return pev;
}

void tt_ipc_ev_destroy(IN tt_ipc_ev_t *pev)
{
    tt_free(pev);
}

tt_result_t tt_ipc_send_ev(IN tt_ipc_t *dst, IN tt_ipc_ev_t *pev)
{
    tt_u32_t n, len, sent;

    n = 0;
    len = (tt_u32_t)sizeof(tt_ipc_ev_t) + pev->size;
    while (TT_OK(tt_ipc_send_ntv(&dst->sys_ipc,
                                 TT_PTR_INC(tt_u8_t, pev, n),
                                 len - n,
                                 &sent)) &&
           ((n += sent) < len)) {
    }
    tt_free(pev);

    TT_ASSERT(n <= len);
    if (n == len) {
        return TT_SUCCESS;
    } else {
        TT_FATAL("ipc data flow may be inconsistent");
        return TT_FAIL;
    }
}

tt_result_t tt_ipc_recv_ev(IN tt_ipc_t *ipc,
                           OUT tt_ipc_ev_t **p_pev,
                           OUT tt_fiber_ev_t **p_fev,
                           OUT tt_tmr_t **p_tmr,
                           OUT tt_skt_t **p_skt)
{
    tt_buf_t *buf = &ipc->buf;
    tt_u8_t *p;
    tt_u32_t len, recvd;
    tt_result_t result;

    *p_pev = NULL;
    *p_fev = NULL;
    *p_tmr = NULL;
    *p_skt = NULL;

    // - must first try parsing an ev, as all data may alreay arrive
    // - should keep parsing if tt_ipc_handle_internal_ev() fail, as
    //   the it must be an internal event and should not return to caller
    while (((*p_pev = __parse_ipc_ev(buf)) != NULL) &&
           TT_OK(tt_ipc_handle_internal_ev(p_pev, p_skt))) {
        return TT_SUCCESS;
    }

    // ev may be parsed in last call and buf has available space in front
    tt_buf_try_refine(buf, 1 << 10);

    if ((TT_BUF_WLEN(buf) == 0) && !TT_OK(tt_buf_extend(buf))) {
        TT_ERROR("ipc buffer is full");
        return TT_FAIL;
    }
    tt_buf_get_wptr(buf, &p, &len);
    while (
        TT_OK(result = tt_ipc_recv(ipc, p, len, &recvd, p_fev, p_tmr, p_skt))) {
        if (recvd != 0) {
            tt_buf_inc_wp(buf, recvd);
            if (((*p_pev = __parse_ipc_ev(buf)) != NULL) &&
                TT_OK(tt_ipc_handle_internal_ev(p_pev, p_skt))) {
                // p_fev and p_tmr may already be set
                return TT_SUCCESS;
            }
        }

        if ((*p_fev != NULL) || (*p_tmr != NULL) || (*p_skt != NULL)) {
            return TT_SUCCESS;
        }

        // only received some data but not enough, so keep receiving
        if ((TT_BUF_WLEN(buf) == 0) && !TT_OK(tt_buf_extend(buf))) {
            TT_ERROR("ipc buffer is full");
            return TT_FAIL;
        }
        tt_buf_get_wptr(buf, &p, &len);
    }

    if (result != TT_E_END) {
        TT_ERROR("ipc data may be broken");
    }
    return result;
}

tt_result_t tt_ipc_send_skt(IN tt_ipc_t *ipc, IN TO tt_skt_t *skt)
{
    tt_result_t result = tt_ipc_send_skt_ntv(&ipc->sys_ipc, skt);
    if (TT_OK(result)) {
        tt_skt_destroy(skt);
        return TT_SUCCESS;
    } else {
        return result;
    }
}

tt_ipc_ev_t *__parse_ipc_ev(IN tt_buf_t *buf)
{
    tt_u8_t *p;
    tt_u32_t len;
    tt_ipc_ev_t *pev;

    tt_buf_get_rptr(buf, &p, &len);
    if (len < sizeof(tt_ipc_ev_t)) {
        return NULL;
    }

    pev = (tt_ipc_ev_t *)p;
    if (len < (sizeof(tt_ipc_ev_t) + pev->size)) {
        return NULL;
    }

    tt_buf_inc_rp(buf, (sizeof(tt_ipc_ev_t) + pev->size));
    return pev;
}
