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
@file tt_der_decode.h
@brief asn1 der decode

this file defines asn1 der decode APIs
*/

#ifndef __TT_DER_DECODE__
#define __TT_DER_DECODE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_DER_PASS_CONTENT (1 << 0)

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

extern tt_result_t tt_der_decode_head(IN struct tt_buf_s *buf,
                                      OUT OPT tt_u32_t *tag,
                                      OUT OPT tt_u32_t *length,
                                      IN tt_u32_t flag);

extern tt_result_t tt_der_decode_sequence(IN struct tt_buf_s *buf,
                                          OUT OPT tt_u32_t *length,
                                          IN tt_u32_t flag);

extern tt_result_t tt_der_decode_oid(IN struct tt_buf_s *buf,
                                     OUT OPT tt_u8_t **oid,
                                     OUT OPT tt_u32_t *oid_len,
                                     IN tt_u32_t flag);

extern tt_result_t tt_der_decode_octstr(IN struct tt_buf_s *buf,
                                        OUT OPT tt_u8_t **octstr,
                                        OUT OPT tt_u32_t *octstr_len,
                                        IN tt_u32_t flag);

extern tt_result_t tt_der_decode_bitstr(IN struct tt_buf_s *buf,
                                        OUT OPT tt_u8_t **bitstr,
                                        OUT OPT tt_u32_t *bitstr_len,
                                        OUT OPT tt_u32_t *pad_bit_num,
                                        IN tt_u32_t flag);

extern tt_result_t tt_der_decode_s32(IN struct tt_buf_s *buf,
                                     OUT OPT tt_u32_t *length,
                                     OUT OPT tt_s32_t *val_s32,
                                     IN tt_u32_t flag);

extern tt_result_t tt_der_decode_null(IN struct tt_buf_s *buf,
                                      IN tt_u32_t flag);

#endif /* __TT_DER_DECODE__ */
