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
 @file tt_blob.h
 @brief blob

 this file blob
 */

#ifndef __TT_BLOB_CPP__
#define __TT_BLOB_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

extern "C" {
#include <memory/tt_memory_alloc.h>
}

#include <tt/misc/throw.h>
#include <tt/misc/util.h>

#include <string.h>

namespace tt {

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

class blob
{
    constexpr static size_t k_obit = ((size_t)1) << ((sizeof(size_t) << 3) - 1);

public:
    blob() = default;
    // - if owner is true, just set addr and len
    // - else if len is not 0, allocate memory and then copy if addr is not null
    blob(OPT void *addr, size_t len, bool owner)
    {
        check_len(len);
        do_set(addr, len, owner);
    }
    blob(const char *s, bool owner): blob((void *)s, strlen(s), owner) {}
    blob(blob &&b) { do_move(b); }
    ~blob() { clear(); }

    void *addr() const { return addr_; }
    size_t size() const { return len_ & ~k_obit; }
    bool is_owner() const { return len_ & k_obit; }
    bool empty() const { return (addr_ == nullptr) || (len_ == 0); }

    blob &set(OPT void *addr, size_t len, bool owner)
    {
        check_len(len);
        clear();
        do_set(addr, len, owner);
        return *this;
    }
    blob &set(const char *s, bool owner)
    {
        return set((void *)s, strlen(s), owner);
    }
    blob &cat(OPT const void *addr, size_t len);
    blob &clear()
    {
        if (is_owner()) { delete addr_; }
        addr_ = nullptr;
        len_ = 0;
        return *this;
    }
    blob &own()
    {
        if (!is_owner()) {
            void *p = addr();
            size_t n = size();
            do_set(p, n, true);
        }
        return *this;
    }

    blob &move(blob &b) { return move(std::move(b)); }
    blob &move(blob &&b)
    {
        clear();
        do_move(b);
        return *this;
    }

    blob &copy(const blob &b)
    {
        clear();
        do_set(b.addr(), b.size(), true);
        return *this;
    }

    ssize_t cmp(const void *addr, size_t len) const
    {
        size_t n = this->size();
        return n != len ? n - len : memcmp(addr_, addr, len);
    }
    ssize_t cmp(const char *s) const { return cmp(s, strlen(s)); }
    ssize_t cmp(const blob &b) const
    {
        return this == &b ? 0 : cmp(b.addr(), b.size());
    }

    bool operator==(const blob &b) const { return cmp(b); }
    bool operator==(const char *s) const { return cmp(s) == 0; }

    bool operator!=(const blob &b) const { return !(*this == b); }
    bool operator!=(const char *s) const { return !(*this == s); }

    bool operator<(const blob &b) const { return cmp(b) < 0; }
    bool operator<(const char *s) const { return cmp(s) < 0; }

    bool operator<=(const blob &b) const { return cmp(b) <= 0; }
    bool operator<=(const char *s) const { return cmp(s) <= 0; }

    bool operator>(const blob &b) const { return cmp(b) > 0; }
    bool operator>(const char *s) const { return cmp(s) > 0; }

    bool operator>=(const blob &b) const { return cmp(b) >= 0; }
    bool operator>=(const char *s) const { return cmp(s) >= 0; }

    blob &operator+=(const blob &b) { return cat(b.addr(), b.size()); }
    blob &operator+=(const char *s) { return cat(s, strlen(s)); }

private:
    uint8_t *addr_{nullptr};
    size_t len_{0};

    TT_NON_COPYABLE(blob)

    void check_len(size_t len)
    {
        TT_INVALID_ARG_IF(len >= k_obit, "too long blob");
    }
    void do_set(void *addr, size_t len, bool owner);
    void do_move(blob &b);
    void assign(void *addr, size_t len, bool owner)
    {
        addr_ = (uint8_t *)addr;
        len_ = len;
        if (owner) { set_owner(); }
    }
    void set_owner() { len_ |= k_obit; }
    void clear_owner() { len_ &= ~k_obit; }
};

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

}

#endif /* __TT_BLOB_CPP__ */
