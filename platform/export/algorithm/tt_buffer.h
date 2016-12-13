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

#include <misc/tt_util.h>

#include <tt_buffer_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_BUFPOS_NUM (~0)

// should be power of 2
#define TT_BUF_INLINE_SIZE 16

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
#define TT_BUF_RPOS(buf) TT_PTR_INC(tt_u8_t, (buf)->addr, (buf)->rd_pos)
#define TT_BUF_RLEN(buf) ((buf)->wr_pos - (buf)->rd_pos)

#define TT_BUF_WPOS(buf) TT_PTR_INC(tt_u8_t, (buf)->addr, (buf)->wr_pos)
#define TT_BUF_WLEN(buf) ((buf)->size - (buf)->wr_pos)

#define TT_BUF_EPOS(buf) TT_PTR_INC(tt_u8_t, (buf)->addr, (buf)->size)

// size that can be refined
#define TT_BUF_REFINABLE(buf) ((buf)->rd_pos)

#define TT_BUF_CHECK(buf)                                                      \
    do {                                                                       \
        TT_ASSERT((buf)->addr != NULL);                                        \
        TT_ASSERT((buf)->rd_pos <= (buf)->wr_pos);                             \
        TT_ASSERT((buf)->wr_pos <= (buf)->size);                               \
    } while (0)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    // - if current < (1<<min_order), next size would be directly set
    //   to (1<<min_order)
    // - if current > (1<<max_order), next size would current plusing
    //   (1<<max_order)
    // - otherwise, next size would be twice as large as current value
    tt_u32_t min_expand_order;
    tt_u32_t max_expand_order;

    tt_u32_t max_size_order;
#define TT_BUF_MAX_SIZE_ORDER (30) // 1G

    tt_bool_t not_copied : 1;

} tt_buf_attr_t;

typedef struct tt_buf_s
{
    tt_u8_t *addr;
    tt_u32_t size;
    tt_u32_t wr_pos;
    tt_u32_t rd_pos;
    tt_u8_t buf_inline[TT_BUF_INLINE_SIZE];

    tt_buf_attr_t attr;
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

// increase size by expand_size
// set new_size to 0 to let function make decision itself
extern tt_result_t tt_buf_expand(IN tt_buf_t *buf, IN OPT tt_u32_t expand_size);

// set new size to new_size if current size is less than it
tt_inline tt_result_t tt_buf_resize(IN tt_buf_t *buf, IN tt_u32_t new_size)
{
    if (new_size <= buf->size) {
        // to shrink?
        return TT_SUCCESS;
    }

    return tt_buf_expand(buf, new_size - buf->size);
}

// to make sure the buf has reserve_size available bytes
extern tt_result_t tt_buf_reserve(IN tt_buf_t *buf, IN tt_u32_t reserve_size);

extern tt_result_t tt_buf_shrink(IN tt_buf_t *buf);

// ========================================
// position operation
// ========================================

extern void tt_buf_getptr_rp(IN tt_buf_t *buf,
                             OUT OPT tt_u8_t **p,
                             OUT OPT tt_u32_t *len);

tt_inline void tt_buf_getptr_rpblob(IN tt_buf_t *buf, OUT tt_blob_t *blob)
{
    tt_buf_getptr_rp(buf, &blob->addr, &blob->len);
}

// rd_ptr could be in [buf start, wr_pos)
extern tt_result_t tt_buf_setptr_rp(IN tt_buf_t *buf, IN tt_u8_t *rd_ptr);

extern tt_result_t tt_buf_inc_rp(IN tt_buf_t *buf, IN tt_u32_t num);

extern tt_result_t tt_buf_dec_rp(IN tt_buf_t *buf, IN tt_u32_t num);

tt_inline void tt_buf_reset_rp(IN tt_buf_t *buf)
{
    buf->rd_pos = 0;
}

extern void tt_buf_getptr_wp(IN tt_buf_t *buf,
                             OUT OPT tt_u8_t **p,
                             OUT OPT tt_u32_t *len);

tt_inline void tt_buf_getptr_wpblob(IN tt_buf_t *buf, OUT tt_blob_t *blob)
{
    tt_buf_getptr_wp(buf, &blob->addr, &blob->len);
}

extern tt_result_t tt_buf_setptr_wr(IN tt_buf_t *buf, IN tt_u8_t *wr_ptr);

extern tt_result_t tt_buf_inc_wp(IN tt_buf_t *buf, IN tt_u32_t num);

extern tt_result_t tt_buf_dec_wp(IN tt_buf_t *buf, IN tt_u32_t num);

tt_inline void tt_buf_reset_rwp(IN tt_buf_t *buf)
{
    // this function should be called at a proper point. if one has
    // ever passed wr_pos to some other async read calls, and later he
    // found rd_pos meet wr_pos and do reset_pos() before async io
    // notification, then the buf status becomes inconsistent when
    // async io is done: the rd_pos and wr_pos are 0 but data read by
    // the async read call are put at original wr_pos. so be sure
    // there is no pending async io when you try to reset pos.

    buf->rd_pos = 0;
    buf->wr_pos = 0;
}

tt_inline void tt_buf_backup_rwp(IN tt_buf_t *buf,
                                 OUT tt_u32_t *rd_pos,
                                 OUT tt_u32_t *wr_pos)
{
    *wr_pos = buf->wr_pos;
    *rd_pos = buf->rd_pos;
}
tt_inline void tt_buf_restore_rwp(IN tt_buf_t *buf,
                                  IN tt_u32_t *rd_pos,
                                  IN tt_u32_t *wr_pos)
{
    buf->wr_pos = *wr_pos;
    buf->rd_pos = *rd_pos;
}

extern tt_u32_t tt_buf_refine(IN tt_buf_t *buf);

tt_inline void tt_buf_clear(IN tt_buf_t *buf)
{
    tt_buf_reset_rwp(buf);
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
extern tt_result_t tt_buf_get(IN tt_buf_t *buf,
                              IN tt_u8_t *addr,
                              IN tt_u32_t len);

// returned addr is from buf, the pointer is for temporarily usage
// once buf is extended, the pointer becomes a wild pointer
extern tt_result_t tt_buf_getptr(IN tt_buf_t *buf,
                                 OUT tt_u8_t **addr,
                                 IN tt_u32_t len);

// return number of bytes written to addr, this function won't append the
// terminating null
extern tt_u32_t tt_buf_get_hexstr(IN tt_buf_t *buf,
                                  OUT OPT tt_char_t *addr,
                                  IN OUT tt_u32_t addr_len);

extern tt_result_t tt_buf_peek(IN tt_buf_t *buf,
                               IN tt_u8_t *addr,
                               IN tt_u32_t len);

#endif /* __TT_BUFFER__ */
