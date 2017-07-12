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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_ssh_basic)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(sshbasic_case)

TT_TEST_CASE("tt_unit_test_ssh_basic",
             "ssh server basic",
             tt_unit_test_ssh_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(sshbasic_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(SSH_BASIC, 0, sshbasic_case)

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

    static const tt_char_t *__ssh_rsapub =
        "-----BEGIN PUBLIC KEY-----\n"
        "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAvv66a0SGXEBqV1cwAmbg\n"
        "oCKfN4AM/jUJRaXxfZk8O3nBFT+LMu8Hou3GfB9B9iYIBUInm5sPF9a0+F6XUojX\n"
        "ErwyYhE0Nv2WK7HgcVTk1QQQQQEKAMBEjCXkxhV6Tqxr5xuy9/zDDZkcO2S1n7E3\n"
        "KGbh4CUzXfX+3maHeU5Bvus9Zf37tKq8kJM9AwVsoCjagqv7AVQpOu0wkjm29hCU\n"
        "J/IKg6liDeREdzka5ULzWH2z1/TwkwFh2f31UD26EvpgnCfrZExp5+D8yhVSgbeh\n"
        "xI9jvkRq9ROdiLlasx9zxZY20667IzQrhK3ABXx4C3qcMMgH4WiOLQiHsVfGKA7Z\n"
        "MQIDAQAB\n"
        "-----END PUBLIC KEY-----\n";

static const tt_char_t *__ssh_rsapriv =
    "-----BEGIN ENCRYPTED PRIVATE KEY-----\n"
    "MIIFHzBJBgkqhkiG9w0BBQ0wPDAbBgkqhkiG9w0BBQwwDgQIdE8ghPaeRnMCAggA\n"
    "MB0GCWCGSAFlAwQBAgQQCxSeXB5CDIBYtcjTWcDDWwSCBNAbs09d0YEkiUXCrKMu\n"
    "Br5ZKZxlMPjUQzpM1mjuYG79TIoZfqXioitntGx9YPghln/iqIUoy/ZbWmz1E+Wg\n"
    "6/xGY4+Ei8dyvmIHbgcv10SJbdoSU10ZMKLfBdnjwuZCZC7NMsMhpGhv8/ZhWMbG\n"
    "eVUQfq1lcYp4R3zU00AF6yKtYiXflyDcsq4cb9ywlkLtlQ7nEj3dP0jn+/FXHCl4\n"
    "f2FRQQ89jKTlHArkFqgkYOi0HNyqisNY6t+2Dy9QeqLyd/kNvZWbURtMLEQ3+K+2\n"
    "q5GjBjYswcbBrHjxCzSSkUpuULNYfk6FVMcsYWKh3ju4kBOTR5auxRL1lHdJU1/Q\n"
    "h+LI2NmFoF/WyLvdEbYW6mX5/j5etbZrHNbM+ToQLZF8e5OurVRmFEjk0JymlN2B\n"
    "c0npIaZiAT2dgRDgeUky6+vDs667vbsqu+IlrtwlrgPeuBQcwH4NmevhMBLEuzU2\n"
    "OxYyNDIp752N5abWx9clk6kXfAAhIYoE48uy32Fzw5u1MHbYkUt7lRnUjYba4kA8\n"
    "jDrg6mjHugd9iq6MIkPVLhZ5VzCiVK+7l3Afhqzd0nrbqdgDv3+G/tkqwZ+17BIp\n"
    "8pGL1Ms2FuUwk9V56dTi89vKQWDgorYchXcmsBXthJI05qhoZNxbXQXnqpeX6vx6\n"
    "1Z7Za40/3Qkh8rosaXUpMiQGw2whjXr8R1h9O5cKPnDH2Nho/jEAEYZb+nKEqn7k\n"
    "JbV/Vo+xQT/ipCozu+edWJjnuY7chiDP1k9IKIhz/9pFfzmb/iGy64p0EQ4/Gh6v\n"
    "SDXHJhRaU0rjouJwZMui3642b9W/qLbazh/cudxPYtFptCuE2y4YLq7epusQdprl\n"
    "O8g8PW88KV7S6BmmbWViFW1pQ6GBycd5MZvtWFtBQdNZ0F24xy25ycwNjXFcg2cq\n"
    "rmtYQmIWxAhNQ4xw+h6THLsz0fN86rbDG9BrSXk7UTOXJ+5avvydPLSLZP582p+p\n"
    "qEAuJBRGqCuwt8hNw/WxvVy8JFtQqwKKFQOJg0KUIr3X5nm3h3wPp/tmgJlJg8+D\n"
    "bAKSGqiMoLcc8OEu7SEFgWqLh31F5YHR4WcHjGly9cWPptXUeEcYbLyOQ3ihiVxm\n"
    "rcrOYX7RQ+hPS3QnXLmee4knVTEiP2MDENj0bQ1ArRZBCBr000nzkWuxwnMbqICz\n"
    "R7KLbLPdSZ6S29XaaF5qrX4WQkLndOzD6/iBHHeR+jVoo+tAHQdsuLYSBRysoCxT\n"
    "Qa54P4pixcmEH8KVueM9uy27PdMJVlHgHjKoveV7psbdouTpOc91bFCSvcLrGWC+\n"
    "zzUJEaVox1JIuJ0fEf7RYpLzGcKHnEPneFygYX6vZfSq6b1XtWuK1ovXSopoen9m\n"
    "dPAKU1k4lZqF+WPp9co7aYwnOm+xJWH16uHbfuVYlGM9VmJ9UQp1N2AlhQ75xF6+\n"
    "/zO/m2aSFcV/zHlw//rYVP2gPFYwp0I0lJ9GnSVFzcWIWL745HMlBhim50qbPhtU\n"
    "1d4RNL/MuFqiscyz0UVfzWU4zl+X57RveVhzoRQCH62UYn0SApscc/aGA9oWmhe2\n"
    "n+l10MjGIiu3fqTChDsJ3VecQ2g2erXHoeWaHWhCO/BwTPdOftImOw3WNkGyCWA1\n"
    "gALQDIvVfw4eXRP+HrmPoz0Vgw==\n"
    "-----END ENCRYPTED PRIVATE KEY-----\n";

void __ut1_ch_on_connect(IN struct tt_sshch_s *ch,
                         IN struct tt_sshmsg_chopen_s *chopen)
{
}

void __ut1_ch_on_recv(IN struct tt_sshch_s *ch,
                      IN tt_u8_t *data,
                      IN tt_u32_t data_len)
{
    static tt_u32_t total_len = 0;

    total_len += data_len;
    if (total_len > 50) {
        total_len = 0;
        tt_sshch_shutdown(ch);
        return;
    }

    // echo server
    tt_sshch_send(ch, data, data_len, 0);
}

tt_result_t __ut1_on_init(IN struct tt_evcenter_s *evc, IN void *on_init_param)
{
    tt_sshsvr_t *server;
    tt_sktaddr_t addr;
    tt_sshsvr_attr_t attr;
    tt_sshch_cb_t ch_cb = {0};

    tt_sktaddr_init(&addr, TT_NET_AF_INET);
    tt_sktaddr_set_ip_p(&addr, "127.0.0.1");
    // tt_sktaddr_set_ip_p(&addr, "192.168.80.1");
    // tt_sktaddr_set_ip_p(&addr, "192.168.10.109");
    // tt_sktaddr_set_ip_p(&addr, "192.168.140.1");
    tt_sktaddr_set_port(&addr, 12323);

    tt_sshsvr_attr_default(&attr);

    attr.rsapub_format = TT_RSA_FORMAT_PKCS1;
    attr.rsapub_key_data.addr = (tt_u8_t *)__ssh_rsapub;
    attr.rsapub_key_data.len = (tt_u32_t)strlen(__ssh_rsapub);
    attr.rsapub_attr.pem_armor = TT_TRUE;

    attr.rsapriv_format = TT_RSA_FORMAT_PKCS8;
    attr.rsapriv_key_data.addr = (tt_u8_t *)__ssh_rsapriv;
    attr.rsapriv_key_data.len = (tt_u32_t)strlen(__ssh_rsapriv);
    attr.rsapriv_attr.password = "abc123";
    attr.rsapriv_attr.pem_armor = TT_TRUE;

    ch_cb.on_connect = __ut1_ch_on_connect;
    ch_cb.on_recv = __ut1_ch_on_recv;

    server = tt_sshsvr_create(&addr, &attr, NULL, NULL, &ch_cb);
    if (server == NULL) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_ssh_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_evcenter_t evc;
    tt_evc_attr_t evc_attr;
    tt_result_t result;


    TT_TEST_CASE_ENTER()
    // test start

    tt_evc_attr_default(&evc_attr);
    evc_attr.on_init = __ut1_on_init;

    result = tt_evc_create(&evc, TT_FALSE, &evc_attr);
    TT_UT_EQUAL(result, TT_SUCCESS, "");

    result = tt_evc_wait(&evc);
    TT_UT_EQUAL(result, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
