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
@file tt_xml_document.h
@brief xml document

this file specifies xml document APIs
*/

#ifndef __TT_XML_DOCUMENT__
#define __TT_XML_DOCUMENT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_xdoc_s
{
    tt_ptr_t p;
} tt_xdoc_t;

typedef enum {
    TT_XDOC_AUTO,
    TT_XDOC_UTF8,
    TT_XDOC_UTF16_LE,
    TT_XDOC_UTF16_BE,
    TT_XDOC_UTF32_LE,
    TT_XDOC_UTF32_BE,

    TT_XDOC_ENCODING_NUM
} tt_xdoc_encoding_t;
#define TT_XDOC_ENCODING_VALID(e) ((e) < TT_XDOC_ENCODING_NUM)

typedef struct
{
    tt_xdoc_encoding_t encoding;

    tt_bool_t pi : 1;
    tt_bool_t comments : 1;
    tt_bool_t cdata : 1;
    tt_bool_t ws_pcdata : 1;
    tt_bool_t escapes : 1;
    tt_bool_t eol : 1;
    tt_bool_t wconv_attribute : 1;
    tt_bool_t wnorm_attribute : 1;
    tt_bool_t declaration : 1;
    tt_bool_t doctype : 1;
    tt_bool_t ws_pcdata_single : 1;
    tt_bool_t trim_pcdata : 1;
    tt_bool_t fragment : 1;
    tt_bool_t embed_pcdata : 1;
} tt_xdoc_parse_attr_t;

typedef struct
{
    const tt_char_t *indent;
    tt_xdoc_encoding_t encoding;

    tt_bool_t do_indent : 1;
    tt_bool_t write_bom : 1;
    tt_bool_t raw : 1;
    tt_bool_t no_declaration : 1;
    tt_bool_t no_escapes : 1;
    tt_bool_t indent_attributes : 1;
    tt_bool_t no_empty_element_tags : 1;
} tt_xdoc_render_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_xdoc_create(IN tt_xdoc_t *xd);

extern void tt_xdoc_destroy(IN tt_xdoc_t *xd);

extern void tt_xdoc_parse_attr_default(IN tt_xdoc_parse_attr_t *attr);

extern tt_result_t tt_xdoc_parse(IN tt_xdoc_t *xd,
                                 IN tt_u8_t *buf,
                                 IN tt_u32_t len,
                                 IN OPT tt_xdoc_parse_attr_t *attr);

extern tt_result_t tt_xdoc_parse_file(IN tt_xdoc_t *xd,
                                      IN const tt_char_t *path,
                                      IN OPT tt_xdoc_parse_attr_t *attr);

extern void tt_xdoc_render_attr_default(IN tt_xdoc_render_attr_t *attr);

extern tt_result_t tt_xdoc_render(IN tt_xdoc_t *xd,
                                  IN tt_u8_t *buf,
                                  IN tt_u32_t len,
                                  IN OPT tt_xdoc_render_attr_t *attr);

extern tt_result_t tt_xdoc_render_file(IN tt_xdoc_t *xd,
                                       IN const tt_char_t *path,
                                       IN OPT tt_xdoc_render_attr_t *attr);

#endif /* __TT_XML_DOCUMENT__ */
