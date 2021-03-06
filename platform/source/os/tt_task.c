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

#include <os/tt_task.h>

#include <memory/tt_memory_alloc.h>
#include <time/tt_timer.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_fiber_t *fb;
    const tt_char_t *name;
    tt_fiber_routine_t routine;
    void *param;
    tt_fiber_attr_t *attr;
    tt_snode_t node;
} __task_fiber_t;

enum
{
    __TSK_FIND_FIBER,

    __TSK_EV_NUM
};

typedef struct
{
    tt_io_ev_t io_ev;

    tt_task_t *task;
    const tt_char_t *name;

    tt_fiber_t *fb;
} __tsk_find_fiber_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_bool_t __do_find_fiber(IN tt_io_ev_t *io_ev);

static tt_poller_io_t __task_poller_io[__TSK_EV_NUM] = {
    __do_find_fiber,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __task_routine(IN void *param);

static tt_fiber_t *__task_find_fiber(IN tt_task_t *t, IN const tt_char_t *name);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_task_create(IN tt_task_t *t, IN OPT tt_task_attr_t *attr)
{
    tt_task_attr_t __attr;

    TT_ASSERT(t != NULL);

    if (attr == NULL) {
        tt_task_attr_default(&__attr);
        attr = &__attr;
    }

    tt_slist_init(&t->tfl);

    t->thread = NULL;
    tt_memcpy(&t->thread_attr, &attr->thread_attr, sizeof(tt_thread_attr_t));

    if (attr->enable_dns) {
        t->dns_cache = tt_dns_cache_create(&attr->dns_cache_attr);
        if (t->dns_cache == NULL) {
            return TT_FAIL;
        }
    } else {
        t->dns_cache = NULL;
    }

    tt_tmr_mgr_init(&t->tmr_mgr, &attr->tmr_mgr_attr);

    if (!TT_OK(tt_io_poller_create(&t->iop, &attr->io_poller_attr))) {
        TT_ERROR("fail to create task io poller");

        if (t->dns_cache != NULL) {
            tt_dns_cache_destroy(t->dns_cache);
        }
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_task_attr_default(IN tt_task_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->enable_dns = TT_FALSE;
    tt_dns_cache_attr_default(&attr->dns_cache_attr);

    tt_tmr_mgr_attr_default(&attr->tmr_mgr_attr);

    tt_thread_attr_default(&attr->thread_attr);
    attr->thread_attr.enable_fiber = TT_TRUE;

    tt_io_poller_attr_default(&attr->io_poller_attr);
}

tt_result_t tt_task_add_fiber(IN tt_task_t *t,
                              IN OPT const tt_char_t *name,
                              IN tt_fiber_routine_t routine,
                              IN void *param,
                              IN OPT tt_fiber_attr_t *attr)
{
    __task_fiber_t *tf;

    TT_ASSERT(t != NULL);
    TT_ASSERT(routine != NULL);

    tf = tt_malloc(sizeof(__task_fiber_t));
    if (tf == NULL) {
        TT_ERROR("no mem to add fiber");
        return TT_FAIL;
    }

    tf->fb = NULL;
    tf->name = name;
    tf->routine = routine;
    tf->param = param;
    tf->attr = attr;
    tt_snode_init(&tf->node);

    tt_slist_push_tail(&t->tfl, &tf->node);
    return TT_SUCCESS;
}

tt_fiber_t *tt_task_find_fiber(IN tt_task_t *t, IN const tt_char_t *name)
{
    tt_task_t *ct = tt_current_task();
    if (t == ct) {
        return __task_find_fiber(t, name);
    } else {
        __tsk_find_fiber_t tff;

        tt_io_ev_init(&tff.io_ev, TT_IO_TASK, __TSK_FIND_FIBER);
        tff.io_ev.src = tt_current_fiber();
        TT_ASSERT(tff.io_ev.src != NULL);

        tff.task = t;
        tff.name = name;

        tff.fb = NULL;

        tt_io_poller_send(&t->iop, &tff.io_ev);
        tt_fiber_suspend();
        return tff.fb;
    }
}

tt_result_t tt_task_run(IN tt_task_t *t)
{
    TT_ASSERT(t != NULL);

    if (tt_slist_empty(&t->tfl)) {
        TT_ERROR("task has no fiber");
        return TT_FAIL;
    }

    t->thread = tt_thread_create(__task_routine, t, &t->thread_attr);
    if (t->thread == NULL) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_task_exit(IN OPT tt_task_t *t)
{
    if (t == NULL) {
        t = tt_current_thread()->task;
    }

    if (t->thread != NULL) {
        tt_io_poller_exit(&t->iop);
    }
}

void tt_task_wait(IN tt_task_t *t)
{
    tt_snode_t *node;
    tt_tmr_t *tmr;

    TT_ASSERT(t != NULL);

    if (t->thread != NULL) {
        tt_thread_wait(t->thread);
    }

    while ((node = tt_slist_pop_head(&t->tfl)) != NULL) {
        tt_free(TT_CONTAINER(node, __task_fiber_t, node));
    }

    if (t->dns_cache != NULL) {
        tt_dns_cache_destroy(t->dns_cache);
    }

    // - when fiber terminates, it will destroy all its own timers, but
    //   any timer that is still in the heap will only be marked as orphan,
    //   so it's possible that here the the timer is still in the tmr mgr
    // - thread is over, timers won't be accessed
    while ((tmr = tt_tmr_mgr_pop(&t->tmr_mgr)) != NULL) {
        tt_tmr_destroy(tmr);
    }
    tt_tmr_mgr_destroy(&t->tmr_mgr);

    tt_io_poller_destroy(&t->iop);
}

tt_result_t tt_task_run_local(IN tt_task_t *t)
{
    if (tt_slist_empty(&t->tfl)) {
        TT_ERROR("task has no fiber");
        return TT_FAIL;
    }

    if (!TT_OK(tt_thread_create_local(&t->thread_attr))) {
        TT_ERROR("fail to create local task");
        return TT_FAIL;
    }
    __task_routine(t);

    t->thread = NULL;
    tt_task_wait(t);

    if (!TT_OK(tt_thread_wait_local())) {
        TT_FATAL("fail to wait local task");
    }

    return TT_SUCCESS;
}

void tt_task_poller_io(IN tt_io_ev_t *io_ev)
{
    if (__task_poller_io[io_ev->ev](io_ev)) {
        if (io_ev->src != NULL) {
            // these events are generally coming from other task
            tt_task_finish(io_ev->src->fs->thread->task, io_ev);
        } else {
            tt_free(io_ev);
        }
    }
}

tt_result_t __task_routine(IN void *param)
{
    tt_task_t *t = (tt_task_t *)param;
    tt_thread_t *thread = tt_current_thread();
    tt_snode_t *node;
    tt_fiber_sched_t *cfs = thread->fiber_sched;
    tt_tmr_mgr_t *tmr_mgr = &t->tmr_mgr;
    tt_fiber_t *fb;

    // note t->thread may not be set yet
    thread->task = t;

    node = tt_slist_head(&t->tfl);
    TT_ASSERT(node != NULL);
    while (node != NULL) {
        __task_fiber_t *tf = TT_CONTAINER(node, __task_fiber_t, node);

        node = node->next;

        tf->fb = tt_fiber_create(tf->name, tf->routine, tf->param, tf->attr);
        if (tf->fb == NULL) {
            TT_ERROR("fail to create task fiber");
            return TT_FAIL;
        }
    }

    // we need resume each fiber at least once
    node = tt_slist_head(&t->tfl);
    while (node != NULL) {
        __task_fiber_t *tf = TT_CONTAINER(node, __task_fiber_t, node);

        node = node->next;

        tt_fiber_resume(tf->fb, TT_FALSE);
    }

    while ((fb = tt_fiber_sched_next(cfs)) != cfs->__main) {
        tt_fiber_resume(fb, TT_FALSE);
        continue;
    }
    while (1) {
        tt_s64_t wait_ms, dns_ms;

        wait_ms = tt_tmr_mgr_run(tmr_mgr);

        if ((t->dns_cache != NULL) &&
            (wait_ms > (dns_ms = tt_dns_cache_run(t->dns_cache)))) {
            wait_ms = dns_ms;
            // TT_INFO("wait: %d", dns_ms);
        }

        fb = tt_fiber_sched_next(cfs);
        if (fb != cfs->__main) {
            // after processing timer events, some fibers become active
            // so let's run all active fibers again, and after that we
            // also need check if there is new timer
            do {
                tt_fiber_resume(fb, TT_FALSE);
            } while ((fb = tt_fiber_sched_next(cfs)) != cfs->__main);
            continue;
        }

        // it's possible that all fibers terminate
        if (tt_fiber_sched_empty(cfs)) {
            break;
        }

        // now we are sure:
        //  - no active fiber, but must be some pending fibers
        //  - all timer events are processed
        //  - wais_ms is accurate
        if (!tt_io_poller_run(&t->iop, wait_ms)) {
            // main fiber indicates exit
            break;
        }
    };

    return TT_SUCCESS;
}

tt_fiber_t *__task_find_fiber(IN tt_task_t *t, IN const tt_char_t *name)
{
    tt_snode_t *node = tt_slist_head(&t->tfl);
    while (node != NULL) {
        __task_fiber_t *tf = TT_CONTAINER(node, __task_fiber_t, node);
        if (tt_strcmp(tf->name, name) == 0) {
            return tf->fb;
        }

        node = node->next;
    }
    return NULL;
}

tt_bool_t __do_find_fiber(IN tt_io_ev_t *io_ev)
{
    __tsk_find_fiber_t *tff = TT_CONTAINER(io_ev, __tsk_find_fiber_t, io_ev);

    tff->fb = __task_find_fiber(tff->task, tff->name);
    return TT_TRUE;
}
