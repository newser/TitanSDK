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

#include <os/tt_thread.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <log/tt_log.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>
#include <os/tt_atomic.h>

#include <tt_cstd_api.h>
#include <tt_thread_native.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_THREAD_DEFAULT_NAME "anonymous"

#define __LOCK_THREAD_LIST()                                                   \
    while (!TT_OK(tt_atomic_s32_cas(&tt_s_thread_list_lock, 0, 1)))
#define __UNLOCK_THREAD_LIST()                                                 \
    TT_ASSERT_ALWAYS(TT_OK(tt_atomic_s32_cas(&tt_s_thread_list_lock, 1, 0)))

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_atomic_s32_t tt_s_thread_list_lock;
tt_list_t tt_s_thread_list;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __thread_component_init(IN tt_component_t *comp,
                                           IN tt_profile_t *profile);

static tt_result_t tt_thread_routine_entry(IN void *param);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_thread_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __thread_component_init,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_THREAD, "Thread", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

tt_result_t __thread_component_init(IN tt_component_t *comp,
                                    IN tt_profile_t *profile)
{
    tt_result_t result = TT_FAIL;

    result = tt_thread_component_init_ntv();
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    tt_atomic_s32_init(&tt_s_thread_list_lock, 0);
    tt_list_init(&tt_s_thread_list);

    return TT_SUCCESS;
}

tt_thread_t *tt_thread_create(IN const tt_char_t *name,
                              IN tt_thread_routine_t routine,
                              IN void *routine_param,
                              IN tt_thread_attr_t *attr)
{
    tt_thread_t *thread;
    tt_bool_t detached;
    tt_result_t result;

    thread = tt_malloc(sizeof(tt_thread_t));
    if (thread == NULL) {
        TT_ERROR("no mem for new thread");
        return NULL;
    }

    tt_memset(thread, 0, sizeof(tt_thread_t));

    // name
    if (name != NULL) {
        tt_strncpy(thread->name, name, TT_MAX_THREAD_NAME_LEN);
    } else {
        tt_strncpy(thread->name,
                   TT_THREAD_DEFAULT_NAME,
                   TT_MAX_THREAD_NAME_LEN);
    }

    // routine
    thread->routine = routine;
    thread->param = routine_param;

    // attribute
    if (attr != NULL) {
        tt_memcpy(&thread->attr, attr, sizeof(tt_thread_attr_t));
    } else {
        tt_thread_attr_default(&thread->attr);
    }
    detached = thread->attr.detached;

    tt_lnode_init(&thread->thread_lst_node);

    thread->last_error = TT_SUCCESS;

    thread->evp = NULL;

    tt_rng_init(&thread->rng);

    // create system thread
    if (thread->attr.local_run) {
        if (tt_current_thread() != NULL) {
            TT_ERROR("can not local run in tt thread");
            goto tc_fail;
        }

        result = tt_thread_local_run_ntv(thread);
    } else {
        TT_ASSERT(thread->routine != NULL);
        result = tt_thread_create_ntv(thread);
    }
    if (!TT_OK(result)) {
        goto tc_fail;
    }
    // from now on, do not change content of "thread" in this
    // function or it may be missed by the created thread that
    // is already running

    if (detached) {
        // do not return the thread to caller
        return (tt_thread_t *)1;
    } else {
        return thread;
    }

tc_fail:

    if (thread != NULL) {
        tt_free(thread);
    }

    return NULL;
}

tt_thread_t *tt_thread_create_local(IN const tt_char_t *name,
                                    IN OPT tt_thread_attr_t *attr)
{
    tt_thread_attr_t __attr;

    if (attr == NULL) {
        tt_thread_attr_default(&__attr);
        attr = &__attr;
    }
    attr->local_run = TT_TRUE;

    return tt_thread_create(name, NULL, NULL, attr);
}

void tt_thread_attr_default(IN tt_thread_attr_t *attr)
{
    attr->detached = TT_FALSE;
    attr->local_run = TT_FALSE;
}

tt_result_t tt_thread_wait(IN tt_thread_t *thread)
{
    // 1 is some detached thread
    TT_ASSERT((thread != NULL) && (thread != (tt_thread_t *)1));
    TT_ASSERT(!thread->attr.detached);

    if (thread->attr.local_run) {
        TT_ERROR("can not wait a local running thread");
        return TT_FAIL;
    }

    if (!TT_OK(tt_thread_wait_ntv(thread))) {
        return TT_FAIL;
    }

    tt_free(thread);
    return TT_SUCCESS;
}

void tt_thread_exit()
{
    tt_thread_exit_ntv();
}

tt_thread_t *tt_current_thread()
{
    return tt_current_thread_ntv();
}

void tt_sleep(IN tt_u32_t millisec)
{
    tt_sleep_ntv(millisec);
}

tt_result_t __thread_on_create(IN tt_thread_t *thread)
{
    __LOCK_THREAD_LIST();
    tt_list_addtail(&tt_s_thread_list, &thread->thread_lst_node);
    __UNLOCK_THREAD_LIST();

    return TT_SUCCESS;
}

tt_result_t __thread_on_exit(IN tt_thread_t *thread)
{
    // event poller
    thread->evp = NULL;

    // remove from numa node thread list
    __LOCK_THREAD_LIST();
    tt_list_remove(&thread->thread_lst_node);
    __UNLOCK_THREAD_LIST();

    // for non-detached and non-local_run thread, the struct will
    // be freed in tt_thread_wait
    if (thread->attr.detached || thread->attr.local_run) {
        tt_free(thread);
    }

    return TT_SUCCESS;
}
