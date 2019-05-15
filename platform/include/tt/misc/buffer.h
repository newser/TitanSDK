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
@brief buf_t addr structure

this file defines buf_t addr structure and related operations.

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
#include <cctype>
#include <cstdio>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

namespace tt {

template<size_t t_init = 3, size_t t_high = 3, size_t t_max = 0>
class buf_t: protected memspg<t_init, t_high, t_max>
{
    using base = memspg<t_init, t_high, t_max>;

public:
    using rwp = std::pair<size_t, size_t>;

    buf_t() = default;
    buf_t(size_t len) { base::resize_discard(len); }
    buf_t(const void *addr, size_t len)
    {
        void *p = base::resize_discard(len);
        memcpy(p, addr, len);
        wp_ = len;
    }
    buf_t(const char *s): buf_t(s, strlen(s)) {}
    buf_t(const buf_t &b): buf_t(b.r(), b.r_size()) {}
    buf_t(buf_t &&b): base(static_cast<base &&>(b))
    {
        rp_ = b.rp_;
        wp_ = b.wp_;
        b.rp_ = 0;
        b.wp_ = 0;
    }

    bool empty() const { return rp_ == wp_; }

    void *r() const { return base::addr_ + rp_; }
    void r(void *addr)
    {
        TT_INVALID_ARG_IF(addr < base::addr_ || addr >= w(), "invalid r_addr");
        rp_ = (uint8_t *)addr - base::addr_;
    }
    size_t r_size() const { return wp_ - rp_; }
    void r_inc(size_t n)
    {
        TT_INVALID_ARG_IF(n > r_size(), "invalid r_inc");
        rp_ += n;
    }
    void r_dec(size_t n)
    {
        TT_INVALID_ARG_IF(rp_ < n, "invalid r_dec");
        rp_ -= n;
    }
    void r_clear() { rp_ = 0; }

    void *w() const { return base::addr_ + wp_; }
    void w(void *addr)
    {
        TT_INVALID_ARG_IF(addr < r() || addr > end_addr(), "invalid w");
        wp_ = (uint8_t *)addr - base::addr_;
    }
    size_t w_size() const { return base::size_ - wp_; }
    void w_inc(size_t n)
    {
        TT_INVALID_ARG_IF(n > w_size(), "invalid w_inc");
        wp_ += n;
    }
    void w_dec(size_t n)
    {
        TT_INVALID_ARG_IF(n > r_size(), "invalid w_dec");
        wp_ -= n;
    }
    void clear() { rp_ = wp_ = 0; }

    void *end_addr() const { return base::addr_ + base::size_; }

    rwp rwpos() const { return std::make_pair(rp_, wp_); }
    void rwpos(const rwp &rwp)
    {
        rp_ = rwp.first;
        wp_ = rwp.second;
    }

    void refine()
    {
        size_t n = r_size();
        memmove(base::addr_, r(), n);
        rp_ = 0;
        wp_ = rp_ + n;
    }
    void refine(size_t threshold)
    {
        if (refinable() >= threshold) { refine(); }
    }
    size_t refinable() const { return rp_; }

    void *reserve(size_t len)
    {
        size_t n = w_size();
        if (n < len) { base::resize(base::size() + len - n, rp_, r_size()); }
        return w();
    }
    // reserve() always reserves more spaces and thus effient, while
    // reserve_head() only reserves specific @len bytes, caller should
    // reserve enough bytes at one time
    void *reserve_head(size_t len)
    {
        if (rp_ < len) {
            base::resize(base::size() + len - rp_, rp_, r_size(), len);
        }
        return (uint8_t *)r() - len;
    }

    // debug
    void std_dump(int flag = 0) const;
    void std_dump_hex(int flag = 0) const;
    void std_dump_cstr(int flag = 0);

    ////////////////////////////////////////////////////////////
    // read, write
    ////////////////////////////////////////////////////////////

    void write(const void *data, size_t len)
    {
        void *p = reserve(len);
        memcpy(p, data, len);
        w_inc(len);
    }
    void write_head(const void *data, size_t len)
    {
        void *p = reserve_head(len);
        memcpy(p, data, len);
        r_dec(len);
    }
    void write_rep(size_t num, uint8_t byte)
    {
        void *p = reserve(num);
        memset(p, byte, num);
        w_inc(num);
    }
    void write_rand(size_t num);

