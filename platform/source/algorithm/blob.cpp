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

#include <tt/algorithm/blob.h>

#include <misc/tt_assert.h>

namespace tt {

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

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

blob &blob::cat(const void *addr, size_t len)
{
    if (len == 0) { return *this; }

    size_t n = this->size();
    TT_OVERFLOW_IF(n + len < n, "cat overflow");
    check_len(n + len);

    uint8_t *p = new uint8_t[n + len];
    memcpy(p, addr_, n);
    if (addr != nullptr) { memcpy(p + n, addr, len); }
    return set(p, n + len, true);
}

void blob::do_set(void *addr, size_t len, bool owner)
{
    if (owner) {
        void *p = new uint8_t[len];
        if (addr != nullptr) { memcpy(p, addr, len); }
        assign(p, len, true);
    } else {
        assign(addr, len, false);
    }
}

void blob::do_move(blob &b)
{
    addr_ = b.addr_;
    len_ = b.len_;
    b.addr_ = nullptr;
    b.len_ = 0;
}

}
