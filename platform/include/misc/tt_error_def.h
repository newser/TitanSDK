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
@file tt_error_def.h
@brief error definition

error definition
*/

#ifndef __TT_ERROR_DEF__
#define __TT_ERROR_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// generate error by range and id
#define TT_ERR_MK(range, id) ((((range)&0xFF) << 24) | ((id)&0xFFFFFF))
#define TT_ERR_MK_SUB(range, sub_range, id)                                    \
    ((((range)&0xFF) << 24) | (((sub_range)&0xFF) << 16) | ((id)&0xFFFF))

// get error range
#define TT_ERR_RANGE(ev_id) (((ev_id) >> 24) & 0xFF)
#define TT_ERR_RANGE_SUB(ev_id) (((ev_id) >> 16) & 0xFF)

// in range
#define TT_ERR_IN(err, err_range)                                              \
    (((err) > (err_range##_START)) && ((err) < (err_range##_END)))
#define TT_ERR_IDX_IN(err, err_range) ((err) - (err_range##_START) - 1)

/**
@def TT_OK(result)
- return TT_TRUE if is an successful result
- return TT_FALSE if is not an successful result
*/
#define TT_OK(result) ((result) == TT_SUCCESS)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

enum
{
    TT_ERR_RANGE_NO_ERROR,
    TT_ERR_RANGE_COMMON,
    TT_ERR_RANGE_BUFFER,
    TT_ERR_RANGE_NATIVE,
};

typedef enum _tt_result_t {
    // ========================================
    // no error
    // ========================================

    TT_ERR_RANGE_NO_ERROR_START = TT_ERR_MK(TT_ERR_RANGE_NO_ERROR, 0),

    // idx: 0
    TT_SUCCESS,
    // set TT_SUCCESS to non-zero to distinguish it with TT_FALSE

    TT_ERR_RANGE_NO_ERROR_END,

    // ========================================
    // common error
    // ========================================

    TT_ERR_RANGE_COMMON_START = TT_ERR_MK(TT_ERR_RANGE_COMMON, 0),

    // idx: 0
    TT_FAIL,
    TT_E_TIMEOUT,
    TT_E_END,
    TT_E_PROCEED,
    TT_BAD_PARAM,

    // idx: 5
    TT_E_NOMEM,
    TT_BAD_ENV,
    TT_E_NOEXIST,
    TT_E_EXIST,
    TT_E_UNSUPPORT,

    // idx: 10
    TT_CANCELLED,

    // below for internal usage
    TT_UNKNOWN_ERROR,
    // TT_UNKNOWN_ERROR is a special error, indicating error value
    // recorded is not valid, or fail to get last error

    TT_ERR_RANGE_COMMON_END,

    // ========================================
    // buffer error
    // ========================================

    TT_ERR_RANGE_BUFFER_START = TT_ERR_MK(TT_ERR_RANGE_BUFFER, 0),

    // not enough data in buffer: this is not error, data in
    // buffer should be kept and to read more data
    // caller should do:
    //  - return
    // callee should do:
    //  - update read pos
    TT_BUFFER_INCOMPLETE,

    TT_ERR_RANGE_BUFFER_END,

    // ========================================
    // native error
    // ========================================

    TT_ERR_RANGE_NATIVE_START = TT_ERR_MK(TT_ERR_RANGE_NATIVE, 0),

    // idx: 0
    TT_NATIVE_FAIL,

    TT_ERR_RANGE_NATIVE_END,

} tt_result_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_ERROR_DEF__ */
