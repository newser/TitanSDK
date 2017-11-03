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

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

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
