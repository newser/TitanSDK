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
#include <zip/tt_zip.h>
#include <zip/tt_zip_file.h>
#include <zip/tt_zip_source.h>
#include <zip/tt_zip_source_blob.h>
#include <zip/tt_zip_source_file.h>

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
TT_TEST_ROUTINE_DECLARE(case_zarc_write_blob)
TT_TEST_ROUTINE_DECLARE(case_zarc_write_file)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(zip_zarc_case)

TT_TEST_CASE("case_zarc_write_blob",
             "zip archive: write blob",
             case_zarc_write_blob,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_zarc_write_file",
                 "zip archive: write file",
                 case_zarc_write_file,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(zip_zarc_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(ZIP_UT_ZARC, 0, zip_zarc_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_zarc_write_file)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_zarc_write_blob)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_zipsrc_t *zs, *zsf1, *zsf2;
    tt_zip_t *za;
    tt_u8_t buf[100], ibuf1[100], ibuf2[100], *data;
    tt_u32_t i, d1, d2, f1, f2;
    tt_result_t ret;
    tt_zip_stat_t zstat;
    tt_zipfile_t *zf;

    TT_TEST_CASE_ENTER()
    // test start

    zs = tt_zipsrc_blob_create(NULL, 0, TT_FALSE);
    TT_UT_NOT_NULL(zs, "");

    za = tt_zip_create(zs, 0, NULL);
    TT_UT_NOT_NULL(za, "");
    tt_zipsrc_ref(zs);

    // add a dir
    d1 = tt_zip_add_dir(za, "dir1", 0);
    TT_UT_NOT_EQUAL(d1, TT_POS_NULL, "");

    // add a dir
    d2 = tt_zip_add_dir(za, "dir1/dir2", 0);
    TT_UT_NOT_EQUAL(d1, TT_POS_NULL, "");
    (void)d2;
    (void)f2;

    // add a file
    for (i = 0; i < sizeof(ibuf1); ++i) {
        ibuf1[i] = i;
        ibuf2[i] = sizeof(ibuf2) - i;
    }

    zsf1 = tt_zipsrc_blob_create(ibuf1, sizeof(ibuf1), TT_FALSE);
    TT_UT_NOT_NULL(zsf1, "");
    f1 = tt_zip_add_file(za, "dir1/dir2/f1", zsf1, 0);
    TT_UT_NOT_EQUAL(f1, TT_POS_NULL, "");

    zsf2 = tt_zipsrc_blob_create(ibuf2, sizeof(ibuf2), TT_FALSE);
    TT_UT_NOT_NULL(zsf2, "");
    f2 = tt_zip_add_file(za, "dir1/f2", zsf2, 0);
    TT_UT_NOT_EQUAL(f1, TT_POS_NULL, "");

    {
        const tt_char_t *c;

        c = tt_zip_get_comment(za, 0);
        TT_UT_EQUAL(c[0], 0, "");

        ret = tt_zip_set_comment(za, "123", 2);
        TT_UT_SUCCESS(ret, "");
        c = tt_zip_get_comment(za, 0);
        TT_UT_EQUAL(tt_strcmp(c, "12"), 0, "");

        ret = tt_zip_set_comment(za, "", 0);
        TT_UT_SUCCESS(ret, "");
        c = tt_zip_get_comment(za, 0);
        TT_UT_EQUAL(c[0], 0, "");

        c = tt_zip_get_fcomment(za, 3, 0);
        TT_UT_EQUAL(c[0], 0, "");

        ret = tt_zip_set_fcomment(za, 3, "123", 2, 0);
        TT_UT_SUCCESS(ret, "");
        c = tt_zip_get_fcomment(za, 3, 0);
        TT_UT_EQUAL(tt_strcmp(c, "12"), 0, "");
    }

    tt_zip_destroy(za, TT_TRUE);

    // compressed
    ret = tt_zipsrc_stat(zs, &zstat);
    TT_UT_SUCCESS(ret, "");
    TT_INFO("zip size: %d", (tt_u32_t)zstat.st.size);

    data = tt_malloc(zstat.st.size);

    ret = tt_zipsrc_open(zs);
    TT_UT_SUCCESS(ret, "");
    ret = tt_zipsrc_read(zs, data, (tt_u32_t)zstat.st.size, &i);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(i, zstat.st.size, "");
    ret = tt_zipsrc_read(zs, data, (tt_u32_t)zstat.st.size, &i);
    TT_UT_EQUAL(ret, TT_E_END, "");
    tt_zipsrc_close(zs);

    tt_zipsrc_release(zs);

    // uncompress
    zs = tt_zipsrc_blob_create(data, (tt_u32_t)zstat.st.size, TT_TRUE);
    TT_UT_NOT_NULL(zs, "");

    za = tt_zip_create(zs, 0, NULL);
    TT_UT_NOT_NULL(za, "");
    TT_UT_EQUAL(tt_zip_count(za, 0), 4, "");
    TT_UT_STREQ(tt_zip_get_fname(za, 0, 0), "dir1/", "");
    TT_UT_STREQ(tt_zip_get_fname(za, 1, 0), "dir1/dir2/", "");
    TT_UT_STREQ(tt_zip_get_fname(za, 2, 0), "dir1/dir2/f1", "");
    TT_UT_STREQ(tt_zip_get_fname(za, 3, 0), "dir1/f2", "");
    TT_UT_NULL(tt_zip_get_fname(za, 4, 0), "");

    // fil1
    zf = tt_zipfile_open(za, "dir1/dir2/f1", 0, NULL);
    TT_UT_NOT_NULL(zf, "");

    // read file 1
    ret = tt_zipfile_read(zf, buf, sizeof(buf), &i);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(i, sizeof(buf), "");
    TT_UT_MEMEQ(buf, ibuf1, sizeof(ibuf1), "");
    ret = tt_zipfile_read(zf, buf, sizeof(buf), &i);
    TT_UT_EQUAL(ret, TT_E_END, "");

    tt_zipfile_close(zf);

    // file2
    zf = tt_zipfile_open_index(za, 3, 0, NULL);
    TT_UT_NOT_NULL(zf, "");

    // read file 2
    ret = tt_zipfile_read(zf, buf, sizeof(buf), &i);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(i, sizeof(buf), "");
    TT_UT_MEMEQ(buf, ibuf2, sizeof(ibuf2), "");
    ret = tt_zipfile_read(zf, buf, sizeof(buf), &i);
    TT_UT_EQUAL(ret, TT_E_END, "");

    tt_zipfile_close(zf);

    tt_zip_destroy(za, TT_FALSE);

    // test end
    TT_TEST_CASE_LEAVE()
}

