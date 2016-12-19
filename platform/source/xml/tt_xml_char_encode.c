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

#include <xml/tt_xml_char_encode.h>

#include <algorithm/tt_buffer.h>
#include <algorithm/tt_buffer_format.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __SHOULD_ESC(c)                                                        \
    ((c == '"') || (c == '\'') || (c == '<') || (c == '>') || (c == '&'))

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

tt_inline const tt_char_t *__char_esc(IN tt_char_t c)
{
    switch (c) {
        case '"':
            return "&quot;";
        case '\'':
            return "&apos;";
        case '<':
            return "&lt;";
        case '>':
            return "&gt;";
        case '&':
            return "&amp;";
        default:
            return "";
    }
}

tt_result_t tt_xml_chenc_len(IN const tt_char_t *s,
                             IN tt_u32_t s_len,
                             OUT struct tt_buf_s *outbuf)
{
    tt_u8_t *prev, *pos, *end;

    prev = (tt_u8_t *)s;
    pos = (tt_u8_t *)s;
    end = (tt_u8_t *)s + s_len;
    while (pos < end) {
        if (__SHOULD_ESC(*pos)) {
            TT_DO(tt_buf_put(outbuf, prev, (tt_u32_t)(pos - prev)));
            TT_DO(tt_buf_put_cstr(outbuf, __char_esc(*pos)));
            ++pos;
            prev = pos;
        } else {
            ++pos;
        }
    }
    TT_DO(tt_buf_put(outbuf, prev, (tt_u32_t)(pos - prev)));

    return TT_SUCCESS;
}
