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
@file tt_io_worker.h
@brief io w
*/

#ifndef __TT_IO_WORKER__
#define __TT_IO_WORKER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <os/tt_thread.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_iowg_s;

typedef struct tt_io_worker_attr_s
{
    tt_thread_attr_t thread_attr;
} tt_io_worker_attr_t;

typedef struct tt_io_worker_s
{
    struct tt_iowg_s *wg;
    tt_thread_t *thread;
} tt_io_worker_t;

enum
{
    TT_IO_WORKER_EXIT,

    TT_IO_WORKER_EV_NUM
};
#define TT_IO_WORKER_EV_VALID(e) ((e) < TT_IO_WORKER_EV_NUM)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_io_worker_init(IN tt_io_worker_t *w)
{
    w->wg = NULL;
    w->thread = NULL;
}

tt_export tt_result_t tt_io_worker_create(IN tt_io_worker_t *w,
                                          IN struct tt_iowg_s *wg,
                                          IN OPT tt_io_worker_attr_t *attr);

tt_export void tt_io_worker_destroy(IN tt_io_worker_t *w);

tt_inline tt_bool_t tt_io_worker_running(IN tt_io_worker_t *w)
{
    return TT_BOOL(w->thread != NULL);
}

tt_export void tt_io_worker_attr_default(IN tt_io_worker_attr_t *attr);

#endif // __TT_IO_WORKER__
