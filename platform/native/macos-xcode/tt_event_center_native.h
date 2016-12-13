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
@file tt_event_center_native.h
@brief event center native

this file defines event center native APIs
*/

#ifndef __TT_EVENT_CENTER_NATIVE__
#define __TT_EVENT_CENTER_NATIVE__

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

struct tt_evcenter_s;
struct tt_evc_attr_s;

typedef struct tt_evcenter_ntv_s
{
    int kq_fd;

    tt_u32_t ev_mark_thread_ev;
} tt_evcenter_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_evc_create_ntv(IN tt_evcenter_ntv_t *sys_evc,
                                     IN struct tt_evc_attr_s *attr);

extern tt_result_t tt_evc_destroy_ntv(IN tt_evcenter_ntv_t *sys_evc);

extern tt_result_t tt_evc_sendto_thread_ntv(IN struct tt_evcenter_s *dst_evc,
                                            IN tt_ev_t *ev);

extern tt_ev_t *tt_evc_recvfrom_thread_ntv(IN struct tt_evcenter_s *evc);

#endif /* __TT_EVENT_CENTER_NATIVE__ */
