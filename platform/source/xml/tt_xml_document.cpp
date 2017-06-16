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

extern "C" {
#include <xml/tt_xml_document.h>

#include <algorithm/tt_buffer_format.h>
#include <io/tt_file_system.h>
#include <misc/tt_assert.h>

#include <tt_cstd_api.h>
}

#include <pugixml.hpp>

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

static pugi::xml_encoding __xdoc_encoding_map[TT_XDOC_ENCODING_NUM] = {
    pugi::encoding_auto,
    pugi::encoding_utf8,
    pugi::encoding_utf16_le,
    pugi::encoding_utf16_be,
    pugi::encoding_utf32_le,
    pugi::encoding_utf32_be,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static unsigned int __xdoc_option(IN tt_xdoc_attr_t *attr);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_xdoc_init(IN tt_xdoc_t *xd)
{
    TT_ASSERT(xd != NULL);

    xd->p = NULL;
}

tt_result_t tt_xdoc_load(IN tt_xdoc_t *xd,
                         IN tt_u8_t *buf,
                         IN tt_u32_t len,
                         IN OPT tt_xdoc_attr_t *attr)
{
    pugi::xml_document *p;
    unsigned int options;
    pugi::xml_encoding e;
    tt_xdoc_attr_t __attr;
    pugi::xml_parse_result r;

    TT_ASSERT(xd != NULL);
    TT_ASSERT(buf != NULL);

    if (xd->p != NULL) {
        p = static_cast<pugi::xml_document *>(xd->p);
        p->reset();
    } else {
        p = new pugi::xml_document();
        if (p == NULL) {
            TT_ERROR("fail to new xml doc");
            return TT_FAIL;
        }
    }

    if (attr == NULL) {
        tt_xdoc_attr_default(&__attr);
        attr = &__attr;
    }

    r = p->load_buffer(buf,
                       len,
                       __xdoc_option(attr),
                       __xdoc_encoding_map[attr->encoding]);
    if (!r) {
        TT_ERROR("fail to parse xml: %s", r.description());
        p->reset();
        return TT_FAIL;
    }

    xd->p = p;
    return TT_SUCCESS;
}

tt_result_t tt_xdoc_load_file(IN tt_xdoc_t *xd,
                              IN const tt_char_t *path,
                              IN OPT tt_xdoc_attr_t *attr)
{
    pugi::xml_document *p;
    tt_buf_t buf;
    unsigned int options;
    pugi::xml_encoding e;
    tt_xdoc_attr_t __attr;
    pugi::xml_parse_result r;

    TT_ASSERT(xd != NULL);
    TT_ASSERT(path != NULL);

    if (xd->p != NULL) {
        p = static_cast<pugi::xml_document *>(xd->p);
        p->reset();
    } else {
        p = new pugi::xml_document();
        if (p == NULL) {
            TT_ERROR("fail to new xml doc");
            return TT_FAIL;
        }
    }

    tt_buf_init(&buf, NULL);
    if (!TT_OK(tt_fcontent_buf(path, &buf)) || !TT_OK(tt_buf_put_u8(&buf, 0))) {
        tt_buf_destroy(&buf);
        p->reset();
        return TT_FAIL;
    }

    r = p->load_buffer(TT_BUF_RPOS(&buf),
                       TT_BUF_RLEN(&buf),
                       __xdoc_option(attr),
                       __xdoc_encoding_map[attr->encoding]);
    if (!r) {
        TT_ERROR("fail to parse xml: %s", r.description());
        p->reset();
        return TT_FAIL;
    }

    xd->p = p;
    return TT_SUCCESS;
}

void tt_xdoc_destroy(IN tt_xdoc_t *xd)
{
    TT_ASSERT(xd != NULL);

    delete static_cast<pugi::xml_document *>(xd->p);
}

void tt_xdoc_attr_default(IN tt_xdoc_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->encoding = TT_XDOC_AUTO;

    attr->parse_pi = TT_FALSE;
    attr->parse_comments = TT_FALSE;
    attr->parse_cdata = TT_TRUE;
    attr->parse_ws_pcdata = TT_FALSE;
    attr->parse_escapes = TT_TRUE;
    attr->parse_eol = TT_TRUE;
    attr->parse_wconv_attribute = TT_TRUE;
    attr->parse_wnorm_attribute = TT_FALSE;
    attr->parse_declaration = TT_FALSE;
    attr->parse_doctype = TT_FALSE;
    attr->parse_ws_pcdata_single = TT_FALSE;
    attr->parse_trim_pcdata = TT_FALSE;
    attr->parse_fragment = TT_FALSE;
    attr->parse_embed_pcdata = TT_FALSE;
}

unsigned int __xdoc_option(IN tt_xdoc_attr_t *attr)
{
    unsigned int options = 0;

    if (attr->parse_pi) {
        options |= pugi::parse_pi;
    }
    if (attr->parse_comments) {
        options |= pugi::parse_comments;
    }
    if (attr->parse_cdata) {
        options |= pugi::parse_cdata;
    }
    if (attr->parse_ws_pcdata) {
        options |= pugi::parse_ws_pcdata;
    }
    if (attr->parse_escapes) {
        options |= pugi::parse_escapes;
    }
    if (attr->parse_eol) {
        options |= pugi::parse_eol;
    }
    if (attr->parse_wconv_attribute) {
        options |= pugi::parse_wconv_attribute;
    }
    if (attr->parse_wnorm_attribute) {
        options |= pugi::parse_wnorm_attribute;
    }
    if (attr->parse_declaration) {
        options |= pugi::parse_declaration;
    }
    if (attr->parse_doctype) {
        options |= pugi::parse_doctype;
    }
    if (attr->parse_ws_pcdata_single) {
        options |= pugi::parse_ws_pcdata_single;
    }
    if (attr->parse_trim_pcdata) {
        options |= pugi::parse_trim_pcdata;
    }
    if (attr->parse_fragment) {
        options |= pugi::parse_fragment;
    }
    if (attr->parse_embed_pcdata) {
        options |= pugi::parse_embed_pcdata;
    }

    return options;
}
