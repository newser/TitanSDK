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
@file tt_event_base.h
@brief event common definitions

this file defines event common types
*/

#ifndef __TT_EVENT_BASE__
#define __TT_EVENT_BASE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <misc/tt_util.h>

#include <tt_event_base_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_ASSERT_EV TT_ASSERT

// make event by range and id
#define TT_EV_MKID(range, id) ((((range)&0xFF) << 24) | ((id)&0xFFFFFF))
#define TT_EV_MKID_SUB(range, sub_range, id)                                   \
    ((((range)&0xFF) << 24) | (((sub_range)&0xFF) << 16) | ((id)&0xFFFF))
#define TT_EV_MKID_SUBSUB(range, sub_range, subsub_range, id)                  \
    ((((range)&0xFF) << 24) | (((sub_range)&0xFF) << 16) |                     \
     (((subsub_range)&0xFF) << 8) | ((id)&0xFF))

// get event range
#define TT_EV_RANGE(ev_id) (((ev_id) >> 24) & 0xFF)
#define TT_EV_RANGE_SUB(ev_id) (((ev_id) >> 16) & 0xFF)
#define TT_EV_RANGE_SUBSUB(ev_id) (((ev_id) >> 8) & 0xFF)

// no event can begin with TT_EV_NULL(0xff)
#define TT_EV_NULL (~0)

#define TT_EV_HDR(ev, type) TT_PTR_INC(type, ev, sizeof(tt_ev_t))
#define TT_EV_OF(hdr) TT_PTR_DEC(tt_ev_t, hdr, sizeof(tt_ev_t))

#define TT_EV_DATA(ev, type)                                                   \
    TT_PTR_INC(type, ev, sizeof(tt_ev_t) + (ev)->hdr_size)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_ev_s;

enum
{
    // application event range definitions must be put before
    // TT_EV_RANGE_APP_END
    TT_EV_RANGE_APP_END,

    // id >= 0xC8000000 are for internal usage
    TT_EV_RANGE_INTERNAL = 200,

    TT_EV_RANGE_INTERNAL_SOCKET,
    TT_EV_RANGE_INTERNAL_FILE,
    TT_EV_RANGE_INTERNAL_DIR,
    TT_EV_RANGE_INTERNAL_EVPOLLER,
    TT_EV_RANGE_INTERNAL_ASYNC_DNS,
    TT_EV_RANGE_INTERNAL_SSL,
    TT_EV_RANGE_INTERNAL_IPC,

    TT_EV_RANGE_MAX = 255
};

typedef tt_result_t (*tt_ev_on_copy_t)(IN struct tt_ev_s *dst,
                                       IN struct tt_ev_s *src);

typedef void (*tt_ev_on_destroy_t)(IN struct tt_ev_s *ev);

typedef struct
{
    tt_ev_on_copy_t on_copy;
    tt_ev_on_destroy_t on_destroy;
} tt_ev_itf_t;

// | ev | hdr | data |
typedef struct tt_ev_s
{
    tt_u8_t hdr_size;
    // indicates whether the event is to be transferred
    // between threads
    tt_bool_t inter_thread : 1;
    tt_u16_t reserved_u16;

    tt_u32_t ev_id;
    // data_size is mainly used to copy event
    tt_u32_t data_size;
    tt_ev_itf_t *itf;
} tt_ev_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_ev_t *tt_ev_create(IN tt_u32_t ev_id,
                             IN tt_u8_t hdr_size,
                             IN tt_u32_t data_size,
                             IN tt_ev_itf_t *itf);

extern void tt_ev_destroy(IN tt_ev_t *ev);

extern tt_ev_t *tt_ev_copy(IN tt_ev_t *ev);

#endif /* __TT_EVENT_BASE__ */
