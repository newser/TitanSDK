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
@file tt_poller.h
@brief io poller
*/

#ifndef __TT_POLLER__
#define __TT_POLLER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <os/tt_poller_def.h>
#include <tt_basic_type.h>

#include <tt_poller_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_poller_attr_s
{
    tt_u32_t reserved;
} tt_poller_attr_t;

typedef struct tt_poller_s
{
    tt_poller_ntv_t sys_poller;
} tt_poller_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_poller_create(IN tt_poller_t *poller,
                                    IN OPT tt_poller_attr_t *attr);

extern void tt_poller_destroy(IN tt_poller_t *poller);

extern void tt_poller_attr_default(IN tt_poller_attr_t *attr);

extern tt_result_t tt_poller_run(IN tt_poller_t *poller);

#endif // __TT_POLLER__
