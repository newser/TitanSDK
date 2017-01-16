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
@file tt_buffer.h
@brief byte buffer

this file defines buffer APIs
*/

#ifndef __TT_BUFFER__
#define __TT_BUFFER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <memory/tt_memory_spring.h>
#include <misc/tt_util.h>

#include <tt_buffer_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// should be power of 2
#define TT_BUF_INIT_SIZE 16

#define tt_hton16 tt_hton16_ntv
#define tt_ntoh16 tt_ntoh16_ntv

#define tt_hton32 tt_hton32_ntv
#define tt_ntoh32 tt_ntoh32_ntv

#define tt_hton64 tt_hton64_ntv
#define tt_ntoh64 tt_ntoh64_ntv

#if 1
#define TT_ASSERT_BUF TT_ASSERT
#else
#define TT_ASSERT_BUF(...)
#endif

// return pointer to read/write address
#define TT_BUF_RPOS(buf) TT_PTR_INC(tt_u8_t, (buf)->p, (buf)->rpos)
#define TT_BUF_RLEN(buf) ((buf)->wpos - (buf)->rpos)

#define TT_BUF_WPOS(buf) TT_PTR_INC(tt_u8_t, (buf)->p, (buf)->wpos)
#define TT_BUF_WLEN(buf) ((buf)->size - (buf)->wpos)

#define TT_BUF_EPOS(buf) TT_PTR_INC(tt_u8_t, (buf)->p, (buf)->size)

// size that can be refined
#define TT_BUF_REFINABLE(buf) ((buf)->rpos)

#define TT_BUF_CHECK(buf)                                                      \
    do {                                                                       \
        TT_ASSERT((buf)->p != NULL);                                           \
        TT_ASSERT((buf)->rpos <= (buf)->wpos);                                 \
        TT_ASSERT((buf)->wpos <= (buf)->size);                                 \
    } while (0)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_u32_t min_extend;
    tt_u32_t max_extend;
    tt_u32_t max_limit;
} tt_buf_attr_t;

typedef struct tt_buf_s
{
    tt_u8_t *p;
    tt_u32_t size;
    tt_memspg_t mspg;
    tt_u32_t wpos;
    tt_u32_t rpos;
    tt_u8_t initbuf[TT_BUF_INIT_SIZE];

    tt_bool_t readonly : 1;
} tt_buf_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// size could 0, then the buf won't alloc mem initially
extern tt_result_t tt_buf_create(IN tt_buf_t *buf,
                                 IN tt_u32_t size,
                                 IN OPT tt_buf_attr_t *attr);

extern tt_result_t tt_buf_create_copy(IN tt_buf_t *buf,
                                      IN tt_u8_t *data,
                                      IN tt_u32_t data_len,
                                      IN OPT tt_buf_attr_t *attr);

// always success unless attr is invalid
extern tt_result_t tt_buf_create_nocopy(IN tt_buf_t *buf,
                                        IN const tt_u8_t *data,
                                        IN tt_u32_t data_len,
                                        IN OPT tt_buf_attr_t *attr);


// init buf which never fail
extern void tt_buf_init(IN tt_buf_t *buf, IN OPT tt_buf_attr_t *attr);

extern void tt_buf_destroy(IN tt_buf_t *buf);

extern void tt_buf_attr_default(IN tt_buf_attr_t *attr);

extern void tt_buf_print(IN tt_buf_t *buf, IN tt_u32_t flag);

extern void tt_buf_print_hexstr(IN tt_buf_t *buf, IN tt_u32_t flag);

extern void tt_buf_print_cstr(IN tt_buf_t *buf, IN tt_u32_t flag);

extern tt_result_t tt_buf_set(IN tt_buf_t *buf,
                              IN tt_u32_t pos,
                              IN tt_u8_t *data,
                              IN tt_u32_t data_len);

// ========================================
// memory operation
// ========================================

extern tt_result_t __buf_extend(IN tt_buf_t *buf, IN tt_u32_t size);

tt_inline tt_result_t tt_buf_reserve(IN tt_buf_t *buf, IN OPT tt_u32_t size)
{
    if (TT_BUF_WLEN(buf) >= size) {
        return TT_SUCCESS;
    }
    return __buf_extend(buf, buf->size + size);
}

tt_inline tt_result_t tt_buf_extend(IN tt_buf_t *buf)
{
    return __buf_extend(buf, buf->size + 1);
}

extern tt_result_t tt_buf_compress(IN tt_buf_t *buf);

// ========================================
// position operation
// ========================================

tt_inline void tt_buf_get_rptr(IN tt_buf_t *buf,
                               OUT OPT tt_u8_t **p,
                               OUT OPT tt_u32_t *len)
{
    TT_SAFE_ASSIGN(p, TT_BUF_RPOS(buf));
    TT_SAFE_ASSIGN(len, TT_BUF_RLEN(buf));
}

tt_inline void tt_buf_get_rblob(IN tt_buf_t *buf, OUT tt_blob_t *blob)
{
    tt_buf_get_rptr(buf, &blob->addr, &blob->len);
}

