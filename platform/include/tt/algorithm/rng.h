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

namespace tt {

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

namespace init {

class rng: public component
{
public:
    static rng &instance() { return s_instance; }

private:
    static rng s_instance;

    rng(): component(component::rng, "random number generator") {}

    bool do_start(void *reserved) override;
    void do_stop() override;
};

}

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

}

#endif /* __TT_RNG_CPP__ */
