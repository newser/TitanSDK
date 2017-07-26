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
@file tt_der_encode.h
@brief asn1 der encode

this file defines asn1 der encode APIs
*/

#ifndef __TT_DER_ENCODE__
#define __TT_DER_ENCODE__

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

struct tt_buf_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_der_encode_head(IN struct tt_buf_s *buf,
                                         IN tt_u32_t tag,
                                         IN tt_u32_t length,
                                         IN tt_u32_t flag);
tt_export tt_u32_t tt_der_head_len(IN tt_u32_t tag, IN tt_u32_t length);

tt_export tt_result_t tt_der_encode_sequence(IN struct tt_buf_s *buf,
                                             IN tt_u32_t length,
                                             IN tt_u32_t flag);
tt_export tt_u32_t tt_der_sequence_len(IN tt_u32_t content_len);

tt_export tt_result_t tt_der_encode_oid(IN struct tt_buf_s *buf,
                                        IN tt_u8_t *oid,
                                        IN tt_u32_t oid_len,
                                        IN tt_u32_t flag);
tt_export tt_u32_t tt_der_oid_len(IN tt_u8_t *oid, IN tt_u32_t oid_len);
// may implement encoding oid with string or array format oid
// as input

tt_export tt_result_t tt_der_encode_octstr(IN struct tt_buf_s *buf,
                                           IN tt_u8_t *octstr,
                                           IN tt_u32_t octstr_len,
                                           IN tt_u32_t flag);
tt_export tt_u32_t tt_der_octstr_len(IN tt_u8_t *octstr,
                                     IN tt_u32_t octstr_len);

tt_export tt_result_t tt_der_encode_bitstr(IN struct tt_buf_s *buf,
                                           IN OPT tt_u8_t *bitstr,
                                           IN tt_u32_t bitstr_len,
                                           IN tt_u32_t pad_bit_num,
                                           IN tt_u32_t flag);
tt_export tt_u32_t tt_der_bitstr_len(IN tt_u8_t *bitstr,
                                     IN tt_u32_t bitstr_len,
                                     IN tt_u32_t pad_bit_num);

tt_export tt_result_t tt_der_encode_s32(IN struct tt_buf_s *buf,
                                        IN tt_s32_t val_s32,
                                        IN tt_u32_t flag);
tt_export tt_u32_t tt_der_s32_len(IN tt_s32_t val_s32);

tt_export tt_result_t tt_der_encode_null(IN struct tt_buf_s *buf,
                                         IN tt_u32_t flag);
tt_export tt_u32_t tt_der_null_len();

// the integer should be already in complement code form
tt_export tt_result_t tt_der_encode_integer(IN struct tt_buf_s *buf,
                                            IN tt_u8_t *integer,
                                            IN tt_u32_t integer_len,
                                            IN tt_u32_t flag);
tt_export tt_u32_t tt_der_integer_len(IN tt_u8_t *integer,
                                      IN tt_u32_t integer_len);

#endif /* __TT_DER_ENCODE__ */
