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
@file fiber.h
@brief log def

this file define all basic types

*/

#ifndef __TT_OS_FIBER_CPP__
#define __TT_OS_FIBER_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

extern "C" {
#include <misc/tt_util.h>
}

#include <tt/container/list.h>
#include <tt/misc/error.h>
#include <tt/native/fcontext/fcontext_boost.h>

#include <any>
#include <functional>
#include <queue>
#include <tuple>

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
class ev;
}

class fiber_mgr;

class fiber
{
    friend class fiber_mgr;

    static constexpr size_t k_stack_size = 1 << 14; // 16k

public:
    template<typename R, typename... P>
    static fiber &create(R &&routine, P &&... params);

    static fiber &current();

    static void yield_current();
    static void suspend_current();
    static void resume_to(fiber &new_fb, bool suspend_current);
    static fiber *find(const char *name);

    void yield() { do_yield(*this, false); }
    void suspend() { do_yield(*this, true); }
    void resume(bool suspend_current) { resume_to(*this, suspend_current); }

    // current_fb must be current fb
    void resume_cautious(fiber &current_fb, bool suspend_current);

    void remove() { node.remove(); }

    const char *name() const { return name_; }
    void name(const char *name) { name_ = name; }

    std::any run() { return routine_->run(); }

    fiber_mgr &mgr() { return *mgr_; }

    // fiber does not need to know what poller is, just
    // keep a pointer
    native::poller *poller();

    void push_ev(io::ev &ev) { ev_.push(&ev); }

    native::fctx &fctx() { return fctx_; }

    bool can_yield() const { return can_yield_; };

    bool end() const { return end_; }
    void end(bool b) { end_ = b; }

    bool recving() const { return recving_; }
    void recving(bool b) { recving_ = b; }

    bool operator==(const fiber &rhs) const
    {
        return (void *)this == (void *)&rhs;
    }
    bool operator!=(const fiber &rhs) const { return !(*this == rhs); }

protected:
    struct i_routine
    {
        virtual ~i_routine(){};
        virtual std::any run() = 0;
    };

    template<typename R, typename... P>
    struct routine: public i_routine
    {
        explicit routine(R &&r, P &&... params):
            routine_(std::forward<R>(r)), params_(std::forward<P>(params)...)
        {
        }

        std::any run() override
        {
            if constexpr (std::is_void<decltype(
                              std::apply(routine_, params_))>::value) {
                std::apply(routine_, params_);
                result_.reset();
            } else {
                result_ = std::apply(routine_, params_);
            }
            return result_;
        }

        R routine_;
        std::tuple<P...> params_;
        std::any result_;
    };

    static void do_yield(fiber &cur, bool suspend_cur);
    static void do_resume(fiber &cur, fiber &new_fb, bool suspend_cur);

    fiber(fiber_mgr &mgr): mgr_(&mgr) {}

    template<typename R, typename... P>
    fiber(fiber_mgr &mgr, size_t stack_size, R &&routine, P &&... params);

    lnode node;
    const char *name_{nullptr};
    std::unique_ptr<i_routine> routine_{nullptr};
    fiber_mgr *mgr_{nullptr};
    std::queue<io::ev *> ev_;
    std::queue<int> unexpired_tmr_;
    std::queue<int> expired_tmr_;
    native::fctx fctx_;
    bool can_yield_{true};
    bool end_{false};
    bool recving_{false};

    TT_NON_COPYABLE(fiber)
};

class fiber_mgr
{
public:
    static fiber_mgr &current()
    {
        thread_local fiber_mgr fb_mgr;
        return fb_mgr;
    }

    bool empty() const { return active_.empty() && pending_.empty(); }

    bool current_is_main() const { return current_ == &main_; }

    fiber *find(const char *name) const;

    fiber &next()
    {
        if (!active_.empty()) {
            return *TT_CONTAINER(active_.head(), fiber, node);
        } else {
            return main_;
        }
    }

    void push_head(bool active, fiber &fb)
    {
        (active ? active_ : pending_).push_head(fb.node);
    }
    void push_tail(bool active, fiber &fb)
    {
        (active ? active_ : pending_).push_tail(fb.node);
    }

    fiber &main() { return main_; }
    bool is_main(const fiber &fb) const { return &fb == &main_; }

    fiber &current_fiber() { return *current_; }
    void current_fiber(fiber &fb) { current_ = &fb; }

    native::poller *poller() { return poller_; }
    void poller(native::poller *p) { poller_ = p; }

private:
    list active_;
    list pending_;
    fiber *current_{&main_};
    native::poller *poller_{nullptr};
    fiber main_{*this};
};

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

template<typename R, typename... P>
fiber &fiber::create(R &&routine, P &&... params)
{
    return *new fiber(fiber_mgr::current(), k_stack_size,
                      std::forward<R>(routine), std::forward<P>(params)...);
}

inline fiber &fiber::current()
{
    return fiber_mgr::current().current_fiber();
}

inline void fiber::yield_current()
{
    do_yield(fiber::current(), false);
}

inline void fiber::suspend_current()
{
    do_yield(fiber::current(), true);
}

inline void fiber::resume_to(fiber &new_fb, bool suspend_current)
{
    do_resume(fiber::current(), new_fb, suspend_current);
}

inline void fiber::resume_cautious(fiber &current_fb, bool suspend_current)
{
    assert(&current_fb == &fiber::current());
    do_resume(current_fb, *this, suspend_current);
}

inline fiber *fiber::find(const char *name)
{
    return fiber_mgr::current().find(name);
}

template<typename R, typename... P>
fiber::fiber(fiber_mgr &mgr, size_t stack_size, R &&routine, P &&... params):
    mgr_(&mgr), fctx_(stack_size)
{
    routine_.reset(new fiber::routine<R, P...>(std::forward<R>(routine),
                                               std::forward<P>(params)...));

    mgr_->push_tail(true, *this);
}

inline native::poller *fiber::poller()
{
    return mgr_ != nullptr ? mgr_->poller() : nullptr;
}

}

#endif /* __TT_OS_FIBER_CPP__ */
