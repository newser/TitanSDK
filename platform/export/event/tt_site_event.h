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
@file tt_site_event.h
@brief inter site event

this file defines inter site event APIs
*/

#ifndef __TT_SITE_EVENT__
#define __TT_SITE_EVENT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <event/tt_event_base.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_evpoller_s;

// callback when received inter site event
typedef tt_result_t (*tt_evc_on_site_ev_t)(IN struct tt_evpoller_s *evp,
                                           IN tt_ev_t *sev);

typedef struct
{
    tt_evc_on_site_ev_t on_site_ev;
} tt_site_ev_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_site_ev_attr_default(OUT tt_site_ev_attr_t *attr);

tt_inline tt_ev_t *tt_site_ev_create(IN tt_u32_t ev_id, IN tt_u32_t data_size)
{
    return tt_ev_create(ev_id, 0, data_size, NULL);
}

#endif /* __TT_SITE_EVENT__ */
