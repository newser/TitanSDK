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

#include <xml/parse/tt_xml_parser.h>

#include <algorithm/tt_buffer_common.h>
#include <memory/tt_memory_alloc.h>
#include <xml/tt_xml_def.h>
#include <xml/tt_xml_memory.h>
#include <xml/tt_xml_node.h>

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

static tt_u8_t __xp_utf8[] = {0xEF, 0xBB, 0xBF};
static tt_u8_t __xp_utf16le[] = {0xFF, 0xFE};
static tt_u8_t __xp_utf16be[] = {0xFE, 0xFF};
static tt_u8_t __xp_utf32le[] = {0xFF, 0xFE, 0, 0};
;
static tt_u8_t __xp_utf32be[] = {
    0, 0, 0xFE, 0xFF,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __xp_guess_charset(IN tt_xmlparser_t *xp,
                                      IN tt_u8_t *data,
                                      IN tt_u32_t data_len);
static tt_result_t __xp_csconv_create(IN tt_xmlparser_t *xp,
                                      IN tt_charset_t from);
static void __xp_csconv_destroy(IN tt_xmlparser_t *xp);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_xmlparser_create(IN tt_xmlparser_t *xp,
                                IN OPT tt_xmlmem_t *xm,
                                IN OPT tt_xmlparser_cb_t *cb,
                                IN OPT tt_xmlparser_attr_t *attr)
{
    TT_ASSERT(xp != NULL);

    if (cb != NULL) {
        tt_memcpy(&xp->cb, cb, sizeof(tt_xmlparser_cb_t));
    } else {
        tt_memset(&xp->cb, 0, sizeof(tt_xmlparser_cb_t));
    }

    if (attr != NULL) {
        tt_memcpy(&xp->attr, attr, sizeof(tt_xmlparser_attr_t));
    } else {
        tt_xmlparser_attr_default(&xp->attr);
    }

    xp->xm = xm;

    xp->state = TT_XMLPARSE_STATE_INIT;
    tt_memset(&xp->state_u, 0, sizeof(xp->state_u));
    tt_buf_init(&xp->common_buf, NULL);

    // charset convert is left to be created when reading file head
    xp->from_encoding = TT_CHARSET_NUM;
    tt_memset(&xp->csconv, 0, sizeof(tt_chsetconv_t));

    return TT_SUCCESS;
}

void tt_xmlparser_destroy(IN tt_xmlparser_t *xp)
{
    TT_ASSERT(xp != NULL);

    if (tt_g_xmlpsst_itf[xp->state]->exit != NULL) {
        tt_g_xmlpsst_itf[xp->state]->exit(xp);
    }

    tt_buf_destroy(&xp->common_buf);

    __xp_csconv_destroy(xp);
}

void tt_xmlparser_attr_default(IN tt_xmlparser_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->trim_text = TT_TRUE;
    attr->trim_cdata = TT_TRUE;
    attr->trim_comment = TT_TRUE;
}

tt_result_t tt_xmlparser_update(IN tt_xmlparser_t *xp,
                                IN tt_u8_t *data,
                                IN tt_u32_t data_len)
{
    tt_u8_t *pos;
    tt_u32_t left_len;

    TT_ASSERT(xp != NULL);
    TT_ASSERT(data != NULL);

    if (data_len == 0) {
        return TT_SUCCESS;
    }

    if (xp->state == TT_XMLPARSE_STATE_INIT) {
        // init is a special state, it needs guess char set rather
        // than doing char set converting
        if (!TT_OK(__xp_guess_charset(xp, data, data_len))) {
            return TT_FAIL;
        }

        // __xp_guess_charset will change state when it collect enough
        // information, otherwise keep init state and wait for more data
        if (xp->state == TT_XMLPARSE_STATE_INIT) {
            return TT_SUCCESS;
        }
    } else if (!TT_OK(tt_chsetconv_input(&xp->csconv, data, data_len))) {
        return TT_FAIL;
    }

    // only read converted data
    pos = TT_BUF_RPOS(&xp->csconv.converted);
    left_len = TT_BUF_RLEN(&xp->csconv.converted);
    while (left_len > 0) {
        tt_xmlparse_state_t prev_state, next_state;
        tt_xmlpsst_itf_t *itf;

        prev_state = xp->state;
        TT_ASSERT_XML(TT_XMLPARSE_STATE_VALID(prev_state));
        itf = tt_g_xmlpsst_itf[prev_state];

        next_state = prev_state;
        if (!TT_OK(itf->update(xp, &pos, &left_len, &next_state))) {
            return TT_FAIL;
        }

        if (next_state != prev_state) {
            // state changed

            if (itf->exit != NULL) {
                itf->exit(xp);
            }

            xp->state = next_state;

            itf = tt_g_xmlpsst_itf[next_state];
            if (itf->enter != NULL) {
                itf->enter(xp);
            }
        }
#if 1
        else if (left_len != 0) {
            // no state change means there are not enough data to switch
            // to new state, so there should be no data left
            TT_WARN("no new xml parse state, [%d] bytes left", left_len);
        }
#endif
    }
    TT_ASSERT_XML(left_len == 0);
    tt_buf_reset_rwp(&xp->csconv.converted);

    return TT_SUCCESS;
}

tt_result_t tt_xmlparser_final(IN tt_xmlparser_t *xp, OUT void *reserved)
{
    tt_buf_t *cbuf;

    TT_ASSERT(xp != NULL);

    cbuf = &xp->common_buf;

    if (xp->state == TT_XMLPARSE_STATE_INIT) {
        if ((xp->from_encoding == TT_CHARSET_NUM) &&
            !TT_OK(__xp_csconv_create(xp, TT_CHARSET_UTF8))) {
            return TT_FAIL;
        }

        if (!TT_OK(tt_chsetconv_input(&xp->csconv,
                                      TT_BUF_RPOS(cbuf),
                                      TT_BUF_RLEN(cbuf)))) {
            return TT_FAIL;
        }
        tt_buf_reset_rwp(cbuf);

        xp->state = TT_XMLPARSE_STATE_TEXT;
    }

    if (xp->state == TT_XMLPARSE_STATE_TEXT) {
        tt_u32_t text_len;
        tt_xp_on_value_t on_text;

        // the only valid state to terminate xml parsing

        if (xp->attr.trim_text) {
            tt_buf_trim_sp(cbuf);
        }
        text_len = TT_BUF_RLEN(cbuf);

        on_text = xp->cb.on_text;
        if ((text_len != 0) && (on_text != NULL) &&
            !TT_OK(on_text(xp, TT_BUF_RPOS(cbuf), text_len))) {
            return TT_FAIL;
        }

        return TT_SUCCESS;
    } else {
        TT_ERROR("can not terminate parsing in state[todo]");
        return TT_FAIL;
    }
}

void tt_xmlparser_reset(IN tt_xmlparser_t *xp, IN tt_u32_t flag)
{
    if ((tt_g_xmlpsst_itf[xp->state] != NULL) &&
        (tt_g_xmlpsst_itf[xp->state]->exit != NULL)) {
        tt_g_xmlpsst_itf[xp->state]->exit(xp);
    }
    xp->state = TT_XMLPARSE_STATE_INIT;
    tt_memset(&xp->state_u, 0, sizeof(xp->state_u));

    tt_buf_reset_rwp(&xp->common_buf);

    xp->from_encoding = TT_CHARSET_NUM;
#if 0
    tt_chsetconv_reset(&xp->csconv);
#else
    // source encoding may change
    __xp_csconv_destroy(xp);
    tt_memset(&xp->csconv, 0, sizeof(tt_chsetconv_t));
#endif
}

tt_result_t __xp_guess_charset(IN tt_xmlparser_t *xp,
                               IN tt_u8_t *data,
                               IN tt_u32_t data_len)
{
    tt_buf_t *cbuf = &xp->common_buf;
    tt_u32_t cbuf_len = TT_BUF_RLEN(cbuf), n;
    tt_u8_t *p;
    tt_charset_t from;

    // collect at least 4 bytes
    if ((cbuf_len + data_len) < 4) {
        TT_DO(tt_buf_put(cbuf, data, data_len));
        return TT_SUCCESS;
    }
    n = 4 - cbuf_len;
    TT_DO(tt_buf_put(cbuf, data, n));
    data += n;
    data_len -= n;

    // guess charset
    p = TT_BUF_RPOS(cbuf);
    if (tt_memcmp(p, __xp_utf32le, sizeof(__xp_utf32le)) == 0) {
        // utf32le must be checked before utf16le
        from = TT_CHARSET_UTF32LE;
        tt_buf_inc_rp(cbuf, sizeof(__xp_utf32le));
    } else if (tt_memcmp(p, __xp_utf16le, sizeof(__xp_utf16le)) == 0) {
        from = TT_CHARSET_UTF16LE;
        tt_buf_inc_rp(cbuf, sizeof(__xp_utf16le));
    } else if (tt_memcmp(p, __xp_utf16be, sizeof(__xp_utf16be)) == 0) {
        from = TT_CHARSET_UTF16BE;
        tt_buf_inc_rp(cbuf, sizeof(__xp_utf16be));
    } else if (tt_memcmp(p, __xp_utf32be, sizeof(__xp_utf32be)) == 0) {
        from = TT_CHARSET_UTF32BE;
        tt_buf_inc_rp(cbuf, sizeof(__xp_utf32be));
    } else if (tt_memcmp(p, __xp_utf8, sizeof(__xp_utf8)) == 0) {
        from = TT_CHARSET_UTF8;
        tt_buf_inc_rp(cbuf, sizeof(__xp_utf8));
    } else {
        // assumes utf8
        from = TT_CHARSET_UTF8;
    }

    if (!TT_OK(__xp_csconv_create(xp, from))) {
        return TT_FAIL;
    }

    // left data should be inputted to csconv
    cbuf_len = TT_BUF_RLEN(cbuf);
    if ((cbuf_len != 0) &&
        !TT_OK(tt_chsetconv_input(&xp->csconv, TT_BUF_RPOS(cbuf), cbuf_len))) {
        return TT_FAIL;
    }
    tt_buf_reset_rwp(cbuf);

    if ((data_len != 0) &&
        !TT_OK(tt_chsetconv_input(&xp->csconv, data, data_len))) {
        return TT_FAIL;
    }

    xp->state = TT_XMLPARSE_STATE_TEXT;
    return TT_SUCCESS;
}

tt_result_t __xp_csconv_create(IN tt_xmlparser_t *xp, IN tt_charset_t from)
{
    tt_chsetconv_attr_t csconv_attr;

    TT_ASSERT(xp != NULL);
    TT_ASSERT(TT_CHARSET_VALID(from));

    if (xp->from_encoding != TT_CHARSET_NUM) {
        TT_ERROR("xml charset convert has been created");
        return TT_FAIL;
    }

    // map attr to csconv_attr
    tt_chsetconv_attr_default(&csconv_attr);

    if (!TT_OK(tt_chsetconv_create(&xp->csconv,
                                   from,
                                   TT_CHARSET_UTF8,
                                   &csconv_attr))) {
        return TT_FAIL;
    }

    xp->from_encoding = from;
    return TT_SUCCESS;
}

void __xp_csconv_destroy(IN tt_xmlparser_t *xp)
{
    if (xp->from_encoding != TT_CHARSET_NUM) {
        tt_chsetconv_destroy(&xp->csconv);
    }
}
