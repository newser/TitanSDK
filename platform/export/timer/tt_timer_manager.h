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
@file tt_tmr_container.h
@brief timer manager

this file specifies interfaces of timer container
*/

#ifndef __TT_TIMER_CONTAINER__
#define __TT_TIMER_CONTAINER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/ptr/tt_ptr_heap.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_tmr_s;

typedef struct
{
    tt_ptrheap_attr_t tmr_heap_attr;
} tt_tmr_mgr_attr_t;

typedef struct tt_tmr_mgr_s
{
    tt_tmr_mgr_attr_t attr;

    tt_ptrheap_t tmr_heap;
} tt_tmr_mgr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_tmr_mgr_component_register()
register timer manager system
*/
extern void tt_tmr_mgr_component_register();

extern tt_result_t tt_tmr_mgr_create(IN tt_tmr_mgr_t *mgr,
                                     IN OPT tt_tmr_mgr_attr_t *attr);

extern void tt_tmr_mgr_destroy(IN tt_tmr_mgr_t *mgr);

extern void tt_tmr_mgr_attr_default(IN tt_tmr_mgr_attr_t *attr);

// call expired timers' callback
// - return how long that next timer would expire
// - return TT_TIME_INFINITE if no timer in mgr
extern tt_s64_t tt_tmr_mgr_run(IN tt_tmr_mgr_t *mgr);

tt_inline struct tt_tmr_s *tt_tmr_mgr_head(IN tt_tmr_mgr_t *mgr)
{
    return (struct tt_tmr_s *)tt_ptrheap_head(&mgr->tmr_heap);
}

#endif /* __TT_TIMER_CONTAINER__ */
