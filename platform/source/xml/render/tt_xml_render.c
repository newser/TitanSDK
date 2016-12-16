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

#include <xml/render/tt_xml_render.h>

#include <algorithm/tt_buffer_format.h>
#include <xml/tt_xml_char_encode.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __xr_indent(IN tt_xmlrender_t *xr);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_xmlrender_create(IN tt_xmlrender_t *xr,
                                IN OPT tt_xmlrender_attr_t *attr)
{
    tt_xmlrender_attr_t __attr;

    TT_ASSERT(xr != NULL);

    if (attr == NULL) {
        tt_xmlrender_attr_default(&__attr);
        attr = &__attr;
    }

    tt_buf_init(&xr->buf, &attr->buf_attr);
    xr->level = 0;
    xr->newline = TT_TRUE;

    xr->line_break = attr->line_break;
    xr->line_break_len = attr->line_break_len;

    xr->indent = attr->indent;
    xr->indent_len = attr->indent_len;

    return TT_SUCCESS;
}

void tt_xmlrender_destroy(IN tt_xmlrender_t *xr)
{
    TT_ASSERT(xr != NULL);

    tt_buf_destroy(&xr->buf);
}

void tt_xmlrender_attr_default(IN tt_xmlrender_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_buf_attr_default(&attr->buf_attr);

#define __DEF_LINE_BREAK "\n"
    attr->line_break = (const tt_u8_t *)__DEF_LINE_BREAK;
    attr->line_break_len = (tt_u32_t)sizeof(__DEF_LINE_BREAK) - 1;

#define __DEF_INDENT "  "
    attr->indent = (const tt_u8_t *)"  ";
    attr->indent_len = (tt_u32_t)sizeof(__DEF_INDENT) - 1;
}

void tt_xmlrender_reset(IN tt_xmlrender_t *xr, IN tt_u32_t flag)
{
    TT_ASSERT(xr != NULL);

    tt_buf_clear(&xr->buf);
    xr->level = 0;
    xr->newline = TT_TRUE;
}

tt_result_t tt_xmlrender_text(IN tt_xmlrender_t *xr, IN const tt_char_t *text)
{
    tt_buf_t *buf = &xr->buf;

    TT_DO(tt_xml_chenc(text, buf));

    return TT_SUCCESS;
}

tt_result_t tt_xmlrender_stag(IN tt_xmlrender_t *xr, IN const tt_char_t *name)
{
    tt_buf_t *buf = &xr->buf;

    if (name[0] == 0) {
        TT_ERROR("tag name can not be empty");
        return TT_FAIL;
    }

    TT_DO(__xr_indent(xr));
    TT_DO(tt_buf_put_u8(buf, '<'));
    TT_DO(tt_xml_chenc(name, buf));

    return TT_SUCCESS;
}

tt_result_t tt_xmlrender_stag_complete(IN tt_xmlrender_t *xr,
                                       IN tt_bool_t newline)
{
    tt_buf_t *buf = &xr->buf;

    TT_DO(tt_buf_put_u8(buf, '>'));

    if (newline) {
        TT_DO(tt_buf_put(buf, xr->line_break, xr->line_break_len));
        ++xr->level;
    }
    xr->newline = newline;

    return TT_SUCCESS;
}

tt_result_t tt_xmlrender_stag_end(IN tt_xmlrender_t *xr)
{
    tt_buf_t *buf = &xr->buf;

    TT_DO(tt_buf_put(buf, (tt_u8_t *)"/>", 2));
    TT_DO(tt_buf_put(buf, xr->line_break, xr->line_break_len));

    return TT_SUCCESS;
}

tt_result_t tt_xmlrender_etag(IN tt_xmlrender_t *xr, IN const tt_char_t *name)
{
    tt_buf_t *buf = &xr->buf;

    if (xr->newline) {
        TT_ASSERT(xr->level > 0);
        --xr->level;
        TT_DO(__xr_indent(xr));
    }

    TT_DO(tt_buf_put(buf, (tt_u8_t *)"</", 2));
    TT_DO(tt_xml_chenc(name, buf));
    TT_DO(tt_buf_put_u8(buf, '>'));

    TT_DO(tt_buf_put(buf, xr->line_break, xr->line_break_len));
    xr->newline = TT_TRUE;

    return TT_SUCCESS;
}

tt_result_t tt_xmlrender_attr(IN tt_xmlrender_t *xr,
                              IN const tt_char_t *name,
                              IN const tt_char_t *value)
{
    tt_buf_t *buf = &xr->buf;

    if (name[0] == 0) {
        TT_ERROR("attr name can not be empty");
        return TT_FAIL;
    }

    TT_DO(tt_buf_put_u8(buf, ' '));
    TT_DO(tt_xml_chenc(name, buf));
    TT_DO(tt_buf_put(buf, (tt_u8_t *)"=\"", 2));
    TT_DO(tt_xml_chenc(value, buf));
    TT_DO(tt_buf_put_u8(buf, '\"'));

    return TT_SUCCESS;
}

tt_result_t tt_xmlrender_pi(IN tt_xmlrender_t *xr,
                            IN const tt_char_t *name,
                            IN const tt_char_t *value)
{
    tt_buf_t *buf = &xr->buf;

    if (name[0] == 0) {
        TT_ERROR("xml pi must have name");
        return TT_FAIL;
    }

    TT_DO(__xr_indent(xr));

    TT_DO(tt_buf_put(buf, (tt_u8_t *)"<?", 2));
    TT_DO(tt_buf_put_cstr(buf, name));
    if (value[0] != 0) {
        TT_DO(tt_buf_put_u8(buf, ' '));
        TT_DO(tt_buf_put_cstr(buf, value));
    }
    TT_DO(tt_buf_put(buf, (tt_u8_t *)"?>", 2));

    TT_DO(tt_buf_put(buf, xr->line_break, xr->line_break_len));

    return TT_SUCCESS;
}

tt_result_t tt_xmlrender_comment(IN tt_xmlrender_t *xr,
                                 IN const tt_char_t *comment)
{
    tt_buf_t *buf = &xr->buf;

    TT_DO(__xr_indent(xr));

    TT_DO(tt_buf_put(buf, (tt_u8_t *)"<!-- ", 5));
    TT_DO(tt_buf_put_cstr(buf, comment));
    TT_DO(tt_buf_put(buf, (tt_u8_t *)" -->", 4));

    TT_DO(tt_buf_put(buf, xr->line_break, xr->line_break_len));

    return TT_SUCCESS;
}

tt_result_t tt_xmlrender_cdata(IN tt_xmlrender_t *xr, IN const tt_char_t *cdata)
{
    tt_buf_t *buf = &xr->buf;

    TT_DO(__xr_indent(xr));

    TT_DO(tt_buf_put(buf, (tt_u8_t *)"<![CDATA[", 9));
    TT_DO(tt_buf_put_cstr(buf, cdata));
    TT_DO(tt_buf_put(buf, (tt_u8_t *)"]]>", 3));

    TT_DO(tt_buf_put(buf, xr->line_break, xr->line_break_len));

    return TT_SUCCESS;
}

tt_result_t __xr_indent(IN tt_xmlrender_t *xr)
{
    tt_u32_t i;
    for (i = 0; i < xr->level; ++i) {
        TT_DO(tt_buf_put(&xr->buf, xr->indent, xr->indent_len));
    }
    return TT_SUCCESS;
}
