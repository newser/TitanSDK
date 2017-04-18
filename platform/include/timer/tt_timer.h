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

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_tmr_mgr_s;
struct tt_tmr_s;

/**
@typedef void (*tt_tmr_cb_t)(IN void* param, \
                                   IN struct tt_tmr_s *timer)
function that is executed when timer expires

@note
it's safely to modify timer in this callback
*/
typedef void (*tt_tmr_cb_t)(IN struct tt_tmr_s *tmr,
                            IN void *cb_param,
                            IN tt_u32_t reason);
// reason
#define TT_TMR_CB_EXPIRED 0
#define TT_TMR_CB_DESTROYED 1

typedef struct tt_tmr_s
{
    __TT_PRIVATE__

    tt_s64_t delay_ms;
    void *opaque;
    tt_tmr_cb_t cb;
    void *cb_param;

    tt_bool_t notify_destroyed : 1;

    tt_u32_t status : 2;
#define __TMR_INACTIVE 0
#define __TMR_ACTIVE 1
#define __TMR_ORPHAN 2

    tt_u32_t heap_idx;
    struct tt_tmr_mgr_s *mgr;
    tt_s64_t absolute_expire_time;
} tt_tmr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn tt_tmr_t *tt_tmr_create(IN IN struct tt_tmr_mgr_s *mgr,
                                IN tt_s64_t delay_ms,
                                IN tt_tmr_cb_t action,
                                IN void* param,
                                IN tt_uintptr_t private_data)
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
extern tt_tmr_t *tt_tmr_create(IN IN struct tt_tmr_mgr_s *mgr,
                               IN tt_s64_t delay_ms,
                               IN void *opaque,
                               IN tt_tmr_cb_t cb,
                               IN OPT void *cb_param,
                               IN tt_u32_t flag);
// flag
#define TT_TMR_NOTIFY_DESTROYED (1 << 0)

/**
@fn void tt_tmr_destroy(IN tt_tmr_t *tmr)
destroy a timer

@param [in] tmr timer to be destroyed

@note
- this function is not thread safe
- be sure the timer is not being managed
*/
extern void tt_tmr_destroy(IN tt_tmr_t *tmr);

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
extern tt_result_t tt_tmr_start(IN tt_tmr_t *tmr);

/**
@fn void tt_tmr_stop(IN tt_tmr_t *tmr)
stop a timer

@param [in] tmr timer to be stopped

@note
this function is not thread safe
*/
extern void tt_tmr_stop(IN tt_tmr_t *tmr);

tt_inline void tt_tmr_set_delay(IN tt_tmr_t *tmr, IN tt_s64_t delay_ms)
{
    tmr->delay_ms = delay_ms;
}

tt_inline void tt_tmr_set_opaque(IN tt_tmr_t *tmr, IN void *opaque)
{
    tmr->opaque = opaque;
}

tt_inline void tt_tmr_set_cb(IN tt_tmr_t *tmr, IN tt_tmr_cb_t cb)
{
    tmr->cb = cb;
}

tt_inline void tt_tmr_set_cbparam(IN tt_tmr_t *tmr, IN void *cb_param)
{
    tmr->cb_param = cb_param;
}

#endif /* __TT_TIMER__ */
