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

#include <tt_md5_native.h>

#ifdef PLATFORM_CRYPTO_ENABLE

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

tt_result_t tt_md5_create_ntv(IN tt_md5_ntv_t *sys_md5)
{
    CC_MD5_Init(&sys_md5->md5);
    return TT_SUCCESS;
}

void tt_md5_destroy_ntv(IN tt_md5_ntv_t *sys_md5)
{
}

tt_result_t tt_md5_update_ntv(IN tt_md5_ntv_t *sys_md5,
                              IN tt_u8_t *input,
                              IN tt_u32_t input_len)
{
    CC_MD5_Update(&sys_md5->md5, input, input_len);
    return TT_SUCCESS;
}

tt_result_t tt_md5_final_ntv(IN tt_md5_ntv_t *sys_md5, OUT tt_u8_t *output)
{
    CC_MD5_Final(output, &sys_md5->md5);
    return TT_SUCCESS;
}

#endif
