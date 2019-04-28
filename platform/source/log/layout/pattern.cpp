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

#include <tt/log/layout/pattern.h>

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

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

bool pattern::parse(const char *format, size_t len)
{
    format_.reset(new char[len + 1]);
    len_ = 0;
    placeholders_.clear();

    const char *p_start = NULL;
    const char *seg = format;
    size_t seg_len = 0;
    char *o_pos = format_.get();

#define __PP_INIT 0
#define __PP_DOLLER 1 // saw "$"
#define __PP_PLACEHOLDER 2 // saw "${"
    int state = __PP_INIT;

    const char *pos = format;
    char c;
    while ((c = *pos) != 0) {
        switch (c) {
        case '$': {
            if (state == __PP_INIT) {
                // "$"
                state = __PP_DOLLER;
            }
        } break;
        case '{': {
            if (state == __PP_DOLLER) {
                // "${"
                state = __PP_PLACEHOLDER;
                p_start = pos;
            }
        } break;
        case '}': {
            if (state == __PP_DOLLER) {
                // "$}"
                state = __PP_INIT;
            } else if (state == __PP_PLACEHOLDER) {
                // "${...}"
                state = __PP_INIT;

                // step 1: copy segment
                //  each field is converted to a '\0', so pattern:
                //  "aaa${field1}bb${field2}${field3}\0" is converted
                //  to "aaa\0bb\0\0\0"
                assert(p_start >= (seg + 1));
                seg_len = p_start - seg - 1;

                memcpy(o_pos, seg, seg_len);
                o_pos += seg_len;
                *o_pos++ = 0;
                len_ += seg_len + 1;

                seg = (pos + 1);

                // step 2: create a placeholder
                assert(pos > p_start);
                placeholder *p = placeholder::parse(p_start, pos - p_start + 1);
                if (p == nullptr) { return false; }
                placeholders_.emplace_back(p);
            }
        } break;

        default: {
            if (state == __PP_DOLLER) { state = __PP_INIT; }
        } break;
        }

        ++pos;
    }

    if (state == __PP_PLACEHOLDER) {
        // partial field: ${...\0
        return false;
    }

    seg_len = pos - seg;

    memcpy(o_pos, seg, seg_len);
    o_pos += seg_len;
    *o_pos++ = 0;
    len_ += seg_len + 1; // do not include last 0 ??
    assert(len_ <= len + 1);

    return true;
}

void pattern::render(const entry &e, OUT buf &b) const
{
    const char *pos = format_.get();
    const char *prev_pos = pos;
    const char *end = pos + len_;

    auto iter = placeholders_.cbegin();

    while (pos < end) {
        if (*pos != 0) {
            ++pos;
            continue;
        }

        b.write(prev_pos, pos - prev_pos);
        ++pos;
        prev_pos = pos;

        if (iter != placeholders_.cend()) {
            (*iter)->render(e, b);
            ++iter;
        }
    }
    b.write(prev_pos, pos - prev_pos);
}

}

}

}
