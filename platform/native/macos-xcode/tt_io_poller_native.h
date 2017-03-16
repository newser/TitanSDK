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
@file tt_io_poller_natvie.h
@brief io poller native
*/

#ifndef __TT_IO_POLLER_NATIVE__
#define __TT_IO_POLLER_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <io/tt_io_event.h>
#include <os/tt_atomic.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_io_poller_ntv_s
{
    int kq;
} tt_io_poller_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_io_poller_create_ntv(IN tt_io_poller_ntv_t *sys_poller);

extern void tt_io_poller_destroy_ntv(IN tt_io_poller_ntv_t *sys_poller);

extern tt_bool_t tt_io_poller_run_ntv(IN tt_io_poller_ntv_t *sys_poller,
                                      IN tt_s64_t wait_ms);

extern void tt_io_poller_yield_ntv(IN tt_io_poller_ntv_t *sys_poller);

extern void tt_io_poller_exit_ntv(IN tt_io_poller_ntv_t *sys_iop);

#endif // __TT_IO_POLLER_NATIVE__
