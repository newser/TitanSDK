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
        thread_ = std::thread(&worker::routine, std::ref(*this));
    }

    ~worker() { thread_.join(); }

private:
    void routine();

    io::ev &pop_ev();

    worker_group &group_;
    std::thread thread_;
};

class worker_group
{
    static constexpr int k_ratio = 2;
    static constexpr int k_max_pending = 1;

public:
    worker_group():
        min_worker_(0),
        max_worker_(std::thread::hardware_concurrency() * k_ratio),
        max_pending_(k_max_pending)
    {
        assert(min_worker_ <= max_worker_);
    }

    worker_group(int min_worker, int max_worker,
                 int max_pending = k_max_pending):
        min_worker_(min_worker),
        max_worker_(max_worker), max_pending_(max_pending)
    {
        assert(min_worker_ <= max_worker_);
    }

    ~worker_group();

    void push_ev(io::ev &ev);
    io::ev &pop_ev();

private:
    void go_work() { worker_.emplace_back(new worker(*this)); }
    void go_home();

    std::vector<std::unique_ptr<worker>> worker_;
    std::queue<io::ev *> ev_q_;
    std::mutex mutex_;
    std::condition_variable cond_;
    int min_worker_;
    int max_worker_;
    int max_pending_;
};

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export worker_group g_worker_group;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

inline io::ev &worker::pop_ev()
{
    return group_.pop_ev();
}

}

#endif /* __TT_IO_WORKER_CPP__ */
