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
@file mgr.h
@brief log mgr

this file define all basic types

*/

#ifndef __TT_LOG_MANAGER_CPP__
#define __TT_LOG_MANAGER_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/init/component.h>
#include <tt/log/context.h>
#include <tt/misc/buffer.h>

#include <atomic>
#include <mutex>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

namespace tt {

namespace log {

class mgr
{
public:
    mgr() = default;
    mgr(const char *logger): logger_(logger) {}

    mgr &debug_v(const char *func, size_t line, const char *format, va_list ap)
    {
        if (level() > e_debug) { return *this; }
        write(e_debug, func, line, format, ap);
        return *this;
    }
    mgr &debug_f(const char *func, size_t line, const char *format, ...)
    {
        va_list ap;
        va_start(ap, format);
        debug_v(func, line, format, ap);
        va_end(ap);
        return *this;
    }
    mgr &debug(const char *format, ...)
    {
        va_list ap;
        va_start(ap, format);
        debug_v(nullptr, 0, format, ap);
        va_end(ap);
        return *this;
    }

    mgr &info_v(const char *func, size_t line, const char *format, va_list ap)
    {
        if (level() > log::e_info) { return *this; }
        write(log::e_info, func, line, format, ap);
        return *this;
    }
    mgr &info_f(const char *func, size_t line, const char *format, ...)
    {
        va_list ap;
        va_start(ap, format);
        info_v(func, line, format, ap);
        va_end(ap);
        return *this;
    }
    mgr &info(const char *format, ...)
    {
        va_list ap;
        va_start(ap, format);
        info_v(nullptr, 0, format, ap);
        va_end(ap);
        return *this;
    }

    mgr &warn_v(const char *func, size_t line, const char *format, va_list ap)
    {
        if (level() > log::e_warn) { return *this; }
        write(log::e_warn, func, line, format, ap);
        return *this;
    }
    mgr &warn_f(const char *func, size_t line, const char *format, ...)
    {
        va_list ap;
        va_start(ap, format);
        warn_v(func, line, format, ap);
        va_end(ap);
        return *this;
    }
    mgr &warn(const char *format, ...)
    {
        va_list ap;
        va_start(ap, format);
        warn_v(nullptr, 0, format, ap);
        va_end(ap);
        return *this;
    }

    mgr &error_v(const char *func, size_t line, const char *format, va_list ap)
    {
        if (level() > log::e_error) { return *this; }
        write(log::e_error, func, line, format, ap);
        return *this;
    }
    mgr &error_f(const char *func, size_t line, const char *format, ...)
    {
        va_list ap;
        va_start(ap, format);
        error_v(func, line, format, ap);
        va_end(ap);
        return *this;
    }
    mgr &error(const char *format, ...)
    {
        va_list ap;
        va_start(ap, format);
        error_v(nullptr, 0, format, ap);
        va_end(ap);
        return *this;
    }

    mgr &fatal_v(const char *func, size_t line, const char *format, va_list ap)
    {
        if (level() > log::e_fatal) { return *this; }
        write(log::e_fatal, func, line, format, ap);
        return *this;
    }
    mgr &fatal_f(const char *func, size_t line, const char *format, ...)
    {
        va_list ap;
        va_start(ap, format);
        fatal_v(func, line, format, ap);
        va_end(ap);
        return *this;
    }
    mgr &fatal(const char *format, ...)
    {
        va_list ap;
        va_start(ap, format);
        fatal_v(nullptr, 0, format, ap);
        va_end(ap);
        return *this;
    }

    const char *logger() const
    {
        std::lock_guard<std::mutex> lk(lock_);
        return logger_;
    }
    mgr &logger(const char *logger)
    {
        std::lock_guard<std::mutex> lk(lock_);
        logger_ = logger;
        return *this;
    }

    level level() const { return (enum level)level_.load(); }
    mgr &level(enum level level)
    {
        level_.store(level);
        return *this;
    }

    uint32_t seqno() const { return seqno_.load(); }
    mgr &seqno(uint32_t seqno)
    {
        seqno_.store(seqno);
        return *this;
    }

    mgr &layout(enum level lv, std::shared_ptr<i_layout> &layout)
    {
        std::lock_guard<std::mutex> lk(lock_);
        assert(lv < log::level_num);
        ctx_[lv].layout(std::forward<std::shared_ptr<i_layout>>(layout));
        return *this;
    }
    mgr &layout(enum level lv, std::shared_ptr<i_layout> &&layout)
    {
        std::lock_guard<std::mutex> lk(lock_);
        assert(lv < log::level_num);
        ctx_[lv].layout(std::forward<std::shared_ptr<i_layout>>(layout));
        return *this;
    }
    mgr &layout(std::shared_ptr<i_layout> &layout)
    {
        std::lock_guard<std::mutex> lk(lock_);
        ctx_[e_debug].layout(std::forward<std::shared_ptr<i_layout>>(layout));
        ctx_[e_info].layout(std::forward<std::shared_ptr<i_layout>>(layout));
        ctx_[e_warn].layout(std::forward<std::shared_ptr<i_layout>>(layout));
        ctx_[e_error].layout(std::forward<std::shared_ptr<i_layout>>(layout));
        ctx_[e_fatal].layout(std::forward<std::shared_ptr<i_layout>>(layout));
        return *this;
    }
    mgr &layout(std::shared_ptr<i_layout> &&layout)
    {
        std::lock_guard<std::mutex> lk(lock_);
        ctx_[e_debug].layout(std::forward<std::shared_ptr<i_layout>>(layout));
        ctx_[e_info].layout(std::forward<std::shared_ptr<i_layout>>(layout));
        ctx_[e_warn].layout(std::forward<std::shared_ptr<i_layout>>(layout));
        ctx_[e_error].layout(std::forward<std::shared_ptr<i_layout>>(layout));
        ctx_[e_fatal].layout(std::forward<std::shared_ptr<i_layout>>(layout));
        return *this;
    }

