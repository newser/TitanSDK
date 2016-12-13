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

this file defines xml document api
*/

#ifndef __TT_XML_DOCUMENT__
#define __TT_XML_DOCUMENT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <xml/tt_xml_memory.h>
#include <xml/tt_xml_namespace.h>
#include <xml/tt_xml_node.h>
#include <xml/tt_xml_node_parser.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_xmlmem_attr_t xm_attr;
    tt_xmlnp_attr_t xnp_attr;
} tt_xmldoc_attr_t;

typedef struct tt_xmldoc_s
{
    tt_xmlmem_t xm;
    tt_xmlns_mgr_t xns_mgr;

    tt_xmlnp_attr_t xnp_attr;
    tt_xmlnp_t *xnp;
    tt_xnode_t *current;
    tt_bool_t well_formed;

    tt_xnode_t *root;
} tt_xmldoc_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_xmldoc_create(IN tt_xmldoc_t *xdoc,
                                    IN OPT tt_xmldoc_attr_t *attr);

extern void tt_xmldoc_destroy(IN tt_xmldoc_t *xdoc);

extern void tt_xmldoc_attr_default(IN tt_xmldoc_attr_t *attr);

extern tt_result_t tt_xmldoc_update(IN tt_xmldoc_t *xdoc,
                                    IN tt_u8_t *data,
                                    IN tt_u32_t data_len);

extern tt_result_t tt_xmldoc_final(IN tt_xmldoc_t *xdoc, OUT void *reserved);

extern void tt_xmldoc_reset(IN tt_xmldoc_t *xdoc, IN tt_u32_t flag);

#endif /* __TT_XML_DOCUMENT__ */
