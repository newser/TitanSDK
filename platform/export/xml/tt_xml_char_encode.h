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
@file tt_xml_char_encode.h
@brief xml char encoding

this file defines xml char encoding
*/

#ifndef __TT_XML_CHAR_ENCODE__
#define __TT_XML_CHAR_ENCODE__

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

struct tt_buf_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_xml_chenc_len(IN const tt_char_t *s,
                                    IN tt_u32_t s_len,
                                    OUT struct tt_buf_s *outbuf);

tt_inline tt_result_t tt_xml_chenc(IN const tt_char_t *s,
                                   OUT struct tt_buf_s *outbuf)
{
    return tt_xml_chenc_len(s, (tt_u32_t)tt_strlen(s), outbuf);
}

#endif /* __TT_XML_CHAR_ENCODE__ */
