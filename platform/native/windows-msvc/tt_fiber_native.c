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

#include <os/tt_fiber.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <os/tt_thread.h>

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

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_fiber_sched_t *tt_fiber_sched_create(IN OPT tt_fiber_sched_attr_t *attr)
{
    tt_fiber_sched_attr_t __attr;
    tt_fiber_sched_t *fs;
    tt_fiber_t *main;

    if (attr == NULL) {
        tt_fiber_sched_attr_default(&__attr);
        attr = &__attr;
    }

    fs = tt_malloc(sizeof(tt_fiber_sched_t));
    if (fs == NULL) {
        TT_ERROR("no mem for new fiber sched");
        return NULL;
    }

    main = tt_fiber_create_local(NULL);
    if (main == NULL) {
        tt_free(fs);
        return NULL;
    }

    tt_dlist_init(&fs->fiber_list);
    fs->main = main;
    fs->current = main;

    tt_dlist_push_tail(&fs->fiber_list, &main->node);

    return fs;
}

void tt_fiber_sched_destroy(IN tt_fiber_sched_t *fs)
{
    tt_dnode_t *node;

    TT_ASSERT(fs != NULL);

    while ((node = tt_dlist_pop_head(&fs->fiber_list)) != NULL) {
        tt_fiber_destroy(TT_CONTAINER(node, tt_fiber_t, node));
    }

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
    tt_fiber_attr_t __attr;
    tt_fiber_t *f;

    TT_ASSERT(routine != NULL);

    if (attr == NULL) {
        tt_fiber_attr_default(&__attr);
        attr = &__attr;
    }

    f = tt_malloc(sizeof(tt_fiber_t));
    if (f == NULL) {
        TT_ERROR("no mem for new fiber");
        return NULL;
    }

    return f;
}

tt_fiber_t *tt_fiber_create_local(IN OPT tt_fiber_attr_t *attr)
{
    tt_fiber_attr_t __attr;
    tt_fiber_t *f;

    if (attr == NULL) {
        tt_fiber_attr_default(&__attr);
        attr = &__attr;
    }

    f = tt_malloc(sizeof(tt_fiber_t));
    if (f == NULL) {
        TT_ERROR("no mem for new fiber");
        return NULL;
    }

    return f;
}

void tt_fiber_destroy(IN tt_fiber_t *fiber)
{
    TT_ASSERT(fiber != NULL);

    tt_free(fiber);
}

void tt_fiber_attr_default(IN tt_fiber_attr_t *attr)
{
    TT_ASSERT(attr != NULL);
}

void tt_fiber_yield()
{
    tt_fiber_sched_t *cfs = __current_fiber_sched();
    tt_fiber_t *cf = cfs->current;

    if (cf == cfs->main) {
        return;
    }

    cfs->current = cfs->main;
    // tt_fiber_resume_ntv(cfs->current, cfs->main);
}

void tt_fiber_resume(IN tt_fiber_t *fiber)
{
    tt_fiber_sched_t *cfs = __current_fiber_sched();
    tt_fiber_t *cf = cfs->current;

    if (cf == fiber) {
        return;
    }

    cfs->current = fiber;
    // tt_fiber_resume_ntv(cfs->current, fiber);
}

tt_fiber_sched_t *__current_fiber_sched()
{
    tt_thread_t *thread = tt_current_thread();
    return TT_COND(thread != NULL, thread->fiber_sched, NULL);
}