    template<int t_retry = 10>
    bool write_f(const char *fmt, ...)
    {
        bool ret;

        va_list args;
        va_start(args, fmt);
        ret = write_v<t_retry>(fmt, args);
        va_end(args);

        return ret;
    }

    template<int t_retry = 10>
    bool write_v(const char *fmt, va_list ap)
    {
        va_list args;
        int tried = 0;

        do {
            size_t n = w_size();
            if (n == 0) {
                base::more();
                n = w_size();
            }

            va_copy(args, ap);
            int wn = std::vsnprintf((char *)w(), n, fmt, args);
            // if truncated, vsnprintf may:
            // - return negative val
            // - return n - 1 or n, depends on platform
            if (wn >= 0 && wn < (n - 1)) {
                w_inc(wn);
                return true;
            }
            base::more();
        } while (++tried <= t_retry);
        return false;
    }

    template<typename T>
    void write(T val)
    {
        write(&val, sizeof(T));
    }
    template<typename T>
    void write_h2n(T val)
    {
        T v = h2n(val);
        write(&v, sizeof(T));
    }
    void write(const char *s) { write(s, strlen(s)); }
    void write(const buf_t &b) { write(b.r(), b.r_size()); }

    bool read(OUT void *data, size_t len)
    {
        if (r_size() >= len) {
            memcpy(data, r(), len);
            r_inc(len);
            return true;
        } else {
            return false;
        }
    }
    size_t read_hex(OUT void *data, size_t len);

    template<typename T>
    bool read(T &val)
    {
        return read(&val, sizeof(T));
    }
    template<typename T>
    bool read_n2h(T &val)
    {
        T v;
        if (read(&v, sizeof(T))) {
            val = n2h(v);
            return true;
        } else {
            return false;
        }
    }

    bool peek(OUT void *data, size_t len) const
    {
        if (r_size() >= len) {
            memcpy(data, r(), len);
            return true;
        } else {
            return false;
        }
    }
    template<typename T>
    bool peek_n2h(T &val) const
    {
        T v;
        if (peek(&v, sizeof(T))) {
            val = n2h(v);
            return true;
        } else {
            return false;
        }
    }

    void swap(buf_t &b)
    {
        base::swap((base &)b);
        std::swap(rp_, b.rp_);
        std::swap(wp_, b.wp_);
    }

    void set(const void *addr, size_t len)
    {
        clear();
        write(addr, len);
    }
    void set(const buf_t &b)
    {
        if (this != &b) { set(b.r(), b.r_size()); }
    }
    void set(const char *s) { set(s, strlen(s)); }

    void cat(OPT const void *addr, size_t len)
    {
        void *p = reserve(len);
        memcpy(p, addr, len);
        w_inc(len);
    }
    void cat(const char *s) { cat(s, strlen(s)); }
    void cat(const buf_t &b) { cat(b.r(), b.r_size()); }

    ssize_t cmp(const void *addr, size_t len) const
    {
        size_t n = r_size();
        return n != len ? n - len : memcmp(r(), addr, len);
    }
    ssize_t cmp(const char *s) const { return cmp(s, strlen(s)); }
    ssize_t cmp(const buf_t &b) const
    {
        return this == &b ? 0 : cmp(b.r(), b.r_size());
    }

    bool startwith(const void *addr, size_t len) const
    {
        return r_size() >= len && memcmp(r(), addr, len) == 0;
    }
    bool startwith(uint8_t b) const
    {
        return r_size() >= 1 && *((uint8_t *)r()) == b;
    }
    bool startwith(const char *s) const { return startwith(s, strlen(s)); }

    bool endwith(const void *addr, size_t len) const
    {
        return r_size() >= len &&
               memcmp(((uint8_t *)w() - len), addr, len) == 0;
    }
    bool endwith(uint8_t b) const
    {
        return r_size() >= 1 && *((uint8_t *)w() - 1) == b;
    }
    bool endwith(const char *s) const { return endwith(s, strlen(s)); }

    void remove(size_t from, size_t len)
    {
        TT_OVERFLOW_IF(from + len < len, "remove overflow");
        TT_INVALID_ARG_IF(from > r_size() || (from + len) > r_size(),
                          "invalid from len");

        uint8_t *p = (uint8_t *)r();
        size_t tail = from + len;
        memmove(p + from, p + tail, r_size() - tail);
        w_dec(len);
    }
    void remove_to(size_t to) { remove(0, to); }
    void remove_from(size_t from) { remove(from, r_size() - from); }
    void remove_head(size_t len) { remove(0, len); }
    void remove_tail(size_t len) { remove(r_size() - len, len); }

