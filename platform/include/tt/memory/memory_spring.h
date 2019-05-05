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
@file tt_memory_spring.h
@brief memory spring APIs

APIs to extend/compress memory
*/

#ifndef __TT_MEMORY_SPRING_CPP__
#define __TT_MEMORY_SPRING_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/misc/throw.h>
#include <tt/misc/util.h>

#include <cassert>
#include <string.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

namespace tt {

template<size_t t_init = 6, size_t t_high = 12, size_t t_max = 0>
class memspg
{
public:
    static constexpr size_t k_init_size = (1 << t_init);
    static constexpr size_t k_high_size = (1 << t_high);
    static constexpr size_t k_max_size = t_max == 0 ? 0 : (1 << t_max);

    static size_t align_size(size_t size)
    {
        if (size <= k_init_size) {
            return k_init_size;
        } else if (size < k_high_size) {
            size_t s = k_high_size;
            while (s >= size) { s >>= 1; }
            return s << 1;
        } else {
            if ((size & (k_high_size - 1)) != 0) {
                size += k_high_size;
                size &= ~(k_high_size - 1);
            }
            return size;
        }
    }

    memspg() = default;
    memspg(memspg &&m)
    {
        if (m.addr_ == m.internal_mem_) {
            addr_ = internal_mem_;
            memcpy(addr_, m.addr_, k_init_size);
        } else {
            addr_ = m.addr_;
        }
        size_ = m.size_;
        m.addr_ = m.internal_mem_;
        m.size_ = k_init_size;
    }
    ~memspg()
    {
        if (addr_ != internal_mem_) { delete addr_; }
    }

    void *addr() const { return addr_; }
    size_t size() const { return size_; }

    void *resize(size_t new_size, size_t from, size_t len, size_t to);
    void *resize(size_t new_size, size_t from, size_t len)
    {
        return resize(new_size, from, len, from);
    }
    void *resize(size_t new_size) { return resize(new_size, 0, size_); }
    void *resize_discard(size_t new_size) { return resize(new_size, 0, 0); }

    void *more(size_t more_bytes)
    {
        TT_OVERFLOW_IF(size_ + more_bytes < size_, "new size overflow");
        return resize(size_ + more_bytes);
    }
    void *more() { return resize(align_size(size_ + 1)); }
    void *less(size_t less_bytes)
    {
        size_t new_size = size_ < less_bytes ? 0 : (size_ - less_bytes);
        return resize(new_size);
    }

    void swap(memspg &m)
    {
        if (this == &m) { return; }

        std::swap(addr_, m.addr_);
        std::swap(size_, m.size_);

        if (addr_ == m.internal_mem_ || m.addr_ == internal_mem_) {
            uint8_t tmp[k_init_size];
            memcpy(tmp, internal_mem_, k_init_size);
            memcpy(internal_mem_, m.internal_mem_, k_init_size);
            memcpy(m.internal_mem_, tmp, k_init_size);

            if (addr_ == m.internal_mem_) { addr_ = internal_mem_; }
            if (m.addr_ == internal_mem_) { m.addr_ = m.internal_mem_; }
        }
    }

protected:
    uint8_t *addr_{internal_mem_};
    size_t size_{k_init_size};
    uint8_t internal_mem_[k_init_size];

    TT_NON_COPYABLE(memspg)
};

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

template<size_t t_init, size_t t_high, size_t t_max>
void *memspg<t_init, t_high, t_max>::resize(size_t new_size, size_t from,
                                            size_t len, size_t to)
{
    TT_OVERFLOW_IF(from + len < from, "overflowed from");
    TT_INVALID_ARG_IF(from > size_ || (from + len) > size_, "invalid from");
    TT_INVALID_ARG_IF(t_max != 0 && new_size > t_max, "new size exceeds t_max");

    new_size = align_size(new_size);
    if (new_size == size_) { return addr_; }

    if (to >= new_size) {
        len = 0;
    } else if (to + len > new_size) {
        len = new_size - to;
    }

    if (new_size <= k_init_size) {
        assert(addr_ != internal_mem_);
        if (len != 0) { memmove(internal_mem_ + to, addr_ + from, len); }
        delete addr_;
        addr_ = internal_mem_;
    } else {
        uint8_t *p = new uint8_t[new_size];
        if (len != 0) { memmove(p + to, addr_ + from, len); }
        if (addr_ != internal_mem_) { delete addr_; }
        addr_ = p;
    }
    size_ = new_size;

    return addr_;
}

}

#endif // __TT_MEMORY_SPRING_CPP__
