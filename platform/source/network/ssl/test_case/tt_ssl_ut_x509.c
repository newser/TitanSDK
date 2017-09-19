/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
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

#if TT_ENV_OS_IS_IOS

#if (TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)
#define __X509_CA "/tmp/tt_ca.crt"
#define __CA_key "/tmp/tt_ca.key"

#define __X509_CA2 "/tmp/tt_ca_int.crt"
#define __CA_key2 "/tmp/tt_ca_int.key"

#define __X509_LEAF "/tmp/tt_ca_leaf.crt"
#define __leaf_key "/tmp/tt_ca_leaf.key"

#define __X509_CRL1 "/tmp/tt_ca.crl"
#else
tt_string_t __ca_path;
#define __X509_CA tt_string_cstr(&__ca_path)

tt_string_t __ca_key_path;
#define __CA_key tt_string_cstr(&__ca_key_path)

tt_string_t __ca2_path;
#define __X509_CA2 tt_string_cstr(&__ca2_path)

tt_string_t __ca_key2_path;
#define __CA_key2 tt_string_cstr(&__ca_key2_path)

tt_string_t __leaf_path;
#define __X509_LEAF tt_string_cstr(&__leaf_path)

tt_string_t __leaf_key_path;
#define __leaf_key tt_string_cstr(&__leaf_key_path)

tt_string_t __crl_path;
#define __X509_CRL1 tt_string_cstr(&__crl_path)

#endif

#elif TT_ENV_OS_IS_ANDROID

#define PATH "/data/data/com.titansdk.titansdkunittest/"

#define __X509_CA PATH "tt_ca.crt"
#define __CA_key PATH "tt_ca.key"

#define __X509_CA2 PATH "tt_ca_int.crt"
#define __CA_key2 PATH "tt_ca_int.key"

#define __X509_LEAF PATH "tt_ca_leaf.crt"
#define __leaf_key PATH "tt_ca_leaf.key"

#define __X509_CRL1 PATH "tt_ca.crl"

#else

#define __X509_CA "tt_ca.crt"
#define __CA_key "tt_ca.key"

#define __X509_CA2 "tt_ca_int.crt"
#define __CA_key2 "tt_ca_int.key"

#define __X509_LEAF "tt_ca_leaf.crt"
#define __leaf_key "tt_ca_leaf.key"

#define __X509_CRL1 "tt_ca.crl"

#endif

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

void __x509_prepare(void *);
tt_bool_t has_x509;

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(case_x509_cert)
TT_TEST_ROUTINE_DECLARE(case_x509_crl)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(crypto_x509_case)

