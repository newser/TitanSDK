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

#include <misc/tt_error.h>

#include <os/tt_thread.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __ERR_RANGE_STRING_START(range)                                        \
    static const tt_char_t *__ers_##range[] = {
#define __ERR_STRING(result, str) (str)
#define __ERR_RANGE_STRING_END(range)                                          \
    }                                                                          \
    ;

#define __ERR_RANGE_STRING_AT(range, idx) __ers_##range[(idx)]

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// clang-format off

// ========================================
// no error
// ========================================
__ERR_RANGE_STRING_START(TT_ERR_RANGE_NO_ERROR)

    // idx: 0
    __ERR_STRING(TT_SUCCESS, "Success"),

__ERR_RANGE_STRING_END(TT_ERR_RANGE_NO_ERROR)

// ========================================
// common error
// ========================================
__ERR_RANGE_STRING_START(TT_ERR_RANGE_COMMON)

    // idx: 0
    __ERR_STRING(TT_FAIL, "Fail"),
    __ERR_STRING(TT_TIME_OUT, "Time out"),
    __ERR_STRING(TT_END, "Operation ends"),
    __ERR_STRING(TT_PROCEEDING, "Proceeding"),
    __ERR_STRING(TT_BAD_PARAM, "Bad parameter"),

    // idx: 5
    __ERR_STRING(TT_NO_RESOURCE, "No resource"),
    __ERR_STRING(TT_BAD_ENV, "Bad environment"),
    __ERR_STRING(TT_NOT_EXIST, "Not exist"),
    __ERR_STRING(TT_E_EXIST, "Already exsit"),
    __ERR_STRING(TT_NOT_SUPPORT, "Not support"),

    // idx: 10
    __ERR_STRING(TT_CANCELLED, "Operation is cancelled"),

    // below for internal usage
    __ERR_STRING(TT_UNKNOWN_ERROR, "Unknown error"),

__ERR_RANGE_STRING_END(TT_ERR_RANGE_COMMON)

// ========================================
// buffer error
// ========================================
__ERR_RANGE_STRING_START(TT_ERR_RANGE_BUFFER)

    // idx: 0
    __ERR_STRING(TT_BUFFER_INCOMPLETE, "Buffer incomplete"),

__ERR_RANGE_STRING_END(TT_ERR_RANGE_BUFFER)

// ========================================
// native error
// ========================================
__ERR_RANGE_STRING_START(TT_ERR_RANGE_NATIVE)

    // idx: 0
    __ERR_STRING(TT_NATIVE_FAIL, "Native failure"),

__ERR_RANGE_STRING_END(TT_ERR_RANGE_NATIVE)

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

    // clang-format on

    tt_result_t tt_err_get_last()
{
    tt_thread_t *current = tt_current_thread();
    if (current != NULL) {
        return current->last_error;
    } else {
        // not able to get last error
        return TT_UNKNOWN_ERROR;
    }
}

void tt_err_set_last(IN tt_result_t result)
{
    tt_thread_t *current = tt_current_thread();
    if (current != NULL) {
        current->last_error = result;
    }
}

const tt_char_t *tt_err_string(tt_result_t result)
{
#define __ES_ENTRY(result, range)                                              \
    case range: {                                                              \
        if (TT_ERR_IN(result, range)) {                                        \
            tt_u32_t idx = TT_ERR_IDX_IN(result, range);                       \
            return __ERR_RANGE_STRING_AT(range, idx);                          \
        } else {                                                               \
            return "Unknown error";                                            \
        }                                                                      \
    } break;

    switch (TT_ERR_RANGE(result)) {
        __ES_ENTRY(result, TT_ERR_RANGE_NO_ERROR);
        __ES_ENTRY(result, TT_ERR_RANGE_COMMON);
        __ES_ENTRY(result, TT_ERR_RANGE_NATIVE);
        default: {
            return "Unknown error";
        } break;
    }

#undef __ES_ENTRY
}
