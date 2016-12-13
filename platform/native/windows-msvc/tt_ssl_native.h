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
@file tt_ssl_native.h
@brief ts ssl native

this file defines ts ssl native
*/

#ifndef __TT_SSL_NATIVE__
#define __TT_SSL_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#ifdef TT_PLATFORM_SSL_ENABLE
// required by sspi.h
#ifdef SECURITY_KERNEL
#undef SECURITY_KERNEL
#endif
#define SECURITY_WIN32
#include <sspi.h>
#endif

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
#ifdef TT_PLATFORM_SSL_ENABLE
    CtxtHandle hctx;
    ULONG pfContextAttr;
    SecPkgContext_StreamSizes stream_size;
#endif

    wchar_t *peer_id;
    tt_u32_t peer_id_len;

    // can not use tt_buf_t, we need 3 info: data, len, plain_data
    tt_blob_t input;
    tt_u32_t input_pos;
    tt_u32_t input_plain_len;
    tt_blob_t output;
    tt_u32_t output_data_len;
    tt_u32_t output_pos;
    tt_u32_t extra_len;
} tt_ssl_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

#ifdef TT_PLATFORM_SSL_ENABLE
extern HMODULE tt_g_security_dll;

typedef PSecurityFunctionTable(APIENTRY *tt_InitSecurityInterface_t)();
extern tt_InitSecurityInterface_t tt_g_pfn_InitSecurityInterfaceW;

extern PSecurityFunctionTable tt_g_sspi;

extern PSecPkgInfoW tt_g_sspi_pkg_info;
#endif

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#ifdef TT_PLATFORM_SSL_ENABLE
extern tt_result_t tt_ssl_component_init_ntv();
#else
tt_inline tt_result_t tt_ssl_component_init_ntv()
{
    return TT_SUCCESS;
}
#endif

tt_inline tt_bool_t tt_ssl_enabled_ntv()
{
#ifdef TT_PLATFORM_SSL_ENABLE
    return TT_TRUE;
#else
    return TT_FALSE;
#endif
}

#endif /* __TT_SSL_NATIVE__ */
