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

void tt_fiber_send(IN tt_fiber_t *dst, IN tt_fiber_ev_t *fev, IN tt_bool_t wait)
{
    TT_ASSERT_FEV(tt_current_fiber()->fs == dst->fs);

    tt_dlist_push_tail(&dst->ev, &fev->node);
    if (dst->recving) {
        tt_fiber_resume(dst, wait);
    } else if (wait) {
        tt_fiber_suspend();
    }
}

tt_fiber_ev_t *tt_fiber_recv(IN tt_fiber_t *current, IN tt_bool_t wait)
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
    tt_dlist_push_tail(&dst->tmr, &tmr->node);
    if (dst->recving) {
        tt_fiber_resume(dst, TT_FALSE);
    }
}

tt_tmr_t *tt_fiber_recv_timer(IN tt_fiber_t *current, IN tt_bool_t wait)
{
    tt_dnode_t *node;

again:
    node = tt_dlist_pop_head(&current->tmr);
    if (node != NULL) {
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
