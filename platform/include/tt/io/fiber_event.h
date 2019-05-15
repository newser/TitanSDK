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
@file fiber_event.h
@brief log def

this file define all basic types

*/

#ifndef __TT_IO_FIBER_EVENT_CPP__
#define __TT_IO_FIBER_EVENT_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/io/event.h>

#include <tt/native/poller.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

namespace tt {

template<bool t_wait>
class fiber_ev: public io::ev
{
public:
    fiber_ev(uint32_t id): io::ev(false, id) { src_fiber(&fiber::current()); }

    fiber_ev(uint32_t id, fiber &src): io::ev(false, id) { src_fiber(&src); }

    bool handle(native::poller &p) override
    {
        // by destination poller
        fiber &dst = *dst_fiber();
        assert(&dst.mgr() == &fiber_mgr::current());
        dst.push_ev(*this);
        if (dst.recving()) {
            assert(dst.mgr().current_is_main());
            dst.resume_cautious(dst.mgr().main(), false);
        }
        return false;
    }

    void to(fiber &dst)
    {
        fiber &src = *src_fiber();
        assert(&src.mgr().current_fiber() == &src);
        if (&src.mgr() == &dst.mgr()) {
            dst.push_ev(*this);
            if (dst.recving()) {
                dst.resume_cautious(src, t_wait);
            } else if constexpr (t_wait) {
                src.suspend();
            }
        } else {
            // cross thread fiber event, be sure dst.poller()
            // is active
            dst_fiber(&dst);
            dst.poller()->recv(*this);
            if constexpr (t_wait) { dst.suspend(); }
        }
    }

    void done()
    {
        // by destination fiber

        if constexpr (!t_wait) {
            delete this;
            return;
        }

        fiber &src = *src_fiber();
        fiber &dst = *dst_fiber();
        assert(&src.mgr().current_fiber() == &dst);
        if (&src.mgr() == &dst.mgr()) {
            // tell src that dst is done
            src.resume_cautious(dst, false);
        } else {
            src.poller()->fiber_done(*this);
        }
    }

    fiber *dst_fiber() { return dst_fiber_; }
    void dst_fiber(fiber *f) { dst_fiber_ = f; }

private:
    fiber *dst_fiber_{nullptr};
};

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

}

#endif /* __TT_IO_FIBER_EVENT_CPP__ */
