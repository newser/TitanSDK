/*
 * import header files
 */
#include "tt_unit_test_case_config.h"
#include <unit_test/tt_unit_test.h>

#include <log/filter/tt_log_filter.h>
#include <os/tt_fiber.h>
#include <tt_platform.h>

/*
 * local definition
 */

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(case_log_context)
TT_TEST_ROUTINE_DECLARE(case_log_manager)

TT_TEST_ROUTINE_DECLARE(case_log_io_file_index)
TT_TEST_ROUTINE_DECLARE(case_log_io_file_date)
TT_TEST_ROUTINE_DECLARE(case_log_io_file_archive)
TT_TEST_ROUTINE_DECLARE(case_log_io_syslog)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(log_case)

TT_TEST_CASE("case_log_context",
             "testing log context",
             case_log_context,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("case_log_manager",
                 "testing log manager",
                 case_log_manager,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_log_io_file_index",
                 "testing log io file by index",
                 case_log_io_file_index,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_log_io_file_date",
                 "testing log io file by date",
                 case_log_io_file_date,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_log_io_file_archive",
                 "testing log io file archiving",
                 case_log_io_file_archive,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("case_log_io_syslog",
                 "testing log io syslog",
                 case_log_io_syslog,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(log_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_LOG, 0, log_case)

    /*
     * interface implementation
     */


    /*
    TT_TEST_ROUTINE_DEFINE(case_log_io_syslog)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(case_log_context)
{
    tt_loglyt_t *lyt;
    tt_logctx_t ctx;
    tt_result_t ret;
    tt_logio_t *lio;
    tt_log_entry_t entry = {0};

    TT_TEST_CASE_ENTER()
    // test start

    lyt = tt_loglyt_pattern_create(
        "${seq_num} ${level} ${logger} ${content} <${function}:${line}>\n");
    TT_UT_NOT_EQUAL(lyt, NULL, "");

    lio = tt_logio_std_create(NULL);
    TT_UT_NOT_EQUAL(lio, NULL, "");
    TT_UT_EQUAL(lio->itf->type, TT_LOGIO_STANDARD, "");

    ret = tt_logctx_create(&ctx, TT_LOG_FATAL, lyt, NULL);
    TT_UT_SUCCESS(ret, "");

    tt_logctx_input(&ctx, &entry);

    entry.content = "aabbcc";
    tt_logctx_input(&ctx, &entry);

    ret = tt_logctx_append_io(&ctx, lio);
    TT_UT_SUCCESS(ret, "");

    entry.function = __FUNCTION__;
    tt_logctx_input(&ctx, &entry);

    ret = tt_logctx_append_io(&ctx, lio);
    TT_UT_SUCCESS(ret, "");

    entry.line = __LINE__;
    tt_logctx_input(&ctx, &entry);

    entry.logger = "HANIU";
    tt_logctx_input(&ctx, &entry);

    entry.level = TT_LOG_DEBUG;
    tt_logctx_input(&ctx, &entry);

    tt_logctx_destroy(&ctx);
    // tt_loglyt_destroy(lyt);
    tt_logio_release(lio);

    // invalid cases
    ret = tt_logctx_create(&ctx, TT_LOG_LEVEL_NUM, lyt, NULL);
    TT_UT_FAIL(ret, "");

    ret = tt_logctx_create(&ctx, TT_LOG_DEBUG, NULL, NULL);
    // TT_UT_FAIL(ret, "");

    ret = tt_logctx_create(NULL, TT_LOG_DEBUG, lyt, NULL);
    TT_UT_FAIL(ret, "");

    tt_logctx_destroy(NULL);
    tt_logctx_attr_default(NULL);

    ret = tt_logctx_append_io(NULL, lio);
    TT_UT_FAIL(ret, "");

    ret = tt_logctx_append_io(&ctx, NULL);
    TT_UT_FAIL(ret, "");

    ret = tt_logctx_input(NULL, &entry);
    TT_UT_FAIL(ret, "");
    ret = tt_logctx_input(&ctx, NULL);
    TT_UT_FAIL(ret, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_bool_t __test_log(tt_logmgr_t *lm)
{
    tt_logmgr_inputf(lm,
                     TT_LOG_DEBUG,
                     __FUNCTION__,
                     __LINE__,
                     "this is [%s] log",
                     tt_g_log_level_name[TT_LOG_DEBUG]);

    tt_logmgr_inputf(lm,
                     TT_LOG_INFO,
                     __FUNCTION__,
                     __LINE__,
                     "this is [%s] log",
                     tt_g_log_level_name[TT_LOG_INFO]);

    tt_logmgr_inputf(lm,
                     TT_LOG_WARN,
                     __FUNCTION__,
                     __LINE__,
                     "this is [%s] log",
                     tt_g_log_level_name[TT_LOG_WARN]);

    tt_logmgr_inputf(lm,
                     TT_LOG_ERROR,
                     __FUNCTION__,
                     __LINE__,
                     "this is [%s] log",
                     tt_g_log_level_name[TT_LOG_ERROR]);

    tt_logmgr_inputf(lm,
                     TT_LOG_FATAL,
                     __FUNCTION__,
                     __LINE__,
                     "this is [%s] log",
                     tt_g_log_level_name[TT_LOG_FATAL]);

    return TT_TRUE;
}

static tt_u32_t __log_filter_true(IN struct tt_logfltr_s *lf,
                                  IN tt_log_entry_t *entry,
                                  IN tt_buf_t *buf)
{
    return TT_LOGFLTR_PASS | TT_LOGFLTR_SELF;
}

static tt_u32_t __log_filter_false(IN struct tt_logfltr_s *lf,
                                   IN tt_log_entry_t *entry,
                                   IN tt_buf_t *buf)
{
    return 0;
}

static tt_logfltr_t *lf, *lf2, *lf3;
static tt_logfltr_itf_t lfi = {NULL, __log_filter_false};
static tt_logfltr_itf_t lfi2 = {NULL, __log_filter_true};
static tt_logfltr_itf_t lfi3 = {NULL, __log_filter_true};

TT_TEST_ROUTINE_DEFINE(case_log_manager)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_logmgr_t lm;
    tt_result_t ret;
    tt_loglyt_t *lyt[TT_LOG_LEVEL_NUM] = {0};
    tt_logio_t *lio;

    TT_TEST_CASE_ENTER()
    // test start

    lyt[TT_LOG_DEBUG] =
        tt_loglyt_pattern_create("${content} <${function} - ${line}>\n");
    TT_UT_NOT_EQUAL(lyt[TT_LOG_DEBUG], NULL, "");

    lyt[TT_LOG_INFO] = tt_loglyt_pattern_create("${content}\n");
    TT_UT_NOT_EQUAL(lyt[TT_LOG_INFO], NULL, "");

    lyt[TT_LOG_WARN] =
        tt_loglyt_pattern_create("${time} ${level:%-6.6s} ${content}\n");
    TT_UT_NOT_EQUAL(lyt[TT_LOG_WARN], NULL, "");

    lyt[TT_LOG_ERROR] =
        tt_loglyt_pattern_create("${time} ${level:%-6.6s} ${content}\n");
    TT_UT_NOT_EQUAL(lyt[TT_LOG_ERROR], NULL, "");

    lyt[TT_LOG_FATAL] =
        tt_loglyt_pattern_create("${time} ${level:%-6.6s} ${content}\n");
    TT_UT_NOT_EQUAL(lyt[TT_LOG_FATAL], NULL, "");

    lio = tt_logio_std_create(NULL);
    TT_UT_NOT_EQUAL(lio, NULL, "");

    // test
    ret = tt_logmgr_create(&lm, "haniu", NULL);
    TT_UT_SUCCESS(ret, "");
    __test_log(&lm);

    // has lio
    ret = tt_logmgr_append_io(&lm, TT_LOG_LEVEL_NUM, lio);
    TT_UT_SUCCESS(ret, "");
    __test_log(&lm);

    // set layout
    tt_logmgr_set_layout(&lm, TT_LOG_DEBUG, lyt[TT_LOG_DEBUG]);
    __test_log(&lm);

    tt_logmgr_set_layout(&lm, TT_LOG_INFO, lyt[TT_LOG_INFO]);
    __test_log(&lm);

    tt_logmgr_set_layout(&lm, TT_LOG_WARN, lyt[TT_LOG_WARN]);
    __test_log(&lm);

    tt_logmgr_set_layout(&lm, TT_LOG_ERROR, lyt[TT_LOG_ERROR]);
    __test_log(&lm);

    tt_logmgr_set_layout(&lm, TT_LOG_FATAL, lyt[TT_LOG_FATAL]);
    __test_log(&lm);

    tt_logmgr_set_level(&lm, TT_LOG_DEBUG);
    __test_log(&lm);

    tt_logmgr_set_level(&lm, 255);
    __test_log(&lm);

    tt_logmgr_set_level(&lm, TT_LOG_LEVEL_NUM);
    __test_log(&lm);
    tt_logmgr_set_level(&lm, TT_LOG_DEBUG);

    // test filter
    {
        lf = tt_logfltr_create(0, &lfi);
        TT_UT_NOT_NULL(lf, "");
        lf2 = tt_logfltr_create(0, &lfi2);
        TT_UT_NOT_NULL(lf, "");
        lf3 = tt_logfltr_create(0, &lfi3);
        TT_UT_NOT_NULL(lf, "");

        ret = tt_logmgr_append_filter(&lm, TT_LOG_DEBUG, NULL);
        TT_UT_FAIL(ret, "");
        __test_log(&lm);

        ret = tt_logmgr_append_filter(&lm, 200, lf);
        TT_UT_FAIL(ret, "");
        __test_log(&lm);

        ret = tt_logmgr_append_filter(&lm, TT_LOG_DEBUG, lf2);
        TT_UT_SUCCESS(ret, "");
        ret = tt_logfltr_append_io(lf2, lio);
        TT_UT_SUCCESS(ret, "");
        __test_log(&lm);

        ret = tt_logmgr_append_filter(&lm, TT_LOG_DEBUG, lf3);
        TT_UT_SUCCESS(ret, "");
        __test_log(&lm);

        ret = tt_logmgr_append_filter(&lm, TT_LOG_DEBUG, lf3);
        TT_UT_SUCCESS(ret, "");
        __test_log(&lm);

        tt_logfltr_release(lf);
        tt_logfltr_release(lf2);
        tt_logfltr_release(lf3);
    }

#if 0
    tt_loglyt_destroy(lyt[TT_LOG_DEBUG]);
    tt_loglyt_destroy(lyt[TT_LOG_INFO]);
    tt_loglyt_destroy(lyt[TT_LOG_WARN]);
    tt_loglyt_destroy(lyt[TT_LOG_ERROR]);
    tt_loglyt_destroy(lyt[TT_LOG_FATAL]);
#endif

    tt_logio_release(lio);

    tt_logmgr_destroy(&lm);

    // test end
    TT_TEST_CASE_LEAVE()
}

#if TT_ENV_OS_IS_IOS

#if (TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)
#define __LIOF_LOG_PATH "/tmp/"
#define __LIOF_ARCH_PATH "/tmp/"
#else
static tt_string_t log_path, arch_path;
#define __LIOF_LOG_PATH tt_string_cstr(&log_path)
#define __LIOF_ARCH_PATH tt_string_cstr(&arch_path)
#endif

#elif TT_ENV_OS_IS_ANDROID
#define __LIOF_LOG_PATH "/data/data/com.titansdk.titansdkunittest/"
#define __LIOF_ARCH_PATH "/data/data/com.titansdk.titansdkunittest/"
#else
#define __LIOF_LOG_PATH ""
#define __LIOF_ARCH_PATH ""
#endif

TT_TEST_ROUTINE_DEFINE(case_log_io_file_index)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_logio_t *lio;
    tt_logio_file_attr_t a;
    tt_result_t ret;
    tt_u32_t n;
    tt_file_t f;
    tt_char_t buf[100];
    tt_u64_t size;
    tt_log_entry_t le = {0};

    TT_TEST_CASE_ENTER()
    // test start

    tt_logio_file_attr_default(&a);
    a.log_name = "tttlog";
    a.max_log_size_order = 6; // 64 bytes
    a.log_suffix = TT_LOGFILE_SUFFIX_INDEX;

    tt_fremove(__LIOF_LOG_PATH "tttlog.1");
    tt_fremove(__LIOF_LOG_PATH "tttlog.2");
    tt_fremove(__LIOF_LOG_PATH "tttlog.3");
    tt_fremove(__LIOF_LOG_PATH "tttlog.4");

    lio = tt_logio_file_create(__LIOF_LOG_PATH, __LIOF_ARCH_PATH, &a);
    TT_UT_NOT_NULL(lio, "");

    ret = tt_fsize(__LIOF_LOG_PATH "tttlog.1", &size);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(size, 0, "");

    // 160 bytes
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");

    ret = tt_fsize(__LIOF_LOG_PATH "tttlog.1", &size);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(size, 80, "");

    // should be 3 log file
    ret =
        tt_fopen(&f, __LIOF_LOG_PATH "tttlog.1", TT_FO_READ | TT_FO_EXCL, NULL);
    TT_UT_SUCCESS(ret, "");
    ret = tt_fread(&f, (tt_u8_t *)buf, sizeof(buf), &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, 80, "");
    TT_UT_EQUAL(tt_memcmp(buf, "123456789 123456789 123456789 123456789 ", 40),
                0,
                "");
    TT_UT_EQUAL(tt_memcmp(buf + 40,
                          "123456789 123456789 123456789 123456789 ",
                          40),
                0,
                "");
    tt_fclose(&f);

    ret =
        tt_fopen(&f, __LIOF_LOG_PATH "tttlog.2", TT_FO_READ | TT_FO_EXCL, NULL);
    TT_UT_SUCCESS(ret, "");
    ret = tt_fread(&f, (tt_u8_t *)buf, sizeof(buf), &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, 80, "");
    TT_UT_EQUAL(tt_memcmp(buf, "123456789 123456789 123456789 123456789 ", 40),
                0,
                "");
    TT_UT_EQUAL(tt_memcmp(buf + 40,
                          "123456789 123456789 123456789 123456789 ",
                          40),
                0,
                "");
    tt_fclose(&f);

    ret =
        tt_fopen(&f, __LIOF_LOG_PATH "tttlog.3", TT_FO_READ | TT_FO_EXCL, NULL);
    TT_UT_SUCCESS(ret, "");
    ret = tt_fread(&f, (tt_u8_t *)buf, sizeof(buf), &n);
    TT_UT_EQUAL(ret, TT_E_END, "");
    tt_fclose(&f);

    tt_logio_release(lio);

    /////////
    // continue output
    /////////

    lio = tt_logio_file_create(__LIOF_LOG_PATH, __LIOF_ARCH_PATH, &a);
    TT_UT_NOT_NULL(lio, "");

    // 160 bytes
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");

    ret =
        tt_fopen(&f, __LIOF_LOG_PATH "tttlog.3", TT_FO_READ | TT_FO_EXCL, NULL);
    TT_UT_SUCCESS(ret, "");
    ret = tt_fread(&f, (tt_u8_t *)buf, sizeof(buf), &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, 80, "");
    TT_UT_EQUAL(tt_memcmp(buf, "123456789 123456789 123456789 123456789 ", 40),
                0,
                "");
    TT_UT_EQUAL(tt_memcmp(buf + 40,
                          "123456789 123456789 123456789 123456789 ",
                          40),
                0,
                "");
    tt_fclose(&f);

    ret =
        tt_fopen(&f, __LIOF_LOG_PATH "tttlog.4", TT_FO_READ | TT_FO_EXCL, NULL);
    TT_UT_SUCCESS(ret, "");
    ret = tt_fread(&f, (tt_u8_t *)buf, sizeof(buf), &n);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(n, 80, "");
    TT_UT_EQUAL(tt_memcmp(buf, "123456789 123456789 123456789 123456789 ", 40),
                0,
                "");
    TT_UT_EQUAL(tt_memcmp(buf + 40,
                          "123456789 123456789 123456789 123456789 ",
                          40),
                0,
                "");
    tt_fclose(&f);

    tt_logio_release(lio);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_log_io_file_date)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_logio_t *lio;
    tt_logio_file_attr_t a;
    tt_result_t ret;
    tt_u32_t n, num;
    tt_file_t f;
    tt_char_t buf[100];
    tt_dir_t d;
    tt_dirent_t de;
    tt_log_entry_t le = {0};

    TT_TEST_CASE_ENTER()
    // test start

    tt_logio_file_attr_default(&a);
    a.log_name = "log";
    a.max_log_size_order = 6; // 64 bytes
    a.log_suffix = TT_LOGFILE_SUFFIX_DATE;
    a.date_format = "%C%N%DT%H%M%S%Z";

#define __LIOF_LOG_PATH_D __LIOF_LOG_PATH "fdate/"
    tt_dremove(__LIOF_LOG_PATH_D);
    tt_dcreate(__LIOF_LOG_PATH_D, NULL);

    lio = tt_logio_file_create(__LIOF_LOG_PATH_D, __LIOF_ARCH_PATH, &a);
    TT_UT_NOT_NULL(lio, "");

    // 160 bytes => 2 file
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");

    // should be 3 log file
    ret = tt_dopen(&d, __LIOF_LOG_PATH_D, NULL);
    TT_UT_SUCCESS(ret, "");
    num = 0;
    while (TT_OK(ret = tt_dread(&d, &de))) {
        tt_fpath_t p;

        if (tt_strcmp(de.name, ".") == 0) {
            continue;
        }
        if (tt_strcmp(de.name, "..") == 0) {
            continue;
        }

        tt_fpath_create(&p, __LIOF_LOG_PATH_D, TT_FPATH_AUTO);
        tt_fpath_set_filename(&p, de.name);

        ret = tt_fopen(&f, tt_fpath_cstr(&p), TT_FO_READ | TT_FO_EXCL, NULL);
        tt_fpath_destroy(&p);
        TT_UT_SUCCESS(ret, "");
        ret = tt_fread(&f, (tt_u8_t *)buf, sizeof(buf), &n);
        if (TT_OK(ret)) {
            TT_UT_EQUAL(n, 80, "");
            TT_UT_EQUAL(tt_memcmp(buf,
                                  "123456789 123456789 123456789 123456789 ",
                                  40),
                        0,
                        "");
            TT_UT_EQUAL(tt_memcmp(buf + 40,
                                  "123456789 123456789 123456789 123456789 ",
                                  40),
                        0,
                        "");

            ++num;
        } else {
            TT_UT_EQUAL(ret, TT_E_END, "");
        }
        tt_fclose(&f);
    }
    tt_dclose(&d);
    TT_UT_EQUAL(ret, TT_E_END, "");
    TT_UT_EQUAL(num, 2, "");

    tt_logio_release(lio);

    //////////
    // again
    //////////

    lio = tt_logio_file_create(__LIOF_LOG_PATH_D, __LIOF_ARCH_PATH, &a);
    TT_UT_NOT_NULL(lio, "");

    // 160 bytes => 2 file
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");

    // should be 3 log file
    ret = tt_dopen(&d, __LIOF_LOG_PATH_D, NULL);
    TT_UT_SUCCESS(ret, "");
    num = 0;
    while (TT_OK(ret = tt_dread(&d, &de))) {
        tt_fpath_t p;

        if (tt_strcmp(de.name, ".") == 0) {
            continue;
        }
        if (tt_strcmp(de.name, "..") == 0) {
            continue;
        }

        tt_fpath_create(&p, __LIOF_LOG_PATH_D, TT_FPATH_AUTO);
        tt_fpath_set_filename(&p, de.name);

        ret = tt_fopen(&f, tt_fpath_cstr(&p), TT_FO_READ | TT_FO_EXCL, NULL);
        tt_fpath_destroy(&p);
        TT_UT_SUCCESS(ret, "");
        ret = tt_fread(&f, (tt_u8_t *)buf, sizeof(buf), &n);
        if (TT_OK(ret)) {
            TT_UT_EQUAL(n, 80, "");
            TT_UT_EQUAL(tt_memcmp(buf,
                                  "123456789 123456789 123456789 123456789 ",
                                  40),
                        0,
                        "");
            TT_UT_EQUAL(tt_memcmp(buf + 40,
                                  "123456789 123456789 123456789 123456789 ",
                                  40),
                        0,
                        "");

            ++num;
        } else {
            TT_UT_EQUAL(ret, TT_E_END, "");
        }
        tt_fclose(&f);
    }
    tt_dclose(&d);
    TT_UT_EQUAL(ret, TT_E_END, "");
    TT_UT_EQUAL(num, 4, "");

    tt_logio_release(lio);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_log_io_file_archive)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_logio_file_attr_t a;
    tt_logio_t *lio;
    tt_u32_t n;
    tt_bool_t oneshot = TT_FALSE;
    tt_log_entry_t le = {0};

    TT_TEST_CASE_ENTER()
    // test start

    tt_logio_file_attr_default(&a);
    a.log_name = "mylog";
    a.max_log_size_order = 6; // 64 bytes
    a.log_suffix = TT_LOGFILE_SUFFIX_INDEX;
    a.keep_log_sec = 3;
    a.keep_archive_sec = 3;
    a.log_purge = TT_LOGFILE_PURGE_REMOVE;

#define __LIOF_P_LOG __LIOF_LOG_PATH "mylog/"
    tt_dremove(__LIOF_P_LOG);
    tt_dcreate(__LIOF_P_LOG, NULL);
#define __LIOF_P_ARCH __LIOF_LOG_PATH "myarch/"
    tt_dremove(__LIOF_P_ARCH);
    tt_dcreate(__LIOF_P_ARCH, NULL);

    lio = tt_logio_file_create(__LIOF_P_LOG, __LIOF_P_ARCH, &a);
    TT_UT_NOT_NULL(lio, "");

    // 160 bytes => 3 file
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio,
                        &le,
                        "123456789 123456789 123456789 123456789 ",
                        40);
    TT_UT_EQUAL(n, 40, "");

    while (1) {
        tt_dir_t d;
        tt_result_t ret;
        tt_dirent_t de;
        tt_zip_t *z;
        tt_bool_t found = TT_FALSE;

        tt_sleep(4000);

        ret = tt_dopen(&d, __LIOF_P_ARCH, NULL);
        TT_UT_SUCCESS(ret, "");

        while (TT_OK(ret = tt_dread(&d, &de))) {
            tt_fpath_t fp;
            tt_zipfile_t *zf;
            tt_char_t b[100];
            tt_u32_t n;

            if (tt_strcmp(de.name, ".") == 0) {
                continue;
            } else if (tt_strcmp(de.name, "..") == 0) {
                continue;
            }
            // this should be the log archive
            found = TT_TRUE;

            tt_fpath_init(&fp, TT_FPATH_AUTO);
            tt_fpath_set(&fp, __LIOF_P_ARCH);
            tt_fpath_to_dir(&fp);
            tt_fpath_set_filename(&fp, de.name);

            z = tt_zip_create_file(tt_fpath_cstr(&fp), 0, 0, 0, NULL);
            tt_fpath_destroy(&fp);
            TT_UT_NOT_NULL(z, "");
            TT_UT_EQUAL(tt_zip_count(z, 0), 2, "");

            zf = tt_zipfile_open_index(z, 0, 0, NULL);
            TT_UT_NOT_NULL(z, "");
            ret = tt_zipfile_read(zf, (tt_u8_t *)b, sizeof(b), &n);
            TT_UT_SUCCESS(ret, "");
            TT_UT_EQUAL(n, 80, "");
            TT_UT_NSTREQ(b, "123456789 123456789 123456789 123456789 ", 40, "");
            TT_UT_NSTREQ(b + 40,
                         "123456789 123456789 123456789 123456789 ",
                         40,
                         "");
            ret = tt_zipfile_read(zf, (tt_u8_t *)b, sizeof(b), &n);
            TT_UT_EQUAL(ret, TT_E_END, "");
            TT_UT_EQUAL(n, 0, "");
            tt_zipfile_close(zf);

            zf = tt_zipfile_open_index(z, 1, 0, NULL);
            TT_UT_NOT_NULL(z, "");
            ret = tt_zipfile_read(zf, (tt_u8_t *)b, sizeof(b), &n);
            TT_UT_SUCCESS(ret, "");
            TT_UT_EQUAL(n, 80, "");
            TT_UT_NSTREQ(b, "123456789 123456789 123456789 123456789 ", 40, "");
            TT_UT_NSTREQ(b + 40,
                         "123456789 123456789 123456789 123456789 ",
                         40,
                         "");
            ret = tt_zipfile_read(zf, (tt_u8_t *)b, sizeof(b), &n);
            TT_UT_EQUAL(ret, TT_E_END, "");
            TT_UT_EQUAL(n, 0, "");
            tt_zipfile_close(zf);

            tt_zip_destroy(z, TT_FALSE);
        }
        tt_dclose(&d);
        if (found) {
            break;
        } else {
            TT_UT_FALSE(oneshot, "");
            oneshot = TT_TRUE;
        }
    }

    tt_sleep(4000);

    // should already be purged
    {
        tt_u32_t n = 0;
        tt_dir_t d;
        tt_result_t ret;
        tt_dirent_t de;

        ret = tt_dopen(&d, __LIOF_P_ARCH, NULL);
        TT_UT_SUCCESS(ret, "");

        while (TT_OK(ret = tt_dread(&d, &de))) {
            if (tt_strcmp(de.name, ".") == 0) {
                continue;
            } else if (tt_strcmp(de.name, "..") == 0) {
                continue;
            }

            ++n;
            TT_INFO("arch: %s", de.name);
        }
        TT_UT_EQUAL(n, 0, "");

        tt_dclose(&d);
    }

    // enable it when cross-thread fiber event is implemented
    tt_logio_release(lio);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_log_io_syslog)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_logio_t *lio;
    tt_log_entry_t le = {0};
    tt_logio_syslog_attr_t a;

    TT_TEST_CASE_ENTER()
    // test start

    lio = tt_logio_syslog_create(NULL);
    TT_UT_NOT_NULL(lio, "");

    le.level = TT_LOG_DEBUG;
    tt_logio_output(lio, &le, "debug log", sizeof("debug log"));

    le.level = TT_LOG_INFO;
    tt_logio_output(lio, &le, "info log", sizeof("debug log"));

    le.level = TT_LOG_WARN;
    tt_logio_output(lio, &le, "warn log", sizeof("debug log"));

    le.level = TT_LOG_ERROR;
    tt_logio_output(lio, &le, "error log", sizeof("debug log"));

    le.level = TT_LOG_FATAL;
    tt_logio_output(lio, &le, "fatal log", sizeof("debug log"));

    tt_logio_release(lio);

    //////////////
    tt_logio_syslog_attr_default(&a);
    a.facility = TT_SYSLOG_LOCAL7;

    lio = tt_logio_syslog_create(&a);
    TT_UT_NOT_NULL(lio, "");

    le.level = TT_LOG_DEBUG;
    tt_logio_output(lio, &le, "debug log", sizeof("debug log"));

    le.level = TT_LOG_INFO;
    tt_logio_output(lio, &le, "info log", sizeof("debug log"));

    le.level = TT_LOG_WARN;
    tt_logio_output(lio, &le, "warn log", sizeof("debug log"));

    le.level = TT_LOG_ERROR;
    tt_logio_output(lio, &le, "error log", sizeof("debug log"));

    le.level = TT_LOG_FATAL;
    tt_logio_output(lio, &le, "fatal log", sizeof("debug log"));

    tt_logio_release(lio);

    // test end
    TT_TEST_CASE_LEAVE()
}
