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

#include <os/tt_fiber_event.h>

#include <algorithm/tt_list.h>
#include <memory/tt_memory_alloc.h>
#include <os/tt_fiber.h>
#include <time/tt_timer.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#if 1
#define TT_ASSERT_FEV TT_ASSERT
#else
#define TT_ASSERT_FEV(...)
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

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_fiber_ev_init(IN tt_fiber_ev_t *fev, IN tt_u32_t ev)
{
    fev->src = tt_current_fiber();
    TT_ASSERT_FEV(fev->src != NULL);

    tt_dnode_init(&fev->node);

    fev->ev = ev;
}

tt_fiber_ev_t *tt_fiber_ev_create(IN tt_u32_t ev, IN tt_u32_t size)
{
    tt_fiber_ev_t *fev;

    fev = tt_malloc(sizeof(tt_fiber_ev_t) + size);
    if (fev != NULL) {
        fev->src = NULL;
        tt_dnode_init(&fev->node);
        fev->ev = ev;
    }

    return fev;
}

void tt_fiber_ev_destroy(IN tt_fiber_ev_t *fev)
{
    tt_free(fev);
}

void tt_fiber_send_ev(IN tt_fiber_t *dst,
                      IN tt_fiber_ev_t *fev,
                      IN tt_bool_t wait)
{
    TT_ASSERT_FEV(tt_current_fiber()->fs == dst->fs);

    tt_dlist_push_tail(&dst->ev, &fev->node);
    if (dst->recving) {
        tt_fiber_resume(dst, wait);
    } else if (wait) {
        tt_fiber_suspend();
    }
}

tt_fiber_ev_t *tt_fiber_recv_ev(IN tt_fiber_t *current, IN tt_bool_t wait)
{
    tt_dnode_t *node;

again:
    node = tt_dlist_pop_head(&current->ev);
    if (node != NULL) {
        return TT_CONTAINER(node, tt_fiber_ev_t, node);
    } else if (wait) {
        current->recving = TT_TRUE;
        tt_fiber_suspend();
        current->recving = TT_FALSE;
        goto again;
    } else {
        return NULL;
    }
}

void tt_fiber_finish(IN tt_fiber_ev_t *fev)
{
    if (fev->src != NULL) {
        tt_fiber_resume(fev->src, TT_FALSE);
    } else {
        tt_fiber_ev_destroy(fev);
    }
}

void tt_fiber_send_timer(IN tt_fiber_t *dst, IN tt_tmr_t *tmr)
{
    TT_ASSERT_FEV(tmr->node.lst == &dst->unexpired_tmr);
    tt_list_remove(&tmr->node);
    tt_list_push_tail(&dst->expired_tmr, &tmr->node);
    if (dst->recving) {
        tt_fiber_resume(dst, TT_FALSE);
    }
}

tt_tmr_t *tt_fiber_recv_timer(IN tt_fiber_t *current, IN tt_bool_t wait)
{
    tt_lnode_t *node;

again:
    node = tt_list_pop_head(&current->expired_tmr);
    if (node != NULL) {
        tt_list_push_tail(&current->unexpired_tmr, node);
        return TT_CONTAINER(node, tt_tmr_t, node);
    } else if (wait) {
        current->recving = TT_TRUE;
        tt_fiber_suspend();
        current->recving = TT_FALSE;
        goto again;
    } else {
        return NULL;
    }
}

tt_bool_t tt_fiber_recv(IN tt_fiber_t *current,
                        IN tt_bool_t wait,
                        OUT tt_fiber_ev_t **p_fev,
                        OUT tt_tmr_t **p_tmr)
{
    tt_dnode_t *fev_node;
    tt_lnode_t *tmr_node;
    tt_bool_t recvd = TT_FALSE;

again:
    fev_node = tt_dlist_pop_head(&current->ev);
    if (fev_node != NULL) {
        *p_fev = TT_CONTAINER(fev_node, tt_fiber_ev_t, node);
        recvd = TT_TRUE;
    } else {
        *p_fev = NULL;
    }

    tmr_node = tt_list_pop_head(&current->expired_tmr);
    if (tmr_node != NULL) {
        tt_list_push_tail(&current->unexpired_tmr, tmr_node);
        *p_tmr = TT_CONTAINER(tmr_node, tt_tmr_t, node);
        recvd = TT_TRUE;
    } else {
        *p_tmr = NULL;
    }

    if (recvd) {
        return TT_TRUE;
    } else if (wait) {
        current->recving = TT_TRUE;
        tt_fiber_suspend();
        current->recving = TT_FALSE;
        goto again;
    } else {
        return TT_FALSE;
    }
}
