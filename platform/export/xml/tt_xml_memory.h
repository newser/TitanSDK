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
@file tt_xml_memory.h
@brief xml memory

this file defines xml memory api
*/

#ifndef __TT_XML_MEMORY__
#define __TT_XML_MEMORY__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <memory/tt_memory_alloc.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_u32_t reserved;
} tt_xmlmem_attr_t;

typedef struct tt_xmlmem_s
{
    tt_u32_t reserved;
} tt_xmlmem_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_xmlmem_create(IN tt_xmlmem_t *xm,
                                    IN OPT tt_xmlmem_attr_t *attr);

extern void tt_xmlmem_destroy(IN tt_xmlmem_t *xm);

extern void tt_xmlmem_attr_default(IN tt_xmlmem_attr_t *attr);

// if xm is NULL, use default mem allocator
tt_inline void *tt_xm_alloc(IN OPT tt_xmlmem_t *xm, IN tt_u32_t size)
{
    return tt_malloc(size);
}

tt_inline void tt_xm_free(IN void *p)
{
    tt_free(p);
}

tt_inline tt_xmlmem_t *tt_xm_xmlmem(IN void *p)
{
    return NULL;
}

extern tt_char_t *tt_xm_copycstr(IN OPT tt_xmlmem_t *xm,
                                 IN const tt_char_t *cstr);

#endif /* __TT_XML_MEMORY__ */