TT_TEST_CASE("case_x509_cert",
             "ssl: x509 cert",
             case_x509_cert,
             NULL,
             __x509_prepare,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_x509_crl",
                 "ssl: x509 crl",
                 case_x509_crl,
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
    TT_TEST_ROUTINE_DEFINE(case_x509_cert)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_x509_cert)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_x509cert_t ca, cert;
    tt_result_t ret;
    tt_u32_t status;
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
    tt_x509cert_dump_verify_status(status, buf, (tt_u32_t)sizeof(buf));
    TT_INFO("%s", buf);

    // root, int => leaf, ok
    ret = tt_x509cert_add_file(&ca, __X509_CA2);
    TT_UT_SUCCESS(ret, "");
    tt_x509cert_dump(&ca, buf, (tt_u32_t)sizeof(buf));
    TT_INFO("ca: %s", buf);
    ret = tt_x509cert_verify(&cert, &ca, NULL, NULL, &status);
    if (!TT_OK(ret)) {
        tt_x509cert_dump_verify_status(status, buf, (tt_u32_t)sizeof(buf));
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
        tt_x509cert_dump_verify_status(status, buf, (tt_u32_t)sizeof(buf));
        TT_INFO("%s", buf);
    }
    TT_UT_SUCCESS(ret, "");

    ret = tt_x509cert_verify(&cert, &ca, NULL, "child-2", &status);
    TT_UT_SUCCESS(ret, "");

    ret = tt_x509cert_verify(&cert, &ca, NULL, "child-20", &status);
    TT_UT_FAIL(ret, "");
    tt_x509cert_dump_verify_status(status, buf, (tt_u32_t)sizeof(buf));
    TT_INFO("%s", buf);

    tt_x509cert_destroy(&ca);
    tt_x509cert_destroy(&cert);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_x509_crl)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_x509cert_t ca, cert;
    tt_x509crl_t crl;
    tt_result_t ret;
    tt_u32_t status;
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
        tt_x509cert_dump_verify_status(status, buf, (tt_u32_t)sizeof(buf));
        TT_INFO("%s", buf);
    }
    TT_UT_SUCCESS(ret, "");

    // with crl, fail
    ret = tt_x509crl_add_file(&crl, __X509_CRL1);
    TT_UT_SUCCESS(ret, "");

    ret = tt_x509cert_verify(&cert, &ca, &crl, NULL, &status);
    TT_UT_FAIL(ret, "");
    tt_x509cert_dump_verify_status(status, buf, (tt_u32_t)sizeof(buf));
    TT_INFO("%s", buf);

    tt_x509cert_destroy(&ca);
    tt_x509cert_destroy(&cert);
    tt_x509crl_destroy(&crl);

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_char_t __ca_1[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDEzCCAfugAwIBAgIJAJOsrJf5XUGSMA0GCSqGSIb3DQEBDQUAMA8xDTALBgNV\n"
    "BAMTBHJvb3QwIBcNMTcwNzMxMDY0NDE2WhgPMjExNzA3MDcwNjQ0MTZaMA8xDTAL\n"
    "BgNVBAMTBHJvb3QwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC4sVTJ\n"
    "+og2NWiyyTSfmjsd+hCgSP06K5z3Qid1vZy6mBhKHQdcojFjBFaQmPcTBYrLlkBc\n"
    "a4NbXWF9Olwd/PKLLUht2QNafODG748pwSLqH3xbOWnghKafRz6QXdy2thIpEZWy\n"
    "JJnKLmzv+horX3G0kYnSANZ66Go2mnC0b9iHw+rTVzvU5N8+7EtDx7RC6WSP54Gm\n"
    "agSWnElRNnp6n5tCPbxBCkf6eSvyGLUbfqb08Xbe0oexb6Q+xpD/0+5XltCLVcpp\n"
    "qsdNY/pYERAi2iSfMDVZJ6zX5UKZgfUPgIHkLUNiz8lmlQOvvPv61/EoAaV2qt8j\n"
    "RHCoTpbsIElmUKH5AgMBAAGjcDBuMB0GA1UdDgQWBBRzn3ySOSVQwMj+7sTzvrTi\n"
    "eMcNljA/BgNVHSMEODA2gBRzn3ySOSVQwMj+7sTzvrTieMcNlqETpBEwDzENMAsG\n"
    "A1UEAxMEcm9vdIIJAJOsrJf5XUGSMAwGA1UdEwQFMAMBAf8wDQYJKoZIhvcNAQEN\n"
    "BQADggEBAEBe/CPklGqXsryA+cAQlEeNZhh+p+DMt75/7lDYvWM+2rGqi5yO/EVu\n"
    "5owYmcXWLsCzAdgjpvs+/eRPk8j+qooh4vWWIQc/dz1AdWD+63tQkmNfcvrkYLAX\n"
    "G/ezJdcE+vs6BMwsOwvDoQuH/dE2jNw+VjC5rQkF+n4x9ksd/bVluLNb8cGaW/CE\n"
    "l1g9IY1iK7TKGOuWt/mmslX9OCOx5gGsof0jPY8LV0XIsZLHiGcmMK91y2o/L0Ju\n"
    "+FdIsa+mJsSCxr3WK5nWlW4rTswvhFddTbriR5k15ZGa0qUXrKGK2G6XYisZW76b\n"
    "gV2o79igIYqXBpODdNwS7SHI1Gqi/jI=\n"
    "-----END CERTIFICATE-----";

// pass: 123
static tt_char_t __key_1[] =
    "-----BEGIN ENCRYPTED PRIVATE KEY-----\n"
    "MIIFDjBABgkqhkiG9w0BBQ0wMzAbBgkqhkiG9w0BBQwwDgQIz+A+MuVGHiICAggA\n"
    "MBQGCCqGSIb3DQMHBAifk+JdAkskWQSCBMj+WE8vUGsoc3D7p1xhxNdX9m1o7MYT\n"
    "7o2vRXuJcKyUrowrsrffbXrT46WFT5b78RyEQhC5MW5XXD4WQVus3pQRYLAE4u39\n"
    "jXe9nFHOGA19uH93t0ARl5zYZLbtF8MZsWDulL4eXyctG1vSheFVO0HhDEjJuSf0\n"
    "o+f4VDMAD7XMyGXqc/jRsL00RktmCivTuJcDw+K4KggyZsaQc0IJHuSy51AEslOO\n"
    "KOpN3fD+T7vSokDnbTaw/IJ1BFJvMaM1/7xulZIpxCgi8CDqOCKVAnOV4Y/Ej7ju\n"
    "vDThNt3CGL20Dh1hQq1L+cOJFlI+IRT7DQJA3zw82eA/VZfJ/rcaTxV6VzdUGbC8\n"
    "x6RNpy+JzwHAaufgTR/APeEH+IfCt30qXJuConfnjS3hgwlFV4rxwvB+VihXXdUV\n"
    "1hxeSF/Lp6D02hb0BqiSGJPBG4rQ1pkjaqeYAnzTbe6Rs8FKVYR9fsqkhZaUas/Y\n"
    "eFlLY9Hcmn1BYZBJhoaD7iQrGwB99T3/mt9DjvXhzBQXSJUspJtv3qhB8Vm8oCoA\n"
    "7o9AzubV36xgqO9HJWdzHQqMXFiq5e/quyJPhqyaEEeK53n1iJc2mdEE8sk59HS4\n"
    "5pOTT1IV9hOcp6/VJCOzGTguEsIPjF8708cALp3t4cQWTmaz6CqqRYP6pN9Xmpo8\n"
    "U9kG3xpOnsHks+p0GYTCcdoiXYuItnmVKTk9Pe+VpyHOJuOe4x1U3W9CDQc1WmcD\n"
    "5evxKtTdRa7vTHJRy60ArDPsFZsdwzeojnu5V5Q8J1buh/i8BMPUmiVQylvg6U8E\n"
    "1e8hZ4wDLJnLm7NdARixTTI6BbAd01HwU0X+RWQhRlnvH1yK5qc38T+2HIv8s5WN\n"
    "lKWBEHmwKyJy5HbaxMl984mgMYy8DQFRpTBgnBOtNkwGKuxLCxjNcnOCPHSBiMnx\n"
    "M4VuXHnmYULjeom0s0O2DaBIaov8pADDPSk5HzXG3dB1bxrl5SjY6SfhR2M76BW9\n"
    "SaLHsEr+OyNzQcR2SD20z0XxM3/4xT2m5tzFVVBsrZwyi+dB3UUMK3KbDEOU/j0l\n"
    "ziVyCla68y5k6ZADrBUJJgisiGjvfzK/Zx6FftrsBmAWv/p+tLLviHYGYXwIUK0R\n"
    "Lo/fe7cLh3DJ+r/6KcBJYPaB/HlFncMfXmJpFGtpmR03jnk4U4yqZ2vhjCuKle8I\n"
    "E20kq83+ze9je2+tAXeq7xIkbJxrZJR1O1BomEH/zZDPQIej9WuRBQEV2k3EAgun\n"
    "jFJ5RzsJkZ/bIruM53nNfVS8G+q1AefoO5/Dmx+OUV5yUzkKBvFp6V2ny1ilqgIS\n"
    "2Dnbk38zA7ctpIlbLDkKRoLhhlX5QcaFSTkyuo70yYDAAPd5j8fpGf36EnMTX+Yk\n"
    "wWgPy+wIqSMuYDdp47dKTREWzJBKQ5OienQj5g+jzXt3zuqhgPz34+umfwRkf9KM\n"
    "yNM4/kzIEZimXG/mV9QbQz+odV62Ia0CVQosf/uDea/TwUAHkoXBtXhhNrpfdLSl\n"
    "Yqe176X5Gqt+Y+o3QXxB7hY0mz8m2BjAI16KR5cOjvyrJuWfDzYJpjlIJVcJx7Nq\n"
    "fMiKxYWZeVWfMSgn402vOcOE5FVOBIr70Pl/d3H5Ey4p2+gIihqdJHokBbfM7L9m\n"
    "raE=\n"
    "-----END ENCRYPTED PRIVATE KEY-----\n";

static tt_char_t __ca_2[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIICrzCCAZegAwIBAgICOwowDQYJKoZIhvcNAQENBQAwDzENMAsGA1UEAxMEcm9v\n"
    "dDAgFw0xNzA3MzEwNjU4MDlaGA8yMTE3MDcwNzA2NTgwOVowEjEQMA4GA1UEAxMH\n"
    "Y2hpbGQtMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOhMN/8ohJvk\n"
    "Pr+D+XqingdhpqghBMU56jY+vMmyJ3AcIwKeFN+bwvb2Rm/3TJY/PZNJXKwOLy3I\n"
    "VF2lSa/c+n+GJgdeC+y36maVMNyFxbZqrQQmpjILn6CSCpfX/ZisdLkdakhDd65k\n"
    "Rw1WV6PsOgrQvvXOXdczt5519ErD4SPSbcaKwH9s5M5JPKbl/Pr/eBFw4aHm6cPF\n"
    "Wl3NZBLlJjQqT5syxUGEvL/d250nb5Qf7DwOGZd6FRQk09qJmgT7q/y11gKw2ceK\n"
    "6/RZ1/O7UUfPHUeB1q2VFMcKD++OqGh4NvgyxMUB0lyROZeEd+hUXP7O3L8r9n38\n"
    "GXyWp/yCkZ0CAwEAAaMQMA4wDAYDVR0TBAUwAwEB/zANBgkqhkiG9w0BAQ0FAAOC\n"
    "AQEAktd6crr0cMuS2kdRYMMYCNdcoTVKHokX680Q9UtEmjuowinQpQnsYpj1lMnn\n"
    "fk/p6mNBckH50FluJ9Job5RmTF2hCxaZIkYn5ACuCSr+d0/IrpaxeXYxleEd2WSH\n"
    "JuDW4Q/9oA+3YUnQuhJJlbQdw7nxvCzJSkySRB8fZtpgGwuEzow18CXXmlPeV0pA\n"
    "D60kSuWHEu5lw731iI4NL0w2KhatweVwEZyA1KiUovf/WgblNnvL4VcAmFdJidIm\n"
    "0nNKz0cNic+aJgjvGShLo9vK7dTjEcJgOmBGfJRduWvwHtkvzNnDZ3luquMWQzOF\n"
    "xU1fLiub1Kb7kDV15ICE1IcPOw==\n"
    "-----END CERTIFICATE-----\n";

// pass: 456
static tt_char_t __key_2[] =
    "-----BEGIN ENCRYPTED PRIVATE KEY-----\n"
    "MIIFDjBABgkqhkiG9w0BBQ0wMzAbBgkqhkiG9w0BBQwwDgQI0gdqA9AqPFcCAggA\n"
    "MBQGCCqGSIb3DQMHBAjgrbzkPvtSYwSCBMiOBhfHwrxWQRKOzX/YmokbJg1Yhbaf\n"
    "4EP9/xKQj6cXU1Vt3ZN278HVZHEpBU44zH6XXcdQozEmWeM+pLxPtdO278sIF7RI\n"
    "wPvXfqyQa1VLO+Hv5JYag6Kgy8xCg5VCwQRW2M/WTyIP+BIvfcdiCtov+ujLFrkq\n"
    "tH1A6Zq36Nkg+aI60xlTqkOru5ZM8SgK60sQWbE/E8p9WcccaTKn7oo6QlknN6gf\n"
    "FpuR0xfsogHCOoo1Wtgl0bhVQ1d8ZOQMgc1XXL5kGuhwaKMByzCK5oSPNmo8xlXP\n"
    "oPF0K6LWBwwzB2eQ62lW+NEKMW+UvwnK9OGYBoYnTLkhw805vkYemJ0A/pcYkfeX\n"
    "M2SaqkWRNygIp/ru7ZPGeD6Iz90NTdd3EsdmCqEWujHr+YY4rJL0xWJNXF3e4ZPJ\n"
    "lT8mlTNv9biUn4RkVscpwbtXJWyrP8jwdutWiRGvm+08UgS1pAKe8sr0P0E8+RcE\n"
    "zUy3CSjEgIahY97AZnF+Czn6J/P6grY65pCpdRvS3rKUbr0GZfXGQeNFhc5brx6N\n"
    "pEnIBa09CLaSFro2nhC2zSIvY3J9H0ofdYu0J8WvYhviGUk5UfkLVDOEw9fL2s2z\n"
    "AxboB3pTBY68UCHu7kmoYSS2haii6PAuEkRj3axaPaYDNWFatrR1txpExjylW49M\n"
    "1Uq2CjEF6EvhTch3blEw0lhcTF9J0R2a4sgHoDpr+cwlVm65yONmIZo1zz/ZxSx0\n"
    "aC9HngmGSWMVkmPW2kP+iGQcI7xG5zgDtIb7n69vyeGR6E7JqzKJaYnmq0OCxjkx\n"
    "SiTVgZyVNlUUcOajChQqN9hwQoibxQaraPH3YwcSxpeejZ++4BzN+2z+cnH1CRab\n"
    "h1ESTn7kwVw7WT9xxG8iIAIPu94v2EM37EtaSkRd5x5+7ptnMO3bAe3H0BCHX7cI\n"
    "K6TJ4qchM9EL0CCTM7fGe32ahaZXxiTyMpPdW9lE2cD3TGyiV9bRpq1pXPtR7sHj\n"
    "Jk6s0akPol4Et9lURZVSdguCtHvdCbWTU89IS0QUAtRgnaZx+U2NZFVoTPrpJqxI\n"
    "8Ab04feZ9FUL47CdAIQy/dImiaVwDdoNNcg92IkXUg44YbTeKFHLzd86H88Uzvej\n"
    "JT2e9HKgCsXfHhYpV2lY6d5lr4gB1Mtf3SguBK9GkVMLRrJqkx+S9x8LnsZLfAHt\n"
    "vDKVUdmEl1HtA4Vb8/qWz/Ph9ilc+l7SXYZ5ZJPysqywchW1mwYaBNiF7/8Rpxzq\n"
    "yGp+Qr2hRn+qq7D1R9w13nOD0VjyRQjdSWZhUf6P7+MupoVRHNzeZjHi7Pu3hG1y\n"
    "QrB7q6lYKNAFqxZ6KcdB9cBMf9fk/uJYKXr7el1VmFzJAbMv8XB1f87Zq95KToB/\n"
    "2k00DWaJkrYBZDDbCZWs3+Fg63Jm1Jp0N23TiJF/97DmEhvSdsH1jZ4E9HpHufcx\n"
    "5IwJeHz73EW3TywGDVGcmavuN2jpe9p5fEft6r8cbQmMm2Kvlu/YGxFzw8brX5gd\n"
    "QdpJ9lduVuMaWbFj5zXtEg2dQt+jXIIPIBgkm0wowOxg0RR2ywaYh21DSnnASJxP\n"
    "/9iM1KRqoompaW6P+z84kh66wKC7YQWlOB9hBT5M1ukAZiTC2lGBpopnlwDy8tST\n"
    "IXs=\n"
    "-----END ENCRYPTED PRIVATE KEY-----";

static tt_char_t __ca_3[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIICmzCCAYMCAgJ3MA0GCSqGSIb3DQEBDQUAMBIxEDAOBgNVBAMTB2NoaWxkLTEw\n"
    "IBcNMTcwNzMxMDcwMDI3WhgPMjExNzA3MTUwNzAwMjdaMBIxEDAOBgNVBAMTB2No\n"
    "aWxkLTIwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDtLykwBMlnth8F\n"
    "ORNR+X07WpPWljfzyx68FYocbb/qRHZvKg6NZ30CZwjAKhc19aiN0cxcaUFT0mk3\n"
    "He7NDUxNTGLy9UcBlAqlVDjhlFkeeo5+dDecQHcyzCc/lM7y9+LnvYRPNtZihZYl\n"
    "dJESw+5/1yNAqz1QO6wO5ckHiMK6+5PiXiB1YlEu8qpKcoDiVOrRqX389K4t1kOW\n"
    "PcaJ9ZGVON1s0xktsDpD8ggAavIxLiogu9MyPSL9AOnISvdeazH5Oru//x69T2Uk\n"
    "RELm7hfCen1Qa1jwlrIpjJ7fU5xcDuCs7yWcgs5FWkv78AA6C4nSIRRBVOfOtK1w\n"
    "Ez1/Jg91AgMBAAEwDQYJKoZIhvcNAQENBQADggEBAEM2N+V/g8wOgvs5u9lq/sUP\n"
    "ypFOuzFyyBxDFV319mRG7lERVoxH3EDn6A5T6ggpa5n94HuoyJ8J3NZ45sBRcQho\n"
    "P5VpWZv49loddOU6c5BsCEmKHanyLxZxEXvGA8Mdlg0xZypTRU8mFJzh8scPXdU5\n"
    "cVhIfCK30tZotvmAn6PKFZvV1xq3vDqHkiqnt2rQvG83U9oyBHDVxcfPWCTdRhY3\n"
    "33gBcnZDOYRR1br4ZKsWeO0FJRbnCdV1viOZX5/I7eZcIvj5qDPoiUbSSupSvqC+\n"
    "+rnS2sX8MoxhvUzeFvLCP3ht2Hst5bBKyYOctGJNbxourcIliRsek3lg2hwY1So=\n"
    "-----END CERTIFICATE-----\n";

// 789
static tt_char_t __key_3[] =
    "-----BEGIN ENCRYPTED PRIVATE KEY-----\n"
    "MIIFDjBABgkqhkiG9w0BBQ0wMzAbBgkqhkiG9w0BBQwwDgQIsWi1+mcMWCgCAggA\n"
    "MBQGCCqGSIb3DQMHBAhYR4D0a242AwSCBMi5z4eQBH+3ViObwD8IiUM4hboAAAE6\n"
    "GWAQNSZGz/aJfAGgdRMQPT3H0CTtSERrJzQKUt2b1h3oM3olBEZChadp420XJDv9\n"
    "WcPbGTl0TlWd6FcVq56RCrODX55cumjlkTB9y8TBpNw6U8EAC0k5/PK7M8viskdI\n"
    "4glFWSNwr8MyNdI4LGkzqYuUpFRiO54m3Qu0VJVZ/QmfZ8UDkSFHXp8SifX/S97r\n"
    "xy5j6NYGEBZn8yn4HgofkuWxsmng7xau8JqDzOWlKQ8ouY1ygIvanJ298NlCxzWZ\n"
    "wvx7M8/fX4QM+k86Dlz5wct3WooMJk3pFvxR+9o+j66xEoWjTN/TAAaE2SxC5KNX\n"
    "YBfZVnjP7Gn/xMtIV3TGB1kG4WB76ah4VtM4iXoKwzFvugunZoGWYJTR9KtMKTaV\n"
    "DlV23Bpa0PicpLElQsuFCNIhwQLGnknRIUdZzPaYI66pcn1QJAAmBmOVU0xTcyLX\n"
    "6UvP+jJa8jx7OS7MZSlBNs+GtFaXqE0sHvAYQeLxWIzX7LfqfiFImteIoN/HxEhR\n"
    "afSFfkqFhhXZvp5crLaFbWMiV+doHm4Os6RqTruu7jLwv9Ye/CyHwghXY5XYoYUi\n"
    "WFbi+9Uw7lo/6o4jxMEL/oKT1TM/ohYs8yPtjMqYM1YiXxJPAYvl98phJw3k0EQ+\n"
    "vSEVvx7kc0lVJtKqzQpTlKGOIQ5C3sQqC4JyOvenuQbxaUc0BJnkGYx7XAPPNlVq\n"
    "3S8C/MLnh8YMUkuNnMpdVe3FbhLxr2bh+i+JUvKDw6V/Qmv961Ns8KPGCdr79um7\n"
    "4/iVoV6OP49j+jwk4i3zLStiE1HRiKtH5Xd/cpDaOK0C/FbSuA7v7sFL/nfPRa7h\n"
    "S0QWruPyZU29P8cl0WfLCi6oiz7ur/2Sxelzezh3IKZCdwEjFil6iU6WJmRqZXmB\n"
    "cmN0vn6eoJpaEVmGz1W8461W8TV5bWvqk2N0UE7WRxk5Pht4k4twLb7zrf4bMmjf\n"
    "IOcn2NFyNntMFjx64+TLE6x5UDDwJoDvXVbseZyfqynfyMRPCl5W9pcxMxISSbYj\n"
    "kSxnjxHHs4M7b5oXdVWhBrXEZlBFC+Ff0TmdQMZIuS76FFneXzaeSbE9Z8wBjs9d\n"
    "ThynkD4Io+vEqx+vZSPKvKNAJq2Di9jqSx9BhZH5mf+2hUoCdlv1e+fGS2dlAEGD\n"
    "CXn5QRfZml1qpscLPpnCygxfgp6AQaQh/5n/yZP6Vvyd99FJ4tEK6XqioyLI1G1m\n"
    "Es6erPeTkecWD1Nt4xDPDm1+bEICIVjTneg/+Ck95Neh+dfUyx1re1r5gBfCOSbx\n"
    "u/NNDA6560hS7Pn+paOC7Ud1wj8QF4Dzm+Ci0vu0JNh1CbBdLO77JK7rqOBEVTlC\n"
    "DN/2v5Cf+oNoiQc2jXBQwR8Aa6q1Vwi2AJsaqDItAFcJnwn4yHxO6htvQAeAlLyJ\n"
    "nqooGRBKHL7NeS/ISrb6IxGGzTkTbTwfKVkLCb0n71fXrrgQ38HZm0HKqoSDcHjk\n"
    "9UEOaEKB6J2YnQhmb+KN/BuBoN3YAecf6xat+rIPOrneJG1X/EUthc+7ARdwLgRA\n"
    "nBltdID7CgvJh/YXIim94WQci0BnneU4VKDiTdGH7IysFtFm1uuoH8vlDriqAM6k\n"
    "LXY=\n"
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
    const tt_char_t *s;

    if (has_x509) {
        return;
    }

#if TT_ENV_OS_IS_IOS && !(TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)

    tt_string_init(&__ca_path, NULL);
    tt_string_init(&__ca_key_path, NULL);
    tt_string_init(&__ca2_path, NULL);
    tt_string_init(&__ca_key2_path, NULL);
    tt_string_init(&__leaf_path, NULL);
    tt_string_init(&__leaf_key_path, NULL);
    tt_string_init(&__crl_path, NULL);

    s = getenv("HOME");
    if (s != NULL) {
        tt_string_append(&__ca_path, s);
        tt_string_append(&__ca_path, "/Library/Caches/tt_ca.crt");
        tt_string_append(&__ca_key_path, s);
        tt_string_append(&__ca_key_path, "/Library/Caches/tt_ca.key");

        tt_string_append(&__ca2_path, s);
        tt_string_append(&__ca2_path, "/Library/Caches/tt_ca_int.crt");
        tt_string_append(&__ca_key2_path, s);
        tt_string_append(&__ca_key2_path, "/Library/Caches/tt_ca_int.key");

        tt_string_append(&__leaf_path, s);
        tt_string_append(&__leaf_path, "/Library/Caches/tt_ca_leaf.crt");
        tt_string_append(&__leaf_key_path, s);
        tt_string_append(&__leaf_key_path, "/Library/Caches/tt_ca_leaf.key");

        tt_string_append(&__crl_path, s);
        tt_string_append(&__crl_path, "/Library/Caches/tt_ca.crl");
    }
#else
    (void)s;
#endif

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
