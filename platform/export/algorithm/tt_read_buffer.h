/* Licensed to the Apache Software Foundation (ASF) under one or more
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
@file tt_read_buffer.h
@brief read buffer

this file defines io read buffer APIs
*/

#ifndef __TT_READ_BUFFER__
#define __TT_READ_BUFFER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>
#include <algorithm/tt_buffer_format.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

// - return success if len is set indicating how many bytes would be decoded,
// - incomplete if should wait for more data,
// - fail if data in raw is invalid
typedef tt_result_t (*tt_rbuf_decode_prepare_t)(IN tt_buf_t *raw,
                                                OUT tt_u32_t *len,
                                                IN void *param);

// - return success if data in raw has be decoded to dec
// - return proceeding if len bytes data in raw should be ignored
// - return fail if decoding failed
// - if success is returned, func should precise set pos of raw and dec
typedef tt_result_t (*tt_rbuf_decode_t)(IN tt_buf_t *raw,
                                        IN tt_u32_t len,
                                        OUT tt_buf_t *dec,
                                        IN void *param);

typedef struct
{
    tt_rbuf_decode_prepare_t prepare;
    tt_rbuf_decode_t decode;
} tt_rbuf_decode_itf_t;

// - return success if len is set indicating how many bytes would be parsed,
// - incomplete if should wait for more data,
// - fail if data in buf is invalid
typedef tt_result_t (*tt_rbuf_parse_prepare_t)(IN tt_buf_t *buf,
                                               OUT tt_u32_t *len,
                                               IN void *param);

// - return success if data in dec has be parsed and parse_ret is set
// - return proceeding if len bytes data in buf should be ignored
// - return fail if parsing failed
// - if success is returned, func should precise set buf pos
typedef tt_result_t (*tt_rbuf_parse_t)(IN tt_buf_t *buf,
                                       IN tt_u32_t len,
                                       OUT void **parse_ret,
                                       IN void *param);

typedef void (*tt_rbuf_parse_done_t)(IN void *parse_ret, IN void *param);

typedef struct
{
    tt_rbuf_parse_prepare_t prepare;
    tt_rbuf_parse_t parse;
    tt_rbuf_parse_done_t done;
} tt_rbuf_parse_itf_t;

typedef struct
{
    tt_buf_attr_t rawbuf_attr;
    tt_buf_attr_t decbuf_attr;
    tt_u32_t refine_threshold;
} tt_rbuf_attr_t;

typedef struct tt_rbuf_s
{
    tt_rbuf_decode_itf_t *d_itf;
    void *d_param;
    tt_rbuf_parse_itf_t *p_itf;
    void *p_param;
    tt_buf_t raw;
    tt_buf_t dec;
    tt_u32_t refine_threshold;
} tt_rbuf_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_rbuf_init(IN tt_rbuf_t *rbuf,
                         IN OPT tt_rbuf_decode_itf_t *d_itf,
                         IN OPT void *d_param,
                         IN tt_rbuf_parse_itf_t *p_itf,
                         IN OPT void *p_param,
                         IN OPT tt_rbuf_attr_t *attr);

extern void tt_rbuf_destroy(IN tt_rbuf_t *rbuf);

extern void tt_rbuf_attr_default(IN tt_rbuf_attr_t *attr);

tt_inline tt_result_t tt_rbuf_reserve(IN tt_rbuf_t *rbuf, IN tt_u32_t size)
{
    return tt_buf_reserve(&rbuf->raw, size);
}

tt_inline void tt_rbuf_clear(IN tt_rbuf_t *rbuf)
{
    tt_buf_clear(&rbuf->raw);
    tt_buf_clear(&rbuf->dec);
}

// ========================================
// put data to rbuf
// ========================================

tt_inline void tt_rbuf_get_wptr(IN tt_rbuf_t *rbuf,
                                IN tt_u8_t **p,
                                IN tt_u32_t *len)
{
    return tt_buf_get_wptr(&rbuf->raw, p, len);
}

extern tt_result_t tt_rbuf_inc_wp(IN tt_rbuf_t *rbuf, IN tt_u32_t num);

#endif /* __TT_READ_BUFFER__ */
