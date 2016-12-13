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
@file tt_adns_question.h
@brief adns question

this file defines adns question
*/

#ifndef __TT_ADNS_QUESTION__
#define __TT_ADNS_QUESTION__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/adns/tt_adns_rr.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_adns_quest_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern struct tt_adns_rr_s *tt_adns_quest_create(IN const tt_char_t *name,
                                                 IN tt_u32_t name_ownership,
                                                 IN tt_adns_rr_type_t type);

extern tt_result_t tt_adns_quest_render_prepare(IN struct tt_adns_rr_s *rr,
                                                OUT tt_u32_t *len);

extern tt_result_t tt_adns_quest_render(IN struct tt_adns_rr_s *rr,
                                        OUT struct tt_buf_s *buf);

extern tt_result_t tt_adns_quest_parse_prepare(IN struct tt_buf_s *buf);

extern struct tt_adns_rr_s *tt_adns_quest_parse(IN struct tt_buf_s *buf,
                                                IN tt_u8_t *pkt,
                                                IN tt_u32_t pkt_len);

#endif /* __TT_ADNS_QUESTION__ */
