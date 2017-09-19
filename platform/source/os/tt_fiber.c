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

#include <os/tt_fiber.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <os/tt_task.h>
#include <os/tt_thread.h>
#include <time/tt_timer.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __FIBER_STACK_SIZE_DEF (1 << 17) // 128K

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

static tt_fiber_t *__fiber_create_main(IN tt_fiber_sched_t *fs);

static void __fiber_destroy_main(IN tt_fiber_t *fb);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_fiber_sched_t *tt_fiber_sched_create(IN OPT tt_fiber_sched_attr_t *attr)
{
    tt_fiber_sched_attr_t __attr;
    tt_fiber_sched_t *fs;
    tt_fiber_t *__main;

    if (attr == NULL) {
        tt_fiber_sched_attr_default(&__attr);
        attr = &__attr;
    }

    fs = tt_malloc(sizeof(tt_fiber_sched_t));
    if (fs == NULL) {
        TT_ERROR("no mem for new fiber sched");
        return NULL;
    }

    tt_list_init(&fs->active);
    tt_list_init(&fs->pending);

    __main = __fiber_create_main(fs);
    if (__main == NULL) {
        tt_free(fs);
        return NULL;
    }
    fs->__main = __main;

    fs->current = __main;
    fs->thread = NULL;

    return fs;
}

void tt_fiber_sched_destroy(IN tt_fiber_sched_t *fs)
{
    tt_lnode_t *node;

    TT_ASSERT(fs != NULL);

    while ((node = tt_list_pop_head(&fs->active)) != NULL) {
        tt_fiber_destroy(TT_CONTAINER(node, tt_fiber_t, node));
    }

    while ((node = tt_list_pop_head(&fs->pending)) != NULL) {
        tt_fiber_destroy(TT_CONTAINER(node, tt_fiber_t, node));
    }

    __fiber_destroy_main(fs->__main);

    tt_free(fs);
}

void tt_fiber_sched_attr_default(IN tt_fiber_sched_attr_t *attr)
{
    TT_ASSERT(attr != NULL);
}

tt_fiber_t *tt_fiber_sched_find(IN tt_fiber_sched_t *fs,
                                IN const tt_char_t *name)
{
    tt_lnode_t *node;

    TT_ASSERT(fs != NULL);
    TT_ASSERT(name != NULL);

    node = tt_list_head(&fs->active);
    while (node != NULL) {
        tt_fiber_t *fb = TT_CONTAINER(node, tt_fiber_t, node);
        if ((fb->name != NULL) && (tt_strcmp(fb->name, name) == 0)) {
            return fb;
        }

        node = node->next;
    }

    node = tt_list_head(&fs->pending);
    while (node != NULL) {
        tt_fiber_t *fb = TT_CONTAINER(node, tt_fiber_t, node);
        if ((fb->name != NULL) && (tt_strcmp(fb->name, name) == 0)) {
            return fb;
        }

        node = node->next;
    }

    return NULL;
}

tt_fiber_t *tt_fiber_sched_next(IN tt_fiber_sched_t *fs)
{
    tt_lnode_t *node = tt_list_head(&fs->active);
    if (node != NULL) {
        return TT_CONTAINER(node, tt_fiber_t, node);
    } else {
        return fs->__main;
    }
}

