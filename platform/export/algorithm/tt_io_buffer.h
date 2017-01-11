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
@file tt_io_buffer.h
@brief io iobfer

this file defines io iobfer APIs
*/

#ifndef __TT_IO_BUFFER__
#define __TT_IO_BUFFER__

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

typedef struct
{
    tt_buf_attr_t ibuf_attr;
    tt_buf_attr_t obuf_attr;
    tt_u32_t refine_threshold;
} tt_iobuf_attr_t;

// - return 0 if do not want to process input yet and then the incoming data
//   would be cached in ibuf
// - note len may be 0
typedef tt_u32_t (*tt_iobuf_prepare_t)(IN tt_u8_t *data,
                                       IN tt_u32_t len,
                                       IN void *param);

// return 0 if transform failed
typedef tt_u32_t (*tt_iobuf_transform_t)(IN tt_u8_t *data,
                                         IN tt_u32_t len,
                                         OUT tt_u8_t *outbuf,
                                         IN tt_u32_t outbuf_len,
                                         IN void *param);

typedef void (*tt_iobuf_clear_t)(IN void *param);

typedef struct
{
    tt_iobuf_prepare_t prepare;
    tt_iobuf_transform_t transform;
    tt_iobuf_clear_t clear;
} tt_iobuf_itf_t;

typedef struct tt_iobuf_s
{
    tt_iobuf_itf_t *itf;
    void *param;
    tt_buf_t ibuf;
    tt_buf_t obuf;
    tt_u32_t refine_threshold;
} tt_iobuf_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_iobuf_init(IN tt_iobuf_t *iob,
                          IN tt_iobuf_itf_t *itf,
                          IN OPT void *param,
                          IN OPT tt_iobuf_attr_t *attr);

extern void tt_iobuf_destroy(IN tt_iobuf_t *iob);

extern void tt_iobuf_attr_default(IN tt_iobuf_attr_t *attr);

extern tt_result_t tt_iobuf_put(IN tt_iobuf_t *iob,
                                IN tt_u8_t *data,
                                IN tt_u32_t len);

extern tt_result_t tt_iobuf_get(IN tt_iobuf_t *iob,
                                IN tt_u8_t *p,
                                IN tt_u32_t len);

extern void tt_iobuf_clear(IN tt_iobuf_t *iob);

// ========================================
// format
// ========================================

tt_inline tt_result_t tt_iobuf_put_u64(IN tt_iobuf_t *iob, IN tt_u64_t val_u64)
{
    return tt_iobuf_put(iob, (tt_u8_t *)&val_u64, sizeof(tt_u64_t));
}

tt_inline tt_result_t tt_iobuf_put_u64_n(IN tt_iobuf_t *iob,
                                         IN tt_u64_t val_u64)
{
    val_u64 = tt_hton64(val_u64);
    return tt_iobuf_put(iob, (tt_u8_t *)&val_u64, sizeof(tt_u64_t));
}

tt_inline tt_result_t tt_iobuf_get_u64(IN tt_iobuf_t *iob, IN tt_u64_t *val_u64)
{
    return tt_buf_get_u64(&iob->obuf, val_u64);
}

tt_inline tt_result_t tt_iobuf_get_u64_h(IN tt_iobuf_t *iob,
                                         IN tt_u64_t *val_u64)
{
    return tt_buf_get_u64_h(&iob->obuf, val_u64);
}

tt_inline tt_result_t tt_iobuf_put_u32(IN tt_iobuf_t *iob, IN tt_u32_t val_u32)
{
    return tt_iobuf_put(iob, (tt_u8_t *)&val_u32, sizeof(tt_u32_t));
}

tt_inline tt_result_t tt_iobuf_put_u32_n(IN tt_iobuf_t *iob,
                                         IN tt_u32_t val_u32)
{
    val_u32 = tt_hton32(val_u32);
    return tt_iobuf_put(iob, (tt_u8_t *)&val_u32, sizeof(tt_u32_t));
}

tt_inline tt_result_t tt_iobuf_get_u32(IN tt_iobuf_t *iob, IN tt_u32_t *val_u32)
{
    return tt_buf_get_u32(&iob->obuf, val_u32);
}

tt_inline tt_result_t tt_iobuf_get_u32_h(IN tt_iobuf_t *iob,
                                         IN tt_u32_t *val_u32)
{
    return tt_buf_get_u32_h(&iob->obuf, val_u32);
}

tt_inline tt_result_t tt_iobuf_peek_u32(IN tt_iobuf_t *iob,
                                        IN tt_u32_t *val_u32)
{
    return tt_buf_peek_u32(&iob->obuf, val_u32);
}

tt_inline tt_result_t tt_iobuf_peek_u32_h(IN tt_iobuf_t *iob,
                                          IN tt_u32_t *val_u32)
{
    return tt_buf_peek_u32_h(&iob->obuf, val_u32);
}

tt_inline tt_result_t tt_iobuf_put_u16(IN tt_iobuf_t *iob, IN tt_u16_t val_u16)
{
    return tt_iobuf_put(iob, (tt_u8_t *)&val_u16, sizeof(tt_u16_t));
}

tt_inline tt_result_t tt_iobuf_put_u16_n(IN tt_iobuf_t *iob,
                                         IN tt_u16_t val_u16)
{
    val_u16 = tt_hton16(val_u16);
    return tt_iobuf_put(iob, (tt_u8_t *)&val_u16, sizeof(tt_u16_t));
}

tt_inline tt_result_t tt_iobuf_get_u16(IN tt_iobuf_t *iob, IN tt_u16_t *val_u16)
{
    return tt_buf_get_u16(&iob->obuf, val_u16);
}

tt_inline tt_result_t tt_iobuf_get_u16_h(IN tt_iobuf_t *iob,
                                         IN tt_u16_t *val_u16)
{
    return tt_buf_get_u16_h(&iob->obuf, val_u16);
}

tt_inline tt_result_t tt_iobuf_put_u8(IN tt_iobuf_t *iob, IN tt_u8_t val_u8)
{
    return tt_iobuf_put(iob, &val_u8, sizeof(tt_u8_t));
}

tt_inline tt_result_t tt_iobuf_get_u8(IN tt_iobuf_t *iob, IN tt_u8_t *val_u8)
{
    return tt_buf_get_u8(&iob->obuf, val_u8);
}

#endif /* __TT_IO_BUFFER__ */
