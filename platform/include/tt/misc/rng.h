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

/**
@file rng.h
@brief hash

*/

#ifndef __TT_RNG_CPP__
#define __TT_RNG_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

extern "C" {
#include <misc/tt_util.h>
}

#include <random>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

namespace tt::rng {

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

class xorshift
{
public:
    using result_type = uint64_t;

    xorshift()
    {
        static_assert(sizeof(std::random_device::result_type) == 4);

        thread_local std::random_device rd;
        s_[0] = (rd() << 4) | rd();
        s_[1] = (rd() << 4) | rd();
    }

    uint64_t operator()()
    {
        uint64_t s1 = s_[0];
        const uint64_t s0 = s_[1];

        s_[0] = s0;
        s1 ^= s1 << 23;
        return (s_[1] = (s1 ^ s0 ^ (s1 >> 17) ^ (s0 >> 26))) + s0;
    }

private:
    uint64_t s_[2];
};

template<typename T>
T &engine()
{
    thread_local T s_engine(std::random_device{}());
    return s_engine;
}

template<>
inline xorshift &engine<xorshift>()
{
    thread_local xorshift s_engine;
    return s_engine;
}

// template<typename T = std::mt19937, typename V = typename T::result_type>
template<typename T = xorshift, typename V = typename T::result_type>
V next()
{
    T &e = engine<T>();
    return e();
}

}

#endif /* __TT_RNG_CPP__ */
