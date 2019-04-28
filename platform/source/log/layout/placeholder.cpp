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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/log/layout/placeholder.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

namespace tt {

namespace log {

namespace layout {

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

const char *level::s_level[level_num] = {
    "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

placeholder *placeholder::parse(const char *p, size_t len)
{
    assert(len >= 2 && p[0] == '{' && p[len - 1] == '}');
    ++p;
    len -= 2;

#define __IS_PH(s) strncmp(p, s, sizeof(s) - 1) == 0
#define __SKIP_PH(s)                                                           \
    p += sizeof(s) - 1;                                                        \
    len -= sizeof(s) - 1;                                                      \
    while (len > 0 && std::isspace(*p)) {                                      \
        ++p;                                                                   \
        --len;                                                                 \
    }                                                                          \
    if (len > 0 && *p == ':') {                                                \
        ++p;                                                                   \
        --len;                                                                 \
    }

    if (__IS_PH("logger")) {
        __SKIP_PH("logger")
        return new logger(p, len);
    } else if (__IS_PH("function")) {
        __SKIP_PH("function")
        return new function(p, len);
    } else if (__IS_PH("content")) {
        __SKIP_PH("content")
        return new content(p, len);
    } else if (__IS_PH("seqno")) {
        __SKIP_PH("seqno")
        return new seqno(p, len);
    } else if (__IS_PH("line")) {
        __SKIP_PH("line")
        return new line(p, len);
    } else if (__IS_PH("level")) {
        __SKIP_PH("level")
        return new level(p, len);
    } else {
        return nullptr;
    }
}

}

}

}
