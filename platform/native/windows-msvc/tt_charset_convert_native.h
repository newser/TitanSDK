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

#ifndef __TT_CHARSET_CONVERT_NATIVE__
#define __TT_CHARSET_CONVERT_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>
#include <misc/tt_charset_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_chsetconv_s;

typedef struct
{
    tt_buf_t intermediate;
} tt_chsetconv_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_chsetconv_create_ntv(IN struct tt_chsetconv_s *csconv);

extern void tt_chsetconv_destroy_ntv(IN struct tt_chsetconv_s *csconv);

extern tt_result_t tt_chsetconv_input_ntv(IN struct tt_chsetconv_s *csconv,
                                          IN tt_u8_t *input,
                                          IN tt_u32_t input_len);

extern void tt_chsetconv_reset_ntv(IN struct tt_chsetconv_s *csconv);

#endif /* __TT_CHARSET_CONVERT_NATIVE__ */
