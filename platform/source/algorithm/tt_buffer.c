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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>

#include <algorithm/tt_buffer_format.h>
#include <memory/tt_memory_alloc.h>
#include <os/tt_thread.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __BUF_MIN_EXPAND_ORDER (7) // 128bytes
#define __BUF_MAX_EXPAND_ORDER (tt_g_page_size_order) // 1 page

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

static tt_result_t __buf_size_align(IN tt_buf_attr_t *attr,
                                    IN OUT tt_u32_t *new_size);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_buf_create(IN tt_buf_t *buf,
                          IN tt_u32_t size,
                          IN OPT tt_buf_attr_t *attr)
{
    TT_ASSERT(buf != NULL);

    if (attr != NULL) {
        tt_buf_attr_t *buf_attr = &buf->attr;

        tt_memcpy(buf_attr, attr, sizeof(tt_buf_attr_t));

        if (buf_attr->min_expand_order < __BUF_MIN_EXPAND_ORDER) {
            buf_attr->min_expand_order = __BUF_MIN_EXPAND_ORDER;
        }

        // max_expand_order has no upper limitation, but must be larger
        // than min expand order
        if (buf_attr->max_expand_order < buf_attr->min_expand_order) {
            buf_attr->max_expand_order = buf_attr->min_expand_order;
        }

        if (buf_attr->max_size_order < buf_attr->max_expand_order) {
            buf_attr->max_size_order = buf_attr->max_expand_order;
        }
    } else {
        tt_buf_attr_default(&buf->attr);
    }

    if (size < TT_BUF_INLINE_SIZE) {
        buf->addr = buf->buf_inline;
        buf->size = TT_BUF_INLINE_SIZE;
    } else {
        if (!TT_OK(__buf_size_align(&buf->attr, &size))) {
            return TT_FAIL;
        }

        size = tt_mem_size(size);
        buf->addr = (tt_u8_t *)tt_mem_alloc(size);
        if (buf->addr == NULL) {
            TT_ERROR("no mem for buf buf");
            return TT_NO_RESOURCE;
        }

        buf->size = size;
    }

    buf->wr_pos = 0;
    buf->rd_pos = 0;

    return TT_SUCCESS;
}

