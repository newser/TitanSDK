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

#include <tt_platform.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __X509_CA "tt_ca.crt"
#define __CA_key "tt_ca.key"

#define __X509_CA2 "tt_ca_int.crt"
#define __CA_key2 "tt_ca_int.key"

#define __X509_LEAF "tt_ca_leaf.crt"
#define __leaf_key "tt_ca_leaf.key"

#define __X509_CRL1 "tt_ca.crl"

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

static void __x509_prepare(void *);
static tt_bool_t has_x509;

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_x509_cert)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_x509_crl)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(crypto_x509_case)

TT_TEST_CASE("tt_unit_test_x509_cert",
             "ssl: x509 cert",
             tt_unit_test_x509_cert,
             NULL,
             __x509_prepare,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_x509_crl",
                 "ssl: x509 crl",
                 tt_unit_test_x509_crl,
                 NULL,
                 __x509_prepare,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(crypto_x509_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(SSL_UT_X509, 0, crypto_x509_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_x509_cert)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_x509_cert)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_x509cert_t ca, cert;
    tt_result_t ret;
    tt_u32_t status, n;
    tt_char_t buf[1000];

    TT_TEST_CASE_ENTER()
    // test start

    tt_x509cert_init(&ca);
    tt_x509cert_init(&cert);

    // root => leaf, should fail
    ret = tt_x509cert_add_file(&ca, __X509_CA);
    TT_UT_SUCCESS(ret, "");
    ret = tt_x509cert_add_file(&cert, __X509_LEAF);
    TT_UT_SUCCESS(ret, "");
    ret = tt_x509cert_verify(&cert, &ca, NULL, NULL, &status);
    TT_UT_FAIL(ret, "");
    n = tt_x509cert_dump_verify_status(status, buf, (tt_u32_t)sizeof(buf));
    TT_INFO("%s", buf);

    // root, int => leaf, ok
    ret = tt_x509cert_add_file(&ca, __X509_CA2);
    TT_UT_SUCCESS(ret, "");
    n = tt_x509cert_dump(&ca, buf, (tt_u32_t)sizeof(buf));
    TT_INFO("ca: %s", buf);
    ret = tt_x509cert_verify(&cert, &ca, NULL, NULL, &status);
    if (!TT_OK(ret)) {
        n = tt_x509cert_dump_verify_status(status, buf, (tt_u32_t)sizeof(buf));
        TT_INFO("%s", buf);
    }
    TT_UT_SUCCESS(ret, "");

    tt_x509cert_destroy(&ca);
    tt_x509cert_destroy(&cert);

    tt_x509cert_init(&ca);
    tt_x509cert_init(&cert);

    // root => int,leaf ok

    ret = tt_x509cert_add_file(&ca, __X509_CA);
    TT_UT_SUCCESS(ret, "");

    ret = tt_x509cert_add_file(&cert, __X509_LEAF);
    TT_UT_SUCCESS(ret, "");
    ret = tt_x509cert_add_file(&cert, __X509_CA2);
    TT_UT_SUCCESS(ret, "");

    ret = tt_x509cert_verify(&cert, &ca, NULL, NULL, &status);
    if (!TT_OK(ret)) {
        n = tt_x509cert_dump_verify_status(status, buf, (tt_u32_t)sizeof(buf));
        TT_INFO("%s", buf);
    }
    TT_UT_SUCCESS(ret, "");

    ret = tt_x509cert_verify(&cert, &ca, NULL, "child-2", &status);
    TT_UT_SUCCESS(ret, "");

    ret = tt_x509cert_verify(&cert, &ca, NULL, "child-20", &status);
    TT_UT_FAIL(ret, "");
    n = tt_x509cert_dump_verify_status(status, buf, (tt_u32_t)sizeof(buf));
    TT_INFO("%s", buf);

    tt_x509cert_destroy(&ca);
    tt_x509cert_destroy(&cert);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_x509_crl)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_x509cert_t ca, cert;
    tt_x509crl_t crl;
    tt_result_t ret;
    tt_u32_t status, n;
    tt_char_t buf[1000];

    TT_TEST_CASE_ENTER()
    // test start

    tt_x509cert_init(&ca);
    tt_x509cert_init(&cert);
    tt_x509crl_init(&crl);

    // root => int,leaf ok

    ret = tt_x509cert_add_file(&ca, __X509_CA);
    TT_UT_SUCCESS(ret, "");

    ret = tt_x509cert_add_file(&cert, __X509_LEAF);
    TT_UT_SUCCESS(ret, "");
    ret = tt_x509cert_add_file(&cert, __X509_CA2);
    TT_UT_SUCCESS(ret, "");

    ret = tt_x509cert_verify(&cert, &ca, NULL, NULL, &status);
    if (!TT_OK(ret)) {
        n = tt_x509cert_dump_verify_status(status, buf, (tt_u32_t)sizeof(buf));
        TT_INFO("%s", buf);
    }
    TT_UT_SUCCESS(ret, "");

    // with crl, fail
    ret = tt_x509crl_add_file(&crl, __X509_CRL1);
    TT_UT_SUCCESS(ret, "");

    ret = tt_x509cert_verify(&cert, &ca, &crl, NULL, &status);
    TT_UT_FAIL(ret, "");
    n = tt_x509cert_dump_verify_status(status, buf, (tt_u32_t)sizeof(buf));
    TT_INFO("%s", buf);

    tt_x509cert_destroy(&ca);
    tt_x509cert_destroy(&cert);
    tt_x509crl_destroy(&crl);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_char_t __ca_1[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDETCCAfmgAwIBAgIJAJD9Yn1n4y2BMA0GCSqGSIb3DQEBDQUAMA8xDTALBgNV\n"
    "BAMTBHJvb3QwHhcNMTcwNjA1MDU0OTUwWhcNMTcwNzA1MDU0OTUwWjAPMQ0wCwYD\n"
    "VQQDEwRyb290MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAszTK5Xex\n"
    "Mnb5aSZrzqjjhKsSP9CHoVoZ+hjHmDWQ+vInt95IKqSv1wi7WBq59otetAEOyikj\n"
    "3nfru2w9y6i6PfUeh71v8VidcBmzoJ3qyomiA4IeQk+2qeqdMaQg56hbj51trGDa\n"
    "t15PSkPDWN3hQoN69aF6vT8OvXTJsgwrFoXWgwtUUyLEbJG/FooPOSFBjkU4nngz\n"
    "tsCphDZrtL8FkiH44efn8wYEnXV9Z0yaFsHdW5cQ5cEtJ3m236UBPju5eqoN7b/z\n"
    "PkzyN2pt1yeZS0RIhWT9CalUsqrZLRnv+2qU06xc7mPJ6tMPT8EJBCzn9hi5sBS/\n"
    "64JCrdCbE5KhwQIDAQABo3AwbjAdBgNVHQ4EFgQUHs3MJR38tCbHnX+i8pbGqXv7\n"
    "qScwPwYDVR0jBDgwNoAUHs3MJR38tCbHnX+i8pbGqXv7qSehE6QRMA8xDTALBgNV\n"
    "BAMTBHJvb3SCCQCQ/WJ9Z+MtgTAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBDQUA\n"
    "A4IBAQCse6TXTMu0M2nW9ZwNHhGGleRp1NKanmpoMx0NFRnuqvrET5IekGY3m1K0\n"
    "tyzvM5BgcIsz5LD58yX4NtQP7E1GCZ6JsmO+cYVUXBBfHfbgtnvTdsODH5SJUhl8\n"
    "Yl6TpnRI8AjcxA4cdWvA/Yss7cjgWDVTgJvoOtTjwpQOfzVuReKfoJC3xN8nrG1D\n"
    "pQlWyvd4hEQcN1Y+3EIaYiHTDuiNpFXvoTtUNMvO8vswrjM+9D/PbnvYjsgUAciF\n"
    "CMVIEUz0L0JPeCbNH2F2EY16jK62mEGHOycuFiQjceU3QpouNL+XQ7dz0knolxaG\n"
    "hIJwzM0RDZKXIqgtIgSgJkSs7wKP\n"
    "-----END CERTIFICATE-----\n";

// pass: 123
static tt_char_t __key_1[] =
    "-----BEGIN ENCRYPTED PRIVATE KEY-----\n"
    "MIIFDjBABgkqhkiG9w0BBQ0wMzAbBgkqhkiG9w0BBQwwDgQIYbHJexJuyN4CAggA\n"
    "MBQGCCqGSIb3DQMHBAgjtuLV5+QbgASCBMilSleyzQ3lIc3u6hAJcXrfCILh5aEf\n"
    "4S0exqevEuOpnx5ltZDKjj7OXE3LTdcCyO7UH1u2nQj10CG93WIZhN0h1OG8aVGw\n"
    "bHbj/Hg19Up/ESMvn8Fk3CacVGlSnNL5EbgyLfLYhuweZ1wubmV7/nfszT1xNx+s\n"
    "+ANaAwrSsCYaL3+MlOCbhvR93WoXlwTz2O4M706T1hVZDFmYr3HR2kNydijL/E9c\n"
    "SBVnyLQyaoqNSlVWwXB4TdaRNH/tz0gjToc2t4gJOVO5DJoTEWyMfgYrpZKPjuRA\n"
    "+FWJ5EWg0WtUhW8YHd6jxNrDx4ZUqpdHyaBhC/TIMVGKCk6FU6YB6MiJFQAsknEV\n"
    "XUV6pLcjTOu7swq7j/7wa/R5izMke/miixHSCgIvetzbwpeV+RcP8oGVpRYxPf1a\n"
    "olbOudTKAIOAjPqxTlglaW7ioB3ZzuOTWMzzC8u0ku9w3ZMjn9AKgikuVmURCGjG\n"
    "EyVrtDpb+6ZI7e/4oFR4rVcgRWhtvJFve3cOdJlo/Ddh1BwKvemDm7A5/hlU2Q62\n"
    "RqwYNNcGP18bVbTVaOZkSFnUuRztJSQDzrwwQR86jEHJKV/CgQI9wxDxLL9U28HU\n"
    "Iwu02bMcWlH51iEPJwOUjoklZmr2ySppGZIQj9eqJDWIu1n1nj+itinKU85ivYHC\n"
    "KX6QmzHc2/GxzXtYnFZfoLhqRfNIo7qGp2BiKtqR65OHDvY3Bg+P1mINvMg7lCjg\n"
    "HqrlVxABufnpFLQRdUC/mpx8Z6HCgdKUAJ2W0/dcBhdSvEolumEyjxZQhZa03HKn\n"
    "yy46gcOWlI6vCYOvaq97UmCBukjhhlDR9G1UDwk38yEmjiOjGPci/CgArfwOzzqw\n"
    "cLOtebsQpYeMboccXzv2OmjQR1JVMrTyJWeZvCxBKRz0FQX6+PHNjYxfGHkNBFoB\n"
    "VZ/CUHQQzB4C7bldRnKHXhe382Be5O4O1J3TiAuCpCJcidUM4Mp73sv59JFByFsG\n"
    "r4ZV2Jq0p2bp6afGgJSzP0IXY6YlVCdNytylr/RJ+HF13m2NyxKCvvhpDAdqI/Bg\n"
    "oVDkiiHxDQtsyiZuR+EU1R2uGjeZGvsASyq33ILzua9hZsTkbUuGQULZTuzkdE1G\n"
    "qPEYHhnoEZQwkRBbY0MHn7wdYexzqbEJhjzX+VLWwl0/BEZhme8buSuGg6BoHLXE\n"
    "+jkAZERWg9ERAfOosqmNOH6xr2509QxcV7j5QT0e7UeZHMpNmgWqPRRMjHjCsKVs\n"
    "fH9Brez/KDo6wJOFimpFlBqHo+gXBBaP5RrbDfyFrSpqBRJSrMaWhm8+34evS2Rw\n"
    "CN6+7IgVNl+Y6yQyNalJCr1/SwxeyVap5kmFG/6+Kf4s0CFJD/p44J6gwurzvkHm\n"
    "5AO6de9HDKpi+mbSHCyjNJEpYVJ5eItke49WaGsriZEk9OCWIlyNG05OfHNYoirp\n"
    "4A3o3OOilFnUqn8S4788QStVmsPLiZkw4eEU4sXR5SuYqKwqEXKmT81FeklXJObd\n"
    "mrui/nAmKmxH4dnoEvr7tbiklzJfxZ08ged7XN1G3bZEMg43khioXKwjuEGmZwgP\n"
    "qRpmKI3zfjeBuvZf8f6af6tQX/sMmxX6sx0jvZ+heP1g/83W24Kj1YSRdQx/502j\n"
    "uB0=\n"
    "-----END ENCRYPTED PRIVATE KEY-----\n";

static tt_char_t __ca_2[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIICrTCCAZWgAwIBAgICR80wDQYJKoZIhvcNAQENBQAwDzENMAsGA1UEAxMEcm9v\n"
    "dDAeFw0xNzA2MDUwNjAyMjNaFw0yNzA2MDMwNjAyMjNaMBIxEDAOBgNVBAMTB2No\n"
    "aWxkLTEwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDCtg8J0u4PhqXo\n"
    "nxOoZNi+zlAej+66qZKAfgqRjJPHRNWMXDxzGk4BRuu68aJwZ9SaIEghmqCb2np5\n"
    "Di4OyQtm8URLEmOjB9l6xpQlD/LAIyCyw/B+ffOu8SR+GAOnKatpNhy/OEz84/Ew\n"
    "fNyHA89bvbleuYjzEspu3rCVKj9X0bnx+vP7Wazrt7B5pSi+PvQHaBrID4rbNDrE\n"
    "DL9sQ6xGa4qjJ8i6YtPh3CR0gNXcJlAduG55hBv7ZGilolSWBfdQ1j7k9jNoQH3P\n"
    "fi378iZ26SV+qVVzVWaIldHNx58M8PAdQYTKI/1LjbAZHiliX14PC+cz8RXp3UhN\n"
    "m9IoMYZpAgMBAAGjEDAOMAwGA1UdEwQFMAMBAf8wDQYJKoZIhvcNAQENBQADggEB\n"
    "ADptQHAhC3Y8Trppkv4F095N2+ImSFN9ON1Z6uDmeS70hFRPJSg7YMIP8pTPqlLy\n"
    "6hgBQR/M+xbdRsDpOAyWPaG67gG/4q+h1WZ6v+qSF4f5mpxQkVuo//Viasu8G4cu\n"
    "0B0zuQ6j9nSTMs0KyqsCoxXTQ1PwJjE/nZn6DPOidr5Xq0EfhUOlG0HYqZEh7aPY\n"
    "Yq7ohLC9r7C4knQ9GRUu9+LxbGzTXh404yaOpgDZ0v+zM0tTWgh7KxkCvWhMVMsW\n"
    "X4gGSR7Mno99UO3Mw1axmxALQFg6aRfLpn0SUc7Mvbxtdjkef+Hcuav443q7YxlO\n"
    "HEZbJUMEEYvrP9IkD2f2pRg=\n"
    "-----END CERTIFICATE-----\n";

// pass: 456
static tt_char_t __key_2[] =
    "-----BEGIN ENCRYPTED PRIVATE KEY-----\n"
    "MIIFDjBABgkqhkiG9w0BBQ0wMzAbBgkqhkiG9w0BBQwwDgQIjGHXDoPCT9wCAggA\n"
    "MBQGCCqGSIb3DQMHBAi9XOAWPlKfJgSCBMiXpxEOqUabC8t0xFvc5iMzHc5WgpS0\n"
    "yPhwQ2YADlGZgZH2cEYyNQ3sbtpuSbThAIRF8w07If3xAmtjXL4bt/ARN3OiktUj\n"
    "Q7P41WYlCy5D52WU0PimtFexNHhyeAydRMYKgep5YrkoyzFXMT9uGduu5yad6NHv\n"
    "mDaSuKUN5Yxk1Gdf7OH8Q9TgiESoqUssOciqfpkH9vAD7wpfr8tgr2e058RFCZoC\n"
    "yPxjZ0cOPExucg0KUG/R1kkOanzAaP4WniRMw+9Z/pADerV4tOOgAz7gFIop+8iD\n"
    "7if27QGPuqFfMhXT7x+RsVQoDEiJZiRSjHDUIqBrA5vfUCgtCIRQZ079EPmhP2xr\n"
    "Te1BepdPqSUXkxAxK8Z54L2qSf/7HVzh+G8Hou83mYpHM2ynzFU6bHRe6AF7oK4l\n"
    "BSCGPcICTI7cJ7xSH6UWKzfog8u60zKZ9GFMDmOOhryd6M1GU5qy95V1QrG5Q47S\n"
    "n+moJiC5TdAw2q7UoO5sDfn2MIMgWsDVmIIrr8hMt/p72VaJeH4fJeTYgK7Dk6XP\n"
    "tExvZa6iWg4UKkmPSw4ddWFgVjZ/SMotcgPO1B9J9zTPu/WhoYsHSZGPepB9vRJC\n"
    "INWHRrrwS5FITCUftVXJb94nmZslCUDfZyFs8DewX2V6AwQo4y77k3pu1Ja5t9fb\n"
    "kfyJ7CkygloGm4F8sKxnvtm2JMEM4TcJ9xxn/qa3e/vNLmi4LAPeSxCLe8Up1vhZ\n"
    "zNGJMLPJ/rOToTPOMJnHN9a0wc+jgwy2/b4d5s9W1Xjz0z7r1aK5ZQeXhgBgKmfn\n"
    "ai4vah54iUMOLNZ7O0v3vxo6xEii01w/DDb3FA9cqES3CCyW235wg0rpkKV7IfeV\n"
    "H71aHSZfDa8k8ytaEbjZ7uNHauqERda6u4Fai+VeqapRug9IVxXPNuPPlFIVTLmp\n"
    "UrAUWhQyE/6ybQMBai05k0mA0wBfU6EVntK9JX5BQEhsH9jTVSg8vz0KIU+DX02Q\n"
    "H/KIpt5nm1wXfSrLreQkFOxa7Q2hXwc2zupWpG4SOuQ9rRdzFP5EVq+txplb2b4u\n"
    "JVsXLjQsMFyLXuFnzQNJXFnUpWoJJAjxml6YUhr25lkhPgpSuvSF6nVv40p31IMv\n"
    "fOGOborRKjMQ2rgACPXpD4w5pcdNyIru2e0wNXBOoc4vENlGvB0ugbB98hZHlNIq\n"
    "dw70uPc8pdm2HemT2kQW5eTVe8dFhWYbgI6r+i4+ctss1398s7INUgPYF1yD2d0U\n"
    "634jeXSivV1MixljQiqhABcSuxSptd9edCQNnHLtJNt3tqSHuiPsrMAGE40dxh7z\n"
    "cw7/L4E8ZNOrtFUwjlmDnBfJOcs9YtVrBkPBeAfvrm+slUjXy/vCnvETGh0M0eCH\n"
    "OFbz20gfpnwTCxUJViMzxMBEF3ZZWDa+fBhmM0HsliX0Wqbjfszty3lWAMipNnZU\n"
    "7Yq4lR4pdbdzt45CAb3dLqeb+irO6Uo1E+wBXcohYfln1vITqByMX0xw+f5w9Qn9\n"
    "daVYnKAoXRYsLIhwvfHDtPyxT35J3vKm+LeFs2nYIaHADTqQ5nMTHQ2Fqf+fScnt\n"
    "kQxD+SE9N1WXf6tX3erQog4jErag4wnJY0h0WYdFOY09PcJ1ovbWMKbHDXvgr1HA\n"
    "lo4=\n"
    "-----END ENCRYPTED PRIVATE KEY-----\n";

static tt_char_t __ca_3[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIICmTCCAYECAihaMA0GCSqGSIb3DQEBDQUAMBIxEDAOBgNVBAMTB2NoaWxkLTEw\n"
    "HhcNMTcwNjA1MDYwMjQ3WhcNMjcwNjAzMDYwMjQ3WjASMRAwDgYDVQQDEwdjaGls\n"
    "ZC0yMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA2QceIQhLDHes2NuJ\n"
    "lBPWSCC10rCyHvxZQc6RWFKOohV+iiLc6TMBT/CVm2TEWujTaiGrz3nnYq5iJVXD\n"
    "G6Xwqbr1QsaYcyh/TTJcipa2CC19ShI5laUfmc+1zucg+uNAkUDw95wkoyZsxCCk\n"
    "3inOphsODOV5PYWzyIsNfZLTvX+ad652V/AZ+EE6OhFB70bkys69ftZvyuueJ1l1\n"
    "2n/BfK9wISlbF1/CBTd212YjWOxhEKtb4O+oLCF6Mh7P5qonc0dS3411sS8nnEk9\n"
    "9LwSULNTEnc9luK7M1SxsilU23rqd2cU3zGu+XEYUjO5Z9vlsy2jzffgiIwelwnX\n"
    "sWqJiQIDAQABMA0GCSqGSIb3DQEBDQUAA4IBAQAyWbEYO0Zi/bDZbrqF+ZdYsDy9\n"
    "TptY3seS8TmpzmWmkbbFVQ6DbM9geCmOTfrrGTR/XM5u5C/KSHPmUPHnwu+joz8f\n"
    "SiLP+FxQykuWMxmiv+0PNLpHsTEhADQ7CNa+5n55HlIkmibJRuRcyRmv7nBIuf9N\n"
    "RKvUsWvgTqo0oXPde3Yrm/9Me6bXRwszXiVdfVZxMD1ay0Obh6pU+HTFMCupkMnK\n"
    "nzLX0brZ9uj8KSQWVnrKG10SMVbFlxEyw1tL0ENRG9zwAYKNc/kguCvd2hAi0swx\n"
    "Qf76djLT0QanZBfmDdg4hR7rIJd+mBzSw/+uqkb06Magi124yXjcTo/AjspV\n"
    "-----END CERTIFICATE-----\n";

// 789
static tt_char_t __key_3[] =
    "-----BEGIN ENCRYPTED PRIVATE KEY-----\n"
    "MIIFDjBABgkqhkiG9w0BBQ0wMzAbBgkqhkiG9w0BBQwwDgQIChAx6a6i3isCAggA\n"
    "MBQGCCqGSIb3DQMHBAiIHN6GY9ySPASCBMj+SbvbToG0oRCSfSylwknGNjEvaPHq\n"
    "mW6ia7MpFEJjGZwP0HDGPtGEk03Bj/05NYtxXT/6AmSqPOFJ65a9a838I39E3byF\n"
    "lJ1IBa6r1ShkQ1KpAU4yLRw0OD0ktXht+EALZt0M+foRoeffPa5cE/a7D/poTDK1\n"
    "9VVdHjMnTAshHWS47Ns2z9ck+7PeiVZXVtLgTsZNWb7ycuzb6XWARwstTer+zn8+\n"
    "WXO89R5NKjN8S+Sd2UDN4m3a5uOMMQIF2JFBxUHNsOTpGQ7MSOYp8Ue/beLzPsZH\n"
    "jP4h4K8LN6rDMTQDKPfYibSjLA2Z7nqh71Bv0ImTErWAFPPaEzEF2PLEYVtIfga5\n"
    "blXH/6FG4QTJksvyt1wwT/g0+ISkI8obBVdC7jVrDgMWq1AVFmjvCA4CmYsfzsUt\n"
    "mcwYuxVGeQe6Xbyx+hELWSK3RDjTwdtykvPEUBLrE3q+iica+pEowuHpYUGmFfvk\n"
    "9p9P9iDswfdMRRsplFH45f8sadfQBmlEaZlo8Hlx/LbFn9LJRBCmQq2VrBMSwX4W\n"
    "bCcRDyvuffxl3LxbE6FTuKvUy3xgxyIccJdbr2/4NJlNIzlNN8asC7BXcsbrxQEq\n"
    "Grm9fros4SGYw9w5X3vOlrxHbi0+ZY1zT/pc/6KVX9toDApJQA/yZ4xyr9JPi3n/\n"
    "DBzUQw/n29qScdt9PHi9YJXoHkFhQ+TIifPRGIU3SSVCl9Z+X+CzScQH9/OenzpB\n"
    "RNFRUrqr5DHvfv8vz41PuIJMNbEhXVDY97LyW+Y1syL+1NVFgVUrxNHHUhX2rVqB\n"
    "T2ixWSSIHZmbhEOoz/aY/5JF2VPVrNV3unSI9FuZ8p2Z7LpHDxFJUrYBDfyykTfi\n"
    "2LAdYh1/oKscX5tmPWWFgFtq2QbNyRhHb2ujUKszbr1PXru+DbaZ5mVDK+pwLOCi\n"
    "MB6FBqgNKSo+xJ5NoR4Gz52rdC1aTUNH0mcrFeVGxxCXa3H+QcvOcs6QfLbfSb0x\n"
    "8+xKiLDqp4/+nqa5sRPzd1kYaf0R+BZJuds8+h27FKuvJALUDh9Uu8lpWNzjEOT5\n"
    "wK17uG0uYrzUMns9VVSQrMuK4mUNGtubgLVbSPoi/lS+h64TG4OCpT5MNx3R86ag\n"
    "VxrALRufeMxMAhMzQpvCGJywGIBMv37EUP7bx4X1n6sR98JAn5JKv+0sVd1NDqvT\n"
    "jK+vPDaJOdYib9L5q9YX8pUb455FbZSKdOjwfZ66RUdK4tcGn13IPjqUBYg+ZHl7\n"
    "6jvdzzFvWay+jH9XTLupxlDDoZhQ3FTcKoNRpDJWFP4RMnjs7u/zBs5+ifjcUFCJ\n"
    "9YT90/5S8FzhPYAhkvdln9sJ525UqiUXjPq/Y5jS3bN24lIoFoe6HyVo1ZhPIvgp\n"
    "aNI9UHQrsnOIEcCZrUSzDSicCQFMIVRpiL3SPLfgj90ZlGYMkwqGjQUooLj9SYsc\n"
    "tQbfP9f55YeI8wd09ZuR/IOPytWE5udxWIxxxfimz1cBeuRqrl9zYnP3QU2xv9EZ\n"
    "9IyHTEojyr+zMgORX8HsSHqcb/pOw4D8jGia/YrKTlXipszAzVMwDxRlkTCzEbaf\n"
    "INHOF4aCwJWopgOFE27uDLjBAcsSSKakyctf06TinQEhFNDk0tcA4B/k/Uw2lCl9\n"
    "e5U=\n"
    "-----END ENCRYPTED PRIVATE KEY-----\n";

static tt_char_t __crl_1[] =
    "-----BEGIN X509 CRL-----\n"
    "MIIBfjBoAgEBMA0GCSqGSIb3DQEBBQUAMA8xDTALBgNVBAMTBHJvb3QXDTE3MDYw\n"
    "NTA4MzkyMloXDTI3MDYwMzA4MzkyMlowFTATAgJHzRcNMTcwNjA1MDgzODUyWqAO\n"
    "MAwwCgYDVR0UBAMCAQIwDQYJKoZIhvcNAQEFBQADggEBAKVobCfxYxdmFsHmTdXK\n"
    "LEMLwY2eFD8IWtAQDYP3n75blpf54cjwO/WP4LsoCEx9r0cEECaygB7S7xuOJ24P\n"
    "GsVCLOj/jMmrxJ5y16yA76iIYXvoMd17OyAdrE7G4fqcy38CRcwiPHZUjDn10nmV\n"
    "7aEmbwoLpjSoUd3VbbZt4DkRTk8YYmXOswBfkmxodcFhXnv+9Tb10txDVb3RDHbv\n"
    "DhqS373XXNhLslVZETpgWVkeut8AsAKDEmvJ70gdsjP/JMkfsCHKC05JIzSSpQH2\n"
    "QRVawYi3kjslnD3AKW7nTd2hfEgVxJ/o9e3zKN9e5YMFsN0n93DyfNjkGWKZvjPK\n"
    "iek=\n"
    "-----END X509 CRL-----\n";

void __x509_prepare(void *p)
{
    tt_file_t f;

    if (has_x509) {
        return;
    }

    // root cert
    if (!TT_OK(tt_fopen(&f,
                        __X509_CA,
                        TT_FO_WRITE | TT_FO_CREAT | TT_FO_TRUNC,
                        NULL))) {
        TT_ERROR("fail to open %s", __X509_CA);
        return;
    }
    if (!TT_OK(tt_fwrite(&f, (tt_u8_t *)__ca_1, sizeof(__ca_1) - 1, NULL))) {
        TT_ERROR("fail to write %s", __X509_CA);
        return;
    }
    tt_fclose(&f);

    // root key
    if (!TT_OK(tt_fopen(&f,
                        __CA_key,
                        TT_FO_WRITE | TT_FO_CREAT | TT_FO_TRUNC,
                        NULL))) {
        TT_ERROR("fail to open %s", __CA_key);
        return;
    }
    if (!TT_OK(tt_fwrite(&f, (tt_u8_t *)__key_1, sizeof(__key_1) - 1, NULL))) {
        TT_ERROR("fail to write %s", __CA_key);
        return;
    }
    tt_fclose(&f);

    // int cert
    if (!TT_OK(tt_fopen(&f,
                        __X509_CA2,
                        TT_FO_WRITE | TT_FO_CREAT | TT_FO_TRUNC,
                        NULL))) {
        TT_ERROR("fail to open %s", __X509_CA2);
        return;
    }
    if (!TT_OK(tt_fwrite(&f, (tt_u8_t *)__ca_2, sizeof(__ca_2) - 1, NULL))) {
        TT_ERROR("fail to write %s", __X509_CA2);
        return;
    }
    tt_fclose(&f);

    // int key
    if (!TT_OK(tt_fopen(&f,
                        __CA_key2,
                        TT_FO_WRITE | TT_FO_CREAT | TT_FO_TRUNC,
                        NULL))) {
        TT_ERROR("fail to open %s", __CA_key2);
        return;
    }
    if (!TT_OK(tt_fwrite(&f, (tt_u8_t *)__key_2, sizeof(__key_2) - 1, NULL))) {
        TT_ERROR("fail to write %s", __CA_key2);
        return;
    }
    tt_fclose(&f);

    // leaf cert
    if (!TT_OK(tt_fopen(&f,
                        __X509_LEAF,
                        TT_FO_WRITE | TT_FO_CREAT | TT_FO_TRUNC,
                        NULL))) {
        TT_ERROR("fail to open %s", __X509_LEAF);
        return;
    }
    if (!TT_OK(tt_fwrite(&f, (tt_u8_t *)__ca_3, sizeof(__ca_3) - 1, NULL))) {
        TT_ERROR("fail to write %s", __X509_LEAF);
        return;
    }
    tt_fclose(&f);

    // leaf key
    if (!TT_OK(tt_fopen(&f,
                        __leaf_key,
                        TT_FO_WRITE | TT_FO_CREAT | TT_FO_TRUNC,
                        NULL))) {
        TT_ERROR("fail to open %s", __leaf_key);
        return;
    }
    if (!TT_OK(tt_fwrite(&f, (tt_u8_t *)__key_3, sizeof(__key_3) - 1, NULL))) {
        TT_ERROR("fail to write %s", __leaf_key);
        return;
    }
    tt_fclose(&f);

    // crl
    if (!TT_OK(tt_fopen(&f,
                        __X509_CRL1,
                        TT_FO_WRITE | TT_FO_CREAT | TT_FO_TRUNC,
                        NULL))) {
        TT_ERROR("fail to open %s", __X509_CRL1);
        return;
    }
    if (!TT_OK(tt_fwrite(&f, (tt_u8_t *)__crl_1, sizeof(__crl_1) - 1, NULL))) {
        TT_ERROR("fail to write %s", __X509_CRL1);
        return;
    }
    tt_fclose(&f);

    has_x509 = TT_TRUE;
}
