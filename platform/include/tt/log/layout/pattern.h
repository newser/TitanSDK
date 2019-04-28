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
@file pattern.h
 @brief log layout: pattern

this file define all basic types

*/

#ifndef __TT_LOG_LAYOUT_PATTERN_CPP__
#define __TT_LOG_LAYOUT_PATTERN_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/log/layout.h>
#include <tt/log/layout/placeholder.h>

#include <list>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

namespace tt {

namespace log {

namespace layout {

class pattern: public i_layout
{
public:
    pattern() = default;

    bool parse(const char *format, size_t len);
    bool parse(const char *format) { return parse(format, strlen(format)); }

    void render(const entry &e, OUT buf &b) const override;

private:
    std::unique_ptr<char[]> format_;
    size_t len_{0}; // includes ending 0
    std::list<std::unique_ptr<placeholder>> placeholders_;
};

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

}

}

}

#endif /* __TT_LOG_LAYOUT_PATTERN_CPP__ */
