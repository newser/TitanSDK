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

/**
@file tt_task.h
@brief task
*/

#ifndef __TT_TASK__
#define __TT_TASK__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_single_linked_list.h>
#include <io/tt_io_poller.h>
#include <network/dns/tt_dns_cache.h>
#include <os/tt_fiber.h>
#include <os/tt_thread.h>
#include <time/tt_timer_manager.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_task_s
{
    tt_slist_t tfl;
    tt_thread_t *thread;
    tt_dns_cache_t *dns_cache;
    tt_tmr_mgr_t tmr_mgr;
    tt_thread_attr_t thread_attr;
    tt_io_poller_t iop;
} tt_task_t;

typedef struct tt_task_attr_s
{
    tt_bool_t enable_dns;
    tt_dns_cache_attr_t dns_cache_attr;

    tt_tmr_mgr_attr_t tmr_mgr_attr;
    tt_thread_attr_t thread_attr;
    tt_io_poller_attr_t io_poller_attr;
} tt_task_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_task_create(IN tt_task_t *t, IN OPT tt_task_attr_t *attr);

extern void tt_task_attr_default(IN tt_task_attr_t *attr);

extern tt_result_t tt_task_add_fiber(IN tt_task_t *t,
                                     IN OPT const tt_char_t *name,
                                     IN tt_fiber_routine_t routine,
                                     IN void *param,
                                     IN OPT tt_fiber_attr_t *attr);

extern tt_result_t tt_task_run(IN tt_task_t *t);

// set NULL to exit current task, but note it does not exit immediately
extern void tt_task_exit(IN OPT tt_task_t *t);

extern void tt_task_wait(IN tt_task_t *t);

extern tt_result_t tt_task_run_local(IN tt_task_t *t);

tt_inline void tt_task_finish(IN tt_task_t *t, IN tt_io_ev_t *io_ev)
{
    tt_io_poller_finish(&t->iop, io_ev);
}

tt_inline tt_task_t *tt_current_task()
{
    tt_thread_t *t = tt_current_thread();
    return TT_COND(t != NULL, t->task, NULL);
}

#endif /* __TT_TASK__ */
