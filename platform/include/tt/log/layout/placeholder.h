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
@file base.h
 @brief log layout: pattern

this file define all basic types

*/

#ifndef __TT_LOG_LAYOUT_PLACEHOLDER_CPP__
#define __TT_LOG_LAYOUT_PLACEHOLDER_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/log/def.h>
#include <tt/log/layout.h>
#include <tt/misc/buffer.h>
#include <tt/misc/util.h>

#include <string.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

namespace tt {

namespace log {

namespace placeholder {

class base
{
public:
    virtual ~base() {}

    void render(const entry &e, OUT buf &b)
    {
        do_render(format_ ? format_.get() : default_format(), e, b);
    }

protected:
    base(const char *format, size_t len)
    {
        if (len > 0) { format_.reset((char *)dup(format, len, 1)); }
    }

    virtual const char *default_format() const = 0;
    virtual void do_render(const char *format, const entry &e,
                           OUT buf &b) const = 0;

    std::unique_ptr<char[]> format_{nullptr};

    TT_NON_COPYABLE(base)
};

class logger: public base
{
public:
    logger(const char *format, size_t len): base(format, len) {}

private:
    const char *default_format() const override { return "%s"; };

    void do_render(const char *format, const entry &e,
                   OUT buf &b) const override
    {
        if (e.logger != nullptr) { b.write_f(format, e.logger); }
    }
};

class function: public base
{
public:
    function(const char *format, size_t len): base(format, len) {}

private:
    const char *default_format() const override { return "%s"; };

    void do_render(const char *format, const entry &e,
                   OUT buf &b) const override
    {
        if (e.function != nullptr) { b.write_f(format, e.function); }
    }
};

class content: public base
{
public:
    content(const char *format, size_t len): base(format, len) {}

private:
    const char *default_format() const override { return "%s"; };

    void do_render(const char *format, const entry &e,
                   OUT buf &b) const override
    {
        if (e.content != nullptr) { b.write_f(format, e.content); }
    }
};

class seqno: public base
{
public:
    seqno(const char *format, size_t len, uint32_t start = 0): base(format, len)
    {
    }

private:
    const char *default_format() const override { return "%u"; };

    void do_render(const char *format, const entry &e,
                   OUT buf &b) const override
    {
        b.write_f(format, e.seqno);
    }
};

class line: public base
{
public:
    line(const char *format, size_t len): base(format, len) {}

private:
    const char *default_format() const override { return "%u"; };

    void do_render(const char *format, const entry &e,
                   OUT buf &b) const override
    {
        if (e.line != 0) { b.write_f(format, e.line); }
    }
};

class level: public base
{
public:
    level(const char *format, size_t len): base(format, len) {}

private:
    static const char *s_level[level_num];

    const char *default_format() const override { return "%s"; };

    void do_render(const char *format, const entry &e,
                   OUT buf &b) const override
    {
        if (e.level < level_num) { b.write_f(format, s_level[e.level]); }
    }
};

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export base *parse(const char *p, size_t len);

}

}

}

#endif /* __TT_LOG_LAYOUT_PLACEHOLDER_CPP__ */
