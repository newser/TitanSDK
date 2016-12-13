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
@file tt_event_poller_native.h
@brief event poller port layer

this file defines event poller port layer APIS
*/

#ifndef __TT_EVENT_POLLER_NATIVE__
#define __TT_EVENT_POLLER_NATIVE__

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

struct tt_evpoller_s;
struct tt_evc_attr_s;

typedef struct
{
    tt_u32_t reserved;
} tt_evpoller_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_evp_create_ntv(IN tt_evpoller_ntv_t *evp,
                                     IN struct tt_evc_attr_s *evc_attr);

extern tt_result_t tt_evp_destroy_ntv(IN tt_evpoller_ntv_t *evp);

/**
 @return
 - TT_SUCCESS, go ahead
 - TT_FAIL, error occurred and evp should stop
 */
extern tt_result_t tt_evp_poll_ntv(IN struct tt_evpoller_s *evp,
                                   IN tt_s64_t wait_ms);

#endif /* __TT_EVENT_POLLER_NATIVE__ */