#if TT_ENV_OS_IS_IOS

#if (TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)
#define Z_FILE "/tmp/zf1"
#define Z_FILE2 "/tmp/zf2"
#define Z_ARCH "/tmp/z.zip"
#else
static tt_string_t zf1_path, zf2_path, za_path;
#define Z_FILE tt_string_cstr(&zf1_path)
#define Z_FILE tt_string_cstr(&zf2_path)
#define Z_ARCH tt_string_cstr(za_path)
#endif

#elif TT_ENV_OS_IS_ANDROID
#define Z_FILE "/data/data/com.titansdk.titansdkunittest/zf1"
#define Z_FILE2 "/data/data/com.titansdk.titansdkunittest/zf2"
#define Z_ARCH "/data/data/com.titansdk.titansdkunittest/z.zip"
#else
#define Z_FILE "zf1"
#define Z_FILE2 "zf2"
#define Z_ARCH "z.zip"
#endif

TT_TEST_ROUTINE_DEFINE(case_zarc_write_file)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_zipsrc_t *zs;
    tt_zip_t *za;
    tt_result_t ret;
    tt_file_t f;
    tt_u32_t i;
    tt_date_t d;
    tt_zipfile_t *zf;
    tt_zipfile_attr_t zattr;

    TT_TEST_CASE_ENTER()
    // test start

    tt_fremove(Z_FILE);
    tt_fopen(&f, Z_FILE, TT_FO_CREAT | TT_FO_WRITE, NULL);
    tt_fwrite(&f, (tt_u8_t *)"123", 3, NULL);
    tt_fclose(&f);

    tt_fremove(Z_FILE2);
    tt_fopen(&f, Z_FILE2, TT_FO_CREAT | TT_FO_WRITE, NULL);
    tt_fwrite(&f, (tt_u8_t *)"666666", 6, NULL);
    tt_fclose(&f);

    // for write
    tt_fremove(Z_ARCH);
    zs = tt_zipsrc_file_create(Z_ARCH, 0, 0);
    TT_UT_NOT_NULL(zs, "");
    za = tt_zip_create(zs, TT_ZA_CREAT, NULL);
    TT_UT_NOT_NULL(za, "");

    // add file1
    zs = tt_zipsrc_file_create(Z_FILE, 0, 0);
    TT_UT_NOT_NULL(zs, "");
    i = tt_zip_add_file(za, Z_FILE, zs, 0);
    TT_UT_EQUAL(i, 0, "");

    // add file2
    zs = tt_zipsrc_file_create(Z_FILE2, 0, 0);
    TT_UT_NOT_NULL(zs, "");
    i = tt_zip_add_file(za, Z_FILE2, zs, 0);
    TT_UT_EQUAL(i, 1, "");

    TT_UT_EQUAL(tt_zip_count(za, 0), 2, "");
    TT_UT_EQUAL(tt_zip_find(za, Z_FILE, 0), 0, "");
    TT_UT_EQUAL(tt_zip_find(za, Z_FILE2, 0), 1, "");

    ret = tt_zip_set_fcipher(za, 0, TT_ZIP_CIPHER_AES256, "z1");
    TT_UT_SUCCESS(ret, "");
    tt_date_now(&d);
    ret = tt_zip_set_fmtime(za, 0, &d);
    TT_UT_SUCCESS(ret, "");
    ret = tt_zip_set_fcomment(za, 0, "password is z1", 0, 0);
    TT_UT_SUCCESS(ret, "");

    ret = tt_zip_set_fcipher(za, 1, TT_ZIP_CIPHER_AES256, "z1");
    TT_UT_SUCCESS(ret, "");
    tt_date_now(&d);
    ret = tt_zip_set_fmtime(za, 1, &d);
    TT_UT_SUCCESS(ret, "");
    ret = tt_zip_set_fcomment(za, 1, "pwd is z1", 0, 0);
    TT_UT_SUCCESS(ret, "");

    tt_zip_destroy(za, TT_TRUE);

    //////////////////
    // uncompress and modify
    //////////////////

    TT_UT_TRUE(tt_fs_exist(Z_ARCH), "");

    zs = tt_zipsrc_file_create(Z_ARCH, 0, 0);
    TT_UT_NOT_NULL(zs, "");
    za = tt_zip_create(zs, 0, NULL);
    TT_UT_NOT_NULL(za, "");

    TT_UT_EQUAL(tt_zip_count(za, 0), 2, "");
    TT_UT_STREQ(tt_zip_get_fname(za, 0, 0), Z_FILE, "");
    TT_UT_STREQ(tt_zip_get_fname(za, 1, 0), Z_FILE2, "");

    // read file1
    tt_zipfile_attr_default(&zattr);
    zattr.password = "z1";
    zf = tt_zipfile_open(za, Z_FILE, 0, &zattr);
    TT_UT_NOT_NULL(zf, "");
    {
        tt_u8_t b[30] = {0};
        ret = tt_zipfile_read(zf, b, 30, &i);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(i, 3, "");
        TT_UT_STREQ(b, "123", "");
        ret = tt_zipfile_read(zf, b, 30, &i);
        TT_UT_EQUAL(ret, TT_E_END, "");
    }
    tt_zipfile_close(zf);

    // read file2
    tt_zipfile_attr_default(&zattr);
    zattr.password = "z1";
    zf = tt_zipfile_open_index(za, 1, 0, &zattr);
    TT_UT_NOT_NULL(zf, "");
    {
        tt_u8_t b[30] = {0};
        ret = tt_zipfile_read(zf, b, 30, &i);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(i, 6, "");
        TT_UT_STREQ(b, "666666", "");
        ret = tt_zipfile_read(zf, b, 30, &i);
        TT_UT_EQUAL(ret, TT_E_END, "");
    }
    tt_zipfile_close(zf);

    // modify
    ret = tt_zip_rename(za, 1, "renamed", 0);
    TT_UT_SUCCESS(ret, "");
    ret = tt_zip_remove(za, 0);
    TT_UT_SUCCESS(ret, "");

    tt_zip_destroy(za, TT_TRUE);

    //////////////////
    // check modified
    //////////////////

    TT_UT_TRUE(tt_fs_exist(Z_ARCH), "");

    zs = tt_zipsrc_file_create(Z_ARCH, 0, 0);
    TT_UT_NOT_NULL(zs, "");
    za = tt_zip_create(zs, 0, NULL);
    TT_UT_NOT_NULL(za, "");

    TT_UT_EQUAL(tt_zip_count(za, 0), 1, "");
    TT_UT_STREQ(tt_zip_get_fname(za, 0, 0), "renamed", "");

    // read
    tt_zipfile_attr_default(&zattr);
    zattr.password = "z1";
    zf = tt_zipfile_open_index(za, 0, 0, &zattr);
    TT_UT_NOT_NULL(zf, "");
    {
        tt_u8_t b[30] = {0};
        ret = tt_zipfile_read(zf, b, 30, &i);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(i, 6, "");
        TT_UT_STREQ(b, "666666", "");
        ret = tt_zipfile_read(zf, b, 30, &i);
        TT_UT_EQUAL(ret, TT_E_END, "");
    }
    tt_zipfile_close(zf);

    tt_zip_destroy(za, TT_FALSE);

    // test end
    TT_TEST_CASE_LEAVE()
}
