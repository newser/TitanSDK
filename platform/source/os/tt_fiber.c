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

#define _XOPEN_SOURCE 600

#include <os/tt_fiber.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <os/tt_thread.h>

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

static tt_fiber_t *__fiber_create_main();

static void __fiber_destroy_main(IN tt_fiber_t *fiber);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_fiber_sched_t *tt_fiber_sched_create(IN OPT tt_fiber_sched_attr_t *attr)
{
    tt_fiber_sched_attr_t __attr;
    tt_fiber_sched_t *fs;
    tt_fiber_t *main_fiber;

    if (attr == NULL) {
        tt_fiber_sched_attr_default(&__attr);
        attr = &__attr;
    }

    fs = tt_malloc(sizeof(tt_fiber_sched_t));
    if (fs == NULL) {
        TT_ERROR("no mem for new fiber sched");
        return NULL;
    }

    main_fiber = __fiber_create_main(NULL);
    if (main_fiber == NULL) {
        tt_free(fs);
        return NULL;
    }

    tt_dlist_init(&fs->fiber_list);
    fs->main_fiber = main_fiber;
    fs->current = main_fiber;

    // the main_fiber fiber should not be added to fiber list

    return fs;
}

void tt_fiber_sched_destroy(IN tt_fiber_sched_t *fs)
{
    tt_dnode_t *node;

    TT_ASSERT(fs != NULL);

    while ((node = tt_dlist_pop_head(&fs->fiber_list)) != NULL) {
        tt_fiber_destroy(TT_CONTAINER(node, tt_fiber_t, node));
    }

    __fiber_destroy_main(fs->main_fiber);

    tt_free(fs);
}

void tt_fiber_sched_attr_default(IN tt_fiber_sched_attr_t *attr)
{
    TT_ASSERT(attr != NULL);
}

tt_fiber_t *tt_fiber_create(IN tt_fiber_routine_t routine,
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

    if (attr == NULL) {
        tt_fiber_attr_default(&__attr);
        attr = &__attr;
    }

    fb = tt_malloc(sizeof(tt_fiber_t));
    if (fb == NULL) {
        TT_ERROR("no mem for new fiber");
        return NULL;
    }

    tt_dnode_init(&fb->node);
    fb->routine = routine;
    fb->param = param;

    if (!TT_OK(tt_fiber_create_wrap(&fb->wrap_fb, attr->stack_size))) {
        tt_free(fb);
        return NULL;
    }

    fb->can_yield = TT_TRUE;

    tt_dlist_push_tail(&fs->fiber_list, &fb->node);

    return fb;
}

void tt_fiber_destroy(IN tt_fiber_t *fiber)
{
    TT_ASSERT(fiber != NULL);

    tt_fiber_destroy_wrap(&fiber->wrap_fb);

    tt_free(fiber);
}

void tt_fiber_attr_default(IN tt_fiber_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->stack_size = __FIBER_STACK_SIZE_DEF;
}

void tt_fiber_yield()
{
    tt_fiber_sched_t *cfs = tt_current_fiber_sched();
    tt_fiber_t *cfb = cfs->current;

    TT_ASSERT(cfs != NULL);
    TT_ASSERT(cfb != cfs->main_fiber);
    TT_ASSERT(cfb->can_yield);

    cfs->current = cfs->main_fiber;
    tt_fiber_switch_wrap(cfs, cfb, cfs->main_fiber);
}

void tt_fiber_switch(IN tt_fiber_t *fiber)
{
    tt_fiber_sched_t *cfs = tt_current_fiber_sched();

    TT_ASSERT(cfs != NULL);
    TT_ASSERT(cfs->current == cfs->main_fiber);
    TT_ASSERT(fiber != cfs->main_fiber);

    cfs->current = fiber;
    tt_fiber_switch_wrap(cfs, cfs->main_fiber, fiber);
}

tt_fiber_t *__fiber_create_main()
{
    tt_fiber_t *fb;

    fb = tt_malloc(sizeof(tt_fiber_t));
    if (fb == NULL) {
        TT_ERROR("no mem for new fiber");
        return NULL;
    }

    tt_dnode_init(&fb->node);
    fb->routine = NULL;
    fb->param = NULL;
    tt_memset(&fb->wrap_fb, 0, sizeof(tt_fiber_wrap_t));
    fb->can_yield = TT_TRUE;

    return fb;
}

void __fiber_destroy_main(IN tt_fiber_t *fiber)
{
    tt_free(fiber);
}