tt_result_t tt_buf_create_copy(IN tt_buf_t *buf,
                               IN tt_u8_t *data,
                               IN tt_u32_t data_len,
                               IN OPT tt_buf_attr_t *attr)
{
    if (!TT_OK(tt_buf_create(buf, data_len, attr))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_buf_put(buf, data, data_len))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_buf_create_nocopy(IN tt_buf_t *buf,
                                 IN const tt_u8_t *data,
                                 IN tt_u32_t data_len,
                                 IN OPT tt_buf_attr_t *attr)
{
    if (!TT_OK(tt_buf_create(buf, 0, attr))) {
        return TT_FAIL;
    }

    buf->addr = (tt_u8_t *)data;
    buf->size = data_len;
    buf->wr_pos = data_len;

    buf->attr.not_copied = TT_TRUE;

    return TT_SUCCESS;
}

void tt_buf_init(IN tt_buf_t *buf, IN OPT tt_buf_attr_t *attr)
{
    tt_buf_create(buf, 0, attr);
    // never fail
}

void tt_buf_attr_default(IN tt_buf_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->min_expand_order = __BUF_MIN_EXPAND_ORDER;
    attr->max_expand_order = __BUF_MAX_EXPAND_ORDER;
    attr->max_size_order = TT_BUF_MAX_SIZE_ORDER;

    attr->not_copied = TT_FALSE;
}

void tt_buf_destroy(IN tt_buf_t *buf)
{
    TT_ASSERT(buf != NULL);

    TT_ASSERT_BUF(buf->addr != NULL);
    if ((buf->addr != buf->buf_inline) && !buf->attr.not_copied) {
        tt_mem_free(buf->addr);
    }
}

void tt_buf_print(IN tt_buf_t *buf, IN tt_u32_t flag)
{
    tt_hex_dump(TT_BUF_RPOS(buf), TT_BUF_RLEN(buf), 16);
}

void tt_buf_print_hexstr(IN tt_buf_t *buf, IN tt_u32_t flag)
{
    tt_u32_t len;
    tt_char_t *hs;

    len = tt_buf_get_hexstr(buf, NULL, 0);
    if (len == 0) {
        TT_INFO("empty buf");
        return;
    }

    hs = tt_mem_alloc(len + 1);
    if (hs == NULL) {
        return;
    }

    len = tt_buf_get_hexstr(buf, hs, len);
    hs[len] = 0;
    TT_INFO("%s", hs);
    tt_mem_free(hs);
}

void tt_buf_print_cstr(IN tt_buf_t *buf, IN tt_u32_t flag)
{
    if (TT_BUF_WLEN(buf) > 0) {
        *TT_BUF_WPOS(buf) = 0;
        TT_INFO("%s", TT_BUF_RPOS(buf));
    }
}

tt_result_t tt_buf_set(IN tt_buf_t *buf,
                       IN tt_u32_t pos,
                       IN tt_u8_t *data,
                       IN tt_u32_t data_len)
{
    // care overflow...
    if ((buf->rd_pos + pos) > buf->wr_pos) {
        TT_ERROR("invalid pos");
        return TT_FAIL;
    }
    if ((buf->rd_pos + pos + data_len) > buf->wr_pos) {
        TT_ERROR("invalid data_len");
        return TT_FAIL;
    }

    tt_memcpy(&buf->addr[buf->rd_pos + pos], data, data_len);
    return TT_SUCCESS;
}

// ========================================
// memory operation
// ========================================

tt_result_t tt_buf_expand(IN tt_buf_t *buf, IN OPT tt_u32_t expand_size)
{
    tt_u8_t *new_buf;
    tt_u32_t new_size;

    if (buf->attr.not_copied) {
        TT_ERROR("not expandable buf");
        return TT_FAIL;
    }

    if (expand_size != 0) {
        new_size = buf->size + expand_size;
    } else {
        new_size = buf->size + 1;
        // new_size would be aligned, expanded space
        // won't be only 1 byte
    }

    if (!TT_OK(__buf_size_align(&buf->attr, &new_size))) {
        return TT_FAIL;
    }

    new_size = tt_mem_size(new_size);
    TT_ASSERT_BUF(new_size > buf->size);
    // note here new_size is enlarged again, and may exceed
    // limitation: 1 << buf->attr.max_size_order. but this
    // is not a big problem, expanding will be prohibited the
    // next time it tries to expand

    new_buf = (tt_u8_t *)tt_mem_alloc(new_size);
    if (new_buf == NULL) {
        TT_ERROR("no mem for expanding");
        return TT_NO_RESOURCE;
    }

    TT_ASSERT_BUF(buf->addr != NULL);
    TT_ASSERT_BUF(buf->wr_pos >= buf->rd_pos);
    if (buf->wr_pos > buf->rd_pos) {
        // only copy [rd_pos, wr_pos)
        tt_memcpy(TT_PTR_INC(tt_u8_t, new_buf, buf->rd_pos),
                  TT_PTR_INC(tt_u8_t, buf->addr, buf->rd_pos),
                  buf->wr_pos - buf->rd_pos);

        if (buf->addr != buf->buf_inline) {
            tt_mem_free(buf->addr);
        }
    }

    buf->addr = new_buf;
    buf->size = new_size;

    // keep other members value

    return TT_SUCCESS;
}

tt_result_t tt_buf_reserve(IN tt_buf_t *buf, IN tt_u32_t reserve_size)
{
    tt_u32_t cur_space = TT_BUF_WLEN(buf);
    if (cur_space < reserve_size) {
        return tt_buf_expand(buf, reserve_size - cur_space);
    }
    return TT_SUCCESS;
}

tt_result_t tt_buf_shrink(IN tt_buf_t *buf)
{
    tt_u32_t len;

    if (buf->attr.not_copied) {
        TT_ERROR("not copied data");
        return TT_FAIL;
    }

    TT_BUF_CHECK(buf);

    len = TT_BUF_RLEN(buf);
    if (len <= TT_BUF_INLINE_SIZE) {
        tt_memmove(buf->buf_inline, TT_BUF_RPOS(buf), len);

        if (buf->addr != buf->buf_inline) {
            tt_mem_free(buf->addr);
        }
        buf->addr = buf->buf_inline;
        buf->size = TT_BUF_INLINE_SIZE;
        buf->rd_pos = 0;
        buf->wr_pos = len;
        TT_BUF_CHECK(buf);

        return TT_SUCCESS;
    } else {
        tt_u8_t *new_buf;
        tt_u32_t new_size;

        new_size = len;
        __buf_size_align(&buf->attr, &new_size);
        new_size = tt_mem_size(new_size);
        TT_ASSERT_BUF(new_size >= len);

        new_buf = (tt_u8_t *)tt_mem_alloc(new_size);
        if (new_buf == NULL) {
            TT_ERROR("no mem for shrinking");
            return TT_NO_RESOURCE;
        }
        tt_memcpy(new_buf, TT_BUF_RPOS(buf), len);

        if (buf->addr != buf->buf_inline) {
            tt_mem_free(buf->addr);
        }
        buf->addr = new_buf;
        buf->size = new_size;
        buf->rd_pos = 0;
        buf->wr_pos = len;
        TT_BUF_CHECK(buf);

        return TT_SUCCESS;
    }
}

// ========================================
// position operation
// ========================================

void tt_buf_getptr_rp(IN tt_buf_t *buf,
                      OUT OPT tt_u8_t **p,
                      OUT OPT tt_u32_t *len)
{
    TT_ASSERT_BUF(buf->addr != NULL);
    TT_ASSERT_BUF(buf->wr_pos >= buf->rd_pos);

    TT_SAFE_ASSIGN(p, TT_BUF_RPOS(buf));
    TT_SAFE_ASSIGN(len, TT_BUF_RLEN(buf));
}

tt_result_t tt_buf_setptr_rp(IN tt_buf_t *buf, IN tt_u8_t *rd_ptr)
{
    if (rd_ptr < buf->addr) {
        TT_ERROR("rd_ptr[%p], buf[%p]", rd_ptr, buf->addr);
        return TT_FAIL;
    }

    if (rd_ptr > TT_BUF_WPOS(buf)) {
        TT_ERROR("rd_ptr[%p], wpos[%d]", rd_ptr, TT_BUF_WPOS(buf));
        return TT_FAIL;
    }

    buf->rd_pos = (tt_u32_t)(rd_ptr - buf->addr);
    TT_BUF_CHECK(buf);
    return TT_SUCCESS;
}

tt_result_t tt_buf_setptr_wr(IN tt_buf_t *buf, IN tt_u8_t *wr_ptr)
{
    if (wr_ptr < TT_BUF_RPOS(buf)) {
        TT_ERROR("wr_ptr[%p], rpos[%p]", wr_ptr, TT_BUF_RPOS(buf));
        return TT_FAIL;
    }

    if (wr_ptr > TT_BUF_EPOS(buf)) {
        TT_ERROR("wr_ptr[%p], epos[%d]", wr_ptr, TT_BUF_EPOS(buf));
        return TT_FAIL;
    }

    buf->wr_pos = (tt_u32_t)(wr_ptr - buf->addr);
    TT_BUF_CHECK(buf);
    return TT_SUCCESS;
}

tt_result_t tt_buf_inc_rp(IN tt_buf_t *buf, IN tt_u32_t num)
{
    tt_u32_t len;

    TT_BUF_CHECK(buf);
    len = TT_BUF_RLEN(buf);

    if (num <= len) {
        buf->rd_pos += num;
        return TT_SUCCESS;
    } else {
        // no more data, return incomplete
        return TT_BUFFER_INCOMPLETE;
    }
}

tt_result_t tt_buf_dec_rp(IN tt_buf_t *buf, IN tt_u32_t num)
{
    TT_BUF_CHECK(buf);

    if (num <= buf->rd_pos) {
        buf->rd_pos -= num;
        return TT_SUCCESS;
    } else {
        TT_ERROR("underflow: %d - %d", buf->rd_pos, num);
        return TT_FAIL;
    }
}

void tt_buf_getptr_wp(IN tt_buf_t *buf,
                      OUT OPT tt_u8_t **p,
                      OUT OPT tt_u32_t *len)
{
    TT_ASSERT_BUF(buf->addr != NULL);
    TT_ASSERT_BUF(buf->size >= buf->wr_pos);

    TT_SAFE_ASSIGN(p, TT_BUF_WPOS(buf));
    TT_SAFE_ASSIGN(len, TT_BUF_WLEN(buf));
}

tt_result_t tt_buf_inc_wp(IN tt_buf_t *buf, IN tt_u32_t num)
{
    tt_u32_t len;

    TT_BUF_CHECK(buf);
    len = TT_BUF_WLEN(buf);

    if (num <= len) {
        buf->wr_pos += num;
        return TT_SUCCESS;
    } else {
        // written more than data in buffer is an error
        return TT_FAIL;
    }
}

tt_result_t tt_buf_dec_wp(IN tt_buf_t *buf, IN tt_u32_t num)
{
    tt_u32_t len;

    TT_BUF_CHECK(buf);
    len = TT_BUF_RLEN(buf);

    if (num <= len) {
        buf->wr_pos -= num;
        return TT_SUCCESS;
    } else {
        TT_ERROR("overflow: %d - %d", buf->wr_pos, num);
        return TT_FAIL;
    }
}

tt_u32_t tt_buf_refine(IN tt_buf_t *buf)
{
    tt_u32_t n, refined;

    // - without refine() rd_pos and wr_pos are increased for ever, but
    //   the space [0, rd_pos) is available of storing data. refine()
    //   actually move data [rd_pos, wr_pos) to buf beginning.
    // - frequently calling refind() on a large buf may impact performance,
    //   so a proper way of using it on a large buf is checking wr_pos and
    //   do refine() when wr_pos reaching some threshold

    TT_ASSERT_BUF(buf->addr != NULL);
    if (buf->rd_pos == 0) {
        return 0;
    }
    refined = buf->rd_pos;

    if (buf->rd_pos == buf->wr_pos) {
        tt_buf_reset_rwp(buf);
        return refined;
    }

    if (buf->attr.not_copied) {
        TT_WARN("not writable buf");
        return 0;
    } else {
        TT_BUF_CHECK(buf);
        n = buf->wr_pos - buf->rd_pos;
        tt_memmove(buf->addr, TT_BUF_RPOS(buf), n);
        buf->rd_pos = 0;
        buf->wr_pos = n;
        return refined;
    }
}

// ========================================
// io operation
// ========================================

tt_result_t tt_buf_put(IN tt_buf_t *buf,
                       IN const tt_u8_t *data,
                       IN tt_u32_t data_len)
{
    TT_BUF_CHECK(buf);

    if (buf->attr.not_copied) {
        TT_ERROR("not writable buf");
        return TT_FAIL;
    }

    if (!TT_OK(tt_buf_reserve(buf, data_len))) {
        return TT_FAIL;
    }

    if ((data != NULL) && (data_len != 0)) {
        tt_memcpy(TT_BUF_WPOS(buf), data, data_len);
        buf->wr_pos += data_len;
    }

    return TT_SUCCESS;
}

tt_result_t tt_buf_put_rep(IN tt_buf_t *buf,
                           IN tt_u8_t byte,
                           IN tt_u32_t rep_num)
{
    TT_BUF_CHECK(buf);

    if (buf->attr.not_copied) {
        TT_ERROR("not writable buf");
        return TT_FAIL;
    }

    if (!TT_OK(tt_buf_reserve(buf, rep_num))) {
        return TT_FAIL;
    }

    if (rep_num != 0) {
        tt_memset(TT_BUF_WPOS(buf), byte, rep_num);
        buf->wr_pos += rep_num;
    }

    return TT_SUCCESS;
}

tt_result_t tt_buf_put_rand(IN tt_buf_t *buf, IN tt_u32_t len)
{
    tt_u32_t n = 0;

    TT_BUF_CHECK(buf);

    if (buf->attr.not_copied) {
        TT_ERROR("not writable buf");
        return TT_FAIL;
    }

    if (!TT_OK(tt_buf_reserve(buf, len))) {
        return TT_FAIL;
    }

    while (n + sizeof(tt_u64_t) < len) {
        TT_DO(tt_buf_put_u64(buf, tt_rand_u64()));
        n += sizeof(tt_u64_t);
    }
    while (n < len) {
        TT_DO(tt_buf_put_u8(buf, tt_rand_u32() & 0xFF));
        n += sizeof(tt_u8_t);
    }

    return TT_SUCCESS;
}

tt_result_t tt_buf_get(IN tt_buf_t *buf, IN tt_u8_t *addr, IN tt_u32_t len)
{
    TT_DO(tt_buf_peek(buf, addr, len));
    buf->rd_pos += len;

    return TT_SUCCESS;
}

tt_result_t tt_buf_getptr(IN tt_buf_t *buf, OUT tt_u8_t **addr, IN tt_u32_t len)
{
    if (len == 0) {
        *addr = TT_BUF_RPOS(buf);
        return TT_SUCCESS;
    }

    if (TT_BUF_RLEN(buf) < len) {
        return TT_BUFFER_INCOMPLETE;
    }

    *addr = TT_BUF_RPOS(buf);
    buf->rd_pos += len;

    return TT_SUCCESS;
}

tt_u32_t tt_buf_get_hexstr(IN tt_buf_t *buf,
                           OUT OPT tt_char_t *addr,
                           IN tt_u32_t addr_len)
{
    tt_u32_t buf_idx, addr_idx;

    if ((addr == NULL) || (addr_len == 0)) {
        return 2 * TT_BUF_RLEN(buf);
    }

    buf_idx = buf->rd_pos;
    addr_idx = 0;
    while (buf_idx < buf->wr_pos) {
        tt_u8_t v;

        if (addr_idx >= addr_len) {
            break;
        }
        v = buf->addr[buf_idx] >> 4;
        addr[addr_idx++] = tt_h2c(v, '?');

        if (addr_idx >= addr_len) {
            break;
        }
        v = buf->addr[buf_idx] & 0xF;
        addr[addr_idx++] = tt_h2c(v, '?');

        ++buf_idx;
    }
    return addr_idx;
}

tt_result_t tt_buf_peek(IN tt_buf_t *buf, IN tt_u8_t *addr, IN tt_u32_t len)
{
    if (len == 0) {
        return TT_SUCCESS;
    }

    if (TT_BUF_RLEN(buf) < len) {
        return TT_BUFFER_INCOMPLETE;
    }

    tt_memcpy(addr, TT_BUF_RPOS(buf), len);

    return TT_SUCCESS;
}

tt_result_t __buf_size_align(IN tt_buf_attr_t *attr, IN OUT tt_u32_t *new_size)
{
    tt_u32_t min_expand = 1 << attr->min_expand_order;
    tt_u32_t max_expand = 1 << attr->max_expand_order;
    tt_u32_t __new_size = *new_size;

    TT_ASSERT_BUF(max_expand >= min_expand);

    if (__new_size >= max_expand) {
        // linear
        TT_U32_ALIGN_INC(__new_size, attr->max_expand_order);
    } else if (__new_size >= min_expand) {
        // exponential
        __new_size = 1 << tt_least_2power(__new_size);
    } else {
        __new_size = min_expand;
    }

    if (__new_size > ((tt_u32_t)1 << attr->max_size_order)) {
        TT_ERROR("new size[%d] exceeds limit[%d]",
                 __new_size,
                 (1 << attr->max_size_order));
        return TT_NO_RESOURCE;
    }

    *new_size = __new_size;
    return TT_SUCCESS;
}
