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

#include <io/tt_io_worker.h>

#include <io/tt_io_event.h>
#include <io/tt_io_worker_group.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <os/tt_fiber.h>

#include <tt_file_system_native.h>

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

static void __worker_io(IN tt_io_ev_t *ev);

static tt_worker_io_t __io_handler[TT_IO_NUM] = {
    __worker_io,
    NULL,
    tt_fs_worker_io,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __io_worker_routine(IN void *param);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_io_worker_create(IN tt_io_worker_t *w, IN tt_iowg_t *wg,
                                IN OPT tt_io_worker_attr_t *attr)
{
    tt_io_worker_attr_t __attr;

    TT_ASSERT(w != NULL);

    if (attr == NULL) {
        tt_io_worker_attr_default(&__attr);
        attr = &__attr;
    }

    w->wg = wg;

    w->thread = tt_thread_create(__io_worker_routine, w, &attr->thread_attr);
    if (w->thread == NULL) {
        TT_ERROR("fail to create w thread");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_io_worker_destroy(IN tt_io_worker_t *w)
{
    TT_ASSERT(w != NULL);

    if (w->thread != NULL) { tt_thread_wait(w->thread); }
}

void tt_io_worker_attr_default(IN tt_io_worker_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_thread_attr_default(&attr->thread_attr);
}

tt_result_t __io_worker_routine(IN void *param)
{
    tt_io_worker_t *w = (tt_io_worker_t *)param;
    tt_iowg_t *wg = w->wg;

    while (1) {
        tt_io_ev_t *io_ev;

        tt_sem_acquire(&wg->sem, TT_TIME_INFINITE);

        io_ev = tt_iowg_pop_ev(wg);
        TT_ASSERT(io_ev != NULL);
        TT_ASSERT(TT_IO_VALID(io_ev->io));
        __io_handler[io_ev->io](io_ev);
    }

    TT_ASSERT(0);
    return TT_SUCCESS;
}

void __worker_io(IN tt_io_ev_t *io_ev)
{
    TT_ASSERT(TT_IO_WORKER_EV_VALID(io_ev->ev));

    // the only event is TT_IO_WORKER_EXIT
    tt_free(io_ev);
    tt_thread_exit();
}
