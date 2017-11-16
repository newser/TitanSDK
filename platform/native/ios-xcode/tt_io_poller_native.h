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

/**
@file tt_io_poller_natvie.h
@brief io poller native
*/

#ifndef __TT_IO_POLLER_NATIVE__
#define __TT_IO_POLLER_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_double_linked_list.h>
#include <io/tt_io_event.h>
#include <os/tt_spinlock.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_io_poller_ntv_s
{
    tt_dlist_t poller_ev;
    tt_dlist_t worker_ev;
    tt_spinlock_t poller_lock;
    tt_spinlock_t worker_lock;
    int kq;
} tt_io_poller_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_io_poller_component_init_ntv();

extern tt_result_t tt_io_poller_create_ntv(IN tt_io_poller_ntv_t *sys_iop);

extern void tt_io_poller_destroy_ntv(IN tt_io_poller_ntv_t *sys_iop);

extern tt_bool_t tt_io_poller_run_ntv(IN tt_io_poller_ntv_t *sys_iop,
                                      IN tt_s64_t wait_ms);

extern tt_result_t tt_io_poller_exit_ntv(IN tt_io_poller_ntv_t *sys_iop);

extern tt_result_t tt_io_poller_finish_ntv(IN tt_io_poller_ntv_t *sys_iop,
                                           IN tt_io_ev_t *io_ev);

extern tt_result_t tt_io_poller_send_ntv(IN tt_io_poller_ntv_t *sys_iop,
                                         IN tt_io_ev_t *io_ev);

#endif // __TT_IO_POLLER_NATIVE__
