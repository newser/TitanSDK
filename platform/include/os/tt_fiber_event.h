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
@file tt_fiber_event.h
@brief fiber event
*/

#ifndef __TT_FIBER_EVENT__
#define __TT_FIBER_EVENT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_double_linked_list.h>
#include <io/tt_io_event.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_FIBER_EV_CAST(fev, type) TT_PTR_INC(type, fev, sizeof(tt_fiber_ev_t))

#define TT_FIBER_EV_UNCAST(p)                                                  \
    TT_PTR_DEC(tt_fiber_ev_t, p, sizeof(tt_fiber_ev_t))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_fiber_s;
struct tt_tmr_s;

#if 0
typedef tt_fiber_ev_t
{
    struct tt_fiber_s *src;
    tt_dnode_t node;
    tt_u32_t ev;
} tt_fiber_ev_t;
#else
typedef tt_io_ev_t tt_fiber_ev_t;
#endif

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_fiber_ev_init(IN tt_fiber_ev_t *fev, IN tt_u32_t ev);

tt_export tt_fiber_ev_t *tt_fiber_ev_create(IN tt_u32_t ev, IN tt_u32_t size);

tt_export void tt_fiber_ev_destroy(IN tt_fiber_ev_t *fev);

tt_export void tt_fiber_send_ev(IN struct tt_fiber_s *dst,
                                IN tt_fiber_ev_t *fev,
                                IN tt_bool_t wait);

tt_export tt_fiber_ev_t *tt_fiber_recv_ev(IN struct tt_fiber_s *current,
                                          IN tt_bool_t wait);

tt_export void tt_fiber_finish(IN tt_fiber_ev_t *fev);

tt_export void tt_fiber_send_timer(IN struct tt_fiber_s *dst,
                                   IN struct tt_tmr_s *tmr);

tt_export struct tt_tmr_s *tt_fiber_recv_timer(IN struct tt_fiber_s *current,
                                               IN tt_bool_t wait);

#if 0
tt_inline tt_bool_t tt_fiber_recv(IN struct tt_fiber_s *current,
                                  IN tt_bool_t wait,
                                  OUT tt_fiber_ev_t **p_fev,
                                  OUT struct tt_tmr_s **p_tmr)
{
    *p_fev = tt_fiber_recv_ev(current, wait);
    *p_tmr = tt_fiber_recv_timer(current, wait);
    return TT_BOOL((*p_fev != NULL) || (*p_tmr != NULL));
}
#else
tt_export tt_bool_t tt_fiber_recv(IN struct tt_fiber_s *current,
                                  IN tt_bool_t wait,
                                  OUT tt_fiber_ev_t **p_fev,
                                  OUT struct tt_tmr_s **p_tmr);
#endif

#endif // __TT_FIBER_EVENT__
