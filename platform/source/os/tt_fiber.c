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
    tt_fiber_t *main_f;

    if (attr == NULL) {
        tt_fiber_sched_attr_default(&__attr);
        attr = &__attr;
    }

    fs = tt_malloc(sizeof(tt_fiber_sched_t));
    if (fs == NULL) {
        TT_ERROR("no mem for new fiber sched");
        return NULL;
    }

    main_f = __fiber_create_main(NULL);
    if (main_f == NULL) {
        tt_free(fs);
        return NULL;
    }

    tt_dlist_init(&fs->fiber_list);
    fs->main_f = main_f;
    fs->current = main_f;

    // the main_f fiber should not be added to fiber list

    return fs;
}

void tt_fiber_sched_destroy(IN tt_fiber_sched_t *fs)
{
    tt_dnode_t *node;

    TT_ASSERT(fs != NULL);

    while ((node = tt_dlist_pop_head(&fs->fiber_list)) != NULL) {
        tt_fiber_destroy(TT_CONTAINER(node, tt_fiber_t, node));
    }

    __fiber_destroy_main(fs->main_f);

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
    tt_fiber_t *f;

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

    f = tt_malloc(sizeof(tt_fiber_t));
    if (f == NULL) {
        TT_ERROR("no mem for new fiber");
        return NULL;
    }

    tt_dnode_init(&f->node);
    f->routine = routine;
    f->param = param;

    if (!TT_OK(tt_fiber_create_wrap(&f->wrap_f, attr->stack_size))) {
        tt_free(f);
        return NULL;
    }

    f->can_yield = TT_TRUE;

    tt_dlist_push_tail(&fs->fiber_list, &f->node);

    return f;
}

void tt_fiber_destroy(IN tt_fiber_t *fiber)
{
    TT_ASSERT(fiber != NULL);

    tt_fiber_destroy_wrap(&fiber->wrap_f);

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
    tt_fiber_t *cf = cfs->current;

    TT_ASSERT(cf != cfs->main_f);
    TT_ASSERT(cf->can_yield);

    cfs->current = cfs->main_f;
    tt_fiber_switch_wrap(cfs, cf, cfs->main_f);
}

void tt_fiber_resume(IN tt_fiber_t *fiber)
{
    tt_fiber_sched_t *cfs = tt_current_fiber_sched();

    TT_ASSERT(cfs->current == cfs->main_f);
    TT_ASSERT(fiber != cfs->main_f);

    cfs->current = fiber;
    tt_fiber_switch_wrap(cfs, cfs->main_f, fiber);
}

tt_fiber_t *__fiber_create_main()
{
    tt_fiber_t *f;

    f = tt_malloc(sizeof(tt_fiber_t));
    if (f == NULL) {
        TT_ERROR("no mem for new fiber");
        return NULL;
    }

    tt_dnode_init(&f->node);
    f->routine = NULL;
    f->param = NULL;
    tt_memset(&f->wrap_f, 0, sizeof(tt_fiber_wrap_t));
    f->can_yield = TT_TRUE;

    return f;
}

void __fiber_destroy_main(IN tt_fiber_t *fiber)
{
    tt_free(fiber);
}
