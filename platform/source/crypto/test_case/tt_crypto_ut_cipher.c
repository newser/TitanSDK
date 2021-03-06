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
TT_TEST_ROUTINE_DECLARE(case_cipher)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(crypto_cipher_case)

TT_TEST_CASE("case_cipher",
             "crypto: message digest",
             case_cipher,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(crypto_cipher_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(CRYPTO_UT_CIPHER, 0, crypto_cipher_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_cipher)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    typedef struct
{
    tt_bool_t all_in;
    tt_u32_t block_size;
    tt_cipher_type_t type;
    const tt_char_t *key;
    const tt_char_t *in;
    const tt_char_t *out;
    const tt_char_t *iv;
    tt_cipher_padding_t pad;
    const tt_char_t *ad;
    const tt_char_t *tag;
    tt_bool_t no_update;
} __cipher_tv_t;

// md test vector
static __cipher_tv_t __cipher_tv[] = {
    {
        TT_TRUE,
        0,
        TT_AES_128_ECB,
        "1234567890abcfef1234567890abcfef",
        "31323334353637383930616263646566",
        "3416dd3d15cbae9632eb3cc2feac788c",
        NULL,
        TT_PADDING_NUM,
    },
    {
        TT_FALSE,
        0,
        TT_AES_256_CBC,
        "1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef",
        "31323334353637383930616263646566",
        "2148c0aee99a6d7354e258929158c1747d7d55f05d64d062422feb450020ace9",
        "0987654321fedcba0987654321fedcba",
        TT_PADDING_PKCS7,
    },
    {
        TT_FALSE,
        0,
        TT_AES_192_CFB128,
        "8E73B0F7DA0E6452C810F32B809079E562F8EAD2522C6B7B",
        "6b",
        "cd",
        "000102030405060708090A0B0C0D0E0F",
        TT_PADDING_NUM,
    },
    {
        TT_FALSE,
        0,
        TT_AES_192_CFB128,
        "8E73B0F7DA0E6452C810F32B809079E562F8EAD2522C6B7B",
        "",
        "",
        "000102030405060708090A0B0C0D0E0F",
        TT_PADDING_NUM,
    },
    {
        TT_FALSE,
        0,
        TT_AES_192_CFB128,
        "8E73B0F7DA0E6452C810F32B809079E562F8EAD2522C6B7B",
        "",
        "",
        "000102030405060708090A0B0C0D0E0F",
        TT_PADDING_NUM,
    },
    {
        TT_FALSE,
        0,
        TT_AES_256_CTR,
        "603DEB1015CA71BE2B73AEF0857D77811F352C073B6108D72D9810A30914DFF4",
        "6BC1BEE22E409F96E93D7E117393172AAE2D8A571E03AC9C9EB76FAC45AF8E5130",
        "601EC313775789A5B7A7F504BBF3D228F443E3CA4D62B59ACA84E990CACAF5C52B",
        "F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF",
        TT_PADDING_NUM,
    },
    {
        TT_FALSE,
        16,
        TT_AES_128_GCM,
        "AD7A2BD03EAC835A6F620FDCB506B345",
        "",
        "",
        "12153524C0895E81B2C28465",
        TT_PADDING_NUM,
        "D609B1F056637A0D46DF998D88E5222AB2C2846512153524C0895E8108000F10111213"
        "1415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F3031323334000"
        "1",
        "F09478A9B09007D06F46E9B6A1DA25DD",
    },
    {
        TT_FALSE,
        16,
        TT_AES_256_GCM,
        "691D3EE909D7F54167FD1CA0B5D769081F2BDE1AEE655FDBAB80BD5295AE6BE7",
        "08000F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F"
        "30313233340004",
        "C1623F55730C93533097ADDAD25664966125352B43ADACBD61C5EF3AC90B5BEE929CE4"
        "630EA79F6CE519",
        "F0761E8DCD3D000176D457ED",
        TT_PADDING_NUM,
        "E20106D7CD0DF0761E8DCD3D88E54C2A76D457ED",
        "12AF39C2D1FDC2051F8B7B3C9D397EF2",
    },
    {
        TT_TRUE,
        0,
        TT_CAMELLIA_128_ECB,
        "000102030405060708090A0B0C0D0E0F",
        "00112233445566778899AABBCCDDEEFF",
        "77CF412067AF8270613529149919546F",
        NULL,
        TT_PADDING_NUM,
    },
    {
        TT_FALSE,
        0,
        TT_CAMELLIA_192_CFB128,
        "8E73B0F7DA0E6452C810F32B809079E562F8EAD2522C6B7B",
        "6BC1BEE22E409F96E93D7E117393172A",
        "C832BB9780677DAA82D9B6860DCD565E",
        "000102030405060708090A0B0C0D0E0F",
        TT_PADDING_NUM,
    },
    {
        TT_FALSE,
        0,
        TT_CAMELLIA_256_CTR,
        "FF7A617CE69148E4F1726E2F43581DE2AA62D9F805532EDFF1EED687FB54153D",
        "000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122"
        "23",
        "A4DA23FCE6A5FFAA6D64AE9A0652A42CD161A34B65F9679F75C01F101F71276F15EF0D"
        "8D",
        "001CC5B751A51D70A1C1114800000001",
        TT_PADDING_NUM,
    },
    {
        TT_FALSE,
        16,
        TT_CAMELLIA_256_GCM,
        "feffe9928665731c6d6a8f9467308308feffe9928665731c6d6a8f9467308308",
        "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c0c"
        "95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39",
        "ad142c11579dd95e41f3c1f324dabc255864d920f1b65759d8f560d4948d447758dfdc"
        "f77aa9f62581c7ff572a037f810cb1a9c4b3ca6ed638179b77",
        "cafebabefacedbaddecaf888",
        TT_PADDING_NUM,
        "feedfacedeadbeeffeedfacedeadbeefabaddad2",
        "4e4b178d8fe26fdc95e2e7246dd94bec",
    },
    {
        TT_TRUE,
        0,
        TT_DES_ECB,
        "8000000000000000",
        "0000000000000000",
        "95A8D72813DAA94D",
        NULL,
        TT_PADDING_NUM,
    },
    {
        TT_FALSE,
        0,
        TT_DES_EDE_CBC,
        "752878397493CB7034E5467B9594406D",
        "112233445566778899AABBCCDDEEFF00",
        "743F140CF372B33C71B8AA2AF2D4976B34A268F29B18F36E",
        "78E95ABF0B8EE2C0",
        TT_PADDING_PKCS7,
    },
    {
        TT_FALSE,
        0,
        TT_DES_EDE3_CBC,
        "0123456789ABCDEF23456789ABCDEF01456789ABCDEF0123",
        "6BC1BEE22E409F96E93D7E117393172AAE2D8A571E03AC9C9EB76FAC45AF8E51",
        "2079C3D53AA763E193B79E2569AB5262516570481F25B50F73C0BDA85C8E0DA7",
        "F69F2445DF4F9B17",
        TT_PADDING_NONE,
    },
    {
        TT_TRUE,
        0,
        TT_BLOWFISH_ECB,
        "86C8A06606C49AD9E3A4",
        "1122334455667788",
        "89BAFB5ED18C1EB4",
        NULL,
        TT_PADDING_NUM,
    },
    {
        TT_TRUE,
        0,
        TT_BLOWFISH_CFB64,
        "7581938532D4F8F8856F6F42BBDF3CC2B9AC751B",
        "58C0D1E7F6205FEB3564D4E0221202FD112233445566778899aabbccddeeff00",
        "45B8AB913430069A2D953A83941E3D8EB8F5B869C359BC927AC950076E72F0A9",
        "0102030405060708",
        TT_PADDING_NUM,
    },
    {
        TT_TRUE,
        0,
        TT_ARC4_128,
        "0102030405060708090a0b0c0d0e0f10",
        "3132333435363738393031323334353637383930",
        "abf5ffae55ab29cf8ba319abfed02ea16570fda5",
        NULL,
        TT_PADDING_NUM,
    },
    {
        TT_FALSE,
        0,
        TT_AES_128_CCM,
        "404142434445464748494A4B4C4D4E4F",
        "202122232425262728292A2B2C2D2E2F",
        "D2A1F0E051EA5F62081A7792073D593D",
        "1011121314151617",
        TT_PADDING_NUM,
        "000102030405060708090A0B0C0D0E0F",
        "1FC64FBFACCD",
        TT_TRUE,
    },
    {
        TT_FALSE,
        0,
        TT_AES_128_CCM,
        "404142434445464748494A4B4C4D4E4F",
        "202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F404142"
        "434445464748494A4B4C4D4E4F505152535455565758595A5B5C5D5E5F",
        "7162015BC051951E5918AEAF3C11F3D4AC363F8D5B6AF3D369603B04F24CAE29964E2F"
        "2BF9D31143F72527CE2DB402EAB7660E4A10B08E82266517CDF60267F9",
        "10111213141516",
        TT_PADDING_NUM,
        NULL,
        "C66B655C",
        TT_TRUE,
    },
    {
        TT_FALSE,
        0,
        TT_CAMELLIA_128_CCM,
        "C0C1C2C3C4C5C6C7C8C9CACBCCCDCECF",
        "0C0D0E0F101112131415161718191A1B1C1D1E1F",
        "2AD3BAD94FC52E92BE438E827C1023B96A8A7725",
        "00000007060504A0A1A2A3A4A5",
        TT_PADDING_NUM,
        "000102030405060708090A0B",
        "8FA17BA7F331DB09",
        TT_TRUE,
    },
    {
        TT_FALSE,
        0,
        TT_CAMELLIA_128_CCM,
        "C0C1C2C3C4C5C6C7C8C9CACBCCCDCECF",
        "0C0D0E0F101112131415161718191A1B1C1D1E1F20",
        "FEA5480BA53FA8D3C34422AACE4DE67FFA3BB73BAB",
        "00000008070605A0A1A2A3A4A5",
        TT_PADDING_NUM,
        "000102030405060708090A0B",
        "AB36A1EE4FE0FE28",
        TT_TRUE,
    },
};

TT_TEST_ROUTINE_DEFINE(case_cipher)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_cipher_t c, dec;
    tt_result_t ret;
    tt_u32_t i, j;
    tt_buf_t kbuf, ibuf, obuf, ebuf, dbuf, ivbuf, adbuf, tagbuf;

    TT_TEST_CASE_ENTER()
    // test start

    tt_buf_init(&kbuf, NULL);
    tt_buf_init(&ibuf, NULL);
    tt_buf_init(&obuf, NULL);
    tt_buf_init(&ebuf, NULL);
    tt_buf_init(&dbuf, NULL);
    tt_buf_init(&ivbuf, NULL);
    tt_buf_init(&adbuf, NULL);
    tt_buf_init(&tagbuf, NULL);

    for (i = 0; i < sizeof(__cipher_tv) / sizeof(__cipher_tv[0]); ++i) {
        __cipher_tv_t *tv = &__cipher_tv[i];
        tt_u32_t ilen;

        tt_buf_clear(&kbuf);
        tt_buf_put_cstr2hex(&kbuf, tv->key);

        tt_buf_clear(&ibuf);
        tt_buf_put_cstr2hex(&ibuf, tv->in);
        ilen = TT_BUF_RLEN(&ibuf);

        tt_buf_clear(&obuf);
        tt_buf_put_cstr2hex(&obuf, tv->out);

        tt_buf_clear(&adbuf);
        if (tv->ad != NULL) {
            tt_buf_put_cstr2hex(&adbuf, tv->ad);
        }

        tt_buf_clear(&tagbuf);
        if (tv->tag != NULL) {
            tt_buf_put_cstr2hex(&tagbuf, tv->tag);
        }

        if (tv->iv != NULL) {
            tt_buf_clear(&ivbuf);
            tt_buf_put_cstr2hex(&ivbuf, tv->iv);
        }

        if (tv->no_update) {
            goto tag_auth;
        }

        tt_cipher_init(&c);
        tt_cipher_init(&dec);

        ret = tt_cipher_setup(&c,
                              tv->type,
                              TT_TRUE,
                              TT_BUF_RPOS(&kbuf),
                              TT_BUF_RLEN(&kbuf));
        TT_UT_SUCCESS(ret, "");
        ret = tt_cipher_setup(&dec,
                              tv->type,
                              TT_FALSE,
                              TT_BUF_RPOS(&kbuf),
                              TT_BUF_RLEN(&kbuf));
        TT_UT_SUCCESS(ret, "");

        if (tv->iv != NULL) {
            ret =
                tt_cipher_set_iv(&c, TT_BUF_RPOS(&ivbuf), TT_BUF_RLEN(&ivbuf));
            TT_UT_SUCCESS(ret, "");
            ret = tt_cipher_set_iv(&dec,
                                   TT_BUF_RPOS(&ivbuf),
                                   TT_BUF_RLEN(&ivbuf));
            TT_UT_SUCCESS(ret, "");
        }

        if (!tt_buf_empty(&adbuf)) {
            ret =
                tt_cipher_set_aad(&c, TT_BUF_RPOS(&adbuf), TT_BUF_RLEN(&adbuf));
            TT_UT_SUCCESS(ret, "");

            ret = tt_cipher_set_aad(&dec,
                                    TT_BUF_RPOS(&adbuf),
                                    TT_BUF_RLEN(&adbuf));
            TT_UT_SUCCESS(ret, "");
        }

        if (tv->pad != TT_PADDING_NUM) {
            ret = tt_cipher_set_pad(&c, tv->pad);
            TT_UT_SUCCESS(ret, "");
            ret = tt_cipher_set_pad(&dec, tv->pad);
            TT_UT_SUCCESS(ret, "");
        }

        // encrypt
        tt_buf_clear(&ebuf);
        if (ilen == 0 || tv->all_in) {
            ret = tt_cipher_update_buf(&c,
                                       TT_BUF_RPOS(&ibuf),
                                       TT_BUF_RLEN(&ibuf),
                                       &ebuf);
            TT_UT_SUCCESS(ret, "");
        } else if (tv->block_size != 0) {
            tt_u8_t *in = TT_BUF_RPOS(&ibuf);
            tt_u32_t in_len = TT_BUF_RLEN(&ibuf);
            for (j = 0; j + tv->block_size < in_len;) {
                ret = tt_cipher_update_buf(&c, in + j, tv->block_size, &ebuf);
                TT_UT_SUCCESS(ret, "");
                j += tv->block_size;
            }
            if (j < in_len) {
                ret = tt_cipher_update_buf(&c, in + j, in_len - j, &ebuf);
                TT_UT_SUCCESS(ret, "");
            }
        } else {
            tt_u8_t *in = TT_BUF_RPOS(&ibuf);
            tt_u32_t in_len = TT_BUF_RLEN(&ibuf);
            for (j = 0; j < in_len;) {
                tt_u32_t n = tt_rand_u32() % (in_len - j) + 1;

                ret = tt_cipher_update_buf(&c, in + j, n, &ebuf);
                TT_UT_SUCCESS(ret, "");
                j += n;
            }
            TT_ASSERT(j == in_len);
        }
        ret = tt_cipher_finish_buf(&c, &ebuf);
        TT_UT_SUCCESS(ret, "");

        TT_UT_EQUAL(tt_buf_cmp(&ebuf, &obuf), 0, "");

        if (!tt_buf_empty(&tagbuf)) {
            tt_u8_t tag[100];
            tt_u32_t tag_len = TT_BUF_RLEN(&tagbuf);

            ret = tt_cipher_tag(&c, tag, tag_len);
            TT_UT_SUCCESS(ret, "");
            TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&tagbuf), tag, tag_len), 0, "");
        }

        // decrypt
        tt_buf_clear(&dbuf);
        if (ilen == 0 || tv->all_in) {
            ret = tt_cipher_update_buf(&dec,
                                       TT_BUF_RPOS(&ebuf),
                                       TT_BUF_RLEN(&ebuf),
                                       &dbuf);
            TT_UT_SUCCESS(ret, "");
        } else if (tv->block_size != 0) {
            tt_u8_t *in = TT_BUF_RPOS(&ebuf);
            tt_u32_t in_len = TT_BUF_RLEN(&ebuf);
            for (j = 0; j + tv->block_size < in_len;) {
                ret = tt_cipher_update_buf(&dec, in + j, tv->block_size, &dbuf);
                TT_UT_SUCCESS(ret, "");
                j += tv->block_size;
            }
            if (j < in_len) {
                ret = tt_cipher_update_buf(&dec, in + j, in_len - j, &dbuf);
                TT_UT_SUCCESS(ret, "");
            }
        } else {
            tt_u8_t *in = TT_BUF_RPOS(&ebuf);
            tt_u32_t in_len = TT_BUF_RLEN(&ebuf);
            for (j = 0; j < in_len;) {
                tt_u32_t n = tt_rand_u32() % (in_len - j) + 1;

                ret = tt_cipher_update_buf(&dec, in + j, n, &dbuf);
                TT_UT_SUCCESS(ret, "");
                j += n;
            }
            TT_ASSERT(j == in_len);
        }
        ret = tt_cipher_finish_buf(&dec, &dbuf);
        TT_UT_SUCCESS(ret, "");

        TT_UT_EQUAL(tt_buf_cmp(&dbuf, &ibuf), 0, "");

        if (!tt_buf_empty(&tagbuf)) {
            ret = tt_cipher_auth(&dec,
                                 TT_BUF_RPOS(&tagbuf),
                                 TT_BUF_RLEN(&tagbuf));
            TT_UT_SUCCESS(ret, "");
        }

        tt_cipher_destroy(&c);
        tt_cipher_destroy(&dec);

    //////////////////////////////////
    //////////////////////////////////

    tag_auth:
        if (!tt_buf_empty(&tagbuf)) {
            tt_u8_t tag[100];
            tt_u32_t tag_len = TT_BUF_RLEN(&tagbuf);

            tt_cipher_init(&c);
            tt_cipher_init(&dec);

            ret = tt_cipher_setup(&c,
                                  tv->type,
                                  TT_TRUE,
                                  TT_BUF_RPOS(&kbuf),
                                  TT_BUF_RLEN(&kbuf));
            TT_UT_SUCCESS(ret, "");
            ret = tt_cipher_setup(&dec,
                                  tv->type,
                                  TT_FALSE,
                                  TT_BUF_RPOS(&kbuf),
                                  TT_BUF_RLEN(&kbuf));
            TT_UT_SUCCESS(ret, "");

            if (tv->pad != TT_PADDING_NUM) {
                ret = tt_cipher_set_pad(&c, tv->pad);
                TT_UT_SUCCESS(ret, "");
                ret = tt_cipher_set_pad(&dec, tv->pad);
                TT_UT_SUCCESS(ret, "");
            }

            tt_buf_clear(&ebuf);
            ret = tt_cipher_encrypt_tag_buf(&c,
                                            TT_BUF_RPOS(&ivbuf),
                                            TT_BUF_RLEN(&ivbuf),
                                            TT_BUF_RPOS(&adbuf),
                                            TT_BUF_RLEN(&adbuf),
                                            TT_BUF_RPOS(&ibuf),
                                            TT_BUF_RLEN(&ibuf),
                                            &ebuf,
                                            tag,
                                            tag_len);
            TT_UT_SUCCESS(ret, "");
            TT_UT_EQUAL(tt_buf_cmp(&ebuf, &obuf), 0, "");
            TT_UT_EQUAL(tt_memcmp(TT_BUF_RPOS(&tagbuf), tag, tag_len), 0, "");

            tt_buf_clear(&dbuf);
            ret = tt_cipher_decrypt_auth_buf(&c,
                                             TT_BUF_RPOS(&ivbuf),
                                             TT_BUF_RLEN(&ivbuf),
                                             TT_BUF_RPOS(&adbuf),
                                             TT_BUF_RLEN(&adbuf),
                                             TT_BUF_RPOS(&ebuf),
                                             TT_BUF_RLEN(&ebuf),
                                             &dbuf,
                                             tag,
                                             tag_len);
            TT_UT_SUCCESS(ret, "");
            TT_UT_EQUAL(tt_buf_cmp(&dbuf, &ibuf), 0, "");

            tt_cipher_destroy(&c);
            tt_cipher_destroy(&dec);
        }
    }

    tt_buf_destroy(&kbuf);
    tt_buf_destroy(&ibuf);
    tt_buf_destroy(&obuf);
    tt_buf_destroy(&ebuf);
    tt_buf_destroy(&dbuf);
    tt_buf_destroy(&ivbuf);
    tt_buf_destroy(&adbuf);
    tt_buf_destroy(&tagbuf);

    // test end
    TT_TEST_CASE_LEAVE()
}
