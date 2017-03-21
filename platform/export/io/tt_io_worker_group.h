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
@file tt_io_worker_group.h
@brief io worker group
*/

#ifndef __TT_IO_WORKER_GROUP__
#define __TT_IO_WORKER_GROUP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_double_linked_list.h>
#include <io/tt_io_worker.h>
#include <os/tt_semaphore.h>
#include <os/tt_spinlock.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_io_ev_s;

typedef struct tt_iowg_attr_s
{
    tt_io_worker_attr_t worker_attr;
    tt_sem_attr_t sem_attr;
    tt_spinlock_attr_t lock_attr;
} tt_iowg_attr_t;

typedef struct tt_iowg_s
{
    tt_dlist_t ev_list;
    tt_io_worker_t *worker;
    tt_sem_t sem;
    tt_spinlock_t lock;
    tt_u32_t worker_num;
} tt_iowg_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

extern tt_iowg_t tt_g_fs_iowg;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_iowg_component_register();

extern tt_result_t tt_iowg_create(IN tt_iowg_t *iowg,
                                  IN OPT tt_u32_t worker_num,
                                  IN OPT tt_iowg_attr_t *attr);

extern void tt_iowg_destroy(IN tt_iowg_t *iowg);

extern void tt_iowg_attr_default(IN tt_iowg_attr_t *attr);

extern struct tt_io_ev_s *tt_iowg_pop_ev(IN tt_iowg_t *iowg);

extern void tt_iowg_push_ev(IN tt_iowg_t *iowg, IN struct tt_io_ev_s *ev);

#endif // __TT_IO_WORKER_GROUP__
