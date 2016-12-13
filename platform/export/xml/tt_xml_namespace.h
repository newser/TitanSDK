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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either exmress or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
@file tt_xml_namespace.h
@brief xml namespace

this file defines xml namespace api
*/

#ifndef __TT_XML_NAMESPACE__
#define __TT_XML_NAMESPACE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_xmlmem_s;
struct tt_xnode_s;

typedef struct tt_xmlns_s
{
    tt_lnode_t node;

    struct tt_xnode_s *scope;
    // prefix could 0 for default namespace
    tt_char_t *prefix;
    tt_char_t *uri;

    union
    {
        tt_lnode_t parse_node;
    } u;
} tt_xmlns_t;

typedef struct tt_xmlns_mgr_s
{
    struct tt_xmlmem_s *xm;

    // it's also possible to store ns in a hash table
    tt_list_t ns_list;
} tt_xmlns_mgr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_xmlns_mgr_init(IN tt_xmlns_mgr_t *xns_mgr,
                              IN OPT struct tt_xmlmem_s *xm);

extern void tt_xmlns_mgr_destroy(IN tt_xmlns_mgr_t *xns_mgr);

extern void tt_xmlns_mgr_reset(IN tt_xmlns_mgr_t *xns_mgr);

extern tt_xmlns_t *tt_xmlns_mgr_find_prefix(IN tt_xmlns_mgr_t *xns_mgr,
                                            IN tt_blob_t *prefix);

extern tt_xmlns_t *tt_xmlns_mgr_find_uri(IN tt_xmlns_mgr_t *xns_mgr,
                                         IN tt_blob_t *uri);

extern tt_xmlns_t *tt_xmlns_create(IN tt_xmlns_mgr_t *xns_mgr,
                                   IN struct tt_xnode_s *scope,
                                   IN OPT TO tt_char_t *prefix,
                                   IN OPT TO tt_char_t *uri);

extern void tt_xmlns_destroy(IN tt_xmlns_t *xns);

extern void tt_xmlns_set_prefix(IN tt_xmlns_t *xns, IN TO tt_char_t *prefix);

extern void tt_xmlns_set_uri(IN tt_xmlns_t *xns, IN TO tt_char_t *uri);

#endif /* __TT_XML_NAMESPACE__ */
