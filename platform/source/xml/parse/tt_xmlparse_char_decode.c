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

#include <xml/parse/tt_xmlparse_char_decode.h>

#include <misc/tt_util.h>
#include <xml/tt_xml_memory.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __MAX_NUM_LEN 11

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

static void __chdec(IN const tt_char_t *s,
                    IN tt_u32_t s_len,
                    OUT tt_char_t *dec);

static void __chdec_amp(IN OUT const tt_char_t **p_ipos,
                        IN const tt_char_t *iend,
                        IN OUT tt_char_t **p_opos);

static tt_bool_t __chdec_decimal(IN const tt_char_t *ipos,
                                 IN tt_u32_t len,
                                 IN OUT tt_char_t **p_opos);

static tt_bool_t __chdec_hex(IN const tt_char_t *ipos,
                             IN tt_u32_t len,
                             IN OUT tt_char_t **p_opos);

static tt_bool_t __chdec_esc(IN const tt_char_t *ipos,
                             IN tt_u32_t len,
                             IN OUT tt_char_t **p_opos);

static void __utf8_enc(IN tt_u32_t val, IN OUT tt_char_t **p_opos);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_char_t *tt_xp_chdec_len(IN tt_xmlmem_t *xm,
                           IN const tt_char_t *s,
                           IN tt_u32_t s_len)
{
    tt_char_t *dec;

    s_len = tt_strnlen(s, s_len);

    // orginal string would always be longer then decoded:
    //  - &#x0 (4B) => 1B utf8
    //  - &#x80 (5B) => 2B utf8
    //  - &#x800 (6B) => 3B utf8
    //  - &#x10000 (8B) => 4B utf8
    //  - &#x200000 (9B) => 5B utf8
    //  - &#x4000000 (10B) => 6B utf8
    dec = (tt_char_t *)tt_xm_alloc(xm, s_len + 1);
    if (dec == NULL) {
        TT_ERROR("no mem to decode xml char");
        return NULL;
    }

    __chdec(s, s_len, dec);

    return dec;
}

void __chdec(IN const tt_char_t *s, IN tt_u32_t s_len, OUT tt_char_t *dec)
{
    tt_char_t *opos;
    const tt_char_t *ipos, *iend;

    opos = dec;

    ipos = s;
    iend = s + s_len;
    while (ipos < iend) {
        if (*ipos == '&') {
            __chdec_amp(&ipos, iend, &opos);
        } else {
            *opos++ = *ipos++;
        }
    }
    TT_ASSERT(opos <= (dec + s_len));
    *opos = 0;
}

void __chdec_amp(IN OUT const tt_char_t **p_ipos,
                 IN const tt_char_t *iend,
                 IN OUT tt_char_t **p_opos)
{
    const tt_char_t *ipos = *p_ipos, *sc;
    tt_char_t *opos = *p_opos;
    tt_bool_t dec_ok = TT_FALSE;

    tt_u32_t mode = 0, len;
#define __DM_DECIMAL 1
#define __DM_HEX 2
#define __DM_ESC 3

    TT_ASSERT(*ipos == '&');
    if (((ipos + 3) <= iend) && (tt_strncmp(ipos, "&#x", 3) == 0)) {
        mode = __DM_HEX;
        ipos += 3;
    } else if (((ipos + 2) <= iend) && (tt_strncmp(ipos, "&#", 2) == 0)) {
        mode = __DM_DECIMAL;
        ipos += 2;
    } else if ((ipos + 1) <= iend) {
        mode = __DM_ESC;
        ipos += 1;
    } else {
        // only '&', can not decode
        *opos++ = *ipos++;
        *p_ipos = ipos;
        *p_opos = opos;
        return;
    }

    sc = ipos;
    while ((sc < iend) && (*sc != ';')) {
        ++sc;
    }
    if (sc == iend) {
        // does not see ';'
        len = (tt_u32_t)(iend - *p_ipos);
        tt_memcpy(*p_opos, *p_ipos, len);
        *p_ipos += len;
        *p_opos += len;
        return;
    }

    len = (tt_u32_t)(sc - ipos);
    if (mode == __DM_HEX) {
        dec_ok = __chdec_hex(ipos, len, p_opos);
    } else if (mode == __DM_DECIMAL) {
        dec_ok = __chdec_decimal(ipos, len, p_opos);
    } else {
        TT_ASSERT(mode == __DM_ESC);
        dec_ok = __chdec_esc(ipos, len, p_opos);
    }

    len = (tt_u32_t)(sc - *p_ipos) + 1;
    if (dec_ok) {
        *p_ipos += len;
        // p_opos should have been updated
    } else {
        tt_memcpy(*p_opos, *p_ipos, len);
        *p_ipos += len;
        *p_opos += len;
    }
}

