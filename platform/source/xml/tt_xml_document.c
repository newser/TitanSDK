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

#include <xml/tt_xml_document.h>

#include <xml/tt_xml_node_element.h>

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

static void __xdoc_on_node_start(IN void *param, IN tt_xnode_t *xn);
static void __xdoc_on_node_end(IN void *param, IN tt_xnode_t *xn);
static void __xdoc_on_node(IN void *param, IN tt_xnode_t *xn);
static void __xdoc_on_error(IN void *param, IN tt_u32_t reserved);

static tt_xmlnp_cb_t tt_s_xdoc_cb = {
    __xdoc_on_node_start, __xdoc_on_node_end, __xdoc_on_node, __xdoc_on_error,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_xmldoc_create(IN tt_xmldoc_t *xdoc,
                             IN OPT tt_xmldoc_attr_t *attr)
{
    tt_xmldoc_attr_t __attr;

    TT_ASSERT(xdoc != NULL);

    if (attr == NULL) {
        tt_xmldoc_attr_default(&__attr);
        attr = &__attr;
    }

    if (!TT_OK(tt_xmlmem_create(&xdoc->xm, &attr->xm_attr))) {
        return TT_FAIL;
    }

    tt_xmlns_mgr_init(&xdoc->xns_mgr, &xdoc->xm);

    tt_memcpy(&xdoc->xnp_attr, &attr->xnp_attr, sizeof(tt_xmlnp_attr_t));
    xdoc->xnp = NULL;
    xdoc->current = NULL;
    xdoc->well_formed = TT_TRUE;

    xdoc->root = NULL;

    return TT_SUCCESS;
}

void tt_xmldoc_destroy(IN tt_xmldoc_t *xdoc)
{
    TT_ASSERT(xdoc != NULL);

    // pay attention to destroy sequence

    if (xdoc->xnp != NULL) {
        tt_xmlnp_destroy(xdoc->xnp);
    }

    if (xdoc->root != NULL) {
        tt_xnode_destroy(xdoc->root);
    }

    tt_xmlns_mgr_destroy(&xdoc->xns_mgr);

    tt_xmlmem_destroy(&xdoc->xm);
}

void tt_xmldoc_attr_default(IN tt_xmldoc_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_xmlmem_attr_default(&attr->xm_attr);

    tt_xmlnp_attr_default(&attr->xnp_attr);
}

tt_result_t tt_xmldoc_update(IN tt_xmldoc_t *xdoc,
                             IN tt_u8_t *data,
                             IN tt_u32_t data_len)
{
    TT_ASSERT(xdoc != NULL);

    if (!xdoc->well_formed) {
        // no need to continue parsing
        return TT_FAIL;
    }

    if (xdoc->xnp == NULL) {
        tt_xmlnp_t *xnp;
        tt_xnode_t *root;

        TT_ASSERT(xdoc->well_formed);
        TT_ASSERT(xdoc->root == NULL);
        TT_ASSERT(xdoc->current == NULL);

        xnp = tt_xm_alloc(&xdoc->xm, sizeof(tt_xmlnp_t));
        if (xnp == NULL) {
            return TT_FAIL;
        }

        if (!TT_OK(tt_xmlnp_create(xnp,
                                   &xdoc->xm,
                                   &xdoc->xns_mgr,
                                   &tt_s_xdoc_cb,
                                   xdoc,
                                   &xdoc->xnp_attr))) {
            tt_xm_free(xnp);
            return TT_FAIL;
        }

        root = tt_xnode_doc_create(&xdoc->xm);
        if (root == NULL) {
            tt_xmlnp_destroy(xnp);
            tt_xm_free(xnp);
            return TT_FAIL;
        }

        xdoc->xnp = xnp;
        xdoc->current = root;
        xdoc->root = root;
    }

    return tt_xmlnp_update(xdoc->xnp, data, data_len);
}

tt_result_t tt_xmldoc_final(IN tt_xmldoc_t *xdoc, OUT void *reserved)
{
    TT_ASSERT(xdoc != NULL);

    if (!xdoc->well_formed) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_xmlnp_final(xdoc->xnp, reserved))) {
        return TT_FAIL;
    }
    // not mandatory to destroy xnp, but could save some resource
    tt_xmlnp_destroy(xdoc->xnp);
    xdoc->xnp = NULL;
    xdoc->current = NULL;

    return TT_SUCCESS;
}

void tt_xmldoc_reset(IN tt_xmldoc_t *xdoc, IN tt_u32_t flag)
{
    TT_ASSERT(xdoc != NULL);

    tt_xmlns_mgr_reset(&xdoc->xns_mgr);

    if (xdoc->xnp != NULL) {
        tt_xmlnp_reset(xdoc->xnp, 0);
    }
    xdoc->current = NULL;
    xdoc->well_formed = TT_TRUE;

    // note xdoc->root can only be destroyed after resetting xdoc->xnp
    tt_xnode_destroy(xdoc->root);
    xdoc->root = NULL;
}

void __xdoc_on_node_start(IN void *param, IN tt_xnode_t *xn)
{
    tt_xmldoc_t *xdoc = (tt_xmldoc_t *)param;
    tt_result_t result;

    TT_ASSERT(xdoc->current != NULL);
    result = tt_xnode_addtail_child(xdoc->current, xn);
    TT_ASSERT(TT_OK(result));

    xdoc->current = xn;
}

void __xdoc_on_node_end(IN void *param, IN tt_xnode_t *xn)
{
    tt_xmldoc_t *xdoc = (tt_xmldoc_t *)param;

    TT_ASSERT(xdoc->current != NULL);
    TT_ASSERT(xdoc->current == xn);

    xdoc->current = tt_xnode_parent(xn);
    TT_ASSERT(xdoc->current != NULL);
}

void __xdoc_on_node(IN void *param, IN tt_xnode_t *xn)
{
    tt_xmldoc_t *xdoc = (tt_xmldoc_t *)param;

    TT_ASSERT(xdoc->current != NULL);
    tt_xnode_addtail_child(xdoc->current, xn);
}

void __xdoc_on_error(IN void *param, IN tt_u32_t reserved)
{
    tt_xmldoc_t *xdoc = (tt_xmldoc_t *)param;

    xdoc->well_formed = TT_FALSE;
}
