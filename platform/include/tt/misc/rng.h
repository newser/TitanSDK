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

#include <tt/init/component.h>
#include <tt/misc/util.h>

#include <rng_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

namespace tt {

namespace init {

class rng: public component
{
public:
    static rng &instance() { return s_instance; }

private:
    static rng s_instance;

    rng(): component(component::e_rng, "random number generator") {}

    bool do_start(void *reserved) override;
    void do_stop() override;
};

}

enum rng_alg
{
    e_xorshift,
};

template<rng_alg t_alg = e_xorshift>
class rng
{
};

template<>
class rng<e_xorshift>
{
public:
    rng()
    {
        if (!native::rng(&s_, sizeof(s_[0]))) {
            s_[0] = (rand() << 4) | rand();
        }
        if (!native::rng(&s_, sizeof(s_[1]))) {
            s_[1] = (rand() << 4) | rand();
        }
    }

    uint64_t next()
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

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

}

#endif /* __TT_RNG_CPP__ */
