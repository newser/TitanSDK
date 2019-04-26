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
@file buffer.h
@brief buf addr structure

this file defines buf addr structure and related operations.

*/

#ifndef __TT_BUFFER_CPP__
#define __TT_BUFFER_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

extern "C" {
#include <misc/tt_util.h>
}

#include <tt/memory/memory_spring.h>
#include <tt/misc/endian.h>
#include <tt/misc/util.h>

#include <algorithm>

namespace tt {

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

template<size_t t_init = 3, size_t t_high = 3, size_t t_max = 0>
class buf: public memspg<t_init, t_high, t_max>
{
    using base = memspg<t_init, t_high, t_max>;

public:
    using rwp = std::pair<size_t, size_t>;

    buf() = default;
    buf(size_t len) { base::resize_discard(len); }
    buf(const void *addr, size_t len)
    {
        void *p = base::resize_discard(len);
        memcpy(p, addr, len);
    }

    bool empty() const { return rp_ == wp_; }

    void *r_addr() const { return base::addr_ + rp_; }
    buf &r_addr(void *addr)
    {
        TT_INVALID_ARG_IF(addr < base::addr_ || addr >= w_addr(),
                          "invalid r_addr");
        rp_ = (uint8_t *)addr - base::addr_;
        return *this;
    }
    size_t r_size() const { return wp_ - rp_; }
    buf &r_inc(size_t n)
    {
        TT_INVALID_ARG_IF(n > r_size(), "invalid r_inc");
        rp_ += n;
        return *this;
    }
    buf &r_dec(size_t n)
    {
        TT_INVALID_ARG_IF(rp_ < n, "invalid r_dec");
        rp_ -= n;
        return *this;
    }
    buf &r_clear()
    {
        rp_ = 0;
        return *this;
    }

    void *w_addr() const { return base::addr_ + wp_; }
    buf &w_addr(void *addr)
    {
        TT_INVALID_ARG_IF(addr < r_addr() || addr > end_addr(),
                          "invalid w_addr");
        wp_ = (uint8_t *)addr - base::addr_;
        return *this;
    }
    size_t w_size() const { return base::size_ - wp_; }
    buf &w_inc(size_t n)
    {
        TT_INVALID_ARG_IF(n > w_size(), "invalid w_inc");
        wp_ += n;
        return *this;
    }
    buf &w_dec(size_t n)
    {
        TT_INVALID_ARG_IF(n > r_size(), "invalid w_dec");
        wp_ -= n;
        return *this;
    }
    buf &clear()
    {
        rp_ = wp_ = 0;
        return *this;
    }

    void *end_addr() const { return base::addr_ + base::size_; }

    rwp rwpos() const { return std::make_pair(rp_, wp_); }
    buf &rwpos(const rwp &rwp)
    {
        rp_ = rwp.first;
        wp_ = rwp.second;
        return *this;
    }

    buf &refine()
    {
        size_t n = r_size();
        memmove(base::addr_, r_addr(), n);
        rp_ = 0;
        wp_ = rp_ + n;
        return *this;
    }
    buf &refine(size_t threshold)
    {
        if (refinable() >= threshold) { refine(); }
        return *this;
    }
    size_t refinable() const { return rp_; }

    void *reserve(size_t len)
    {
        size_t n = w_size();
        if (n < len) { base::resize(base::size() + len - n, rp_, r_size()); }
        return w_addr();
    }
    // reserve() always reserves more spaces and thus effient, while
    // reserve_head() only reserves specific @len bytes, caller should
    // reserve enough bytes at one time
    void *reserve_head(size_t len)
    {
        if (rp_ < len) {
            base::resize(base::size() + len - rp_, rp_, r_size(), len);
        }
        return (uint8_t *)r_addr() - len;
    }

    buf &replace(size_t rwpos, const void *addr, size_t len)
    {
        return replace(rwpos, len, addr, len);
    }
    buf &replace(size_t rwpos, size_t n, const void *addr, size_t len)
    {
        TT_OVERFLOW_IF(rp_ + rwpos < rwpos || rp_ + rwpos + n < n,
                       "rwpos overflow");
        TT_INVALID_ARG_IF(rp_ + rwpos > wp_ || rp_ + rwpos + n > wp_,
                          "invalid rwpos to replace");

        uint8_t *p = (uint8_t *)r_addr() + rwpos;
        if (n < len) {
            size_t more_bytes = len - n;
            base::more(more_bytes);

            memmove(p + len, p + n, r_size() - rwpos - n);
            memcpy(p, addr, len);
            wp_ += more_bytes;
        } else if (n == len) {
            memcpy(p, addr, len);
        } else {
            size_t less_bytes = n - len;

            memmove(p + len, p + n, r_size() - rwpos - len);
            memcpy(p, addr, len);
            wp_ -= less_bytes;
        }

        return *this;
    }

    // debug
    buf &std_dump(int flag = 0) const;
    buf &std_dump_hex(int flag = 0) const;
    buf &std_dump_cstr(int flag = 0);

    ////////////////////////////////////////////////////////////
    // read, write
    ////////////////////////////////////////////////////////////

    buf &write(void *data, size_t len)
    {
        void *p = reserve(len);
        memcpy(p, data, len);
        w_inc(len);
        return *this;
    }
    buf &write_head(void *data, size_t len)
    {
        void *p = reserve_head(len);
        memcpy(p, data, len);
        r_dec(len);
        return *this;
    }
    buf &write_rep(size_t num, uint8_t byte)
    {
        void *p = reserve(num);
        memset(p, byte, num);
        w_inc(num);
        return *this;
    }
    buf &write_rand(size_t num);

    bool read(OUT void *data, size_t len)
    {
        if (r_size() >= len) {
            memcpy(data, r_addr(), len);
            r_inc(len);
            return true;
        } else {
            return false;
        }
    }
    size_t read_hex(OUT void *data, size_t len);

    bool peek(OUT void *data, size_t len) const
    {
        if (r_size() >= len) {
            memcpy(data, r_addr(), len);
            return true;
        } else {
            return false;
        }
    }

private:
    size_t rp_{0};
    size_t wp_{0};

    TT_NON_COPYABLE(buf)
};

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

}

#endif /* __TT_BUFFER_CPP__ */
