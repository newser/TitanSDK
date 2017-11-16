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
@file tt_timer.h
@brief timer APIs

this file specifies interfaces of timer

<hr>

<b>Note</b><br>
- timer/timer_manager are implemented with assumption that they are
  running in same thread, so the source code does not care sync matters.
- thread safety is implemented by upper layers
*/

#ifndef __TT_TIMER__
#define __TT_TIMER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_fiber_s;
struct tt_tmr_mgr_s;

typedef enum {
    TT_TMR_INACTIVE,
    TT_TMR_ACTIVE,
    TT_TMR_ORPHAN,

    TT_TMR_STATUS_NUM
} tt_tmr_status_t;
#define TT_TMR_STATUS_VALID(s) ((s) < TT_TMR_STATUS_NUM)

typedef struct tt_tmr_s
{
    tt_s64_t delay_ms;
    tt_s64_t absolute_ref;
    struct tt_fiber_s *owner;
    tt_lnode_t node;
    void *param;
    struct tt_tmr_mgr_s *mgr;
    tt_u32_t ev;
    tt_u32_t heap_pos;
    tt_tmr_status_t status;
} tt_tmr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
 @fn tt_tmr_t *tt_tmr_create(IN tt_s64_t delay_ms,
                             IN tt_u32_t ev,
                             IN OPT void *param)
create a timer

@param [in] mgr timer manager
@param [in] delay_ms how long the time would expire in millisends
@param [in] action what should be done when timer expires
@param [in] param action param
@param [in] private_data private_data associated with this timer

@return
- created timer
- NULL otherwise

@note
- this function is thread safe, could be called by another thread to create
  a timer, but the timer returned is not managed by @ref mgr. and @ref
  tt_tmr_start() is not thread safe
- accessing the timer outside the thread where the timer manager is running
  is not consistent as the manager may be changing it
*/
tt_export tt_tmr_t *tt_tmr_create(IN tt_s64_t delay_ms,
                                  IN tt_u32_t ev,
                                  IN OPT void *param);

/**
@fn void tt_tmr_destroy(IN tt_tmr_t *tmr)
destroy a timer

@param [in] tmr timer to be destroyed

@note
- this function is not thread safe
- be sure the timer is not being managed
*/
tt_export void tt_tmr_destroy(IN tt_tmr_t *tmr);

/**
@fn tt_result_t tt_tmr_start(IN tt_tmr_t *tmr)
start a timer

@param [in] tmr timer to be started

@return
- TT_SUCCESS if timer is started successfully
- TT_FAIL if failed

@note
- this function is not thread safe
*/
tt_export tt_result_t tt_tmr_start(IN tt_tmr_t *tmr);

/**
@fn void tt_tmr_stop(IN tt_tmr_t *tmr)
stop a timer

@param [in] tmr timer to be stopped

@note
this function is not thread safe
*/
tt_inline void tt_tmr_stop(IN tt_tmr_t *tmr)
{
    tmr->status = TT_TMR_INACTIVE;
    // - no need to remove tmr out from heap so as to avoid rebuilding heap
    // - when the timer manager sees an inactive timer, it would pop it
    //   from heap and then the timer becomes wild(not in heap)
}

tt_inline void tt_tmr_set_delay(IN tt_tmr_t *tmr, IN tt_s64_t delay_ms)
{
    tmr->delay_ms = delay_ms;
}

tt_inline void tt_tmr_set_ev(IN tt_tmr_t *tmr, IN tt_u32_t ev)
{
    tmr->ev = ev;
}

tt_inline void tt_tmr_set_param(IN tt_tmr_t *tmr, IN void *param)
{
    tmr->param = param;
}

#endif /* __TT_TIMER__ */
