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
TT_TEST_ROUTINE_DECLARE(case_zsrc_blob)
TT_TEST_ROUTINE_DECLARE(case_zsrc_readfile)
TT_TEST_ROUTINE_DECLARE(case_zsrc_readfile_off)
TT_TEST_ROUTINE_DECLARE(case_zsrc_writefile)
TT_TEST_ROUTINE_DECLARE(case_zsrc_writefile_off)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(zip_zsrc_case)

TT_TEST_CASE("case_zsrc_blob", "zip source: blob", case_zsrc_blob, NULL, NULL,
             NULL, NULL, NULL)
,

    TT_TEST_CASE("case_zsrc_readfile", "zip source: read file",
                 case_zsrc_readfile, NULL, NULL, NULL, NULL, NULL),

    TT_TEST_CASE("case_zsrc_readfile_off", "zip source: read file with offset",
                 case_zsrc_readfile_off, NULL, NULL, NULL, NULL, NULL),

    TT_TEST_CASE("case_zsrc_writefile", "zip source: write file",
                 case_zsrc_writefile, NULL, NULL, NULL, NULL, NULL),

    TT_TEST_CASE("case_zsrc_writefile_off",
                 "zip source: write file with offset", case_zsrc_writefile_off,
                 NULL, NULL, NULL, NULL, NULL),

    TT_TEST_CASE_LIST_DEFINE_END(zip_zsrc_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(ZIP_UT_ZSOURCE, 0, zip_zsrc_case)

    ////////////////////////////////////////////////////////////
    // interface declaration
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // interface implementation
    ////////////////////////////////////////////////////////////

    /*
    TT_TEST_ROUTINE_DEFINE(case_zsrc_writefile)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_zsrc_blob)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_zipsrc_t *zs;
    tt_char_t buf[100] = "123", obuf[200];
    tt_result_t ret;
    tt_u32_t n;

    TT_TEST_CASE_ENTER()
    // test start

    zs = tt_zipsrc_blob_create(buf, sizeof(buf), TT_FALSE);
    TT_UT_NOT_NULL(zs, "");

    tt_zipsrc_ref(zs);
    tt_zipsrc_ref(zs);
    tt_zipsrc_release(zs);
    tt_zipsrc_release(zs);
    // still has 1 ref

    ret = tt_zipsrc_open(zs);
    TT_UT_SUCCESS(ret, "");

    ret = tt_zipsrc_read(zs, (tt_u8_t *)obuf, sizeof(obuf), &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, 100, "");
    TT_UT_EQUAL(tt_memcmp(buf, obuf, n), 0, "");
    ret = tt_zipsrc_read(zs, (tt_u8_t *)obuf, sizeof(obuf), &n);
    TT_UT_EQUAL(ret, TT_E_END, "");
    TT_UT_EQUAL(n, 0, "");

    tt_zipsrc_release(zs);

    // test end
    TT_TEST_CASE_LEAVE()
}

#if TT_ENV_OS_IS_IOS

#if (TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)
#define Z_FILE "/tmp/zf1"
#else
static tt_string_t zf1_path;
#define Z_FILE tt_string_cstr(&zf1_path)
#endif

#elif TT_ENV_OS_IS_ANDROID
#define Z_FILE "/data/data/com.titansdk.titansdkunittest/zf1"
#else
#define Z_FILE "zf1"
#endif

TT_TEST_ROUTINE_DEFINE(case_zsrc_readfile)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_zipsrc_t *zs;
    tt_u8_t buf[256], obuf[256];
    tt_result_t ret;
    tt_u32_t i, n;
    tt_file_t f;
    tt_zip_stat_t zstat;
    tt_u64_t loc;

    TT_TEST_CASE_ENTER()
    // test start

    tt_fremove(Z_FILE);
    tt_fopen(&f, Z_FILE, TT_FO_CREAT | TT_FO_RDWR, NULL);
    for (i = 0; i < sizeof(buf); ++i) { buf[i] = i; }
    tt_fwrite(&f, buf, sizeof(buf), NULL);
    tt_fclose(&f);

    zs = tt_zipsrc_file_create(Z_FILE, 0, 0);
    TT_UT_NOT_NULL(zs, "");

    // open
    ret = tt_zipsrc_open(zs);
    TT_UT_SUCCESS(ret, "");

    // read
    ret = tt_zipsrc_read(zs, obuf, sizeof(obuf), &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, sizeof(obuf), "");
    TT_UT_EXP((obuf[0] == 0) && (obuf[255] == 255), "");

    ret = tt_zipsrc_tell(zs, &loc);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(loc, sizeof(obuf), "");

    // seek
    ret = tt_zipsrc_seek(zs, TT_ZSSEEK_CUR, -156);
    TT_UT_SUCCESS(ret, "");
    ret = tt_zipsrc_read(zs, obuf, 56, &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, 56, "");
    TT_UT_EXP((obuf[0] == 100) && (obuf[55] == 155), "");

    ret = tt_zipsrc_seek(zs, TT_ZSSEEK_END, -100);
    TT_UT_SUCCESS(ret, "");
    ret = tt_zipsrc_read(zs, obuf, 100, &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, 100, "");
    TT_UT_EXP((obuf[0] == 156) && (obuf[99] == 255), "");

    ret = tt_zipsrc_seek(zs, TT_ZSSEEK_BEGIN, 200);
    TT_UT_SUCCESS(ret, "");
    ret = tt_zipsrc_read(zs, obuf, 56, &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, 56, "");
    TT_UT_EXP((obuf[0] == 200) && (obuf[55] == 255), "");

    ret = tt_zipsrc_read(zs, obuf, sizeof(obuf), &n);
    TT_UT_EQUAL(ret, TT_E_END, "");
    TT_UT_EQUAL(n, 0, "");

    // stat
    ret = tt_zipsrc_stat(zs, &zstat);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(zstat.st.size, sizeof(buf), "");

    // error
    {
        zip_error_t *ze = zip_source_error(zs);
        TT_UT_EQUAL(ze->zip_err, 0, "");
    }

    // close
    tt_zipsrc_close(zs);

    tt_zipsrc_release(zs);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_zsrc_readfile_off)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_zipsrc_t *zs;
    tt_u8_t buf[256], obuf[256];
    tt_result_t ret;
    tt_u32_t i, n;
    tt_file_t f;
    tt_zip_stat_t zstat;
    tt_u64_t loc;

    TT_TEST_CASE_ENTER()
    // test start

    tt_fremove(Z_FILE);
    tt_fopen(&f, Z_FILE, TT_FO_CREAT | TT_FO_RDWR, NULL);
    for (i = 0; i < sizeof(buf); ++i) { buf[i] = i; }
    tt_fwrite(&f, buf, sizeof(buf), NULL);
    tt_fclose(&f);

    zs = tt_zipsrc_file_create(Z_FILE, 100, 100);
    TT_UT_NOT_NULL(zs, "");

    // open
    ret = tt_zipsrc_open(zs);
    TT_UT_SUCCESS(ret, "");

    // read
    ret = tt_zipsrc_read(zs, obuf, 100, &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, 100, "");
    TT_UT_EXP((obuf[0] == 100) && (obuf[99] == 199), "");

    ret = tt_zipsrc_tell(zs, &loc);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(loc, 100, "");

    // seek
    ret = tt_zipsrc_seek(zs, TT_ZSSEEK_CUR, -50);
    TT_UT_SUCCESS(ret, "");
    ret = tt_zipsrc_read(zs, obuf, 10, &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, 10, "");
    TT_UT_EXP((obuf[0] == 150) && (obuf[9] == 159), "");

    ret = tt_zipsrc_seek(zs, TT_ZSSEEK_END, -80);
    TT_UT_SUCCESS(ret, "");
    ret = tt_zipsrc_read(zs, obuf, 80, &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, 80, "");
    TT_UT_EXP((obuf[0] == 120) && (obuf[79] == 199), "");

    ret = tt_zipsrc_seek(zs, TT_ZSSEEK_BEGIN, 10);
    TT_UT_SUCCESS(ret, "");
    ret = tt_zipsrc_read(zs, obuf, 90, &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, 90, "");
    TT_UT_EXP((obuf[0] == 110) && (obuf[89] == 199), "");

    ret = tt_zipsrc_read(zs, obuf, sizeof(obuf), &n);
    TT_UT_EQUAL(ret, TT_E_END, "");
    TT_UT_EQUAL(n, 0, "");

    // stat
    ret = tt_zipsrc_stat(zs, &zstat);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(zstat.st.size, 100, "");

    // error
    {
        zip_error_t *ze = zip_source_error(zs);
        TT_UT_EQUAL(ze->zip_err, 0, "");
    }

    ret = tt_zipsrc_seek(zs, TT_ZSSEEK_BEGIN, 110);
    TT_UT_FAIL(ret, "");

    // close
    tt_zipsrc_close(zs);

    tt_zipsrc_release(zs);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_zsrc_writefile)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_zipsrc_t *zs;
    tt_u8_t buf[256], obuf[256];
    tt_result_t ret;
    tt_u32_t i, n;
    tt_file_t f;
    tt_u64_t loc;

    TT_TEST_CASE_ENTER()
    // test start

    tt_fremove(Z_FILE);

    for (i = 0; i < sizeof(buf); ++i) { buf[i] = i; }

    zs = tt_zipsrc_file_create(Z_FILE, 0, 0);
    TT_UT_NOT_NULL(zs, "");

    // begin write
    ret = tt_zipsrc_begin_write(zs);
    TT_UT_SUCCESS(ret, "");

    // rollback write
    tt_zipsrc_rollback_write(zs);
    TT_UT_EQUAL(tt_fs_exist(Z_FILE), TT_FALSE, "");

    ret = tt_zipsrc_begin_write(zs);
    TT_UT_SUCCESS(ret, "");

    ret = tt_zipsrc_tell_write(zs, &loc);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(loc, 0, "");

    ret = tt_zipsrc_seek_write(zs, TT_ZSSEEK_BEGIN, 0);
    TT_UT_SUCCESS(ret, "");

    // write
    ret = tt_zipsrc_write(zs, buf, sizeof(buf), &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, sizeof(buf), "");

    tt_memset(obuf, 'a', 10);
    ret = tt_zipsrc_seek_write(zs, TT_ZSSEEK_BEGIN, 0);
    TT_UT_SUCCESS(ret, "");
    ret = tt_zipsrc_write(zs, obuf, 10, &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, 10, "");

    ret = tt_zipsrc_tell_write(zs, &loc);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(loc, 10, "");

    tt_memset(obuf, 'b', 20);
    ret = tt_zipsrc_seek_write(zs, TT_ZSSEEK_CUR, 90);
    TT_UT_SUCCESS(ret, "");
    ret = tt_zipsrc_write(zs, obuf, 20, &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, 20, "");

    ret = tt_zipsrc_tell_write(zs, &loc);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(loc, 120, "");

    tt_memset(obuf, 'c', 30);
    ret = tt_zipsrc_seek_write(zs, TT_ZSSEEK_END, -56);
    TT_UT_SUCCESS(ret, "");
    ret = tt_zipsrc_write(zs, obuf, 30, &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, 30, "");

    ret = tt_zipsrc_tell_write(zs, &loc);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(loc, 230, "");

    ret = tt_zipsrc_commit_write(zs);
    TT_UT_SUCCESS(ret, "");

    tt_zipsrc_release(zs);

    // check
    TT_UT_EQUAL(tt_fs_exist(Z_FILE), TT_TRUE, "");
    tt_fopen(&f, Z_FILE, TT_FO_READ, NULL);

    ret = tt_fread(&f, obuf, sizeof(obuf), &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, sizeof(obuf), "");
    ret = tt_fread(&f, obuf, sizeof(obuf), &n);
    TT_UT_EQUAL(ret, TT_E_END, "");

    TT_UT_EXP((obuf[0] == 'a') && (obuf[9] == 'a'), "");
    TT_UT_EXP((obuf[10] == 10) && (obuf[99] == 99), "");
    TT_UT_EXP((obuf[100] == 'b') && (obuf[119] == 'b'), "");
    TT_UT_EXP((obuf[120] == 120) && (obuf[199] == 199), "");
    TT_UT_EXP((obuf[200] == 'c') && (obuf[229] == 'c'), "");
    TT_UT_EXP((obuf[230] == 230) && (obuf[255] == 255), "");

    tt_fclose(&f);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_zsrc_writefile_off)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_zipsrc_t *zs;
    tt_u8_t buf[256];
    tt_result_t ret;
    tt_u32_t i, n;
    tt_u64_t loc;

    TT_TEST_CASE_ENTER()
    // test start

    tt_fremove(Z_FILE);

    for (i = 0; i < sizeof(buf); ++i) { buf[i] = i; }

    zs = tt_zipsrc_file_create(Z_FILE, 100, 256);
    TT_UT_NOT_NULL(zs, "");

    // begin write
    ret = tt_zipsrc_begin_write(zs);
    TT_UT_FAIL(ret, "");

    // rollback write
    tt_zipsrc_rollback_write(zs);
    TT_UT_EQUAL(tt_fs_exist(Z_FILE), TT_FALSE, "");

    ret = tt_zipsrc_commit_write(zs);
    TT_UT_FAIL(ret, "");

    ret = tt_zipsrc_tell_write(zs, &loc);
    TT_UT_FAIL(ret, "");

    ret = tt_zipsrc_seek_write(zs, TT_ZSSEEK_BEGIN, 0);
    TT_UT_FAIL(ret, "");

    // write
    ret = tt_zipsrc_write(zs, buf, sizeof(buf), &n);
    TT_UT_FAIL(ret, "");

    tt_zipsrc_release(zs);

    // test end
    TT_TEST_CASE_LEAVE()
}
