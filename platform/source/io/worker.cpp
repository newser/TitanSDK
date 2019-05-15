/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/io/worker.h>

#include <tt/io/poller.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

namespace tt::io {

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

worker_group g_worker_group;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void worker::routine()
{
    while (1) {
        io::ev &ev = pop_ev();
        bool end = ev.work();
        ev.work_done();

#if 0
        if (!ev.del()) {
            native::poller *poller = ev.src_poller();
            assert(poller != nullptr);
            poller->worker_done(ev);
        } else {
            delete &ev;
        }
#endif
        if (ev.del()) { delete &ev; }
        if (end) { break; }
    }
}

worker_group::~worker_group()
{
    for (int i = 0; i < worker_.size(); ++i) { go_home(); }
    worker_.clear();
}

void worker_group::push_ev(io::ev &ev)
{
    int pending;
    {
        std::unique_lock<std::mutex> l(mutex_);
        // must get pending ev count before pushing
        pending = ev_q_.size();
        ev_q_.push(&ev);
        cond_.notify_one();
    }

    if ((worker_.empty() || pending >= max_pending_) &&
        (worker_.size() < max_worker_)) {
        go_work();
    }
}

io::ev &worker_group::pop_ev()
{
    std::unique_lock<std::mutex> l(mutex_);
    while (ev_q_.empty()) { cond_.wait(l); }

    io::ev *ev = ev_q_.front();
    assert(ev != nullptr);
    ev_q_.pop();
    return *ev;
}

void worker_group::go_home()
{
    struct worker_exit: public io::ev_static
    {
        bool work() override { return true; }
    };
    static worker_exit s_worker_exit;

    push_ev(s_worker_exit);
}

}
