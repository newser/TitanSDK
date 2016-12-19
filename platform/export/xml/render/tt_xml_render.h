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
@file tt_xml_render.h
@brief xml render

this file defines xml render API
*/

#ifndef __TT_XML_RENDER__
#define __TT_XML_RENDER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_buf_attr_t buf_attr;

    const tt_u8_t *line_break;
    tt_u32_t line_break_len;

    const tt_u8_t *indent;
    tt_u32_t indent_len;

    tt_bool_t ignore_cdata : 1;
    tt_bool_t ignore_comment : 1;
    tt_bool_t ignore_pi : 1;
} tt_xmlrender_attr_t;

typedef struct tt_xmlrender_s
{
    tt_buf_t buf;
    tt_u32_t level;
    tt_bool_t newline;

    const tt_u8_t *line_break;
    tt_u32_t line_break_len;

    const tt_u8_t *indent;
    tt_u32_t indent_len;

    tt_bool_t ignore_cdata : 1;
    tt_bool_t ignore_comment : 1;
    tt_bool_t ignore_pi : 1;
} tt_xmlrender_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_xmlrender_create(IN tt_xmlrender_t *xr,
                                       IN OPT tt_xmlrender_attr_t *attr);

extern void tt_xmlrender_destroy(IN tt_xmlrender_t *xr);

extern void tt_xmlrender_attr_default(IN tt_xmlrender_attr_t *attr);

extern void tt_xmlrender_reset(IN tt_xmlrender_t *xr, IN tt_u32_t flag);

extern tt_result_t tt_xmlrender_text(IN tt_xmlrender_t *xr,
                                     IN const tt_char_t *text);

extern tt_result_t tt_xmlrender_stag(IN tt_xmlrender_t *xr,
                                     IN const tt_char_t *name);

extern tt_result_t tt_xmlrender_stag_complete(IN tt_xmlrender_t *xr,
                                              IN tt_bool_t newline);

extern tt_result_t tt_xmlrender_stag_end(IN tt_xmlrender_t *xr);

extern tt_result_t tt_xmlrender_etag(IN tt_xmlrender_t *xr,
                                     IN const tt_char_t *name);

extern tt_result_t tt_xmlrender_attr(IN tt_xmlrender_t *xr,
                                     IN const tt_char_t *name,
                                     IN const tt_char_t *value);

extern tt_result_t tt_xmlrender_pi(IN tt_xmlrender_t *xr,
                                   IN const tt_char_t *name,
                                   IN const tt_char_t *value);

extern tt_result_t tt_xmlrender_comment(IN tt_xmlrender_t *xr,
                                        IN const tt_char_t *comment);

extern tt_result_t tt_xmlrender_cdata(IN tt_xmlrender_t *xr,
                                      IN const tt_char_t *cdata);

#endif /* __TT_XML_RENDER__ */