    void replace(size_t from, const void *addr, size_t len)
    {
        replace(from, len, addr, len);
    }
    void replace(size_t from, size_t n, const void *addr, size_t len);

    void insert(size_t from, const void *addr, size_t len)
    {
        replace(from, 0, addr, len);
    }
    void insert(size_t from, const char *s) { insert(from, s, strlen(s)); }
    void insert_head(const void *addr, size_t len) { insert(0, addr, len); }
    void insert_head(const char *s) { insert(0, s); }
    void insert_tail(const void *addr, size_t len)
    {
        insert(r_size(), addr, len);
    }
    void insert_tail(const char *s) { insert(r_size(), s); }

    void trim_head(const std::function<bool(uint8_t)> &_if = [](uint8_t b) {
        return std::isspace(b);
    })
    {
        if (rp_ < wp_) {
            uint8_t *p = (uint8_t *)base::addr();
            size_t pos = rp_;
            while ((pos < wp_) && _if(p[pos])) { ++pos; }
            rp_ = pos;
        }
    }
    void trim_tail(const std::function<bool(uint8_t)> &_if = [](uint8_t b) {
        return std::isspace(b);
    })
    {
        if (rp_ < wp_) {
            uint8_t *p = (uint8_t *)base::addr();
            size_t pos = wp_ - 1;
            while ((pos > rp_) && _if(p[pos])) { --pos; }
            wp_ = pos + 1;
        }
    }
    void trim(const std::function<bool(uint8_t)> &_if = [](uint8_t b) {
        return std::isspace(b);
    })
    {
        trim_head(_if);
        trim_tail(_if);
    }

    bool operator==(const buf_t &b) const { return cmp(b) == 0; }
    bool operator==(const char *s) const { return cmp(s) == 0; }

    bool operator!=(const buf_t &b) const { return !(*this == b); }
    bool operator!=(const char *s) const { return !(*this == s); }

    bool operator<(const buf_t &b) const { return cmp(b) < 0; }
    bool operator<(const char *s) const { return cmp(s) < 0; }

    bool operator<=(const buf_t &b) const { return cmp(b) <= 0; }
    bool operator<=(const char *s) const { return cmp(s) <= 0; }

    bool operator>(const buf_t &b) const { return cmp(b) > 0; }
    bool operator>(const char *s) const { return cmp(s) > 0; }

    bool operator>=(const buf_t &b) const { return cmp(b) >= 0; }
    bool operator>=(const char *s) const { return cmp(s) >= 0; }

    buf_t &operator+=(const buf_t &b)
    {
        cat(b.r(), b.r_size());
        return *this;
    }
    buf_t &operator+=(const char *s)
    {
        cat(s, strlen(s));
        return *this;
    }

    const buf_t &operator=(const buf_t &b)
    {
        set(b);
        return *this;
    }
    const buf_t &operator=(const char *s)
    {
        set(s);
        return *this;
    }

private:
    size_t rp_{0};
    size_t wp_{0};
};

using buf = buf_t<>;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

template<size_t t_init, size_t t_high, size_t t_max>
void buf_t<t_init, t_high, t_max>::replace(size_t from, size_t n,
                                           const void *addr, size_t len)
{
    TT_OVERFLOW_IF(rp_ + from < from || rp_ + from + n < n, "rwpos overflow");
    TT_INVALID_ARG_IF(rp_ + from > wp_ || rp_ + from + n > wp_,
                      "invalid rwpos to replace");

    if (n < len) {
        size_t more_bytes = len - n;
        reserve(more_bytes);

        uint8_t *p = (uint8_t *)r() + from;
        memmove(p + len, p + n, r_size() - from - n);
        memcpy(p, addr, len);
        wp_ += more_bytes;
    } else if (n == len) {
        memcpy((uint8_t *)r() + from, addr, len);
    } else {
        size_t less_bytes = n - len;

        uint8_t *p = (uint8_t *)r() + from;
        memmove(p + len, p + n, r_size() - from - len);
        memcpy(p, addr, len);
        wp_ -= less_bytes;
    }
}

}

#endif /* __TT_BUFFER_CPP__ */