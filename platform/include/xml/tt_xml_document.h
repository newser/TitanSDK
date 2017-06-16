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

typedef struct
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

    tt_bool_t parse_pi : 1;
    tt_bool_t parse_comments : 1;
    tt_bool_t parse_cdata : 1;
    tt_bool_t parse_ws_pcdata : 1;
    tt_bool_t parse_escapes : 1;
    tt_bool_t parse_eol : 1;
    tt_bool_t parse_wconv_attribute : 1;
    tt_bool_t parse_wnorm_attribute : 1;
    tt_bool_t parse_declaration : 1;
    tt_bool_t parse_doctype : 1;
    tt_bool_t parse_ws_pcdata_single : 1;
    tt_bool_t parse_trim_pcdata : 1;
    tt_bool_t parse_fragment : 1;
    tt_bool_t parse_embed_pcdata : 1;
} tt_xdoc_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_xdoc_init(IN tt_xdoc_t *xd);

extern tt_result_t tt_xdoc_load(IN tt_xdoc_t *xd,
                                IN tt_u8_t *buf,
                                IN tt_u32_t len,
                                IN OPT tt_xdoc_attr_t *attr);

extern tt_result_t tt_xdoc_load_file(IN tt_xdoc_t *xd,
                                     IN const tt_char_t *path,
                                     IN OPT tt_xdoc_attr_t *attr);

extern void tt_xdoc_destroy(IN tt_xdoc_t *xd);

extern void tt_xdoc_attr_default(IN tt_xdoc_attr_t *attr);

#endif /* __TT_XML_DOCUMENT__ */
