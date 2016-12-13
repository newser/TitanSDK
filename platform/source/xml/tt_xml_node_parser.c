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

#include <xml/tt_xml_node_parser.h>

#include <xml/parse/tt_xmlparse_char_decode.h>
#include <xml/tt_xml_memory.h>
#include <xml/tt_xml_namespace.h>
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

static tt_result_t __xps_on_text(IN struct tt_xmlparser_s *parser,
                                 IN tt_u8_t *value,
                                 IN tt_u32_t value_len);

static tt_result_t __xps_on_stag(IN struct tt_xmlparser_s *parser,
                                 IN tt_u8_t *value,
                                 IN tt_u32_t value_len);

static tt_result_t __xps_on_stag_end(IN struct tt_xmlparser_s *parser);

static tt_result_t __xps_on_stag_complete(IN struct tt_xmlparser_s *parser);

static tt_result_t __xps_on_etag(IN struct tt_xmlparser_s *parser,
                                 IN tt_u8_t *value,
                                 IN tt_u32_t value_len);

static tt_result_t __xps_on_attr(IN struct tt_xmlparser_s *parser,
                                 IN tt_u8_t *name,
                                 IN tt_u32_t name_len,
                                 IN tt_u8_t *value,
                                 IN tt_u32_t value_len);

static tt_result_t __xps_on_pi(IN struct tt_xmlparser_s *parser,
                               IN tt_u8_t *name,
                               IN tt_u32_t name_len,
                               IN tt_u8_t *value,
                               IN tt_u32_t value_len);

static tt_result_t __xps_on_cdata(IN struct tt_xmlparser_s *parser,
                                  IN tt_u8_t *value,
                                  IN tt_u32_t value_len);

static tt_result_t __xps_on_comment(IN struct tt_xmlparser_s *parser,
                                    IN tt_u8_t *value,
                                    IN tt_u32_t value_len);

