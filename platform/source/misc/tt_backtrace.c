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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <misc/tt_backtrace.h>

#include <algorithm/tt_buffer_format.h>
#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <os/tt_thread.h>

#include <tt_backtrace_native.h>

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

static tt_ptrq_t tt_s_crash_handler;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __bt_component_init(IN tt_component_t *comp,
                                       IN tt_profile_t *profile);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_backtrace_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __bt_component_init,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_BACKTRACE, "Backtrace", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

const tt_char_t *tt_backtrace(IN OPT const tt_char_t *prefix,
                              IN OPT const tt_char_t *suffix)
{
    tt_thread_t *t;
    tt_buf_t *buf;

    t = tt_current_thread();
    if (t == NULL) {
        return "";
    }

    if (t->backtrace == NULL) {
        t->backtrace = tt_malloc(sizeof(tt_buf_t));
        if (t->backtrace == NULL) {
            return "";
        }
        tt_buf_init(t->backtrace, NULL);
    }
    buf = t->backtrace;

    tt_buf_clear(buf);
    if (!TT_OK(tt_backtrace_ntv(buf, prefix, suffix))) {
        return "";
    }
    tt_buf_put_u8(buf, 0);

    return (tt_char_t *)TT_BUF_RPOS(buf);
}

tt_result_t tt_push_crash_handler(IN void (*crash_handler)())
{
    return tt_ptrq_push_tail(&tt_s_crash_handler, crash_handler);
}

tt_crash_handler_t tt_pop_crash_handler()
{
    return (tt_crash_handler_t)tt_ptrq_pop_tail(&tt_s_crash_handler);
}

tt_result_t __bt_component_init(IN tt_component_t *comp,
                                IN tt_profile_t *profile)
{
    // init low level socket system
    if (!TT_OK(tt_backtrace_component_init_ntv(profile))) {
        TT_ERROR("fail to initialize backtrace native");
        return TT_FAIL;
    }

    tt_ptrq_init(&tt_s_crash_handler, NULL);

    return TT_SUCCESS;
}
