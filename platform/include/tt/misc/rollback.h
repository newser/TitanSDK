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
@file rollback.h
@brief all basic type definitions

this file define all basic types

*/

#ifndef __TT_ROLLBACK_CPP__
#define __TT_ROLLBACK_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/misc/util.h>

#include <utility>

namespace tt {

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

template<typename T>
class rollback
{
public:
    rollback(T &&f): f_(std::forward<T>(f)) {}
    rollback(rollback &&r): f_(std::forward<T>(r.f_)), dismissed_(r.dismissed_)
    {
    }

    ~rollback()
    {
        if (!dismissed_) { f_(); }
    }

    void dismiss() { dismissed_ = true; }

private:
    T f_;
    bool dismissed_{false};

    rollback() = delete;
    TT_NON_COPYABLE(rollback)
};

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

template<typename T>
inline rollback<T> make_rollback(T &&f)
{
    return rollback<T>(std::forward<T>(f));
}

}

#endif /* __TT_ROLLBACK_CPP__ */
