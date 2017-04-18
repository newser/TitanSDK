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
@file tt_xml_char_decode.h
@brief xml char decoding

this file defines xml char decoding
*/

#ifndef __TT_XML_CHAR_DECODE__
#define __TT_XML_CHAR_DECODE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_xmlmem_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// - s_len will be recalculated if it is longer than length,
// - returned string should be freed by tt_xm_free
extern tt_char_t *tt_xml_chdec_len(IN struct tt_xmlmem_s *xm,
                                   IN const tt_char_t *s,
                                   IN tt_u32_t s_len);

tt_inline tt_char_t *tt_xml_chdec(IN struct tt_xmlmem_s *xm,
                                  IN const tt_char_t *s)
{
    return tt_xml_chdec_len(xm, s, (tt_u32_t)tt_strlen(s));
}

#endif /* __TT_XML_CHAR_DECODE__ */
