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

#ifdef TT_PLATFORM_SSL_ENABLE

#include <schannel.h>

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

HMODULE tt_g_security_dll = NULL;

tt_InitSecurityInterface_t tt_g_pfn_InitSecurityInterfaceW = NULL;

PSecurityFunctionTable tt_g_sspi = NULL;

PSecPkgInfoW tt_g_sspi_pkg_info = NULL;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_ssl_component_init_ntv()
{
    SECURITY_STATUS sec_st;

    tt_g_security_dll = LoadLibraryW(L"security.dll");
    if (tt_g_security_dll == NULL) {
        TT_ERROR("fail to load security.dll");
        return TT_FAIL;
    }

    tt_g_pfn_InitSecurityInterfaceW =
        (tt_InitSecurityInterface_t)GetProcAddress(tt_g_security_dll,
                                                   "InitSecurityInterfaceW");
    if (tt_g_pfn_InitSecurityInterfaceW == NULL) {
        TT_ERROR("fail to get InitSecurityInterfaceW");
        return TT_FAIL;
    }

    tt_g_sspi = tt_g_pfn_InitSecurityInterfaceW();
    if (tt_g_sspi == NULL) {
        TT_ERROR("InitSecurityInterface fail");
        return TT_FAIL;
    }

    sec_st =
        tt_g_sspi->QuerySecurityPackageInfoW(UNISP_NAME_W, &tt_g_sspi_pkg_info);
    if (sec_st != SEC_E_OK) {
        TT_ERROR("fail to get package info");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

#endif