tt_bool_t __chdec_decimal(IN const tt_char_t *ipos,
                          IN tt_u32_t len,
                          IN OUT tt_char_t **p_opos)
{
    tt_char_t num[__MAX_NUM_LEN + 1] = {0};
    tt_char_t *endptr;
    tt_u32_t val = 0, i = 0;

    while ((i < len) && (i < __MAX_NUM_LEN) && tt_isdigit(ipos[i])) {
        num[i] = ipos[i];
        ++i;
    }
    // let __MAX_NUM_LEN be large enough, so that:
    //  - if num is full, it must be a invalid number
    //  - otherwise num may be invalid and tt_strtou32 would fail
    if ((i == 0) || (i == __MAX_NUM_LEN)) {
        return TT_FALSE;
    }

    if (TT_OK(tt_strtou32(num, &endptr, 10, &val))) {
        __utf8_enc(val, p_opos);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_bool_t __chdec_hex(IN const tt_char_t *ipos,
                      IN tt_u32_t len,
                      IN OUT tt_char_t **p_opos)
{
    tt_char_t num[__MAX_NUM_LEN + 1] = {0};
    tt_char_t *endptr;
    tt_u32_t val = 0, i = 0;

    while ((i < len) && (i < __MAX_NUM_LEN) && tt_isxdigit(ipos[i])) {
        num[i] = ipos[i];
        ++i;
    }
    if ((i == 0) || (i == __MAX_NUM_LEN)) {
        return TT_FALSE;
    }

    if (TT_OK(tt_strtou32(num, &endptr, 16, &val))) {
        __utf8_enc(val, p_opos);
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_bool_t __chdec_esc(IN const tt_char_t *ipos,
                      IN tt_u32_t len,
                      IN OUT tt_char_t **p_opos)
{
    tt_char_t *opos = *p_opos;

    if ((len == 4) && (tt_strncmp(ipos, "quot", 4) == 0)) {
        *opos = '"';
        *p_opos += 1;
        return TT_TRUE;
    }

    if ((len == 4) && (tt_strncmp(ipos, "apos", 4) == 0)) {
        *opos = '\'';
        *p_opos += 1;
        return TT_TRUE;
    }

    if ((len == 2) && (tt_strncmp(ipos, "gt", 2) == 0)) {
        *opos = '>';
        *p_opos += 1;
        return TT_TRUE;
    }

    if ((len == 2) && (tt_strncmp(ipos, "lt", 2) == 0)) {
        *opos = '<';
        *p_opos += 1;
        return TT_TRUE;
    }

    if ((len == 3) && (tt_strncmp(ipos, "amp", 3) == 0)) {
        *opos = '&';
        *p_opos += 1;
        return TT_TRUE;
    }

    return TT_FALSE;
}

void __utf8_enc(IN tt_u32_t val, IN OUT tt_char_t **p_opos)
{
#define __ue_bit(val, lshift, bit_num)                                         \
    ((tt_u8_t)(((val) >> (lshift)) & ((1 << (bit_num)) - 1)))

    tt_u8_t *pos = (tt_u8_t *)*p_opos;
    if (val <= 0x7F) {
        pos[0] = __ue_bit(val, 0, 7);
        *p_opos += 1;
    } else if (val <= 0x7FF) {
        pos[0] = 0xC0 | __ue_bit(val, 6, 5);
        pos[1] = 0x80 | __ue_bit(val, 0, 6);
        *p_opos += 2;
    } else if (val <= 0xFFFF) {
        pos[0] = 0xE0 | __ue_bit(val, 12, 4);
        pos[1] = 0x80 | __ue_bit(val, 6, 6);
        pos[2] = 0x80 | __ue_bit(val, 0, 6);
        *p_opos += 3;
    } else if (val <= 0x1FFFFF) {
        pos[0] = 0xF0 | __ue_bit(val, 18, 3);
        pos[1] = 0x80 | __ue_bit(val, 12, 6);
        pos[2] = 0x80 | __ue_bit(val, 6, 6);
        pos[3] = 0x80 | __ue_bit(val, 0, 6);
        *p_opos += 4;
    } else if (val <= 0x3FFFFFF) {
        pos[0] = 0xF8 | __ue_bit(val, 24, 2);
        pos[1] = 0x80 | __ue_bit(val, 18, 6);
        pos[2] = 0x80 | __ue_bit(val, 12, 6);
        pos[3] = 0x80 | __ue_bit(val, 6, 6);
        pos[4] = 0x80 | __ue_bit(val, 0, 6);
        *p_opos += 5;
    } else {
        // value <= 0x7FFFFFFF are correctly encoded, otherwise encoded bytes
        // are undefined as values >= 0x80000000 are not defined in unicode
        pos[0] = 0xFC | __ue_bit(val, 30, 1);
        pos[1] = 0x80 | __ue_bit(val, 24, 6);
        pos[2] = 0x80 | __ue_bit(val, 18, 6);
        pos[3] = 0x80 | __ue_bit(val, 12, 6);
        pos[4] = 0x80 | __ue_bit(val, 6, 6);
        pos[5] = 0x80 | __ue_bit(val, 0, 6);
        *p_opos += 6;
    }
}
