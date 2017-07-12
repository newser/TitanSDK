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

#include <network/adns/tt_adns_domain_name.h>

#include <algorithm/tt_buffer_common.h>
#include <algorithm/tt_buffer_format.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_util.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __ADN_MAX_COMPRESS_LEVEL (100)

#if 1
#define TT_ASSERT_ADN TT_ASSERT
#else
#define TT_ASSERT_ADN(...)
#endif

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

static tt_result_t __adn_label_verify(IN const tt_u8_t *start,
                                      IN const tt_u8_t *end);

static tt_result_t __adn_label_render(IN const tt_u8_t *start,
                                      IN const tt_u8_t *end,
                                      IN tt_buf_t *buf);

static tt_result_t __adn_uncompress_name_len(IN tt_u8_t *pkt,
                                             IN tt_u32_t pkt_len,
                                             IN tt_u16_t ptr,
                                             OUT tt_u32_t *len);
static void __adn_uncompress_name(IN tt_u8_t *pkt,
                                  IN tt_u32_t pkt_len,
                                  IN tt_u16_t ptr,
                                  IN tt_u8_t *name,
                                  IN tt_u32_t name_len);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_bool_t tt_adns_name_verify(IN const tt_char_t *name, IN tt_u32_t name_len)
{
    tt_buf_t buf;
    tt_u8_t *label;
    tt_u32_t label_len;

    /*
     RFC1035:

     <domain> ::= <subdomain> | " "
     <subdomain> ::= <label> | <subdomain> "." <label>
     <label> ::= <letter> [ [ <ldh-str> ] <let-dig> ]
     <ldh-str> ::= <let-dig-hyp> | <let-dig-hyp> <ldh-str>
     <let-dig-hyp> ::= <let-dig> | "-"
     <let-dig> ::= <letter> | <digit>
     <letter> ::= any one of the 52 alphabetic characters A through Z in
     upper case and a through z in lower case
     <digit> ::= any one of the ten digits 0 through 9
     */

    if (name_len == 0) {
        name_len = (tt_u32_t)tt_strlen(name);
    } else {
        name_len -= 1;
    }

    // <domain> ::= " "
    if ((name_len == 1) && (name[0] == ' ')) {
        return TT_TRUE;
    }

    if (!TT_OK(tt_buf_create_nocopy(&buf,
                                    (const tt_u8_t *)name,
                                    name_len,
                                    NULL))) {
        return TT_FALSE;
    }

    label = NULL;
    label_len = 0;
    while (tt_buf_tok(&buf, (tt_u8_t *)".", 1, 0, &label, &label_len) !=
           TT_END) {
        if (!TT_OK(__adn_label_verify(label, label + label_len))) {
            tt_buf_destroy(&buf);
            return TT_FALSE;
        }
    }

    tt_buf_destroy(&buf);
    return TT_TRUE;
}

tt_result_t tt_adns_name_render_prepare(IN const tt_char_t *name,
                                        IN tt_u32_t name_len,
                                        IN tt_u16_t compress_ptr,
                                        OUT tt_u32_t *len)
{
    tt_u32_t total_len;

    TT_ASSERT(name != NULL);
    TT_ASSERT(name_len != 0);
    TT_ASSERT(len != NULL);

    // need?
    TT_ASSERT_ADN(tt_adns_name_verify(name, name_len));

    // plus the begining label len
    total_len = name_len + 1;

    if (compress_ptr != 0) {
        // replace ending 0 with 2 bytes
        total_len += 1;
    }

    *len = total_len;
    return TT_SUCCESS;
}

tt_result_t tt_adns_name_render(IN tt_buf_t *buf,
                                IN const tt_char_t *name,
                                IN tt_u32_t name_len,
                                IN tt_u16_t compress_ptr)
{
    tt_buf_t name_buf;
    tt_u8_t *label;
    tt_u32_t label_len;

    TT_ASSERT(name != NULL);
    TT_ASSERT(name_len != 0);

    if (!TT_OK(tt_buf_create_nocopy(&name_buf,
                                    (const tt_u8_t *)name,
                                    name_len - 1,
                                    NULL))) {
        return TT_FAIL;
    }

    label = NULL;
    label_len = 0;
    while (tt_buf_tok(&name_buf, (tt_u8_t *)".", 1, 0, &label, &label_len) !=
           TT_END) {
        if (!TT_OK(__adn_label_render(label, label + label_len, buf))) {
            tt_buf_destroy(&name_buf);
            return TT_FAIL;
        }
    }

    tt_buf_destroy(&name_buf);

    if (compress_ptr != 0) {
        TT_DO(tt_buf_put_u16_n(buf, compress_ptr | 0xC0));
    } else {
        TT_DO(tt_buf_put_u8(buf, 0));
    }

    return TT_SUCCESS;
}

