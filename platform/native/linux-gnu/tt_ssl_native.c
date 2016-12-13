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

#include <tt_ssl_native.h>

#include <log/tt_log.h>

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

int tt_g_x509_store_ctx_private_idx;

int tt_g_ssl_private_idx;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_ssl_component_init_ntv()
{
#ifdef TT_PLATFORM_SSL_ENABLE
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    tt_g_x509_store_ctx_private_idx =
        X509_STORE_CTX_get_ex_new_index(0, NULL, NULL, NULL, NULL);
    if (tt_g_x509_store_ctx_private_idx == -1) {
        TT_ERROR("fail to new X509_STORE_CTX private index");
        return TT_FAIL;
    }

    tt_g_ssl_private_idx = SSL_get_ex_new_index(0, NULL, NULL, NULL, NULL);
    if (tt_g_ssl_private_idx == -1) {
        TT_ERROR("fail to new ssl private index");
        return TT_FAIL;
    }
#endif

    return TT_SUCCESS;
}
