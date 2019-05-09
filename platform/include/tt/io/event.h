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
@file event.h
@brief log def

this file define all basic types

 level 1: io::ev, poller, worker
 level 0: fiber, fiber_mgr
*/

#ifndef __TT_IO_EVENT_CPP__
#define __TT_IO_EVENT_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <tt/container/double_linked_list.h>
#include <tt/misc/error.h>
#include <tt/os/fiber.h>

#include <functional>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

namespace tt {

namespace native {
class poller;
}

namespace io {

enum
{
    e_poller,
    e_worker,
    e_fiber,

    io_num
};

class ev
{
public:
    ev(uint32_t io, uint32_t id): id_(id), io_(io) { assert(io < io_num); };
    virtual ~ev() {}

    // [ del, end ]
    virtual std::pair<bool, bool> handle(native::poller &p) = 0;

    // [end], work() would be executed by worker thread
    virtual bool work() { return false; }

    fiber *src_fiber() { return fiber_; }
    void src_fiber(fiber *f) { fiber_ = f; }

    native::poller *src_poller()
    {
        return fiber_ != nullptr ? fiber_->poller() : nullptr;
    }

    enum err::code err() const { return err_.code(); }
    void err(enum err::code code) { err_ = code; }

    uint32_t io() const { return io_; }
    void io(uint32_t io)
    {
        assert(io < io_num);
        io_ = io;
    }
    uint32_t id() const { return id_; }
    void id(uint32_t id) { id_ = id; }
    bool is(uint32_t io, uint32_t id) const { return io_ == io && id_ == id; }

private:
    fiber *fiber_{nullptr};
#if TT_ENV_OS_IS_WINDOWS
    union
    {
        OVERLAPPED ov;
        WSAOVERLAPPED wov;
    } u;
    uint32_t io_bytes{0};
#elif TT_ENV_OS_IS_LINUX || TT_ENV_OS_IS_ANDROID
    struct epoll_event *epev{nullptr};
#endif
    class err err_ = err::e_ok;
    uint32_t id_;
    uint16_t io_;
#if TT_ENV_OS_IS_MACOS || TT_ENV_OS_IS_IOS

public:
    uint16_t kev_flags() const { return kev_flags_; }
    void kev_flags(uint16_t f) { kev_flags_ = f; }

private:
    uint16_t kev_flags_{0};

#endif
};

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

}

}

#endif /* __TT_IO_EVENT_CPP__ */
