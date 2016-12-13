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
@file tt_xml_node.h
@brief xml node

this file defines xml node api
*/

#ifndef __TT_XML_NODE__
#define __TT_XML_NODE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>
#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_XNODE_CAST(xn, type) TT_PTR_INC(type, xn, sizeof(tt_xnode_t))
#define TT_XNODE_OF(p) TT_PTR_DEC(tt_xnode_t, p, sizeof(tt_xnode_t))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_xnode_s;
struct tt_xmlmem_s;
struct tt_xmlns_s;

typedef enum {
    TT_XNODE_TYPE_DOC,
    TT_XNODE_TYPE_ELEMENT,
    TT_XNODE_TYPE_ATTR,
    TT_XNODE_TYPE_TEXT,
    TT_XNODE_TYPE_CDATA,
    TT_XNODE_TYPE_COMMENT,
    TT_XNODE_TYPE_PI,

    TT_XNODE_TYPE_NUM
} tt_xnode_type_t;
#define TT_XNODE_TYPE_VALID(t) ((t) < TT_XNODE_TYPE_NUM)

typedef tt_result_t (*tt_xnode_create_t)(IN struct tt_xnode_s *xn);

typedef void (*tt_xnode_destroy_t)(IN struct tt_xnode_s *xn);

typedef struct
{
    tt_xnode_create_t create;
    tt_xnode_destroy_t desetroy;
} tt_xnode_itf_t;

typedef struct tt_xnode_s
{
    tt_lnode_t node;
    tt_char_t *name;
    tt_char_t *value;
    tt_xnode_type_t type;
    struct tt_xmlns_s *ns;
} tt_xnode_t;

typedef tt_xnode_t tt_xnode_text_t;
typedef tt_xnode_t tt_xnode_cdata_t;
typedef tt_xnode_t tt_xnode_comment_t;
typedef tt_xnode_t tt_xnode_attr_t;
typedef tt_xnode_t tt_xnode_pi_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_xnode_t *tt_xnode_create(IN struct tt_xmlmem_s *xm,
                                   IN tt_u32_t size,
                                   IN tt_xnode_type_t type,
                                   IN OPT TO tt_char_t *name,
                                   IN OPT TO tt_char_t *value);

tt_inline tt_xnode_t *tt_xnode_text_create(IN struct tt_xmlmem_s *xm,
                                           IN OPT TO tt_char_t *data)
{
    return tt_xnode_create(xm, 0, TT_XNODE_TYPE_TEXT, NULL, data);
}

tt_inline tt_xnode_t *tt_xnode_cdata_create(IN struct tt_xmlmem_s *xm,
                                            IN OPT TO tt_char_t *cdata)
{
    return tt_xnode_create(xm, 0, TT_XNODE_TYPE_CDATA, NULL, cdata);
}

tt_inline tt_xnode_t *tt_xnode_comment_create(IN struct tt_xmlmem_s *xm,
                                              IN OPT TO tt_char_t *comment)
{
    return tt_xnode_create(xm, 0, TT_XNODE_TYPE_COMMENT, NULL, comment);
}

tt_inline tt_xnode_t *tt_xnode_attr_create(IN struct tt_xmlmem_s *xm,
                                           IN OPT TO tt_char_t *name,
                                           IN OPT TO tt_char_t *value)
{
    return tt_xnode_create(xm, 0, TT_XNODE_TYPE_ATTR, name, value);
}

tt_inline tt_xnode_t *tt_xnode_pi_create(IN struct tt_xmlmem_s *xm,
                                         IN OPT TO tt_char_t *name,
                                         IN OPT TO tt_char_t *value)
{
    return tt_xnode_create(xm, 0, TT_XNODE_TYPE_PI, name, value);
}

extern void tt_xnode_destroy(IN tt_xnode_t *xn);

extern void tt_xnode_set_name(IN tt_xnode_t *xn, IN TO tt_char_t *name);

extern void tt_xnode_set_value(IN tt_xnode_t *xn, IN TO tt_char_t *value);

extern tt_xnode_t *tt_xnode_parent(IN tt_xnode_t *xn);

tt_inline tt_xnode_t *tt_xnode_next(IN tt_xnode_t *xn)
{
    return TT_COND(xn->node.next != NULL,
                   TT_CONTAINER(xn->node.next, tt_xnode_t, node),
                   NULL);
}

tt_inline tt_xnode_t *tt_xnode_prev(IN tt_xnode_t *xn)
{
    return TT_COND(xn->node.prev != NULL,
                   TT_CONTAINER(xn->node.prev, tt_xnode_t, node),
                   NULL);
}

extern tt_xnode_t *tt_xnode_first_child(IN tt_xnode_t *xn);

extern tt_xnode_t *tt_xnode_last_child(IN tt_xnode_t *xn);

extern tt_xnode_t *tt_xnode_first_attr(IN tt_xnode_t *xn);

extern tt_xnode_t *tt_xnode_last_attr(IN tt_xnode_t *xn);

extern tt_result_t tt_xnode_add_child(IN tt_xnode_t *xn, IN tt_xnode_t *child);

extern tt_result_t tt_xnode_add_attr(IN tt_xnode_t *xn, IN tt_xnode_t *attr);

#endif /* __TT_XML_NODE__ */