static tt_xmlparser_cb_t tt_s_xp_cb = {
    __xps_on_text,
    __xps_on_stag,
    __xps_on_stag_complete,
    __xps_on_stag_end,
    __xps_on_etag,
    __xps_on_attr,
    __xps_on_pi,
    __xps_on_cdata,
    __xps_on_comment,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __xn_destroy(IN void *obj);

static tt_result_t __parse_ns(IN tt_xmlnp_t *xnp, IN tt_xnode_t *xn);

static tt_result_t __check_new_ns(IN tt_xmlnp_t *xnp, IN tt_xnode_elmt_t *xne);

static void __set_ns(IN tt_xnode_t *xn, IN tt_xmlnp_t *xnp);

static void __parse_prefix_name(IN const tt_char_t *value,
                                OUT tt_blob_t *prefix,
                                OUT tt_blob_t *name);

static void __push_ns(IN tt_xmlnp_t *xnp, IN tt_xmlns_t *xns);

static tt_xmlns_t *__find_ns(IN tt_xmlnp_t *xnp, IN tt_blob_t *prefix);

static void __push_def_ns(IN tt_xmlnp_t *xnp, IN tt_xmlns_t *xns);

static tt_xmlns_t *__top_def_ns(IN tt_xmlnp_t *xnp);

static tt_xnode_t *__pop_xnode(IN tt_xmlnp_t *xnp);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_xmlnp_create(IN tt_xmlnp_t *xnp,
                            IN OPT tt_xmlmem_t *xm,
                            IN OPT struct tt_xmlns_mgr_s *xns_mgr,
                            IN OPT tt_xmlnp_cb_t *cb,
                            IN OPT void *cb_param,
                            IN OPT tt_xmlnp_attr_t *attr)
{
    tt_xmlnp_attr_t __attr;
    tt_stack_attr_t stack_attr;

    tt_u32_t __done = 0;
#define __XC_XP (1 << 0)
#define __XC_XNODE (1 << 1)

    TT_ASSERT(xnp != NULL);

    if (attr == NULL) {
        tt_xmlnp_attr_default(&__attr);
        attr = &__attr;
    }

    if (!TT_OK(
            tt_xmlparser_create(&xnp->xp, xm, &tt_s_xp_cb, &attr->xp_attr))) {
        goto xc_fail;
    }
    __done |= __XC_XP;

    xnp->xm = xm;
    xnp->xns_mgr = xns_mgr;
    tt_list_init(&xnp->ns);
    tt_list_init(&xnp->def_ns);

    tt_stack_attr_default(&stack_attr);
    stack_attr.obj_destroy = __xn_destroy;
    if (!TT_OK(tt_ptrstack_create(&xnp->xnode, 128, NULL))) {
        goto xc_fail;
    }
    __done |= __XC_XNODE;

    if (cb != NULL) {
        tt_memcpy(&xnp->cb, cb, sizeof(tt_xmlnp_cb_t));
    } else {
        tt_memset(&xnp->cb, 0, sizeof(tt_xmlnp_cb_t));
    }
    xnp->cb_param = cb_param;
    xnp->ignore_pi = attr->ignore_pi;
    xnp->ignore_cdata = attr->ignore_cdata;
    xnp->ignore_comment = attr->ignore_comment;

    return TT_SUCCESS;

xc_fail:

    if (__done & __XC_XNODE) {
        tt_ptrstack_destroy(&xnp->xnode);
    }

    if (__done & __XC_XP) {
        tt_xmlparser_destroy(&xnp->xp);
    }

    return TT_FAIL;
}

void tt_xmlnp_destroy(IN tt_xmlnp_t *xnp)
{
    tt_lnode_t *node;

    TT_ASSERT(xnp != NULL);

    tt_xmlparser_destroy(&xnp->xp);

    // the namespaces does not belong to xnp
    while ((node = tt_list_pophead(&xnp->ns)) != NULL)
        ;
    while ((node = tt_list_pophead(&xnp->def_ns)) != NULL)
        ;

    tt_ptrstack_destroy(&xnp->xnode);
}

void tt_xmlnp_attr_default(IN tt_xmlnp_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_xmlparser_attr_default(&attr->xp_attr);

    attr->ignore_cdata = TT_FALSE;
    attr->ignore_comment = TT_FALSE;
    attr->ignore_pi = TT_FALSE;
}

tt_result_t tt_xmlnp_update(IN tt_xmlnp_t *xnp,
                            IN tt_u8_t *data,
                            IN tt_u32_t data_len)
{
    TT_ASSERT(xnp != NULL);

    return tt_xmlparser_update(&xnp->xp, data, data_len);
}

tt_result_t tt_xmlnp_final(IN tt_xmlnp_t *xnp, OUT void *reserved)
{
    TT_ASSERT(xnp != NULL);

    if (tt_ptrstack_top(&xnp->xnode) != NULL) {
        TT_ERROR("some elements are not terminated");
        return TT_FAIL;
    }

    return tt_xmlparser_final(&xnp->xp, reserved);
}

void tt_xmlnp_reset(IN tt_xmlnp_t *xnp, IN tt_u32_t flag)
{
    tt_lnode_t *node;

    TT_ASSERT(xnp != NULL);

    tt_xmlparser_reset(&xnp->xp, flag);

    while ((node = tt_list_pophead(&xnp->ns)) != NULL)
        ;
    while ((node = tt_list_pophead(&xnp->def_ns)) != NULL)
        ;

    tt_ptrstack_clear(&xnp->xnode);
}

tt_result_t __xps_on_text(IN struct tt_xmlparser_s *parser,
                          IN tt_u8_t *value,
                          IN tt_u32_t value_len)
{
    tt_xmlnp_t *xnp = TT_CONTAINER(parser, tt_xmlnp_t, xp);
    tt_char_t *text;
    tt_xnode_t *xn;

    if (xnp->cb.on_node == NULL) {
        return TT_SUCCESS;
    }

    // text need be escaped
    text = tt_xp_chdec_len(xnp->xm, (tt_char_t *)value, value_len);
    if (text == NULL) {
        TT_ERROR("no mem for data xnode");
        return TT_FAIL;
    }

    xn = tt_xnode_text_create(xnp->xm, text);
    if (xn == NULL) {
        tt_xm_free(text);
        return TT_FAIL;
    }

    xnp->cb.on_node(xnp->cb_param, xn);

    return TT_SUCCESS;
}

tt_result_t __xps_on_stag(IN struct tt_xmlparser_s *parser,
                          IN tt_u8_t *value,
                          IN tt_u32_t value_len)
{
    tt_xmlnp_t *xnp = TT_CONTAINER(parser, tt_xmlnp_t, xp);
    tt_char_t *name;
    tt_xnode_t *xn;

    // name need be escaped
    name = tt_xp_chdec_len(xnp->xm, (tt_char_t *)value, value_len);
    if (name == NULL) {
        TT_ERROR("no mem for xnode tag name");
        return TT_FAIL;
    }

    xn = tt_xnode_elmt_create(xnp->xm, name);
    if (xn == NULL) {
        tt_xm_free(name);
        return TT_FAIL;
    }

    if (!TT_OK(tt_ptrstack_push(&xnp->xnode, xn))) {
        tt_xnode_destroy(xn);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __xps_on_stag_complete(IN struct tt_xmlparser_s *parser)
{
    tt_xmlnp_t *xnp = TT_CONTAINER(parser, tt_xmlnp_t, xp);
    tt_xnode_t *xn;

    xn = tt_ptrstack_top(&xnp->xnode);
    if (xn == NULL) {
        TT_ERROR("must have start tag");
        return TT_FAIL;
    }

    if (xnp->xns_mgr != NULL) {
        __parse_ns(xnp, xn);
    }

    if (xnp->cb.on_node_start != NULL) {
        xnp->cb.on_node_start(xnp->cb_param, xn);
    }

    return TT_SUCCESS;
}

tt_result_t __xps_on_stag_end(IN struct tt_xmlparser_s *parser)
{
    tt_xmlnp_t *xnp = TT_CONTAINER(parser, tt_xmlnp_t, xp);
    tt_xnode_t *xn;

    xn = __pop_xnode(xnp);
    if (xn == NULL) {
        TT_ERROR("must have start tag");
        return TT_FAIL;
    }

    if (xnp->xns_mgr != NULL) {
        __parse_ns(xnp, xn);
    }

    if (xnp->cb.on_node != NULL) {
        xnp->cb.on_node(xnp->cb_param, xn);
    } else {
        tt_xnode_destroy(xn);
    }

    return TT_SUCCESS;
}

tt_result_t __xps_on_etag(IN struct tt_xmlparser_s *parser,
                          IN tt_u8_t *value,
                          IN tt_u32_t value_len)
{
    tt_xmlnp_t *xnp = TT_CONTAINER(parser, tt_xmlnp_t, xp);
    tt_char_t *name;
    tt_xnode_t *xn;
    tt_bool_t match;

    // name need be escaped
    name = tt_xp_chdec_len(xnp->xm, (tt_char_t *)value, value_len);
    if (name == NULL) {
        TT_ERROR("no mem for xnode tag name");
        return TT_FAIL;
    }

    xn = __pop_xnode(xnp);
    if (xn == NULL) {
        TT_ERROR("must have start tag");
        tt_xm_free(name);
        return TT_FAIL;
    }

    match = TT_FALSE;
    if ((xn->ns != NULL) && (xn->ns->prefix != NULL)) {
        tt_blob_t pb, nb;

        __parse_prefix_name(name, &pb, &nb);
        if ((pb.addr != NULL) && tt_blob_cmpcstr(&pb, xn->ns->prefix) &&
            tt_blob_cmpcstr(&nb, xn->name)) {
            match = TT_TRUE;
        }
    } else if (tt_strcmp(xn->name, name) == 0) {
        match = TT_TRUE;
    }
    tt_xm_free(name);
    if (!match) {
        TT_ERROR("different start tag and end tag");
        if (xnp->cb.on_error != NULL) {
            xnp->cb.on_error(xnp->cb_param, 0);
        }

        // do not destroy xn, as app may be holding it
        return TT_FAIL;
    }

    if (xnp->cb.on_node_end != NULL) {
        xnp->cb.on_node_end(xnp->cb_param, xn);
    } else {
        tt_xnode_destroy(xn);
    }

    return TT_SUCCESS;
}

tt_result_t __xps_on_attr(IN struct tt_xmlparser_s *parser,
                          IN tt_u8_t *name,
                          IN tt_u32_t name_len,
                          IN tt_u8_t *value,
                          IN tt_u32_t value_len)
{
    tt_xmlnp_t *xnp = TT_CONTAINER(parser, tt_xmlnp_t, xp);
    tt_char_t *a_name = NULL, *a_val = NULL;
    tt_xnode_t *xn, *parent;

    a_name = tt_xp_chdec_len(xnp->xm, (tt_char_t *)name, name_len);
    if (a_name == NULL) {
        TT_ERROR("no mem for attr xnode name");
        return TT_FAIL;
    }

    a_val = tt_xp_chdec_len(xnp->xm, (tt_char_t *)value, value_len);
    if (a_val == NULL) {
        TT_ERROR("no mem for attr xnode value");
        tt_xm_free(a_name);
        return TT_FAIL;
    }

    xn = tt_xnode_attr_create(xnp->xm, a_name, a_val);
    if (xn == NULL) {
        tt_xm_free(a_val);
        tt_xm_free(a_name);
        return TT_FAIL;
    }

    parent = tt_ptrstack_top(&xnp->xnode);
    if (parent == NULL) {
        TT_ERROR("must have parent element");
        tt_xnode_destroy(xn);
        return TT_FAIL;
    }

    // actually this should always succeed
    if (!TT_OK(tt_xnode_add_attr(parent, xn))) {
        tt_xnode_destroy(xn);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __xps_on_pi(IN struct tt_xmlparser_s *parser,
                        IN tt_u8_t *name,
                        IN tt_u32_t name_len,
                        IN tt_u8_t *value,
                        IN tt_u32_t value_len)
{
    tt_xmlnp_t *xnp = TT_CONTAINER(parser, tt_xmlnp_t, xp);
    tt_char_t *pi_name = NULL, *pi_val = NULL;
    tt_xnode_t *xn;

    if ((xnp->cb.on_node == NULL) || xnp->ignore_pi) {
        return TT_SUCCESS;
    }

    pi_name = tt_cstr_copy_n((tt_char_t *)name, name_len);
    if (pi_name == NULL) {
        TT_ERROR("no mem for pi xnode name");
        return TT_FAIL;
    }

    pi_val = tt_cstr_copy_n((tt_char_t *)value, value_len);
    if (pi_val == NULL) {
        TT_ERROR("no mem for pi xnode name");
        tt_mem_free(pi_name);
        return TT_FAIL;
    }

    xn = tt_xnode_pi_create(xnp->xm, pi_name, pi_val);
    if (xn == NULL) {
        tt_mem_free(pi_val);
        tt_mem_free(pi_name);
        return TT_FAIL;
    }

    xnp->cb.on_node(xnp->cb_param, xn);

    return TT_SUCCESS;
}

tt_result_t __xps_on_cdata(IN struct tt_xmlparser_s *parser,
                           IN tt_u8_t *value,
                           IN tt_u32_t value_len)
{
    tt_xmlnp_t *xnp = TT_CONTAINER(parser, tt_xmlnp_t, xp);
    tt_char_t *cdata;
    tt_xnode_t *xn;

    if ((xnp->cb.on_node == NULL) || xnp->ignore_cdata) {
        return TT_SUCCESS;
    }

    cdata = tt_cstr_copy_n((tt_char_t *)value, value_len);
    if (cdata == NULL) {
        TT_ERROR("no mem for cdata xnode");
        return TT_FAIL;
    }

    xn = tt_xnode_cdata_create(xnp->xm, cdata);
    if (xn == NULL) {
        tt_mem_free(cdata);
        return TT_FAIL;
    }

    xnp->cb.on_node(xnp->cb_param, xn);

    return TT_SUCCESS;
}

tt_result_t __xps_on_comment(IN struct tt_xmlparser_s *parser,
                             IN tt_u8_t *value,
                             IN tt_u32_t value_len)
{
    tt_xmlnp_t *xnp = TT_CONTAINER(parser, tt_xmlnp_t, xp);
    tt_char_t *comm;
    tt_xnode_t *xn;

    if ((xnp->cb.on_node == NULL) || xnp->ignore_comment) {
        return TT_SUCCESS;
    }

    comm = tt_cstr_copy_n((tt_char_t *)value, value_len);
    if (comm == NULL) {
        TT_ERROR("no mem for comm xnode");
        return TT_FAIL;
    }

    xn = tt_xnode_comment_create(xnp->xm, comm);
    if (xn == NULL) {
        tt_mem_free(comm);
        return TT_FAIL;
    }

    xnp->cb.on_node(xnp->cb_param, xn);

    return TT_SUCCESS;
}

void __xn_destroy(IN void *obj)
{
    tt_xnode_destroy((tt_xnode_t *)obj);
}

tt_result_t __parse_ns(IN tt_xmlnp_t *xnp, IN tt_xnode_t *xn)
{
    tt_xnode_elmt_t *xne;
    tt_lnode_t *node;

    TT_ASSERT(xn->type == TT_XNODE_TYPE_ELEMENT);
    xne = TT_XNODE_CAST(xn, tt_xnode_elmt_t);

    // find if there is new namespace
    __check_new_ns(xnp, xne);

    // parse ns of tag
    __set_ns(xn, xnp);

    // parse ns of attributes
    node = tt_list_head(&xne->attr);
    while (node != NULL) {
        tt_xnode_t *attr = TT_CONTAINER(node, tt_xnode_t, node);

        node = node->next;

        __set_ns(attr, xnp);
    }

    return TT_SUCCESS;
}

tt_result_t __check_new_ns(IN tt_xmlnp_t *xnp, IN tt_xnode_elmt_t *xne)
{
    tt_lnode_t *node;

    node = tt_list_head(&xne->attr);
    while (node != NULL) {
        tt_xnode_t *attr = TT_CONTAINER(node, tt_xnode_t, node);

        node = node->next;

        if (tt_strncmp(attr->name, "xmlns:", 6) == 0) {
            tt_u32_t len;
            tt_xmlns_t *ns;

            // xmlns:p="test://ns.com"

            len = (tt_u32_t)tt_strlen(attr->name) - 6;
            if (len == 0) {
                TT_ERROR("invalid xmlns: %s", attr->name);
                return TT_FAIL;
            }
            tt_memmove(attr->name, attr->name + 6, len + 1);

            ns = tt_xmlns_create(xnp->xns_mgr,
                                 TT_XNODE_OF(xne),
                                 attr->name,
                                 attr->value);
            if (ns == NULL) {
                return TT_FAIL;
            }
            __push_ns(xnp, ns);

            tt_list_remove(&attr->node);
            // note ns has taken the ownership of name and value
            attr->name = NULL;
            attr->value = NULL;
            tt_xnode_destroy(attr);
        } else if (tt_strcmp(attr->name, "xmlns") == 0) {
            tt_xmlns_t *ns;

            // xmlns="test://ns.com"

            ns = tt_xmlns_create(xnp->xns_mgr,
                                 TT_XNODE_OF(xne),
                                 NULL,
                                 attr->value);
            if (ns == NULL) {
                return TT_FAIL;
            }
            __push_def_ns(xnp, ns);

            tt_list_remove(&attr->node);
            // note ns has taken the ownership of value
            attr->value = NULL;
            tt_xnode_destroy(attr);
        }
    }

    return TT_SUCCESS;
}

void __set_ns(IN tt_xnode_t *xn, IN tt_xmlnp_t *xnp)
{
    tt_blob_t prefix, name;

    __parse_prefix_name(xn->name, &prefix, &name);
    if ((prefix.len != 0) && (name.len != 0)) {
        TT_ASSERT(prefix.addr != NULL);
        TT_ASSERT(name.addr != NULL);
        xn->ns = __find_ns(xnp, &prefix);
        if (xn->ns != NULL) {
            tt_memmove(xn->name, name.addr, name.len + 1);
        }
        // else give an error??
    } else {
        // use default ns
        xn->ns = __top_def_ns(xnp);
    }
}

void __parse_prefix_name(IN const tt_char_t *value,
                         OUT tt_blob_t *prefix,
                         OUT tt_blob_t *name)
{
    tt_u32_t i = 0;
    tt_u32_t len = (tt_u32_t)tt_strlen(value);

    while ((i < len) && (value[i] != ':')) {
        ++i;
    }

    if (i < len) {
        prefix->addr = (tt_u8_t *)value;
        prefix->len = i;

        name->addr = (tt_u8_t *)&value[i + 1];
        name->len = len - i - 1;
    } else {
        prefix->addr = NULL;
        prefix->len = 0;

        name->addr = (tt_u8_t *)value;
        name->len = len;
    }
}

void __push_ns(IN tt_xmlnp_t *xnp, IN tt_xmlns_t *xns)
{
    tt_list_addtail(&xnp->ns, &xns->u.parse_node);
}

tt_xmlns_t *__find_ns(IN tt_xmlnp_t *xnp, IN tt_blob_t *prefix)
{
    tt_lnode_t *node;

    node = tt_list_tail(&xnp->ns);
    while (node != NULL) {
        tt_xmlns_t *xns = TT_CONTAINER(node, tt_xmlns_t, u.parse_node);

        node = node->prev;

        if (tt_blob_cmpcstr(prefix, xns->prefix)) {
            return xns;
        }
    }
    return NULL;
}

void __push_def_ns(IN tt_xmlnp_t *xnp, IN tt_xmlns_t *xns)
{
    tt_list_addtail(&xnp->def_ns, &xns->u.parse_node);
}

tt_xmlns_t *__top_def_ns(IN tt_xmlnp_t *xnp)
{
    tt_lnode_t *node = tt_list_tail(&xnp->def_ns);
    return TT_COND(node != NULL,
                   TT_CONTAINER(node, tt_xmlns_t, u.parse_node),
                   NULL);
}

tt_xnode_t *__pop_xnode(IN tt_xmlnp_t *xnp)
{
    tt_xnode_t *xn = tt_ptrstack_pop(&xnp->xnode);
    if (xn != NULL) {
        tt_lnode_t *node;
        // pop its namespaces

        node = tt_list_tail(&xnp->ns);
        while (node != NULL) {
            tt_xmlns_t *xns = TT_CONTAINER(node, tt_xmlns_t, u.parse_node);

            node = node->prev;

            if (xns->scope == xn) {
                tt_list_remove(&xns->u.parse_node);
            }
        }

        node = tt_list_tail(&xnp->def_ns);
        while (node != NULL) {
            tt_xmlns_t *xns = TT_CONTAINER(node, tt_xmlns_t, u.parse_node);

            node = node->prev;

            if (xns->scope == xn) {
                tt_list_remove(&xns->u.parse_node);
            }
        }
    }
    return xn;
}
