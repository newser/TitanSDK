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
@file worker.h
@brief log def

this file define all basic types

*/

#ifndef __TT_IO_WORKER_CPP__
#define __TT_IO_WORKER_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/io/event.h>
#include <tt/os/spinlock.h>

#include <array>
#include <mutex>
#include <queue>
#include <thread>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

namespace tt::io {

class worker_group;

class worker
{
public:
    worker(worker_group &group): group_(group)
    {
        // be sure all members are well initialized
        thread_ = std::thread(&worker::routine, this);
    }

    bool exit();

private:
    void routine();

    io::ev &pop_ev();

    worker_group &group_;
    std::thread thread_;
};

class worker_group
{
public:
    void push_ev(io::ev &ev)
    {
        std::unique_lock<std::mutex> l(mutex_);
        ev_q_.push(&ev);
        cond_.notify_one();
    }

    io::ev &pop_ev()
    {
        std::unique_lock<std::mutex> l(mutex_);
        while (ev_q_.empty()) { cond_.wait(l); }

        io::ev *ev = ev_q_.front();
        assert(ev != nullptr);
        ev_q_.pop();
        return *ev;
    }

private:
    std::vector<worker> worker_;
    std::queue<io::ev *> ev_q_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

inline io::ev &worker::pop_ev()
{
    return group_.pop_ev();
}

}

#endif /* __TT_IO_WORKER_CPP__ */
