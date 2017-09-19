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
@file tt_io_event.h
@brief io event
*/

#ifndef __TT_IO_EVENT__
#define __TT_IO_EVENT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_double_linked_list.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_fiber_s;
struct epoll_event;

enum
{
    TT_IO_WORKER,
    TT_IO_POLLER,
    TT_IO_FS,
    TT_IO_SOCKET,
    TT_IO_IPC,
    TT_IO_TIMER,
    TT_IO_DNS,

    TT_IO_NUM
};
#define TT_IO_VALID(e) ((e) < TT_IO_NUM)

typedef struct tt_io_ev_s
{
    struct tt_fiber_s *src;
    struct tt_fiber_s *dst;
    tt_dnode_t node;
#if TT_ENV_OS_IS_WINDOWS
    union
    {
        OVERLAPPED ov;
        WSAOVERLAPPED wov;
    };
    tt_u32_t io_bytes;
    tt_result_t io_result;
#elif TT_ENV_OS_IS_LINUX || TT_ENV_OS_IS_ANDROID
    struct epoll_event *epev;
#endif
    tt_u16_t io;
    tt_u16_t ev;
} tt_io_ev_t;

typedef void (*tt_worker_io_t)(IN tt_io_ev_t *io_ev);

// return true if io is completed, either succeed or fail
typedef tt_bool_t (*tt_poller_io_t)(IN tt_io_ev_t *io_ev);

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif // __TT_IO_EVENT__
