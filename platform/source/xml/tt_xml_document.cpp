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
#include <xml/tt_xml_path.h>
#include <xml/tt_xml_util.h>

#include <tt_cstd_api.h>
}

#include <pugixml.hpp>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

class __xw_mem : public pugi::xml_writer
{
  public:
    __xw_mem(IN tt_u8_t *buf, IN tt_u32_t len)
        : buf_(buf), len_(len), error_(TT_FALSE)
    {
    }

    void write(const void *data, size_t size)
    {
        if (!error_) {
            if ((tt_u32_t)size <= len_) {
                tt_memcpy(buf_, data, (tt_u32_t)size);
                buf_ += (tt_u32_t)size;
                len_ -= (tt_u32_t)size;
            } else {
                error_ = TT_TRUE;
            }
        }
    }

    tt_bool_t error()
    {
        return error_;
    }

  private:
    tt_u8_t *buf_;
    tt_u32_t len_;
    tt_bool_t error_ : 1;
};

class __xw_file : public pugi::xml_writer
{
  public:
    __xw_file(IN const tt_char_t *path)
        : path_(path), error_(TT_FALSE), opened_(TT_FALSE)
    {
    }

    ~__xw_file()
    {
        if (opened_) {
            tt_fclose(&f_);
        }
    }

    void write(const void *data, size_t size)
    {
        if (!error_) {
            if (!opened_) {
                if (!TT_OK(tt_fopen(&f_,
                                    path_,
                                    TT_FO_CREAT | TT_FO_TRUNC | TT_FO_WRITE,
                                    NULL))) {
                    error_ = TT_TRUE;
                    return;
                }
                opened_ = TT_TRUE;
            }

            if (!TT_OK(tt_fwrite_all(&f_, (tt_u8_t *)data, (tt_u32_t)size))) {
                error_ = TT_TRUE;
                return;
            }
        }
    }

    tt_bool_t error()
    {
        return error_;
    }

