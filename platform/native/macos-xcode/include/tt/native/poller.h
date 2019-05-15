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
@file poller.h
@brief all basic type definitions

this file define all basic types

*/

#ifndef __TT_NATIVE_POLLER_CPP__
#define __TT_NATIVE_POLLER_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <tt/os/spinlock.h>

#include <list>
#include <mutex>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

namespace tt::io {
class ev;
}

namespace tt::native {

class poller
{
    // larger k_kev_num could save time for calling kevent, but may
    // impact timer accuracy and vice vesa
    static constexpr int k_kev_num = 1;

public:
    static poller *current();

    poller() = default;
    ~poller();

    bool init();
    bool run(uint64_t wait_ms);
    bool exit();

    bool worker_done(io::ev &ev)
    {
        {
            std::scoped_lock<tt::spinlock> s(worker_lock_);
            worker_ev_.push_back(&ev);
        }
        return wakeup_worker();
    }

    // same as worker_done(), just resume ev->src_fiber()
    bool fiber_done(io::ev &ev) { return worker_done(ev); }

    void get_poller_ev(OUT std::list<io::ev *> &l)
    {
        std::scoped_lock s(poller_lock_);
        l.splice(l.end(), poller_ev_);
    }

    void get_worker_ev(OUT std::list<io::ev *> &l)
    {
        std::scoped_lock s(worker_lock_);
        l.splice(l.end(), worker_ev_);
    }

    bool recv(io::ev &ev)
    {
        {
            std::scoped_lock<tt::spinlock> s(poller_lock_);
            poller_ev_.push_back(&ev);
        }
        return wakeup_poller();
    }

private:
    bool wakeup_poller();
    bool wakeup_worker();

    std::list<io::ev *> poller_ev_;
    std::list<io::ev *> worker_ev_;
    int kq_{-1};
    spinlock poller_lock_;
    spinlock worker_lock_;
};

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

}

#endif /* __TT_NATIVE_POLLER_CPP__ */
