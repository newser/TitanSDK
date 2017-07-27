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
@file tt_ssh_render.h
@brief ts ssh render api
*/

#ifndef __TT_SSH_RENDER__
#define __TT_SSH_RENDER__

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

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/*
 byte
 A byte represents an arbitrary 8-bit value (octet). Fixed length
 data is sometimes represented as an array of bytes, written
 byte[n], where n is the number of bytes in the array.
 */
tt_inline tt_u32_t tt_ssh_byte_render_prepare()
{
    return 1;
}

tt_inline tt_result_t tt_ssh_byte_render(IN tt_buf_t *buf, IN tt_u8_t byte_val)
{
    return tt_buf_put_u8(buf, byte_val);
}

/*
 boolean
 A boolean value is stored as a single byte. The value 0
 represents FALSE, and the value 1 represents TRUE. All non-zero
 values MUST be interpreted as TRUE; however, applications MUST NOT
 store values other than 0 and 1.
 */
tt_inline tt_u32_t tt_ssh_boolean_render_prepare()
{
    return 1;
}

tt_inline tt_result_t tt_ssh_boolean_render(IN tt_buf_t *buf,
                                            IN tt_bool_t boolean_val)
{
    return tt_buf_put_u8(buf, TT_COND(boolean_val, 1, 0));
}

/*
 uint32
 Represents a 32-bit unsigned integer. Stored as four bytes in the
 order of decreasing significance (network byte order). For
 example: the value 699921578 (0x29b7f4aa) is stored as 29 b7 f4
 aa.
 */
tt_inline tt_u32_t tt_ssh_uint32_render_prepare()
{
    return 4;
}

tt_inline tt_result_t tt_ssh_uint32_render(IN tt_buf_t *buf,
                                           IN tt_u32_t uint32_val)
{
    return tt_buf_put_u32_n(buf, uint32_val);
}

/*
 uint64
 Represents a 64-bit unsigned integer. Stored as eight bytes in
 the order of decreasing significance (network byte order).
 */
tt_inline tt_u32_t tt_ssh_uint64_render_prepare()
{
    return 8;
}

tt_inline tt_result_t tt_ssh_uint64_render(IN tt_buf_t *buf,
                                           IN tt_u64_t uint64_val)
{
    return tt_buf_put_u64_n(buf, uint64_val);
}

/*
 string
 Arbitrary length binary string. Strings are allowed to contain
 arbitrary binary data, including null characters and 8-bit
 characters. They are stored as a uint32 containing its length
 (number of bytes that follow) and zero (= empty string) or more
 bytes that are the value of the string. Terminating null
 characters are not used.
 */
tt_inline tt_u32_t tt_ssh_string_render_prepare(IN tt_u8_t *string_val,
                                                IN tt_u32_t string_len)
{
    return 4 + string_len;
}

tt_export tt_result_t tt_ssh_string_render(IN tt_buf_t *buf,
                                           IN tt_u8_t *string_val,
                                           IN tt_u32_t string_len);

/*
 mpint
 Represents multiple precision integers in two’s complement format,
 stored as a string, 8 bits per byte, MSB first. Negative numbers
 have the value 1 as the most significant bit of the first byte of
 the data partition. If the most significant bit would be set for
 a positive number, the number MUST be preceded by a zero byte.
 Unnecessary leading bytes with the value 0 or 255 MUST NOT be
 included. The value zero MUST be stored as a string with zero
 bytes of data.
 */
tt_inline tt_u32_t tt_ssh_mpint_render_prepare(IN tt_u8_t *mpint,
                                               IN tt_u32_t mpint_len,
                                               IN tt_bool_t negative)
{
    if (mpint_len == 0) {
        return 4;
    }

    return 4 + mpint_len + TT_COND(!negative && (mpint[0] & 0x80), 1, 0);
}

tt_export tt_result_t tt_ssh_mpint_render(IN tt_buf_t *buf,
                                          IN tt_u8_t *mpint,
                                          IN tt_u32_t mpint_len,
                                          IN tt_bool_t negative);

/*
 name-list
 A string containing a comma-separated list of names. A name-list
 is represented as a uint32 containing its length (number of bytes
 that follow) followed by a comma-separated list of zero or more
 names. A name MUST have a non-zero length, and it MUST NOT
 contain a comma (","). As this is a list of names, all of the
 elements contained are names and MUST be in US-ASCII. Context may
 impose additional restrictions on the names. For example, the
 names in a name-list may have to be a list of valid algorithm
 identifiers (see Section 6 below), or a list of [RFC3066] language
 tags. The order of the names in a name-list may or may not be
 significant. Again, this depends on the context in which the list
 is used. Terminating null characters MUST NOT be used, neither
 for the individual names, nor for the list as a whole.
 */
tt_export tt_u32_t tt_ssh_namelist_render_prepare(IN const tt_char_t *name[],
                                                  IN tt_u32_t name_num);

tt_export tt_result_t tt_ssh_namelist_render(IN tt_buf_t *buf,
                                             IN const tt_char_t *name[],
                                             IN tt_u32_t name_num);

// - if block_size is 0, it uses 8 as block size
// - if max_pad_block is 0, it uses
tt_export tt_u8_t tt_sshmsg_padlen(IN tt_u32_t data_len,
                                   IN tt_u32_t block_size,
                                   IN tt_u32_t pad_block,
                                   IN tt_u32_t max_pad_block);

#endif /* __TT_SSH_RENDER__ */
