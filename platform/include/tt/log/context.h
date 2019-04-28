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
@file context.h
@brief log context

this file define all basic types

*/

#ifndef __TT_LOG_CONTEXT_CPP__
#define __TT_LOG_CONTEXT_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/log/def.h>
#include <tt/log/io.h>
#include <tt/log/layout.h>
#include <tt/misc/buffer.h>
#include <tt/misc/rollback.h>

#include <list>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

namespace tt {

namespace log {

class ctx
{
public:
    ctx() = default;

    ctx &layout(std::shared_ptr<i_layout> &layout)
    {
        layout_ = layout;
        return *this;
    }
    ctx &layout(std::shared_ptr<i_layout> &&layout)
    {
        layout_ = layout;
        return *this;
    }

    ctx &append_io(std::shared_ptr<i_io> &io)
    {
        io_.emplace_back(io);
        return *this;
    }
    ctx &append_io(std::shared_ptr<i_io> &&io)
    {
        io_.emplace_back(io);
        return *this;
    }

    void write(const entry &e)
    {
        TT_BAD_CALL_IF(layout_ == nullptr);

        buf_.clear();
        layout_->render(e, buf_);
        buf_.write((uint8_t)0);

        for (auto i : io_) { i->write(buf_.r(), buf_.r_size()); }
    }

private:
    std::shared_ptr<i_layout> layout_{nullptr};
    std::list<std::shared_ptr<i_io>> io_;
    buf buf_;
};

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

}

}

#endif /* __TT_LOG_CONTEXT_CPP__ */
