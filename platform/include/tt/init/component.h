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
@file component.h
@brief all basic type definitions

this file define all basic types

*/

#ifndef __TT_INIT_COMPONENT_CPP__
#define __TT_INIT_COMPONENT_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/misc/throw.h>
#include <tt/misc/util.h>

#include <cassert>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

namespace tt {

class component_mgr;

class component
{
    friend class component_mgr;

protected:
    enum
    {
        e_log,
        cid_num
    };

    component(int cid, const char *name): name_(name), cid_(cid)
    {
        TT_INVALID_ARG_IF(name == nullptr, "null name");
        TT_INVALID_ARG_IF(cid_ >= cid_num, "invalid component id");
    }

    ~component() { assert(state_ == e_stopped); }

    virtual bool do_start(void *reserved) = 0;
    virtual void do_stop() = 0;

    const char *name_;
    int cid_;

private:
    bool start(void *reserved = nullptr)
    {
        assert(state_ == e_stopped);
        if (do_start(reserved)) {
            state_ = e_started;
            return true;
        } else {
            return false;
        }
    }
    void stop()
    {
        assert(state_ == e_started);
        do_stop();
        state_ = e_stopped;
    }

    enum state
    {
        e_stopped,
        e_started,
    } state_{e_stopped};

    TT_NON_COPYABLE(component);
};

class component_mgr
{
public:
    static component_mgr &instance() { return s_instance; }

    bool start(void *reserved = nullptr);
    void stop();

private:
    static component_mgr s_instance;

    component_mgr();

    component *components_[component::cid_num];

    TT_NON_COPYABLE(component_mgr);
};

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

}

#endif /* __TT_INIT_COMPONENT_CPP__ */
