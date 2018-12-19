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

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_buf_create(IN tt_buf_t *buf,
                          IN tt_u32_t size,
                          IN OPT tt_buf_attr_t *attr)
{
    tt_buf_attr_t __attr;

    TT_ASSERT(buf != NULL);

    if (attr == NULL) {
        tt_buf_attr_default(&__attr);
        attr = &__attr;
    }
    tt_memspg_init(&buf->mspg,
                   attr->min_extend,
                   attr->max_extend,
                   attr->max_limit);

    if (size < TT_BUF_INIT_SIZE) {
        buf->p = buf->initbuf;
        buf->size = TT_BUF_INIT_SIZE;
    } else {
        buf->p = NULL;
        buf->size = 0;
        TT_DO(tt_memspg_extend(&buf->mspg, &buf->p, &buf->size, size));
    }

    buf->wpos = 0;
    buf->rpos = 0;
    tt_memset(buf->initbuf, 0, TT_BUF_INIT_SIZE);
    buf->readonly = TT_FALSE;

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

    buf->p = (tt_u8_t *)data;
    buf->size = data_len;
    buf->wpos = data_len;

    buf->readonly = TT_TRUE;

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

    attr->min_extend = 64;
    attr->max_extend = (1 << 12); // 4K
    attr->max_limit = 0;
}