    mgr &append_io(enum level lv, std::shared_ptr<log::i_io> &io)
    {
        std::lock_guard<std::mutex> lk(lock_);
        assert(lv < log::level_num);
        ctx_[lv].append_io(std::forward<std::shared_ptr<log::i_io>>(io));
        return *this;
    }
    mgr &append_io(enum level lv, std::shared_ptr<log::i_io> &&io)
    {
        std::lock_guard<std::mutex> lk(lock_);
        assert(lv < log::level_num);
        ctx_[lv].append_io(std::forward<std::shared_ptr<log::i_io>>(io));
        return *this;
    }
    mgr &append_io(std::shared_ptr<log::i_io> &io)
    {
        std::lock_guard<std::mutex> lk(lock_);
        ctx_[e_debug].append_io(std::forward<std::shared_ptr<log::i_io>>(io));
        ctx_[e_info].append_io(std::forward<std::shared_ptr<log::i_io>>(io));
        ctx_[e_warn].append_io(std::forward<std::shared_ptr<log::i_io>>(io));
        ctx_[e_error].append_io(std::forward<std::shared_ptr<log::i_io>>(io));
        ctx_[e_fatal].append_io(std::forward<std::shared_ptr<log::i_io>>(io));
        return *this;
    }
    mgr &append_io(std::shared_ptr<log::i_io> &&io)
    {
        std::lock_guard<std::mutex> lk(lock_);
        ctx_[e_debug].append_io(std::forward<std::shared_ptr<log::i_io>>(io));
        ctx_[e_info].append_io(std::forward<std::shared_ptr<log::i_io>>(io));
        ctx_[e_warn].append_io(std::forward<std::shared_ptr<log::i_io>>(io));
        ctx_[e_error].append_io(std::forward<std::shared_ptr<log::i_io>>(io));
        ctx_[e_fatal].append_io(std::forward<std::shared_ptr<log::i_io>>(io));
        return *this;
    }

private:
    void write(enum level lv, const char *func, size_t line, const char *format,
               va_list ap)
    {
        log::entry e;
        e.logger = logger_;
        e.function = func;

        buf_.clear();
        buf_.write_v(format, ap);
        buf_.write((uint8_t)0);
        e.content = (char *)buf_.r();

        e.seqno = seqno_.fetch_add(1);
        e.line = line;
        e.level = lv;

        std::lock_guard<std::mutex> lk(lock_);
        ctx_[lv].write(e);
    }

    const char *logger_{nullptr};
    mutable std::mutex lock_;
    buf buf_;
    std::atomic<uint32_t> level_{log::e_warn};
    std::atomic<uint32_t> seqno_{0};
    log::ctx ctx_[log::level_num];
};

namespace init {

class mgr: public component
{
public:
    static mgr &instance() { return s_instance; }

private:
    static mgr s_instance;

    mgr(): component(component::e_log_mgr, "default log manager") {}

    bool do_start(void *reserved) override;
    void do_stop() override;
};

}

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export mgr *g_log_mgr;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

inline void debug_f(const char *func, size_t line, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    g_log_mgr->debug_v(func, line, format, ap);
    va_end(ap);
}
#define _TT_DEBUG(format, ...)                                                 \
    debug_f(__FUNCTION__, __LINE__, format, __VA_ARGS__)

inline void debug(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    g_log_mgr->debug_v(nullptr, 0, format, ap);
    va_end(ap);
}

inline void info_f(const char *func, size_t line, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    g_log_mgr->info_v(func, line, format, ap);
    va_end(ap);
}
#define _TT_INFO(format, ...)                                                  \
    info_f(__FUNCTION__, __LINE__, format, __VA_ARGS__)

inline void info(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    g_log_mgr->info_v(nullptr, 0, format, ap);
    va_end(ap);
}

inline void warn_f(const char *func, size_t line, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    g_log_mgr->warn_v(func, line, format, ap);
    va_end(ap);
}
#define _TT_WARN(format, ...)                                                  \
    warn_f(__FUNCTION__, __LINE__, format, __VA_ARGS__)

inline void warn(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    g_log_mgr->warn_v(nullptr, 0, format, ap);
    va_end(ap);
}

inline void error_f(const char *func, size_t line, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    g_log_mgr->error_v(func, line, format, ap);
    va_end(ap);
}
#define _TT_ERROR(format, ...)                                                 \
    error_f(__FUNCTION__, __LINE__, format, __VA_ARGS__)

inline void error(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    g_log_mgr->error_v(nullptr, 0, format, ap);
    va_end(ap);
}

inline void fatal_f(const char *func, size_t line, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    g_log_mgr->fatal_v(func, line, format, ap);
    va_end(ap);
}
#define _TT_FATAL(format, ...)                                                 \
    fatal_f(__FUNCTION__, __LINE__, format, __VA_ARGS__)

inline void fatal(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    g_log_mgr->fatal_v(nullptr, 0, format, ap);
    va_end(ap);
}

}

}

#endif /* __TT_LOG_MANAGER_CPP__ */