  private:
    const tt_char_t *path_;
    tt_file_t f_;
    tt_bool_t error_ : 1;
    tt_bool_t opened_ : 1;
};

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static pugi::xml_encoding __encoding_map[TT_XDOC_ENCODING_NUM] = {
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

static unsigned int __parse_option(IN tt_xdoc_parse_attr_t *attr);

static unsigned int __render_option(IN tt_xdoc_render_attr_t *attr);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_xdoc_create(IN tt_xdoc_t *xd)
{
    TT_ASSERT(xd != NULL);

    xd->p = new pugi::xml_document();
    if (xd->p == NULL) {
        TT_ERROR("fail to new xml doc");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_xdoc_destroy(IN tt_xdoc_t *xd)
{
    TT_ASSERT(xd != NULL);

    delete static_cast<pugi::xml_document *>(xd->p);
}

void tt_xdoc_parse_attr_default(IN tt_xdoc_parse_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->encoding = TT_XDOC_AUTO;

    attr->pi = TT_FALSE;
    attr->comments = TT_FALSE;
    attr->cdata = TT_TRUE;
    attr->ws_pcdata = TT_FALSE;
    attr->escapes = TT_TRUE;
    attr->eol = TT_TRUE;
    attr->wconv_attribute = TT_TRUE;
    attr->wnorm_attribute = TT_FALSE;
    attr->declaration = TT_FALSE;
    attr->doctype = TT_FALSE;
    attr->ws_pcdata_single = TT_FALSE;
    attr->trim_pcdata = TT_FALSE;
    attr->fragment = TT_FALSE;
    attr->embed_pcdata = TT_FALSE;
}

tt_result_t tt_xdoc_parse(IN tt_xdoc_t *xd,
                          IN tt_u8_t *buf,
                          IN tt_u32_t len,
                          IN OPT tt_xdoc_parse_attr_t *attr)
{
    TT_ASSERT(xd != NULL);
    TT_ASSERT(buf != NULL);

    pugi::xml_document *p;
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

    tt_xdoc_parse_attr_t __attr;
    if (attr == NULL) {
        tt_xdoc_parse_attr_default(&__attr);
        attr = &__attr;
    }

    pugi::xml_parse_result r;
    r = p->load_buffer(buf,
                       len,
                       __parse_option(attr),
                       __encoding_map[attr->encoding]);
    if (!r) {
        TT_ERROR("fail to parse xml: %s", r.description());
        p->reset();
        return TT_FAIL;
    }

    xd->p = p;
    return TT_SUCCESS;
}

tt_result_t tt_xdoc_parse_file(IN tt_xdoc_t *xd,
                               IN const tt_char_t *path,
                               IN OPT tt_xdoc_parse_attr_t *attr)
{
    TT_ASSERT(xd != NULL);
    TT_ASSERT(path != NULL);

    pugi::xml_document *p;
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

    tt_xdoc_parse_attr_t __attr;
    if (attr == NULL) {
        tt_xdoc_parse_attr_default(&__attr);
        attr = &__attr;
    }

    tt_buf_t buf;
    tt_buf_init(&buf, NULL);
    if (!TT_OK(tt_fcontent_buf(path, &buf)) || !TT_OK(tt_buf_put_u8(&buf, 0))) {
        tt_buf_destroy(&buf);
        p->reset();
        return TT_FAIL;
    }

    pugi::xml_parse_result r;
    r = p->load_buffer(TT_BUF_RPOS(&buf),
                       TT_BUF_RLEN(&buf),
                       __parse_option(attr),
                       __encoding_map[attr->encoding]);
    if (!r) {
        TT_ERROR("fail to parse xml: %s", r.description());
        p->reset();
        return TT_FAIL;
    }

    xd->p = p;
    return TT_SUCCESS;
}

void tt_xdoc_render_attr_default(IN tt_xdoc_render_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->indent = "    ";
    attr->encoding = TT_XDOC_UTF8;

    attr->do_indent = TT_TRUE;
    attr->write_bom = TT_FALSE;
    attr->raw = TT_FALSE;
    attr->no_declaration = TT_FALSE;
    attr->no_escapes = TT_FALSE;
    attr->indent_attributes = TT_FALSE;
    attr->no_empty_element_tags = TT_FALSE;
}

tt_result_t tt_xdoc_render(IN tt_xdoc_t *xd,
                           IN tt_u8_t *buf,
                           IN tt_u32_t len,
                           IN OPT tt_xdoc_render_attr_t *attr)
{
    TT_ASSERT(xd != NULL);
    TT_ASSERT(buf != NULL);

    pugi::xml_document *p = static_cast<pugi::xml_document *>(xd->p);
    TT_ASSERT(p != NULL);

    tt_xdoc_render_attr_t __attr;
    if (attr == NULL) {
        tt_xdoc_render_attr_default(&__attr);
        attr = &__attr;
    }

    __xw_mem xwm(buf, len);
    p->save(xwm,
            attr->indent,
            __render_option(attr),
            __encoding_map[attr->encoding]);
    if (xwm.error()) {
        TT_ERROR("fail to render xml");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_xdoc_render_file(IN tt_xdoc_t *xd,
                                IN const tt_char_t *path,
                                IN OPT tt_xdoc_render_attr_t *attr)
{
    TT_ASSERT(xd != NULL);
    TT_ASSERT(path != NULL);

    pugi::xml_document *p = static_cast<pugi::xml_document *>(xd->p);
    TT_ASSERT(p != NULL);

    tt_xdoc_render_attr_t __attr;
    if (attr == NULL) {
        tt_xdoc_render_attr_default(&__attr);
        attr = &__attr;
    }

    __xw_file xwf(path);
    p->save(xwf,
            attr->indent,
            __render_option(attr),
            __encoding_map[attr->encoding]);
    if (xwf.error()) {
        TT_ERROR("fail to render xml");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

// ========================================
// xml path
// ========================================

void tt_xdoc_select(IN tt_xdoc_t *xd,
                    IN const tt_char_t *xp,
                    IN OPT tt_xpvars_t *xpvs,
                    OUT tt_xnode_t *o_xn,
                    OUT tt_xattr_t *o_xa)
{
    pugi::xpath_node p =
        static_cast<pugi::xml_document *>(xd->p)->select_node(xp, P_XPVS(xpvs));

    pugi::xml_node pn = p.node();
    *o_xn = TN(pn);

    pugi::xml_attribute pa = p.attribute();
    *o_xa = TA(pa);
}

void tt_xdoc_select_all(IN tt_xdoc_t *xd,
                        IN const tt_char_t *xp,
                        IN OPT tt_xpvars_t *xpvs,
                        OUT tt_xpnodes_t *xpns)
{
    *P_XPNS(xpns) =
        static_cast<pugi::xml_document *>(xd->p)->select_nodes(xp,
                                                               P_XPVS(xpvs));
}

void tt_xdoc_selectxp(IN tt_xdoc_t *xd,
                      IN tt_xpath_t *xp,
                      OUT tt_xnode_t *o_xn,
                      OUT tt_xattr_t *o_xa)
{
    pugi::xpath_node p =
        static_cast<pugi::xml_document *>(xd->p)->select_node(*P_XP(xp));

    pugi::xml_node pn = p.node();
    *o_xn = TN(pn);

    pugi::xml_attribute pa = p.attribute();
    *o_xa = TA(pa);
}

void tt_xdoc_selectxp_all(IN tt_xdoc_t *xd,
                          IN tt_xpath_t *xp,
                          OUT tt_xpnodes_t *xpns)
{
    *P_XPNS(xpns) =
        static_cast<pugi::xml_document *>(xd->p)->select_nodes(*P_XP(xp));
}

tt_bool_t tt_xdoc_eval_bool(IN tt_xdoc_t *xd, IN struct tt_xpath_s *xp)
{
    if (P_XP(xp)->evaluate_boolean(*static_cast<pugi::xml_document *>(xd->p))) {
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_double_t tt_xdoc_eval_number(IN tt_xdoc_t *xd, IN struct tt_xpath_s *xp)
{
    return P_XP(xp)->evaluate_number(*static_cast<pugi::xml_document *>(xd->p));
}

tt_u32_t tt_xdoc_eval_cstr(IN tt_xdoc_t *xd,
                           IN struct tt_xpath_s *xp,
                           OUT tt_char_t *buf,
                           IN tt_u32_t len)
{
    return (tt_u32_t)P_XP(xp)
        ->evaluate_string(buf,
                          len,
                          (*static_cast<pugi::xml_document *>(xd->p)));
}

unsigned int __parse_option(IN tt_xdoc_parse_attr_t *attr)
{
    unsigned int options = 0;

    if (attr->pi) {
        options |= pugi::parse_pi;
    }
    if (attr->comments) {
        options |= pugi::parse_comments;
    }
    if (attr->cdata) {
        options |= pugi::parse_cdata;
    }
    if (attr->ws_pcdata) {
        options |= pugi::parse_ws_pcdata;
    }
    if (attr->escapes) {
        options |= pugi::parse_escapes;
    }
    if (attr->eol) {
        options |= pugi::parse_eol;
    }
    if (attr->wconv_attribute) {
        options |= pugi::parse_wconv_attribute;
    }
    if (attr->wnorm_attribute) {
        options |= pugi::parse_wnorm_attribute;
    }
    if (attr->declaration) {
        options |= pugi::parse_declaration;
    }
    if (attr->doctype) {
        options |= pugi::parse_doctype;
    }
    if (attr->ws_pcdata_single) {
        options |= pugi::parse_ws_pcdata_single;
    }
    if (attr->trim_pcdata) {
        options |= pugi::parse_trim_pcdata;
    }
    if (attr->fragment) {
        options |= pugi::parse_fragment;
    }
    if (attr->embed_pcdata) {
        options |= pugi::parse_embed_pcdata;
    }

    return options;
}

unsigned int __render_option(IN tt_xdoc_render_attr_t *attr)
{
    unsigned int options = 0;

    if (attr->do_indent) {
        options |= pugi::format_indent;
    }
    if (attr->write_bom) {
        options |= pugi::format_write_bom;
    }
    if (attr->raw) {
        options |= pugi::format_raw;
    }
    if (attr->no_declaration) {
        options |= pugi::format_no_declaration;
    }
    if (attr->no_escapes) {
        options |= pugi::format_no_escapes;
    }
    if (attr->indent_attributes) {
        options |= pugi::format_indent_attributes;
    }
    if (attr->no_empty_element_tags) {
        options |= pugi::format_no_empty_element_tags;
    }

    return options;
}