void tt_buf_destroy(IN tt_buf_t *buf)
{
    TT_ASSERT(buf != NULL);

    TT_ASSERT_BUF(buf->p != NULL);
    if ((buf->p != buf->initbuf) && !buf->readonly) {
        tt_memspg_compress(&buf->mspg, &buf->p, &buf->size, 0);
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

    hs = tt_malloc(len + 1);
    if (hs == NULL) {
        return;
    }

    len = tt_buf_get_hexstr(buf, hs, len);
    hs[len] = 0;
    TT_INFO("%s", hs);
    tt_free(hs);
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
    if ((buf->rpos + pos) < pos) {
        TT_ERROR("overflow");
        return TT_FAIL;
    }

    // [buf->rpos + pos, buf->rpos + pos + data_len) must be
    // in readable area
    if ((buf->rpos + pos) > buf->wpos) {
        TT_ERROR("invalid pos");
        return TT_FAIL;
    }
    if ((buf->rpos + pos + data_len) > buf->wpos) {
        TT_ERROR("invalid data_len");
        return TT_FAIL;
    }

    tt_memcpy(&buf->p[buf->rpos + pos], data, data_len);
    TT_BUF_CHECK(buf);
    return TT_SUCCESS;
}

tt_result_t tt_buf_set_range(IN tt_buf_t *buf,
                             IN tt_u32_t pos,
                             IN tt_u32_t len,
                             IN tt_u8_t *data,
                             IN tt_u32_t data_len)
{
    if (TT_ADD_WOULD_OVFL(tt_u32_t, buf->rpos, pos, tt_u64_t) ||
        TT_ADD_WOULD_OVFL(tt_u32_t, buf->rpos, pos + len, tt_u64_t)) {
        TT_ERROR("overflow");
        return TT_FAIL;
    }

    // [buf->rpos + pos, buf->rpos + pos + data_len) must be
    // in readable area
    if ((buf->rpos + pos) > buf->wpos) {
        TT_ERROR("invalid pos");
        return TT_FAIL;
    }
    if ((buf->rpos + pos + len) > buf->wpos) {
        TT_ERROR("invalid len");
        return TT_FAIL;
    }

    if (len < data_len) {
        tt_u8_t *p;
        tt_u32_t n = data_len - len;
        TT_DO(tt_buf_reserve(buf, n));
        p = &buf->p[buf->rpos + pos];
        tt_memmove(p + data_len, p + len, TT_BUF_RLEN(buf) - pos - len);
        tt_memcpy(p, data, data_len);
        buf->wpos += n;
    } else if (len == data_len) {
        tt_memcpy(&buf->p[buf->rpos + pos], data, data_len);
    } else {
        tt_u8_t *p = &buf->p[buf->rpos + pos];
        tt_u32_t n = len - data_len;
        tt_memmove(p + data_len, p + len, TT_BUF_RLEN(buf) - pos - len);
        tt_memcpy(p, data, data_len);
        buf->wpos -= n;
    }

    return TT_SUCCESS;
}

// ========================================
// memory operation
// ========================================

tt_result_t __buf_extend(IN tt_buf_t *buf, IN tt_u32_t size)
{
    tt_u32_t flag = 0;

    if (buf->readonly) {
        TT_ERROR("readonly buf");
        return TT_FAIL;
    }

    if (buf->p == buf->initbuf) {
        flag |= TT_MSPGEXT_NOFREE;
    }

    return tt_memspg_extend_ex(&buf->mspg, &buf->p, &buf->size, size, flag);
}

tt_result_t tt_buf_compress(IN tt_buf_t *buf)
{
    tt_u32_t len;

    if (buf->readonly) {
        TT_ERROR("readonly buf");
        return TT_FAIL;
    }

    len = TT_BUF_RLEN(buf);
    if (len <= TT_BUF_INIT_SIZE) {
        tt_memmove(buf->initbuf, TT_BUF_RPOS(buf), len);
        if (buf->p != buf->initbuf) {
            tt_memspg_compress(&buf->mspg, &buf->p, &buf->size, 0);
        }

        buf->p = buf->initbuf;
        buf->size = TT_BUF_INIT_SIZE;
        buf->rpos = 0;
        buf->wpos = len;
        TT_BUF_CHECK(buf);

        return TT_SUCCESS;
    } else {
        return tt_memspg_compress_range(&buf->mspg,
                                        &buf->p,
                                        &buf->size,
                                        buf->rpos,
                                        buf->wpos);
    }
}

// ========================================
// position operation
// ========================================

tt_u32_t tt_buf_refine(IN tt_buf_t *buf)
{
    tt_u32_t refined;

    if (buf->readonly) {
        TT_WARN("readonly buf");
        return 0;
    }

    // - without refine() rpos and wpos are increased for ever, but
    //   the space [0, rpos) is available of storing data. refine()
    //   actually move data [rpos, wpos) to buf beginning.
    // - frequently calling refind() on a large buf may impact performance,
    //   so a proper way of using it on a large buf is checking wpos and
    //   do refine() when wpos reaching some threshold
    if (buf->rpos == 0) {
        return 0;
    }

    refined = buf->rpos;
    if (buf->rpos == buf->wpos) {
        tt_buf_reset_rwp(buf);
    } else {
        tt_u32_t len = TT_BUF_RLEN(buf);
        tt_memmove(buf->p, TT_BUF_RPOS(buf), len);
        buf->rpos = 0;
        buf->wpos = len;
    }
    TT_BUF_CHECK(buf);
    return refined;
}

// ========================================
// io operation
// ========================================

tt_result_t tt_buf_put(IN tt_buf_t *buf,
                       IN const tt_u8_t *data,
                       IN tt_u32_t data_len)
{
    if (buf->readonly) {
        TT_ERROR("readonly buf");
        return TT_FAIL;
    }

    if ((data != NULL) && (data_len != 0)) {
        TT_DO(tt_buf_reserve(buf, data_len));
        tt_memcpy(TT_BUF_WPOS(buf), data, data_len);
        buf->wpos += data_len;
        TT_BUF_CHECK(buf);
    }

    return TT_SUCCESS;
}

tt_result_t tt_buf_put_head(IN tt_buf_t *buf,
                            IN const tt_u8_t *data,
                            IN tt_u32_t data_len)
{
    if (buf->readonly) {
        TT_ERROR("readonly buf");
        return TT_FAIL;
    }

    if (buf->rpos < data_len) {
        tt_u32_t n;

        // reserve data_len or (data_len - rpos)?
        // n = data_len - buf->rpos;
        n = data_len;

        if (!TT_OK(tt_buf_reserve(buf, n))) {
            return TT_FAIL;
        }

        tt_memmove(TT_BUF_RPOS(buf) + n, TT_BUF_RPOS(buf), TT_BUF_RLEN(buf));
        buf->rpos += n;
        buf->wpos += n;
    }

    TT_ASSERT(buf->rpos >= data_len);
    tt_memcpy(TT_BUF_RPOS(buf) - data_len, data, data_len);
    buf->rpos -= data_len;

    return TT_SUCCESS;
}

tt_result_t tt_buf_put_rep(IN tt_buf_t *buf,
                           IN tt_u8_t byte,
                           IN tt_u32_t rep_num)
{
    if (buf->readonly) {
        TT_ERROR("readonly buf");
        return TT_FAIL;
    }

    if (rep_num != 0) {
        TT_DO(tt_buf_reserve(buf, rep_num));
        tt_memset(TT_BUF_WPOS(buf), byte, rep_num);
        buf->wpos += rep_num;
        TT_BUF_CHECK(buf);
    }

    return TT_SUCCESS;
}

tt_result_t tt_buf_put_rand(IN tt_buf_t *buf, IN tt_u32_t len)
{
    if (buf->readonly) {
        TT_ERROR("readonly buf");
        return TT_FAIL;
    }

    if (len != 0) {
        tt_u32_t n = 0;

        TT_DO(tt_buf_reserve(buf, len));

        while ((n + sizeof(tt_u64_t)) <= len) {
            TT_DO(tt_buf_put_u64(buf, tt_rand_u64()));
            n += sizeof(tt_u64_t);
        }
        while (n < len) {
            TT_DO(tt_buf_put_u8(buf, (tt_u8_t)tt_rand_u32()));
            n += sizeof(tt_u8_t);
        }
    }

    return TT_SUCCESS;
}

tt_result_t tt_buf_get(IN tt_buf_t *buf, IN tt_u8_t *p, IN tt_u32_t len)
{
    TT_DO(tt_buf_peek(buf, p, len));
    buf->rpos += len;

    return TT_SUCCESS;
}

tt_result_t tt_buf_get_nocopy(IN tt_buf_t *buf,
                              OUT tt_u8_t **p,
                              IN tt_u32_t len)
{
    if (TT_BUF_RLEN(buf) < len) {
        return TT_E_BUF_NOBUFS;
    }

    *p = TT_BUF_RPOS(buf);
    buf->rpos += len;
    return TT_SUCCESS;
}

tt_u32_t tt_buf_get_hexstr(IN tt_buf_t *buf,
                           OUT OPT tt_char_t *p,
                           IN tt_u32_t addr_len)
{
    tt_u32_t buf_idx, addr_idx;

    if ((p == NULL) || (addr_len == 0)) {
        return 2 * TT_BUF_RLEN(buf);
    }

    buf_idx = buf->rpos;
    addr_idx = 0;
    while (buf_idx < buf->wpos) {
        tt_u8_t v;

        if (addr_idx >= addr_len) {
            break;
        }
        v = buf->p[buf_idx] >> 4;
        p[addr_idx++] = tt_h2c(v, '?');

        if (addr_idx >= addr_len) {
            break;
        }
        v = buf->p[buf_idx] & 0xF;
        p[addr_idx++] = tt_h2c(v, '?');

        ++buf_idx;
    }
    return addr_idx;
}

tt_result_t tt_buf_peek(IN tt_buf_t *buf, IN tt_u8_t *p, IN tt_u32_t len)
{
    if (TT_BUF_RLEN(buf) < len) {
        return TT_E_BUF_NOBUFS;
    }

    tt_memcpy(p, TT_BUF_RPOS(buf), len);
    return TT_SUCCESS;
}
