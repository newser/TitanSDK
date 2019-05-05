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

#include <io/tt_io_worker_group.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <io/tt_io_event.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>

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

tt_iowg_t tt_g_fs_iowg;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __iowg_component_init(IN tt_component_t *comp,
                                         IN tt_profile_t *profile);

static void __iowg_component_exit(IN tt_component_t *comp);

static void __iowg_destroy_worker(IN tt_iowg_t *wg);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_iowg_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __iowg_component_init,
        __iowg_component_exit,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_IO_WORKER_GROUP, "IO Worker Group",
                      NULL, &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t tt_iowg_create(IN tt_iowg_t *wg, IN OPT tt_u32_t min_num,
                           IN OPT tt_u32_t max_num, IN OPT tt_iowg_attr_t *attr)
{
    tt_iowg_attr_t __attr;
    tt_u32_t i;

    tt_u32_t __done = 0;
#define __IOWG_MEM (1 << 0)
#define __IOWG_SEM (1 << 1)
#define __IOWG_LOCK (1 << 2)
#define __IOWG_WKR (1 << 2)

    TT_ASSERT(wg != NULL);
    TT_ASSERT(min_num <= max_num);

    if (attr == NULL) {
        tt_iowg_attr_default(&__attr);
        attr = &__attr;
    }

    tt_dlist_init(&wg->ev_list);

    wg->worker = tt_malloc(sizeof(tt_iowg_t) * max_num);
    if (wg->worker == NULL) {
        TT_ERROR("no mem for worker");
        goto fail;
    }
    __done |= __IOWG_MEM;

    if (!TT_OK(tt_sem_create(&wg->sem, 0, &attr->sem_attr))) {
        TT_ERROR("fail to create worker group sem");
        goto fail;
    }
    __done |= __IOWG_SEM;

    tt_memcpy(&wg->worker_attr, &attr->worker_attr,
              sizeof(tt_io_worker_attr_t));

    if (!TT_OK(tt_spinlock_create(&wg->lock, &attr->lock_attr))) {
        TT_ERROR("fail to create worker group lock");
        goto fail;
    }
    __done |= __IOWG_LOCK;

    wg->worker_num = min_num;
    wg->max_num = max_num;
    for (i = 0; i < max_num; ++i) { tt_io_worker_init(&wg->worker[i]); }
    __done |= __IOWG_WKR;

    for (i = 0; i < min_num; ++i) {
        if (!TT_OK(tt_io_worker_create(&wg->worker[i], wg, &wg->worker_attr))) {
            TT_ERROR("fail to create worker[%d]", i);
            goto fail;
        }
    }

    return TT_SUCCESS;

fail:

    if (__done & __IOWG_WKR) { __iowg_destroy_worker(wg); }

    if (__done & __IOWG_LOCK) { tt_spinlock_destroy(&wg->lock); }

    if (__done & __IOWG_SEM) { tt_sem_destroy(&wg->sem); }

    if (__done & __IOWG_MEM) { tt_free(wg->worker); }

    return TT_FAIL;
}

void tt_iowg_destroy(IN tt_iowg_t *wg)
{
    TT_ASSERT(wg != NULL);

    if (!tt_dlist_empty(&wg->ev_list)) {
        TT_FATAL("still event in worker group");
    }

    __iowg_destroy_worker(wg);
    tt_free(wg->worker);

    tt_sem_destroy(&wg->sem);

    tt_spinlock_destroy(&wg->lock);
}

void tt_iowg_attr_default(IN tt_iowg_attr_t *attr)
{
    tt_io_worker_attr_default(&attr->worker_attr);

    tt_sem_attr_default(&attr->sem_attr);

    tt_spinlock_attr_default(&attr->lock_attr);
}

tt_io_ev_t *tt_iowg_pop_ev(IN tt_iowg_t *wg)
{
    tt_dnode_t *node;

    tt_spinlock_acquire(&wg->lock);
    node = tt_dlist_pop_head(&wg->ev_list);
    tt_spinlock_release(&wg->lock);

    return TT_COND(node != NULL, TT_CONTAINER(node, tt_io_ev_t, node), NULL);
}

void tt_iowg_push_ev(IN tt_iowg_t *wg, IN tt_io_ev_t *ev)
{
    tt_bool_t i = wg->max_num;

    tt_spinlock_acquire(&wg->lock);
    tt_dlist_push_tail(&wg->ev_list, &ev->node);
    if ((wg->worker_num < wg->max_num) &&
        (tt_dlist_count(&wg->ev_list) > wg->worker_num)) {
        i = wg->worker_num++;
    }
    tt_spinlock_release(&wg->lock);

    tt_sem_release(&wg->sem);

    if ((i < wg->max_num) &&
        (!TT_OK(tt_io_worker_create(&wg->worker[i], wg, &wg->worker_attr)))) {
        TT_ERROR("fail to create worker[%d]", i);
        tt_io_worker_init(&wg->worker[i]);
        // the ith worker has no opportunity to be created
    }
}

tt_result_t __iowg_component_init(IN tt_component_t *comp,
                                  IN tt_profile_t *profile)
{
    if (!TT_OK(tt_iowg_create(&tt_g_fs_iowg, 0, tt_g_cpu_num, NULL))) {
        TT_ERROR("fail to create global io worker group");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void __iowg_component_exit(IN tt_component_t *comp)
{
    tt_iowg_destroy(&tt_g_fs_iowg);
}

void __iowg_destroy_worker(IN tt_iowg_t *wg)
{
    tt_u32_t i;

    for (i = 0; (i < wg->worker_num) && tt_io_worker_running(&wg->worker[i]);
         ++i) {
        tt_io_ev_t *ev = tt_xmalloc(sizeof(tt_io_ev_t));

        ev->src = NULL;
        ev->dst = NULL;
        tt_dnode_init(&ev->node);
        ev->io = TT_IO_WORKER;
        ev->ev = TT_IO_WORKER_EXIT;

        tt_iowg_push_ev(wg, ev);
    }

    for (i = 0; (i < wg->worker_num) && tt_io_worker_running(&wg->worker[i]);
         ++i) {
        tt_io_worker_destroy(&wg->worker[i]);
    }
}
