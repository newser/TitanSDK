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

#include <os/tt_thread.h>

#include <algorithm/tt_blobex.h>
#include <algorithm/tt_buffer.h>
#include <algorithm/tt_rng_xorshift.h>
#include <crypto/tt_ctr_drbg.h>
#include <crypto/tt_entropy.h>
#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <memory/tt_memory_alloc.h>
#include <memory/tt_slab.h>
#include <misc/tt_assert.h>
#include <network/http/tt_http_host_set.h>
#include <os/tt_fiber.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __THREAD_NAME_DEFAULT "anonymous"

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

static tt_result_t __thread_component_init(IN tt_component_t *comp,
                                           IN tt_profile_t *profile);

static void __thread_component_exit(IN tt_component_t *comp);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_thread_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __thread_component_init, __thread_component_exit,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_THREAD, "Thread", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

tt_thread_t *tt_thread_create(IN tt_thread_routine_t routine,
                              IN void *param,
                              IN OPT tt_thread_attr_t *attr)
{
    tt_thread_t *thread;
    tt_thread_attr_t __attr;
    tt_bool_t detached;

    TT_ASSERT(routine != NULL);

    if (attr == NULL) {
        tt_thread_attr_default(&__attr);
        attr = &__attr;
    }
    detached = attr->detached;

    thread = tt_c_malloc(sizeof(tt_thread_t));
    if (thread == NULL) {
        TT_ERROR("no mem for new thread");
        return NULL;
    }
    tt_memset(thread, 0, sizeof(tt_thread_t));

    thread->routine = routine;
    thread->param = param;
    thread->name = attr->name;

    thread->evp = NULL;

    thread->rng = tt_rng_xorshift_create();
    if (thread->rng == NULL) {
        goto tc_fail;
    }

    thread->fiber_sched = NULL;
    thread->task = NULL;
    thread->entropy = NULL;
    thread->ctr_drbg = NULL;
    thread->backtrace = NULL;
    thread->http_rawhdr = NULL;
    thread->http_rawval = NULL;
    thread->http_hostset = NULL;

    thread->last_error = TT_SUCCESS;
    thread->detached = detached;
    thread->local = TT_FALSE;
    thread->enable_fiber = attr->enable_fiber;
    thread->log = TT_THREAD_LOG_DEFAULT;

    if (!TT_OK(tt_thread_create_ntv(thread))) {
        goto tc_fail;
    }
    // now the thread is created, and it may be running or even
    // terminate, so accessing the "thread", either reading or
    // writing is unsafe
    if (detached) {
        // do not return the thread to caller
        return (tt_thread_t *)1;
    } else {
        return thread;
    }

tc_fail:

    if (thread->rng != NULL) {
        tt_rng_destroy(thread->rng);
    }

    tt_c_free(thread);

    return NULL;
}

tt_result_t tt_thread_create_local(IN OPT tt_thread_attr_t *attr)
{
    tt_thread_t *thread;
    tt_thread_attr_t __attr;

    if (tt_current_thread() != NULL) {
        TT_ERROR("can not local run in tt thread");
        return TT_FAIL;
    }

    if (attr == NULL) {
        tt_thread_attr_default(&__attr);
        attr = &__attr;
    }

    thread = tt_c_malloc(sizeof(tt_thread_t));
    if (thread == NULL) {
        TT_ERROR("no mem for new thread");
        return TT_FAIL;
    }
    tt_memset(thread, 0, sizeof(tt_thread_t));

    thread->routine = NULL;
    thread->param = NULL;
    thread->name = attr->name;

    thread->evp = NULL;

    thread->rng = tt_rng_xorshift_create();
    if (thread->rng == NULL) {
        goto tcl_fail;
    }

    thread->fiber_sched = NULL;
    thread->task = NULL;
    thread->entropy = NULL;
    thread->ctr_drbg = NULL;
    thread->backtrace = NULL;

    thread->last_error = TT_SUCCESS;
    thread->detached = TT_FALSE;
    thread->local = TT_TRUE;
    thread->enable_fiber = attr->enable_fiber;
    thread->log = TT_THREAD_LOG_DEFAULT;

    if (!TT_OK(tt_thread_create_local_ntv(thread))) {
        goto tcl_fail;
    }

    return TT_SUCCESS;

tcl_fail:

    if (thread->rng != NULL) {
        tt_rng_destroy(thread->rng);
    }

    tt_c_free(thread);

    return TT_FAIL;
}

void tt_thread_attr_default(IN tt_thread_attr_t *attr)
{
    attr->name = __THREAD_NAME_DEFAULT;

    attr->detached = TT_FALSE;

    attr->enable_fiber = TT_FALSE;
}

tt_result_t tt_thread_wait(IN tt_thread_t *thread)
{
    // 1 is some detached thread
    TT_ASSERT((thread != NULL) && (thread != (tt_thread_t *)1));
    TT_ASSERT(!thread->detached && !thread->local);

    if (!TT_OK(tt_thread_wait_ntv(thread))) {
        return TT_FAIL;
    }

    tt_c_free(thread);

    return TT_SUCCESS;
}

tt_result_t tt_thread_wait_local()
{
    tt_thread_t *thread = tt_current_thread();

    TT_ASSERT((thread != NULL) && (thread->local));

    // structure thread would be released in __thread_on_exit called by
    // tt_thread_wait_local_ntv()
    return tt_thread_wait_local_ntv(thread);
}

tt_result_t __thread_component_init(IN tt_component_t *comp,
                                    IN tt_profile_t *profile)
{
    if (!TT_OK(tt_thread_component_init_ntv())) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void __thread_component_exit(IN tt_component_t *comp)
{
    tt_thread_component_exit_ntv();
}

tt_result_t __thread_on_create(IN tt_thread_t *thread)
{
    // must be created in new thread context
    if (thread->enable_fiber) {
        thread->fiber_sched = tt_fiber_sched_create(NULL);
        if (thread->fiber_sched == NULL) {
            return TT_FAIL;
        }

        TT_ASSERT(thread->fiber_sched->thread == NULL);
        thread->fiber_sched->thread = thread;
    }

    return TT_SUCCESS;
}

void __thread_on_exit(IN tt_thread_t *thread)
{
    thread->evp = NULL;

    if (thread->rng != NULL) {
        tt_rng_destroy(thread->rng);
    }

    if (thread->fiber_sched != NULL) {
        tt_fiber_sched_destroy(thread->fiber_sched);
    }

    if (thread->ctr_drbg != NULL) {
        tt_ctr_drbg_destroy(thread->ctr_drbg);
    }

    if (thread->entropy != NULL) {
        tt_entropy_destroy(thread->entropy);
    }

    if (thread->backtrace != NULL) {
        tt_buf_destroy(thread->backtrace);
        tt_free(thread->backtrace);
    }

    if (thread->http_rawhdr != NULL) {
        tt_slab_destroy(thread->http_rawhdr);
        tt_free(thread->http_rawhdr);
    }

    if (thread->http_rawval != NULL) {
        tt_slab_destroy(thread->http_rawval);
        tt_free(thread->http_rawval);
    }

    if (thread->http_hostset != NULL) {
        tt_http_hostset_destroy(thread->http_hostset);
        tt_free(thread->http_hostset);
    }

    // for non-detached and non-local thread, the struct will
    // be freed in tt_thread_wait
    if (thread->detached || thread->local) {
        tt_c_free(thread);
    }
}
