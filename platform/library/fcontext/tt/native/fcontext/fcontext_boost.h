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
@file fcontext_boost.h
@brief fcontext boost_handle
*/

#ifndef __TT_FCONTEXT_BOOST_CPP__
#define __TT_FCONTEXT_BOOST_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/misc/macro.h>

#include <tt_fcontext.h>

#include <memory>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

namespace tt {

class fiber;
class fiber_mgr;

namespace native {

class fctx_boost
{
public:
    static void jump(fiber_mgr &fb_mgr, fiber &from, fiber &to);

    fctx_boost() = default;
    fctx_boost(size_t stack_size)
    {
        assert(stack_size > 0);
        size_ = stack_size;
        stack_ = new uint8_t[size_] + size_;
        boost_handle_ = tt_make_fcontext(stack_, size_, routine);
    }
    ~fctx_boost()
    {
        if (size_ > 0) { delete ((uint8_t *)stack_ - size_); }
    }

    fiber *from() const { return from_; }
    void from(fiber &fb) { from_ = &fb; }

    tt_fcontext_t boost_handle() const { return boost_handle_; }
    void boost_handle(tt_fcontext_t boost_handle)
    {
        boost_handle_ = boost_handle;
    }

private:
    static void routine(tt_transfer_t t);

    fiber *from_{nullptr};
    void *stack_{nullptr};
    size_t size_{0};
    tt_fcontext_t boost_handle_{nullptr};

    TT_NON_COPYABLE(fctx_boost)
};

using fctx = fctx_boost;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

}

}

#endif /* __TT_FCONTEXT_BOOST_CPP__ */
