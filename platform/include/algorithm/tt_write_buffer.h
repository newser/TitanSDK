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
@file tt_write_buffer.h
@brief write buffer

this file defines io write buffer APIs
*/

#ifndef __TT_WRITE_BUFFER__
#define __TT_WRITE_BUFFER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

// - return success if len is set indicating how many bytes would be encoded,
// - incomplete if should wait for more data
// - fail if data in raw is invalid
typedef tt_result_t (*tt_wbuf_encode_prepare_t)(IN tt_buf_t *raw,
                                                OUT tt_u32_t *len,
                                                IN void *param);

// - return success if data in raw has be encoded to dec
// - return fail if encoding failed
// - if success is returned, func should precise set pos of raw and dec
typedef tt_result_t (*tt_wbuf_encode_t)(IN tt_buf_t *raw,
                                        IN tt_u32_t len,
                                        OUT tt_buf_t *enc,
                                        IN void *param);

typedef struct
{
    tt_wbuf_encode_prepare_t prepare;
    tt_wbuf_encode_t encode;
} tt_wbuf_encode_itf_t;

// - return bytes for rendered data
// - return 0 if can node rendered data
typedef tt_u32_t (*tt_wbuf_render_prepare_t)(IN void *to_render,
                                             IN void *param);

// - len is returned by render_prepare()
// - return success if data has be rendered to buf
// - return fail if rendering failed
// - if success is returned, func should precisely set buf pos
typedef tt_result_t (*tt_wbuf_render_t)(IN tt_buf_t *buf,
                                        IN tt_u32_t len,
                                        IN void *to_render,
                                        IN void *param);

typedef struct
{
    tt_wbuf_render_prepare_t prepare;
    tt_wbuf_render_t render;
} tt_wbuf_render_itf_t;

typedef struct
{
    tt_buf_attr_t rawbuf_attr;
    tt_buf_attr_t encbuf_attr;
    tt_u32_t refine_threshold;
} tt_wbuf_attr_t;

typedef struct tt_wbuf_s
{
    tt_wbuf_encode_itf_t *e_itf;
    void *e_param;
    tt_wbuf_render_itf_t *r_itf;
    void *r_param;
    tt_buf_t raw;
    tt_buf_t enc;
    tt_u32_t refine_threshold;
} tt_wbuf_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface enclaration
////////////////////////////////////////////////////////////

tt_export void tt_wbuf_init(IN tt_wbuf_t *wbuf,
                            IN OPT tt_wbuf_encode_itf_t *e_itf,
                            IN OPT void *e_param,
                            IN tt_wbuf_render_itf_t *p_itf,
                            IN OPT void *p_param,
                            IN OPT tt_wbuf_attr_t *attr);

tt_export void tt_wbuf_destroy(IN tt_wbuf_t *wbuf);

tt_export void tt_wbuf_attr_default(IN tt_wbuf_attr_t *attr);

tt_inline tt_result_t tt_wbuf_reserve(IN tt_wbuf_t *wbuf, IN tt_u32_t size)
{
    return tt_buf_reserve(&wbuf->raw, size);
}

tt_inline void tt_wbuf_clear(IN tt_wbuf_t *wbuf)
{
    tt_buf_clear(&wbuf->raw);
    tt_buf_clear(&wbuf->enc);
}

// ========================================
// write wbuf
// ========================================

// - return success if to_render is rendered and encoded
// - return incomplete only if none of data are encoded
// - rendered and len is the rendered data and only valid when this function
//   return. user could copy it out if user want to
tt_export tt_result_t tt_wbuf_render(IN tt_wbuf_t *wbuf,
                                     IN void *to_render,
                                     OUT OPT tt_u8_t **rendered,
                                     OUT OPT tt_u32_t *len);

tt_export tt_result_t tt_wbuf_put(IN tt_wbuf_t *wbuf,
                                  IN tt_u8_t *data,
                                  IN tt_u32_t data_len);

// ========================================
// read wbuf
// ========================================

tt_export void tt_wbuf_get_rptr(IN tt_wbuf_t *wbuf,
                                IN tt_u8_t **p,
                                IN tt_u32_t *len);

tt_export tt_result_t tt_wbuf_inc_rp(IN tt_wbuf_t *wbuf, IN tt_u32_t num);

#endif /* __TT_WRITE_BUFFER__ */
