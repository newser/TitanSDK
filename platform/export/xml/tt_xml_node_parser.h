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
@file tt_xml_node_parser.h
@brief xml node parser

this file defines xml node parser API
*/

#ifndef __TT_XML_NODE_PARSER__
#define __TT_XML_NODE_PARSER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_stack.h>
#include <xml/parse/tt_xml_parser.h>
#include <xml/tt_xml_node.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_xmlmem_s;
struct tt_xmlns_s;
struct tt_xmlns_mgr_s;

// - @ref end indicate a node is terminated. each time the parser
//   sees "<element ", it set @ref end to false, and true when sees
//   "/>" or "</element>"
// - if @ref end is false, data of xn may be changed during parsing
//   until @ref end is true, but the pointer is always valid
// - if @ref end is true, app is taking the ownership of xn, otherwise
//   app can not destroy returned xn
typedef void (*tt_xmlnp_on_node_t)(IN void *param,
                                   IN tt_xnode_t *xn,
                                   IN tt_bool_t end);

typedef void (*tt_xmlnp_on_error_t)(IN void *param, IN tt_u32_t reserved);

typedef struct
{
    tt_xmlnp_on_node_t on_node;
    tt_xmlnp_on_error_t on_error;
} tt_xmlnp_cb_t;

typedef struct
{
    tt_xmlparser_attr_t xp_attr;

    tt_bool_t ignore_cdata : 1;
    tt_bool_t ignore_comment : 1;
    tt_bool_t ignore_pi : 1;
} tt_xmlnp_attr_t;

typedef struct tt_xmlnp_s
{
    tt_xmlparser_t xp;
    struct tt_xmlmem_s *xm;
    struct tt_xmlns_mgr_s *xns_mgr;
    tt_list_t ns;
    tt_list_t def_ns;
    tt_ptrstack_t xnode;

    tt_xmlnp_cb_t cb;
    void *cb_param;
    tt_bool_t ignore_cdata : 1;
    tt_bool_t ignore_comment : 1;
    tt_bool_t ignore_pi : 1;
} tt_xmlnp_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// if xns_mgr is not null, the parser would try parsing namespaces
extern tt_result_t tt_xmlnp_create(IN tt_xmlnp_t *xnp,
                                   IN OPT struct tt_xmlmem_s *xm,
                                   IN OPT struct tt_xmlns_mgr_s *xns_mgr,
                                   IN OPT tt_xmlnp_cb_t *cb,
                                   IN OPT void *cb_param,
                                   IN OPT tt_xmlnp_attr_t *attr);

extern void tt_xmlnp_destroy(IN tt_xmlnp_t *xnp);

extern void tt_xmlnp_attr_default(IN tt_xmlnp_attr_t *attr);

extern tt_result_t tt_xmlnp_update(IN tt_xmlnp_t *xnp,
                                   IN tt_u8_t *data,
                                   IN tt_u32_t data_len);

extern tt_result_t tt_xmlnp_final(IN tt_xmlnp_t *xnp, OUT void *reserved);

// this function must be called before trying to parse another
// new xml file
extern void tt_xmlnp_reset(IN tt_xmlnp_t *xnp, IN tt_u32_t flag);

#endif /* __TT_XML_NODE_PARSER__ */
