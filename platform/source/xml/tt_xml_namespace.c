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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <xml/tt_xml_namespace.h>

#include <misc/tt_util.h>
#include <xml/parse/tt_xml_parser.h>
#include <xml/tt_xml_memory.h>

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

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_xmlns_mgr_init(IN tt_xmlns_mgr_t *xns_mgr, IN OPT tt_xmlmem_t *xm)
{
    TT_ASSERT(xns_mgr != NULL);

    xns_mgr->xm = xm;

    tt_list_init(&xns_mgr->ns_list);
}

void tt_xmlns_mgr_destroy(IN tt_xmlns_mgr_t *xns_mgr)
{
    tt_lnode_t *node;

    TT_ASSERT(xns_mgr != NULL);

    while ((node = tt_list_pop_head(&xns_mgr->ns_list)) != NULL) {
        tt_xmlns_destroy(TT_CONTAINER(node, tt_xmlns_t, node));
    }
}

void tt_xmlns_mgr_reset(IN tt_xmlns_mgr_t *xns_mgr)
{
    tt_lnode_t *node;

    TT_ASSERT(xns_mgr != NULL);

    while ((node = tt_list_pop_head(&xns_mgr->ns_list)) != NULL) {
        tt_xmlns_destroy(TT_CONTAINER(node, tt_xmlns_t, node));
    }
}

tt_xmlns_t *tt_xmlns_mgr_find_prefix(IN tt_xmlns_mgr_t *xns_mgr,
                                     IN tt_blob_t *prefix)
{
    tt_lnode_t *node;

    TT_ASSERT(xns_mgr != NULL);
    TT_ASSERT(prefix != NULL);

    node = tt_list_head(&xns_mgr->ns_list);
    while (node != NULL) {
        tt_xmlns_t *xns = TT_CONTAINER(node, tt_xmlns_t, node);

        node = node->next;

        if (tt_blob_strcmp(prefix, xns->prefix) == 0) {
            return xns;
        }
    }
    return NULL;
}

tt_xmlns_t *tt_xmlns_mgr_find_uri(IN tt_xmlns_mgr_t *xns_mgr, IN tt_blob_t *uri)
{
    tt_lnode_t *node;

    TT_ASSERT(xns_mgr != NULL);
    TT_ASSERT(uri != NULL);

    node = tt_list_head(&xns_mgr->ns_list);
    while (node != NULL) {
        tt_xmlns_t *xns = TT_CONTAINER(node, tt_xmlns_t, node);

        node = node->next;

        if (tt_blob_strcmp(uri, xns->uri) == 0) {
            return xns;
        }
    }
    return NULL;
}

tt_xmlns_t *tt_xmlns_create(IN tt_xmlns_mgr_t *xns_mgr,
                            IN struct tt_xnode_s *scope,
                            IN OPT TO tt_char_t *prefix,
                            IN OPT TO tt_char_t *uri)
{
    tt_xmlns_t *xns;

    TT_ASSERT(xns_mgr != NULL);
    TT_ASSERT(scope != NULL);

    xns = tt_xm_alloc(xns_mgr->xm, sizeof(tt_xmlns_t));
    if (xns == NULL) {
        TT_ERROR("new mem for new xml ns");
        return NULL;
    }

    tt_lnode_init(&xns->node);

    xns->scope = scope;
    xns->prefix = prefix;
    xns->uri = uri;

    tt_lnode_init(&xns->u.parse_node);

    return xns;
}

void tt_xmlns_destroy(IN tt_xmlns_t *xns)
{
    TT_ASSERT(xns != NULL);
    TT_ASSERT(xns->node.lst == NULL);

    if (xns->prefix != NULL) {
        tt_xm_free(xns->prefix);
    }

    if (xns->uri != NULL) {
        tt_xm_free(xns->uri);
    }

    tt_xm_free(xns);
}

void tt_xmlns_set_prefix(IN tt_xmlns_t *xns, IN TO tt_char_t *prefix)
{
    if (xns->prefix != NULL) {
        tt_xm_free(xns->prefix);
    }
    xns->prefix = prefix;
}

extern void tt_xmlns_set_uri(IN tt_xmlns_t *xns, IN TO tt_char_t *uri)
{
    if (xns->uri != NULL) {
        tt_xm_free(xns->uri);
    }
    xns->uri = uri;
}
