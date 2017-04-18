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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either exrress or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
@file tt_xml_node_render.h
@brief xml node render

this file defines xml node render API
*/

#ifndef __TT_XML_NODE_RENDER__
#define __TT_XML_NODE_RENDER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <xml/render/tt_xml_render.h>
#include <xml/tt_xml_node.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_xmlrender_attr_t xr_attr;
} tt_xmlnr_attr_t;

typedef struct tt_xmlnr_s
{
    tt_xmlrender_t xr;
} tt_xmlnr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// if xns_mgr is not null, the parser would try parsing namespaces
extern tt_result_t tt_xmlnr_create(IN tt_xmlnr_t *xnr,
                                   IN OPT tt_xmlnr_attr_t *attr);

extern void tt_xmlnr_destroy(IN tt_xmlnr_t *xnr);

extern void tt_xmlnr_attr_default(IN tt_xmlnr_attr_t *attr);

extern tt_result_t tt_xmlnr_render(IN tt_xmlnr_t *xnr, IN OPT tt_xnode_t *xn);

tt_inline void tt_xmlnr_data(IN tt_xmlnr_t *xnr,
                             OUT tt_u8_t **data,
                             OUT tt_u32_t *len)
{
    *data = TT_BUF_RPOS(&xnr->xr.buf);
    *len = TT_BUF_RLEN(&xnr->xr.buf);
}

tt_inline void tt_xmlnr_reset(IN tt_xmlnr_t *xnr, IN tt_u32_t flag)
{
    tt_xmlrender_reset(&xnr->xr, 0);
}

#endif /* __TT_XML_NODE_RENDER__ */
