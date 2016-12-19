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

#include <xml/tt_xml_node_render.h>

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

static tt_result_t __render_doc(IN tt_xmlnr_t *xnr, IN tt_xnode_t *xn);

static tt_result_t __render_elmt(IN tt_xmlnr_t *xnr, IN tt_xnode_t *xn);

static tt_result_t __render_attrlist(IN tt_xmlrender_t *xr, IN tt_xnode_t *xn);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_xmlnr_create(IN tt_xmlnr_t *xnr, IN OPT tt_xmlnr_attr_t *attr)
{
    tt_xmlnr_attr_t __attr;

    TT_ASSERT(xnr != NULL);

    if (attr == NULL) {
        tt_xmlnr_attr_default(&__attr);
        attr = &__attr;
    }

    if (!TT_OK(tt_xmlrender_create(&xnr->xr, &attr->xr_attr))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_xmlnr_destroy(IN tt_xmlnr_t *xnr)
{
    TT_ASSERT(xnr != NULL);

    tt_xmlrender_destroy(&xnr->xr);
}

void tt_xmlnr_attr_default(IN tt_xmlnr_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_xmlrender_attr_default(&attr->xr_attr);
}

tt_result_t tt_xmlnr_render(IN tt_xmlnr_t *xnr, IN OPT tt_xnode_t *xn)
{
    tt_xmlrender_t *xr;

    TT_ASSERT(xnr != NULL);

    if (xn == NULL) {
        return TT_SUCCESS;
    }

    xr = &xnr->xr;
    switch (xn->type) {
        case TT_XNODE_TYPE_DOC: {
            return __render_doc(xnr, xn);
        } break;
        case TT_XNODE_TYPE_ELEMENT: {
            return __render_elmt(xnr, xn);
        } break;
        case TT_XNODE_TYPE_ATTR: {
            return tt_xmlrender_attr(xr, xn->name, xn->value);
        } break;
        case TT_XNODE_TYPE_TEXT: {
            return tt_xmlrender_text(xr, xn->value);
        } break;
        case TT_XNODE_TYPE_CDATA: {
            return tt_xmlrender_cdata(xr, xn->value);
        } break;
        case TT_XNODE_TYPE_COMMENT: {
            return tt_xmlrender_comment(xr, xn->value);
        } break;
        case TT_XNODE_TYPE_PI: {
            return tt_xmlrender_pi(xr, xn->name, xn->value);
        } break;

        default: {
            TT_ERROR("unknown type: %d", xn->type);
        } break;
    }

    return TT_SUCCESS;
}

tt_result_t __render_doc(IN tt_xmlnr_t *xnr, IN tt_xnode_t *xn)
{
    tt_xnode_t *c = tt_xnode_first_child(xn);
    while (c != NULL) {
        TT_DO(tt_xmlnr_render(xnr, c));
        c = tt_xnode_next(c);
    }
    return TT_SUCCESS;
}

tt_result_t __render_elmt(IN tt_xmlnr_t *xnr, IN tt_xnode_t *xn)
{
    tt_xmlrender_t *xr = &xnr->xr;
    tt_u32_t n;
    tt_xnode_t *c;

    n = tt_xnode_child_num(xn);
    if (n == 0) {
        // <tag a1="v1"/>
        TT_DO(tt_xmlrender_stag(xr, xn->name));
        TT_DO(__render_attrlist(xr, xn));
        TT_DO(tt_xmlrender_stag_end(xr));
    } else if ((n == 1) &&
               ((c = tt_xnode_first_child(xn))->type == TT_XNODE_TYPE_TEXT)) {
        // <tag a1="v1">text</tag>
        TT_DO(tt_xmlrender_stag(xr, xn->name));
        TT_DO(__render_attrlist(xr, xn));
        TT_DO(tt_xmlrender_stag_complete(xr, TT_FALSE));

        TT_DO(tt_xmlrender_text(xr, c->value));

        TT_DO(tt_xmlrender_etag(xr, xn->name));
    } else {
        // <tag a1="v1">
        //   text
        // </tag>
        TT_DO(tt_xmlrender_stag(xr, xn->name));
        TT_DO(__render_attrlist(xr, xn));
        TT_DO(tt_xmlrender_stag_complete(xr, TT_TRUE));

        c = tt_xnode_first_child(xn);
        while (c != NULL) {
            TT_DO(tt_xmlnr_render(xnr, c));
            c = tt_xnode_next(c);
        }

        TT_DO(tt_xmlrender_etag(xr, xn->name));
    }

    return TT_SUCCESS;
}

tt_result_t __render_attrlist(IN tt_xmlrender_t *xr, IN tt_xnode_t *xn)
{
    tt_xnode_t *attr = tt_xnode_first_attr(xn);
    while (attr != NULL) {
        TT_DO(tt_xmlrender_attr(xr, attr->name, attr->value));
        attr = tt_xnode_next(attr);
    }
    return TT_SUCCESS;
}
