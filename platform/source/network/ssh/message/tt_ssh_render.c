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

#include <network/ssh/message/tt_ssh_render.h>

#include <os/tt_thread.h>

#include <tt_cstd_api.h>

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

tt_result_t tt_ssh_string_render(IN tt_buf_t *buf,
                                 IN tt_u8_t *string_val,
                                 IN tt_u32_t string_len)
{
    TT_DO(tt_buf_put_u32_n(buf, string_len));

    if (string_len > 0) {
        TT_DO(tt_buf_put(buf, string_val, string_len));
    }

    return TT_SUCCESS;
}

tt_result_t tt_ssh_mpint_render(IN tt_buf_t *buf,
                                IN tt_u8_t *mpint,
                                IN tt_u32_t mpint_len,
                                IN tt_bool_t negative)
{
    tt_u32_t h1 = TT_COND(!negative && (mpint[0] & 0x80), 1, 0);

    TT_DO(tt_buf_put_u32_n(buf, mpint_len + h1));

    if (mpint_len > 0) {
        if (h1 != 0) {
            TT_DO(tt_buf_put_u8(buf, 0));
        }

        TT_DO(tt_buf_put(buf, mpint, mpint_len));
    }

    return TT_SUCCESS;
}

tt_u32_t tt_ssh_namelist_render_prepare(IN const tt_char_t *name[],
                                        IN tt_u32_t name_num)

{
    tt_u32_t len = 4, i;

    if (name_num > 0) {
        len += (tt_u32_t)tt_strlen(name[0]);
    }

    for (i = 1; i < name_num; ++i) {
        tt_u32_t n = (tt_u32_t)tt_strlen(name[i]);
        if (n > 0) {
            len += 1 + n;
        }
    }

    return len;
}

tt_result_t tt_ssh_namelist_render(IN tt_buf_t *buf,
                                   IN const tt_char_t *name[],
                                   IN tt_u32_t name_num)
{
    tt_u32_t len = 0, i;

    if (name_num > 0) {
        len += (tt_u32_t)tt_strlen(name[0]);
    }

    for (i = 1; i < name_num; ++i) {
        tt_u32_t n = (tt_u32_t)tt_strlen(name[i]);
        if (n > 0) {
            len += 1 + n;
        }
    }

    TT_DO(tt_buf_put_u32_n(buf, len));

    if (name_num > 0) {
        tt_u32_t n = (tt_u32_t)tt_strlen(name[0]);
        if (n > 0) {
            TT_DO(tt_buf_put(buf, (tt_u8_t *)name[0], n));
        }
    }

    for (i = 1; i < name_num; ++i) {
        tt_u32_t n = (tt_u32_t)tt_strlen(name[i]);
        if (n > 0) {
            TT_DO(tt_buf_put_u8(buf, (tt_u8_t)','));
            TT_DO(tt_buf_put(buf, (tt_u8_t *)name[i], n));
        }
    }

    return TT_SUCCESS;
}

tt_u8_t tt_sshmsg_padlen(IN tt_u32_t data_len,
                         IN tt_u32_t block_size,
                         IN tt_u32_t pad_block,
                         IN tt_u32_t max_pad_block)
{
    tt_u32_t padlen;
    tt_u32_t n;

    if (block_size == 0) {
        // a multiple of the cipher block size or 8, whichever is larger
        block_size = 8;
    }

    if (max_pad_block == 0) {
        max_pad_block = 2;
    }

    /*
     uint32 packet_length
     byte padding_length
     ...
     */
    data_len += 5;

    /*
     - the total length of (packet_length || padding_length || payload
       || random padding) is a multiple of the cipher block size or 8,
     - There MUST be at least four bytes of padding
     */
    n = block_size - (data_len + 4) % block_size;
    padlen = n + 4;

    if (pad_block == 0) {
        n = tt_rand_u32() % max_pad_block;
    } else {
        TT_ASSERT(pad_block < max_pad_block);
        n = pad_block;
    }
    padlen += n * block_size;

    TT_ASSERT(padlen < 255);
    return (tt_u8_t)padlen;
}
