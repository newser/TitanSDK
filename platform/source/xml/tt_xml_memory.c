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

#include <xml/tt_xml_memory.h>

#include <misc/tt_util.h>

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

tt_result_t tt_xmlmem_create(IN tt_xmlmem_t *xm, IN OPT tt_xmlmem_attr_t *attr)
{
    TT_ASSERT(xm != NULL);

    // todo
    tt_memset(xm, 0, sizeof(tt_xmlmem_t));

    return TT_SUCCESS;
}

void tt_xmlmem_destroy(IN tt_xmlmem_t *xm)
{
    TT_ASSERT(xm != NULL);

    // todo
}

void tt_xmlmem_attr_default(IN tt_xmlmem_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_memset(attr, 0, sizeof(tt_xmlmem_attr_t));
}