// rd_ptr could be in [buf start, wpos)
tt_inline void tt_buf_set_rptr(IN tt_buf_t *buf, IN tt_u8_t *rptr)
{
    TT_ASSERT_ALWAYS((rptr >= buf->p) && (rptr <= TT_BUF_WPOS(buf)));
    buf->rpos = (tt_u32_t)(rptr - buf->p);
}

tt_inline tt_result_t tt_buf_inc_rp(IN tt_buf_t *buf, IN tt_u32_t num)
{
    if (num <= TT_BUF_RLEN(buf)) {
        buf->rpos += num;
        return TT_SUCCESS;
    } else {
        return TT_BUFFER_INCOMPLETE;
    }
}

tt_inline tt_result_t tt_buf_dec_rp(IN tt_buf_t *buf, IN tt_u32_t num)
{
    if (num <= buf->rpos) {
        buf->rpos -= num;
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_inline void tt_buf_reset_rp(IN tt_buf_t *buf)
{
    buf->rpos = 0;
}

tt_inline void tt_buf_get_wptr(IN tt_buf_t *buf,
                               OUT OPT tt_u8_t **p,
                               OUT OPT tt_u32_t *len)
{
    TT_SAFE_ASSIGN(p, TT_BUF_WPOS(buf));
    TT_SAFE_ASSIGN(len, TT_BUF_WLEN(buf));
}

tt_inline void tt_buf_get_wblob(IN tt_buf_t *buf, OUT tt_blob_t *blob)
{
    tt_buf_get_wptr(buf, &blob->addr, &blob->len);
}

tt_inline void tt_buf_set_wptr(IN tt_buf_t *buf, IN tt_u8_t *wptr)
{
    TT_ASSERT_ALWAYS((wptr >= TT_BUF_RPOS(buf)) && (wptr <= TT_BUF_EPOS(buf)));
    buf->wpos = (tt_u32_t)(wptr - buf->p);
}

tt_inline tt_result_t tt_buf_inc_wp(IN tt_buf_t *buf, IN tt_u32_t num)
{
    if (num <= TT_BUF_WLEN(buf)) {
        buf->wpos += num;
        return TT_SUCCESS;
    } else {
        return TT_BUFFER_INCOMPLETE;
    }
}

tt_inline tt_result_t tt_buf_dec_wp(IN tt_buf_t *buf, IN tt_u32_t num)
{
    if (num <= TT_BUF_RLEN(buf)) {
        buf->wpos -= num;
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_inline void tt_buf_reset_rwp(IN tt_buf_t *buf)
{
    buf->rpos = 0;
    buf->wpos = 0;
}

tt_inline void tt_buf_backup_rwp(IN tt_buf_t *buf,
                                 OUT tt_u32_t *rpos,
                                 OUT tt_u32_t *wpos)
{
    *wpos = buf->wpos;
    *rpos = buf->rpos;
}

tt_inline void tt_buf_restore_rwp(IN tt_buf_t *buf,
                                  IN tt_u32_t *rpos,
                                  IN tt_u32_t *wpos)
{
    buf->wpos = *wpos;
    buf->rpos = *rpos;
}

tt_inline void tt_buf_clear(IN tt_buf_t *buf)
{
    tt_buf_reset_rwp(buf);
}

tt_inline tt_bool_t tt_buf_empty(IN tt_buf_t *buf)
{
    return TT_BOOL(buf->rpos == buf->wpos);
}

extern tt_u32_t tt_buf_refine(IN tt_buf_t *buf);

tt_inline tt_u32_t tt_buf_try_refine(IN tt_buf_t *buf, IN tt_u32_t threshold)
{
    if (tt_buf_empty(buf) || (TT_BUF_REFINABLE(buf) >= threshold)) {
        return tt_buf_refine(buf);
    } else {
        return 0;
    }
}

// ========================================
// io operation
// ========================================

extern tt_result_t tt_buf_put(IN tt_buf_t *buf,
                              IN const tt_u8_t *data,
                              IN tt_u32_t data_len);

extern tt_result_t tt_buf_put_rep(IN tt_buf_t *buf,
                                  IN tt_u8_t byte,
                                  IN tt_u32_t rep_num);

extern tt_result_t tt_buf_put_rand(IN tt_buf_t *buf, IN tt_u32_t len);

// note get() return fail when size of data in buf
// is less than buf_len, rather than putting partial
// data to the buf
extern tt_result_t tt_buf_get(IN tt_buf_t *buf, IN tt_u8_t *p, IN tt_u32_t len);

// returned p is from buf, the pointer is for temporarily usage
// once buf is extended, the pointer becomes a wild pointer
extern tt_result_t tt_buf_get_nocopy(IN tt_buf_t *buf,
                                     OUT tt_u8_t **p,
                                     IN tt_u32_t len);

// return number of bytes written to p, this function won't append the
// terminating null
extern tt_u32_t tt_buf_get_hexstr(IN tt_buf_t *buf,
                                  OUT OPT tt_char_t *p,
                                  IN OUT tt_u32_t addr_len);

extern tt_result_t tt_buf_peek(IN tt_buf_t *buf,
                               IN tt_u8_t *p,
                               IN tt_u32_t len);

#endif /* __TT_BUFFER__ */
