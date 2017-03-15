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
@file tt_poller_natvie.h
@brief poller native
*/

#ifndef __TT_POLLER_NATIVE__
#define __TT_POLLER_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <os/tt_poller_def.h>
#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_poller_ntv_s
{
    int kq_fd;
} tt_poller_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_poller_create_ntv(IN tt_poller_ntv_t *sys_poller);

extern void tt_poller_destroy_ntv(IN tt_poller_ntv_t *sys_poller);

extern tt_result_t tt_poller_run_ntv(IN tt_poller_ntv_t *sys_poller,
                                     IN tt_s64_t wait_ms,
                                     OUT tt_poller_ev_t *ev,
                                     OUT void **data);

#endif // __TT_POLLER_NATIVE__