tt_result_t tt_adns_name_parse_prepare(IN tt_buf_t *buf)
{
    while (1) {
        tt_u8_t label_len;

        TT_DO(tt_buf_get_u8(buf, &label_len));

        // loop until it sees:
        //  - 0
        //  - compress pointer

        if (label_len == 0) {
            break;
        } else if ((label_len & 0xC0) == 0xC0) {
            // compressed
            TT_DO(tt_buf_inc_rp(buf, 1));
            break;
        } else {
            /*
             rfc1035 section 2.3.4:
             labels 63 octets or less

             the reason that we do not put such limitation in __adn_label_verify
             but here is that we may receive a dns packet in which the label
             length
             is a huge number so that parse_prepare always return
             buffer_incomplete
             and then adns can never parse a packet out of buffer
             */
            if (label_len > 63) {
                TT_ERROR("too long lable: %d, limit: %d", label_len, 63);
                return TT_FAIL;
            }

            TT_DO(tt_buf_inc_rp(buf, label_len));
        }
    }
    return TT_SUCCESS;
}

tt_char_t *tt_adns_name_parse(IN tt_buf_t *buf,
                              OUT tt_char_t *name,
                              IN OUT tt_u32_t *name_len,
                              IN tt_u8_t *pkt,
                              IN tt_u32_t pkt_len)
{
    tt_u32_t __name_len = 0, __name_pos = 0;

    tt_u8_t *pos, *end;
    tt_u32_t left_len;

    pos = TT_BUF_RPOS(buf);
    end = TT_BUF_WPOS(buf);
    left_len = TT_BUF_RLEN(buf);
    (void)left_len;

    // calculate name length
    __name_len = tt_adns_name_parse_len(buf, pkt, pkt_len);
    if (__name_len == 0) {
        return NULL;
    }
    // note buf has been updated, so use saved pos and
    // left_len instead

    // alloc domain name
    if (name == NULL) {
        name = (tt_char_t *)tt_malloc(__name_len);
        if (name == NULL) {
            TT_ERROR("no mem for domain name");
            return NULL;
        }
    } else {
        if ((name_len == NULL) || (*name_len < __name_len)) {
            TT_ERROR("buffer[%d bytes] less than required[%d]",
                     *name_len,
                     __name_len);
            return NULL;
        }
    }

    while (pos < end) {
        tt_u8_t label_len = *pos;
        if (label_len == 0) {
            break;
        } else if ((label_len & 0xC0) == 0xC0) {
            tt_u16_t ptr = ((((tt_u16_t)label_len) & 0x3F) << 8) |
                           ((tt_u16_t)(*(pos + 1)));

            TT_ASSERT(__name_pos <= __name_len);
            __adn_uncompress_name(pkt,
                                  pkt_len,
                                  ptr,
                                  (tt_u8_t *)&name[__name_pos],
                                  __name_len - __name_pos);

            pos += 2;
            break;
        } else {
            ++pos;
            tt_memcpy(&name[__name_pos], pos, label_len);
            __name_pos += label_len;
            name[__name_pos++] = '.';

            pos += label_len;
        }
    }
    TT_ASSERT(name[__name_len - 1] == '.');
    name[__name_len - 1] = 0;

    if (name_len != NULL) {
        *name_len = __name_len;
    }
    // TT_DEBUG("parsed domain: %s", name);
    return name;
}

tt_u32_t tt_adns_name_parse_len(IN tt_buf_t *buf,
                                IN tt_u8_t *pkt,
                                IN tt_u32_t pkt_len)
{
    tt_u32_t name_len = 0;
    while (1) {
        tt_u8_t label_len;

        TT_DO_R(0, tt_buf_get_u8(buf, &label_len));

        if (label_len == 0) {
            break;
        } else if ((label_len & 0xC0) == 0xC0) {
            tt_u8_t len2;
            tt_u16_t ptr;
            tt_u32_t compressed_len;

            // compressed
            TT_DO_R(0, tt_buf_get_u8(buf, &len2));
            ptr = (((tt_u16_t)label_len & 0x3F) << 8 | len2);

            if (TT_OK(__adn_uncompress_name_len(pkt,
                                                pkt_len,
                                                ptr,
                                                &compressed_len))) {
                name_len += compressed_len;
                break;
            } else {
                return 0;
            }
        } else {
            name_len += (label_len + 1); // plus dot or ending 0
            if (name_len > TT_MAX_DOMAIN_NAME_LEN) {
                TT_ERROR("too long domain name: %d, limit: %d",
                         name_len,
                         TT_MAX_DOMAIN_NAME_LEN);
                return 0;
            }

            TT_DO_R(0, tt_buf_inc_rp(buf, label_len));
        }
    }
    return name_len;
}

