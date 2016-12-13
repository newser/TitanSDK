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

#include <init/tt_version_info.h>
#include <misc/tt_distinguished_name.h>
#include <unit_test/tt_unit_test.h>

#include <event/tt_event_center.h>
#include <io/tt_socket_addr.h>
#include <io/tt_socket_aio.h>
#include <network/ssl/tt_ssl_context.h>

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

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_cert_pkcs12)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_ca_x509)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_ca_pkcs7)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_ca_pkcs12)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_si_verify)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_nsi_verify)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_chain_verify)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_basic_cli)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssl_basic_svr)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(sslctx_case)

#if 1
TT_TEST_CASE("tt_unit_test_ssl_cert_pkcs12",
             "ssl ctx use pkcs12 certificate",
             tt_unit_test_ssl_cert_pkcs12,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,
#endif

#if 1
    TT_TEST_CASE("tt_unit_test_ssl_ca_x509",
                 "ssl ctx use x509 ca",
                 tt_unit_test_ssl_ca_x509,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_ssl_ca_pkcs7",
                 "ssl ctx use pkcs7 ca",
                 tt_unit_test_ssl_ca_pkcs7,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_ssl_ca_pkcs12",
                 "ssl ctx use pkcs12 ca",
                 tt_unit_test_ssl_ca_pkcs12,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

#if 1
    TT_TEST_CASE("tt_unit_test_ssl_si_verify",
                 "ssl ctx cert verify, with self signed ca",
                 tt_unit_test_ssl_si_verify,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
    TT_TEST_CASE("tt_unit_test_ssl_nsi_verify",
                 "ssl ctx cert verify, with non-self signed ca",
                 tt_unit_test_ssl_nsi_verify,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
    TT_TEST_CASE("tt_unit_test_ssl_chain_verify",
                 "ssl ctx chain cert verify, with chain ca",
                 tt_unit_test_ssl_chain_verify,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

#if 0
    TT_TEST_CASE("tt_unit_test_ssl_basic_cli",
                 "ssl ctx basic client", 
                 tt_unit_test_ssl_basic_cli, NULL, 
                 NULL, NULL,
                 NULL, NULL),
#endif

#if 0
    TT_TEST_CASE("tt_unit_test_ssl_basic_svr",
                 "ssl ctx basic server",
                 tt_unit_test_ssl_basic_svr, NULL,
                 NULL, NULL,
                 NULL, NULL),
#endif

    TT_TEST_CASE_LIST_DEFINE_END(sslctx_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(SSL_UT_CTX, 0, sslctx_case)

#if TT_ENV_OS_IS_IOS
        extern int use_cert_verify_date;
extern double cert_verify_date;
#endif

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

/*
TT_TEST_ROUTINE_DEFINE(name)
{
    //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    TT_TEST_CASE_ENTER()
    // test start

    // test end
    TT_TEST_CASE_LEAVE()
}
*/

// private keys
#define ____IMPORT_KEY_PKCS8
#include "tt_ssl_ut_privkey_pkcs8.h"

// x509
#define ____IMPORT_CERT_X509
#include "tt_ssl_ut_cert_x509.h"
// PKCS7
#define ____IMPORT_CERT_PKCS7
#include "tt_ssl_ut_cert_pkcs7.h"
// PKCS12
#define ____IMPORT_CERT_PKCS12
#include "tt_ssl_ut_cert_pkcs12.h"

// ========================================
// use certificate
// ========================================

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_cert_pkcs12)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sslctx_t sc;
    tt_result_t ret;
    tt_blob_t cert;
    tt_ssl_cert_attr_t cert_attr;
    tt_distname_t dn;

    TT_TEST_CASE_ENTER()
    // test start

    tt_distname_init(&dn);

    ret = tt_sslctx_create(&sc, TT_SSL_ROLE_CLIENT, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // pkcs12 cert
    cert.addr = __s_cert_111_11_1_p12_enc;
    cert.len = sizeof(__s_cert_111_11_1_p12_enc);
    cert_attr.pem_armor = TT_FALSE;
    cert_attr.encrypted = TT_TRUE;
    cert_attr.password = __s_pwd;

    ret = tt_sslctx_add_cert(&sc,
                             TT_SSL_CERT_FMT_PKCS12,
                             &cert,
                             &cert_attr,
                             TT_SSL_PRIVKEY_FMT_NONE,
                             NULL,
                             NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    ret = tt_sslctx_commit(&sc, &dn, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_sslctx_destroy(&sc);

    // test end
    TT_TEST_CASE_LEAVE()
}

// ========================================
// add certificate auth
// ========================================

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_ca_x509)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sslctx_t sc;
    tt_result_t ret;
    tt_blob_t cert;
    tt_ssl_cert_attr_t cert_attr;
    tt_distname_t dn;

    TT_TEST_CASE_ENTER()
    // test start

    tt_distname_init(&dn);

    ret = tt_sslctx_create(&sc, TT_SSL_ROLE_CLIENT, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // X509 der
    cert.addr = __s_cert_1_x509_der;
    cert.len = sizeof(__s_cert_1_x509_der);
    cert_attr.pem_armor = TT_FALSE;
    cert_attr.encrypted = TT_FALSE;

    ret = tt_sslctx_add_ca(&sc, TT_SSL_CERT_FMT_X509, &cert, &cert_attr);
    TT_TEST_CHECK_SUCCESS(ret, "");

    ret = tt_sslctx_commit(&sc, &dn, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

#if 1
    // 2 X509 pem
    cert.addr = __s_cert_111_11_x509_pem;
    cert.len = sizeof(__s_cert_111_11_x509_pem);
    cert_attr.pem_armor = TT_TRUE;
    cert_attr.encrypted = TT_FALSE;

    ret = tt_sslctx_add_ca(&sc, TT_SSL_CERT_FMT_X509, &cert, &cert_attr);
    TT_TEST_CHECK_SUCCESS(ret, "");

    ret = tt_sslctx_commit(&sc, &dn, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#endif

    tt_sslctx_destroy(&sc);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_ca_pkcs7)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sslctx_t sc;
    tt_result_t ret;
    tt_blob_t cert;
    tt_ssl_cert_attr_t cert_attr;
    tt_distname_t dn;

    TT_TEST_CASE_ENTER()
    // test start

    tt_distname_init(&dn);

    // pkcs7 pem cert
    ret = tt_sslctx_create(&sc, TT_SSL_ROLE_CLIENT, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    cert.addr = __s_ca_11_1_p7_pem;
    cert.len = sizeof(__s_ca_11_1_p7_pem);
    cert_attr.pem_armor = TT_TRUE;
    cert_attr.encrypted = TT_FALSE;
    cert_attr.password = NULL;

    ret = tt_sslctx_add_ca(&sc, TT_SSL_CERT_FMT_PKCS7, &cert, &cert_attr);
#if TT_ENV_OS_IS_IOS
    // ios does not support pkcs7
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");
#else
    TT_TEST_CHECK_SUCCESS(ret, "");
#endif

    ret = tt_sslctx_commit(&sc, &dn, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_sslctx_destroy(&sc);


    ret = tt_sslctx_create(&sc, TT_SSL_ROLE_CLIENT, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // pkcs7 der cert
    cert.addr = __s_ca_11_1_p7_der;
    cert.len = sizeof(__s_ca_11_1_p7_der);
    cert_attr.pem_armor = TT_FALSE;
    cert_attr.encrypted = TT_FALSE;
    cert_attr.password = NULL;

    ret = tt_sslctx_add_ca(&sc, TT_SSL_CERT_FMT_PKCS7, &cert, &cert_attr);
#if TT_ENV_OS_IS_IOS
    // ios does not support pkcs7
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");
#else
    TT_TEST_CHECK_SUCCESS(ret, "");
#endif

    ret = tt_sslctx_commit(&sc, &dn, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_sslctx_destroy(&sc);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_ca_pkcs12)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sslctx_t sc;
    tt_result_t ret;
    tt_blob_t cert;
    tt_ssl_cert_attr_t cert_attr;
    tt_distname_t dn;

    TT_TEST_CASE_ENTER()
    // test start

    tt_distname_init(&dn);

    ret = tt_sslctx_create(&sc, TT_SSL_ROLE_CLIENT, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // pkcs12 cert
    cert.addr = __s_cert_111_11_1_p12_enc;
    cert.len = sizeof(__s_cert_111_11_1_p12_enc);
    cert_attr.pem_armor = TT_FALSE;
    cert_attr.encrypted = TT_TRUE;
    cert_attr.password = __s_pwd;

    ret = tt_sslctx_add_ca(&sc, TT_SSL_CERT_FMT_PKCS12, &cert, &cert_attr);
    TT_TEST_CHECK_SUCCESS(ret, "");

    ret = tt_sslctx_commit(&sc, &dn, 0);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_sslctx_destroy(&sc);

    // test end
    TT_TEST_CASE_LEAVE()
}

// ========================================
// verify certificate
// ========================================

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_si_verify)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sslctx_t sc;
    tt_result_t ret;
    tt_blob_t cert;
    tt_ssl_cert_attr_t cert_attr;
    tt_distname_t dn;
    tt_ssl_verify_t ssl_verify;

    TT_TEST_CASE_ENTER()
    // test start

    tt_distname_init(&dn);

    ret = tt_sslctx_create(&sc, TT_SSL_ROLE_CLIENT, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // X509 der
    cert.addr = __s_cert_1_x509_der;
    cert.len = sizeof(__s_cert_1_x509_der);
    cert_attr.pem_armor = TT_FALSE;
    cert_attr.encrypted = TT_FALSE;

    ret = tt_sslctx_add_ca(&sc, TT_SSL_CERT_FMT_X509, &cert, &cert_attr);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_ssl_verify_init(&ssl_verify);
    ssl_verify.allow_expired_cert = TT_TRUE;
    tt_sslctx_set_verify(&sc, &ssl_verify);
#if TT_ENV_OS_IS_IOS
    use_cert_verify_date = 0;
    if (sc.verify.allow_expired_cert) {
        use_cert_verify_date = 1;
        cert_verify_date = 454751541.94778; // 2015-05-31
    }
#endif

    ret = tt_sslctx_commit(&sc, &dn, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

#if 1
    ////////////////////////////////////////
    // verify itself, should ok or fail???
    ////////////////////////////////////////
    cert.addr = __s_cert_1_x509_der;
    cert.len = sizeof(__s_cert_1_x509_der);
    cert_attr.pem_armor = TT_FALSE;
    cert_attr.encrypted = TT_FALSE;
    ret = tt_sslctx_verify(&sc, TT_SSL_CERT_FMT_X509, &cert, &cert_attr, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
// TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");
#endif

#if 1
    ////////////////////////////////////////
    // directly signed by it
    ////////////////////////////////////////
    cert.addr = __s_cert_11_x509_der;
    cert.len = sizeof(__s_cert_11_x509_der);
    cert_attr.pem_armor = TT_FALSE;
    cert_attr.encrypted = TT_FALSE;
    ret = tt_sslctx_verify(&sc, TT_SSL_CERT_FMT_X509, &cert, &cert_attr, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#endif

#if 1
    ////////////////////////////////////////
    // indirectly signed by it
    ////////////////////////////////////////
    cert.addr = __s_cert_111_11_x509_pem;
    cert.len = sizeof(__s_cert_111_11_x509_pem);
    cert_attr.pem_armor = TT_TRUE;
    cert_attr.encrypted = TT_FALSE;
    ret = tt_sslctx_verify(&sc, TT_SSL_CERT_FMT_X509, &cert, &cert_attr, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#endif

#if 1
    ////////////////////////////////////////
    // not signed by it
    ////////////////////////////////////////
    cert.addr = __s_cert_4_x509_pem;
    cert.len = sizeof(__s_cert_4_x509_pem);
    cert_attr.pem_armor = TT_TRUE;
    cert_attr.encrypted = TT_FALSE;
    ret = tt_sslctx_verify(&sc, TT_SSL_CERT_FMT_X509, &cert, &cert_attr, NULL);
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");
#endif

    tt_sslctx_destroy(&sc);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_nsi_verify)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sslctx_t sc;
    tt_result_t ret;
    tt_blob_t cert;
    tt_ssl_cert_attr_t cert_attr;
    tt_distname_t dn;
    tt_ssl_verify_t ssl_verify;

    TT_TEST_CASE_ENTER()
    // test start

    tt_distname_init(&dn);

    ret = tt_sslctx_create(&sc, TT_SSL_ROLE_CLIENT, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // X509 der
    cert.addr = __s_cert_11_x509_der;
    cert.len = sizeof(__s_cert_11_x509_der);
    cert_attr.pem_armor = TT_FALSE;
    cert_attr.encrypted = TT_FALSE;

    ret = tt_sslctx_add_ca(&sc, TT_SSL_CERT_FMT_X509, &cert, &cert_attr);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_ssl_verify_init(&ssl_verify);
    ssl_verify.allow_expired_cert = TT_TRUE;
    tt_sslctx_set_verify(&sc, &ssl_verify);
#if TT_ENV_OS_IS_IOS
    use_cert_verify_date = 0;
    if (sc.verify.allow_expired_cert) {
        use_cert_verify_date = 1;
        cert_verify_date = 454751541.94778; // 2015-05-31
    }
#endif

    ret = tt_sslctx_commit(&sc, &dn, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

#if 1
    ////////////////////////////////////////
    // verify itself, should ok or fail???
    ////////////////////////////////////////
    cert.addr = __s_cert_11_x509_der;
    cert.len = sizeof(__s_cert_11_x509_der);
    cert_attr.pem_armor = TT_FALSE;
    cert_attr.encrypted = TT_FALSE;
    ret = tt_sslctx_verify(&sc, TT_SSL_CERT_FMT_X509, &cert, &cert_attr, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
// TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");
#endif

#if 1
    ////////////////////////////////////////
    // directly signed by it
    ////////////////////////////////////////
    cert.addr = __s_cert_111_x509_pem;
    cert.len = sizeof(__s_cert_111_x509_pem);
    cert_attr.pem_armor = TT_TRUE;
    cert_attr.encrypted = TT_FALSE;
    ret = tt_sslctx_verify(&sc, TT_SSL_CERT_FMT_X509, &cert, &cert_attr, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#endif

#if 1
    ////////////////////////////////////////
    // indirectly signed by it
    ////////////////////////////////////////
    cert.addr = __s_cert_4_111_x509_pem;
    cert.len = sizeof(__s_cert_4_111_x509_pem);
    cert_attr.pem_armor = TT_TRUE;
    cert_attr.encrypted = TT_FALSE;
    ret = tt_sslctx_verify(&sc, TT_SSL_CERT_FMT_X509, &cert, &cert_attr, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#endif

#if 1
    ////////////////////////////////////////
    // not signed by it
    ////////////////////////////////////////
    cert.addr = __s_cert_b_1_x509_pem;
    cert.len = sizeof(__s_cert_b_1_x509_pem);
    cert_attr.pem_armor = TT_TRUE;
    cert_attr.encrypted = TT_FALSE;
    ret = tt_sslctx_verify(&sc, TT_SSL_CERT_FMT_X509, &cert, &cert_attr, NULL);
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");
#endif

    tt_sslctx_destroy(&sc);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssl_chain_verify)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_sslctx_t sc;
    tt_result_t ret;
    tt_blob_t cert;
    tt_ssl_cert_attr_t cert_attr;
    tt_distname_t dn;
    tt_ssl_verify_t ssl_verify;

    TT_TEST_CASE_ENTER()
// test start

#if 0
    do
    {
        extern void __test_sec_trust(tt_blob_t *ca, tt_u32_t ca_num,
                                     tt_blob_t *cert, tt_u32_t cert_num);
        
        tt_blob_t ca[3], cert[3];
        
        ca[0].addr = __s_cert_1_x509_der;
        ca[0].len = sizeof(__s_cert_1_x509_der);
        cert[0].addr = __s_cert_11_x509_der;
        cert[0].len = sizeof(__s_cert_11_x509_der);
        __test_sec_trust(ca, 1, cert, 1);
        
        ca[0].addr = __s_cert_11_x509_der;
        ca[0].len = sizeof(__s_cert_11_x509_der);
        cert[0].addr = __s_cert_111_x509_der;
        cert[0].len = sizeof(__s_cert_111_x509_der);
        __test_sec_trust(ca, 1, cert, 1);
        
        ca[0].addr = __s_cert_11_x509_der;
        ca[0].len = sizeof(__s_cert_11_x509_der);
        ca[1].addr = __s_cert_1_x509_der;
        ca[1].len = sizeof(__s_cert_1_x509_der);
        cert[0].addr = __s_cert_111_x509_der;
        cert[0].len = sizeof(__s_cert_111_x509_der);
        cert[1].addr = __s_cert_11_x509_der;
        cert[1].len = sizeof(__s_cert_11_x509_der);
        cert[2].addr = __s_cert_1_x509_der;
        cert[2].len = sizeof(__s_cert_1_x509_der);
        __test_sec_trust(ca, 0, cert, 3);
    } while (0);
#endif

    tt_distname_init(&dn);

    ret = tt_sslctx_create(&sc, TT_SSL_ROLE_CLIENT, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ////////////////////////////////////////
    // self signed chain
    ////////////////////////////////////////

    // X509 der
    cert.addr = __s_cert_11_1_x509_pem;
    cert.len = sizeof(__s_cert_11_1_x509_pem);
    cert_attr.pem_armor = TT_TRUE;
    cert_attr.encrypted = TT_FALSE;

    ret = tt_sslctx_add_ca(&sc, TT_SSL_CERT_FMT_X509, &cert, &cert_attr);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_ssl_verify_init(&ssl_verify);
    ssl_verify.allow_expired_cert = TT_TRUE;
    tt_sslctx_set_verify(&sc, &ssl_verify);
#if TT_ENV_OS_IS_IOS
    use_cert_verify_date = 0;
    if (sc.verify.allow_expired_cert) {
        use_cert_verify_date = 1;
        cert_verify_date = 454751541.94778; // 2015-05-31
    }
#endif

    ret = tt_sslctx_commit(&sc, &dn, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

#if 1
    ////////////////////////////////////////
    // verify itself, should ok or fail???
    ////////////////////////////////////////
    cert.addr = __s_cert_11_1_x509_pem;
    cert.len = sizeof(__s_cert_11_1_x509_pem);
    cert_attr.pem_armor = TT_TRUE;
    cert_attr.encrypted = TT_TRUE;
    ret = tt_sslctx_verify(&sc, TT_SSL_CERT_FMT_X509, &cert, &cert_attr, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#endif

#if 1
    ////////////////////////////////////////
    // directly signed by it
    ////////////////////////////////////////
    cert.addr = __s_cert_111_x509_pem;
    cert.len = sizeof(__s_cert_111_x509_pem);
    cert_attr.pem_armor = TT_TRUE;
    cert_attr.encrypted = TT_FALSE;
    ret = tt_sslctx_verify(&sc, TT_SSL_CERT_FMT_X509, &cert, &cert_attr, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#endif

#if 1
    ////////////////////////////////////////
    // indirectly signed by it
    ////////////////////////////////////////
    cert.addr = __s_cert_4_111_x509_pem;
    cert.len = sizeof(__s_cert_4_111_x509_pem);
    cert_attr.pem_armor = TT_TRUE;
    cert_attr.encrypted = TT_FALSE;
    ret = tt_sslctx_verify(&sc, TT_SSL_CERT_FMT_X509, &cert, &cert_attr, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#endif

#if 1
    ////////////////////////////////////////
    // not signed by it
    ////////////////////////////////////////
    cert.addr = __s_cert_b_1_x509_pem;
    cert.len = sizeof(__s_cert_b_1_x509_pem);
    cert_attr.pem_armor = TT_TRUE;
    cert_attr.encrypted = TT_FALSE;
    ret = tt_sslctx_verify(&sc, TT_SSL_CERT_FMT_X509, &cert, &cert_attr, NULL);
    TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");
#endif

    tt_sslctx_destroy(&sc);

    // test end
    TT_TEST_CASE_LEAVE()
}
