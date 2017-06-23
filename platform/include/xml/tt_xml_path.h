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
@file tt_xml_path.h
@brief xml path

this file specifies xml path APIs
*/

#ifndef __TT_XML_PATH__
#define __TT_XML_PATH__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_xnode_s;
struct tt_xattr_s;

// ========================================
// xml path
// ========================================

typedef struct tt_xpath_s
{
    tt_u8_t p[sizeof(tt_ptr_t) * 3];
} tt_xpath_t;

// ========================================
// xml path variable set
// ========================================

typedef struct tt_xpvars_s
{
    tt_u8_t p[sizeof(tt_ptr_t) * 64];
} tt_xpvars_t;

// ========================================
// xml path node set
// ========================================

typedef struct tt_xpnodes_s
{
    tt_ptr_t p[sizeof(tt_ptr_t) * 4];
} tt_xpnodes_t;

typedef struct
{
    tt_xpnodes_t *xpns;
    const void *p;
} tt_xpnodes_iter_t;

typedef enum {
    TT_XPNODES_UNSORTED,
    TT_XPNODES_SORTED,
    TT_XPNODES_SORTED_REVERSE,

    TT_XPNODES_SORT_NUM
} tt_xpnodes_sort_t;
#define TT_XPNODES_SORT_VALID(s) ((s) < TT_XPNODES_SORT_NUM)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_xpath_component_register();

// ========================================
// xml path
// ========================================

extern tt_result_t tt_xpath_create(IN tt_xpath_t *xp,
                                   IN const tt_char_t *expr,
                                   IN OPT tt_xpvars_t *xpvs);

extern void tt_xpath_destroy(IN tt_xpath_t *xp);

// ========================================
// xml path variable set
// ========================================

extern void tt_xpvars_init(IN tt_xpvars_t *xpvs);

extern void tt_xpvars_destroy(IN tt_xpvars_t *xpvs);

extern tt_result_t tt_xpvars_set_bool(IN tt_xpvars_t *xpvs,
                                      IN const tt_char_t *name,
                                      IN tt_bool_t val);

extern tt_result_t tt_xpvars_set_number(IN tt_xpvars_t *xpvs,
                                        IN const tt_char_t *name,
                                        IN tt_double_t val);

extern tt_result_t tt_xpvars_set_cstr(IN tt_xpvars_t *xpvs,
                                      IN const tt_char_t *name,
                                      IN const tt_char_t *val);

// ========================================
// xml path node set
// ========================================

extern void tt_xpnodes_init(IN tt_xpnodes_t *xpns);

extern void tt_xpnodes_destroy(IN tt_xpnodes_t *xpns);

extern void tt_xpnodes_sort(IN tt_xpnodes_t *xpns, IN tt_bool_t reverse);

extern tt_u32_t tt_xpnodes_count(IN tt_xpnodes_t *xpns);

extern void tt_xpnodes_iter(IN tt_xpnodes_t *xpns, OUT tt_xpnodes_iter_t *iter);

extern tt_result_t tt_xpnodes_iter_next(IN OUT tt_xpnodes_iter_t *iter,
                                        OUT tt_ptr_t *xn,
                                        OUT tt_ptr_t *xa);

#endif /* __TT_XML_PATH__ */
