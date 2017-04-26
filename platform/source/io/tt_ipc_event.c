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

#include <io/tt_ipc_event.h>

#include <io/tt_ipc.h>
#include <memory/tt_memory_alloc.h>
#include <os/tt_fiber_event.h>

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
    return tt_ipc_send(dst,
                       (tt_u8_t *)pev,
                       sizeof(tt_ipc_ev_t) + pev->size,
                       NULL);
}

tt_result_t tt_ipc_recv_ev(IN tt_ipc_t *ipc,
                           OUT tt_ipc_ev_t **p_pev,
                           OUT OPT tt_fiber_ev_t **p_fev)
{
    tt_buf_t *buf = &ipc->buf;
    tt_u8_t *p;
    tt_u32_t len, recvd;
    tt_result_t result;

    *p_pev = NULL;
    TT_SAFE_ASSIGN(p_fev, NULL);

    // must first try parsing an ev, as all data may alreay arrive
    if ((p_pev != NULL) && ((*p_pev = __parse_ipc_ev(buf)) != NULL)) {
        return TT_SUCCESS;
    }

    // ev may be parsed in last call and buf has available space in front
    tt_buf_try_refine(buf, 1 << 10);

    if ((TT_BUF_WLEN(buf) == 0) && !TT_OK(tt_buf_extend(buf))) {
        TT_ERROR("ipc buffer is full");
        return TT_FAIL;
    }
    tt_buf_get_wptr(buf, &p, &len);
    while (TT_OK(result = tt_ipc_recv(ipc, p, len, &recvd, p_fev, NULL))) {
        tt_buf_inc_wp(buf, recvd);

        if ((p_pev != NULL) && ((*p_pev = __parse_ipc_ev(buf)) != NULL)) {
            return TT_SUCCESS;
        }

        if ((p_fev != NULL) && (*p_fev != NULL)) {
            return TT_SUCCESS;
        }

        if ((TT_BUF_WLEN(buf) == 0) && !TT_OK(tt_buf_extend(buf))) {
            TT_ERROR("ipc buffer is full");
            return TT_FAIL;
        }
        tt_buf_get_wptr(buf, &p, &len);
    }

    if (result != TT_END) {
        TT_ERROR("ipc data may be broken");
    }
    return result;
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
