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
@file tt_xml_parser.h
@brief xml parser

this file defines xml parser API
*/

#ifndef __TT_XML_PARSER__
#define __TT_XML_PARSER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_charset_convert.h>
#include <xml/parse/tt_xmlparse_state.h>
#include <xml/parse/tt_xmlparse_state_attr.h>
#include <xml/parse/tt_xmlparse_state_etag.h>
#include <xml/parse/tt_xmlparse_state_exclamation.h>
#include <xml/parse/tt_xmlparse_state_pi.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_xmlparser_s;
struct tt_xmlmem_s;

typedef tt_result_t (*tt_xp_on_state_t)(IN struct tt_xmlparser_s *xp);

typedef tt_result_t (*tt_xp_on_value_t)(IN struct tt_xmlparser_s *xp,
                                        IN tt_u8_t *value,
                                        IN tt_u32_t value_len);

typedef tt_result_t (*tt_xp_on_name_value_t)(IN struct tt_xmlparser_s *xp,
                                             IN tt_u8_t *name,
                                             IN tt_u32_t name_len,
                                             IN tt_u8_t *value,
                                             IN tt_u32_t value_len);

typedef struct
{
    // whether trimming text depends on trim_text
    tt_xp_on_value_t on_text;
    // element name would be trimmed: <elmt  a1="">, name would "elmt"
    tt_xp_on_value_t on_start_tag;
    // sees ">" of a stag
    tt_xp_on_state_t on_start_tag_complete;
    // sees "/>" of a stag
    tt_xp_on_state_t on_start_tag_end;
    // element name would be trimmed: </elmt  >, name would "elmt"
    tt_xp_on_value_t on_end_tag;
    // name would be trimmed and value would not be trimmed
    // <elmt a1 = " attr1 ">, name could be "a1", value is " attr1 "
    tt_xp_on_name_value_t on_attr;
    // both name and value would be trimmed
    // <?pi  va1  va2 va3 >, name could be "pi", value is "va1  va2 va3"
    tt_xp_on_name_value_t on_pi;
    // whether trimming text depends on trim_cdata
    tt_xp_on_value_t on_cdata;
    // whether trimming text depends on trim_comment
    tt_xp_on_value_t on_comment;
} tt_xmlparser_cb_t;

typedef struct
{
    tt_bool_t trim_text : 1;
    tt_bool_t trim_cdata : 1;
    tt_bool_t trim_comment : 1;
} tt_xmlparser_attr_t;

typedef struct tt_xmlparser_s
{
    tt_xmlparser_cb_t cb;
    tt_xmlparser_attr_t attr;
    struct tt_xmlmem_s *xm;

    tt_xmlparse_state_t state;
    union
    {
        tt_xmlpsst_attr_t attr;
        tt_xmlpsst_etag_t etag;
        tt_xmlpsst_pi_t pi;
        tt_xmlpsst_exclm_t exclm;
    } state_u;
    tt_buf_t common_buf;

    tt_charset_t from_encoding;
    tt_chsetconv_t csconv;
} tt_xmlparser_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_xmlparser_create(IN tt_xmlparser_t *xp,
                                       IN OPT struct tt_xmlmem_s *xm,
                                       IN OPT tt_xmlparser_cb_t *cb,
                                       IN OPT tt_xmlparser_attr_t *attr);

extern void tt_xmlparser_destroy(IN tt_xmlparser_t *xp);

extern void tt_xmlparser_attr_default(IN tt_xmlparser_attr_t *attr);

extern tt_result_t tt_xmlparser_update(IN tt_xmlparser_t *xp,
                                       IN tt_u8_t *data,
                                       IN tt_u32_t data_len);

extern tt_result_t tt_xmlparser_final(IN tt_xmlparser_t *xp,
                                      OUT void *reserved);

// this function must be called before trying to parse another
// new xml file
extern void tt_xmlparser_reset(IN tt_xmlparser_t *xp, IN tt_u32_t flag);

#endif /* __TT_XML_PARSER__ */