tt_fiber_t *tt_fiber_create(IN OPT const tt_char_t *name,
                            IN tt_fiber_routine_t routine,
                            IN void *param,
                            IN OPT tt_fiber_attr_t *attr)
{
    tt_fiber_sched_t *fs;
    tt_fiber_attr_t __attr;
    tt_fiber_t *fb;

    TT_ASSERT(routine != NULL);

    fs = tt_current_fiber_sched();
    if (fs == NULL) {
        TT_ERROR("no fiber scheduler");
        return NULL;
    }

    if ((name != NULL) && (tt_fiber_sched_find(fs, name) != NULL)) {
        TT_ERROR("duplicated fiber name: %s", name);
        return NULL;
    }

    if (attr == NULL) {
        tt_fiber_attr_default(&__attr);
        attr = &__attr;
    }

    fb = tt_malloc(sizeof(tt_fiber_t));
    if (fb == NULL) {
        TT_ERROR("no mem for new fiber");
        return NULL;
    }

    tt_lnode_init(&fb->node);
    fb->name = name;
    fb->routine = routine;
    fb->param = param;
    fb->fs = fs;
    tt_dlist_init(&fb->ev);
    tt_list_init(&fb->unexpired_tmr);
    tt_list_init(&fb->expired_tmr);

    if (!TT_OK(tt_fiber_create_wrap(&fb->wrap_fb, attr->stack_size))) {
        tt_free(fb);
        return NULL;
    }

    fb->can_yield = TT_TRUE;
    fb->end = TT_FALSE;
    fb->recving = TT_FALSE;

    // put to pending list, will be scheduled later
    tt_list_push_tail(&fs->pending, &fb->node);

    return fb;
}

void tt_fiber_destroy(IN tt_fiber_t *fb)
{
    tt_dnode_t *node;
    tt_lnode_t *lnode;

    TT_ASSERT(fb != NULL);

    while ((node = tt_dlist_pop_head(&fb->ev)) != NULL) {
        // howto??
    }

    while ((lnode = tt_list_pop_head(&fb->unexpired_tmr)) != NULL) {
        tt_tmr_destroy(TT_CONTAINER(lnode, tt_tmr_t, node));
    }
    while ((lnode = tt_list_pop_head(&fb->expired_tmr)) != NULL) {
        tt_tmr_destroy(TT_CONTAINER(lnode, tt_tmr_t, node));
    }

    tt_fiber_destroy_wrap(&fb->wrap_fb);

    tt_free(fb);
}

void tt_fiber_attr_default(IN tt_fiber_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->stack_size = __FIBER_STACK_SIZE_DEF;
}

void tt_fiber_suspend()
{
    tt_fiber_sched_t *cfs = tt_current_fiber_sched();
    tt_fiber_t *cfb = cfs->current;
    tt_fiber_t *next;

    TT_ASSERT(cfb->can_yield);

    if (cfb != cfs->__main) {
        tt_list_remove(&cfb->node);
        tt_list_push_tail(&cfs->pending, &cfb->node);
    }

    next = tt_fiber_sched_next(cfs);

    if (next != cfb) {
        cfs->current = next;
        tt_fiber_switch_wrap(cfs, cfb, next);
        cfs->current = cfb;
    }
}

void tt_fiber_resume(IN tt_fiber_t *fb, IN tt_bool_t suspend)
{
    tt_fiber_sched_t *cfs = fb->fs;
    tt_fiber_t *cfb = cfs->current;

    if (cfb != fb) {
        if (cfb != cfs->__main) {
            tt_list_remove(&cfb->node);
            if (suspend) {
                tt_list_push_tail(&cfs->pending, &cfb->node);
            } else {
                tt_list_push_tail(&cfs->active, &cfb->node);
            }
        }

        if (fb != cfs->__main) {
            tt_list_remove(&fb->node);
            tt_list_push_head(&cfs->active, &fb->node);
        }

        cfs->current = fb;
        tt_fiber_switch_wrap(cfs, cfb, fb);
        cfs->current = cfb;
    }
}

tt_fiber_t *__fiber_create_main(IN tt_fiber_sched_t *fs)
{
    tt_fiber_t *fb;

    fb = tt_malloc(sizeof(tt_fiber_t));
    if (fb == NULL) {
        TT_ERROR("no mem for new fiber");
        return NULL;
    }

    if (!TT_OK(tt_fiber_create_local_wrap(&fb->wrap_fb))) {
        tt_free(fb);
        return NULL;
    }

    tt_lnode_init(&fb->node);
    fb->routine = NULL;
    fb->param = NULL;
    fb->fs = fs;
    fb->can_yield = TT_TRUE;
    fb->end = TT_FALSE;

    return fb;
}

void __fiber_destroy_main(IN tt_fiber_t *fb)
{
    tt_fiber_destroy_local_wrap(&fb->wrap_fb);

    tt_free(fb);
}