tt_result_t __adn_label_verify(IN const tt_u8_t *start, IN const tt_u8_t *end)
{
    // <label> ::= <letter> [ [ <ldh-str> ] <let-dig> ]

    // at least 1 letter
    if ((start + 1) > end) {
        TT_ERROR("0 length label");
        return TT_FAIL;
    }

#if 0
    // <letter>
    if (!tt_isalpha(*start)) {
        TT_ERROR("label can not begin with non-letter");
        return TT_FAIL;
    }
#else
    // <let-dig>
    if (!tt_isalnum(*start)) {
        TT_ERROR("label can not begin with non-letter");
        return TT_FAIL;
    }
#endif

#if 0
    // <ldh-str>
    start += 1;
    end -= 1;
    while (start < end) {
        if (!tt_isalnum(*start) && (*start != '-')) {
            TT_ERROR("label can not include non-letter-dig-hyphen");
            return TT_FAIL;
        }
        ++start;
    }
#else
    end -= 1;
#endif

    // <let-dig>
    // if label has only 1 char, end is pointing to the first char
    if (!tt_isalnum(*end)) {
        TT_ERROR("label can not end with non-letter-dig");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __adn_label_render(IN const tt_u8_t *start,
                               IN const tt_u8_t *end,
                               IN tt_buf_t *buf)
{
    tt_u8_t label_len;

    if (start >= end) {
        TT_WARN("0 length label");
        return TT_FAIL; // ignore?
    }

    label_len = (tt_u8_t)(end - start);
    TT_DO(tt_buf_put_u8(buf, label_len));
    TT_DO(tt_buf_put(buf, start, label_len));

    return TT_SUCCESS;
}

// return length including ending 0
tt_result_t __adn_uncompress_name_len(IN tt_u8_t *pkt,
                                      IN tt_u32_t pkt_len,
                                      IN tt_u16_t ptr,
                                      OUT tt_u32_t *len)
{
    tt_u32_t __len = 0;
    tt_u8_t *pos;
    tt_u8_t *end = TT_PTR_INC(tt_u8_t, pkt, pkt_len);
    tt_u32_t compress_level = 0;

__uncompress:
    pos = TT_PTR_INC(tt_u8_t, pkt, ptr);
    while (pos < end) {
        tt_u8_t label_len = *pos;
        if (label_len == 0) {
            *len = __len;
            return TT_SUCCESS;
        } else if ((label_len & 0xC0) == 0xC0) {
            if (compress_level++ > __ADN_MAX_COMPRESS_LEVEL) {
                TT_ERROR("exceed max compress level[%d]",
                         __ADN_MAX_COMPRESS_LEVEL);
                return TT_FAIL;
            }

            if ((pos + 1) >= end) {
                TT_ERROR("partial pointer");
                return TT_FAIL;
            }

            ptr =
                ((((tt_u16_t)label_len) & 0x3F) << 8) | ((tt_u16_t)(*(++pos)));
            goto __uncompress;
        } else {
            __len += (label_len + 1); // plus dot or ending 0
            pos += (1 + label_len);
            // it would break loop if it's too long
        }
    }

    TT_ERROR("pos: %p, end: %p", pos, end);
    return TT_FAIL;
}

void __adn_uncompress_name(IN tt_u8_t *pkt,
                           IN tt_u32_t pkt_len,
                           IN tt_u16_t ptr,
                           IN tt_u8_t *name,
                           IN tt_u32_t name_len)
{
    tt_u8_t *pos;
    tt_u8_t *end = TT_PTR_INC(tt_u8_t, pkt, pkt_len);
    tt_u8_t *name_pos = name;
    tt_u32_t compress_level = 0;

// __adn_uncompress_name_len has checked consistency

__uncompress:
    pos = TT_PTR_INC(tt_u8_t, pkt, ptr);
    while (pos < end) {
        tt_u8_t label_len = *pos;
        if (label_len == 0) {
            TT_ASSERT(name_pos <= TT_PTR_INC(tt_u8_t, name, name_len));
            return;
        } else if ((label_len & 0xC0) == 0xC0) {
            if (compress_level++ > __ADN_MAX_COMPRESS_LEVEL) {
                TT_ERROR("exceed max compress level[%d]",
                         __ADN_MAX_COMPRESS_LEVEL);
                return;
            }

            ptr = ((((tt_u16_t)label_len) & 0x3F) << 8) |
                  ((tt_u16_t)(*(pos + 1)));
            goto __uncompress;
        } else {
            ++pos;
            tt_memcpy(name_pos, pos, label_len);
            name_pos += label_len;
            *name_pos++ = '.';

            pos += label_len;
        }
    }

    TT_FATAL("should not reach here");
}
