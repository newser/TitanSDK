/*
 * import header files
 */
#include "tt_unit_test_case_config.h"
#include <unit_test/tt_unit_test.h>

#include <tt_platform.h>

/*
 * local definition
 */

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(case_log_context)
TT_TEST_ROUTINE_DECLARE(case_log_manager)

TT_TEST_ROUTINE_DECLARE(case_log_io_file_index)
TT_TEST_ROUTINE_DECLARE(case_log_io_file_date)
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

    TT_TEST_CASE_LIST_DEFINE_END(log_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_LOG, 0, log_case)

    /*
     * interface implementation
     */


    /*
    TT_TEST_ROUTINE_DEFINE(case_log_io_file_index)
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
    tt_loglyt_destroy(lyt);
    tt_logio_destroy(lio);

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

static tt_bool_t __log_filter_true(IN OUT tt_log_entry_t *entry)
{
    return TT_TRUE;
}

static tt_bool_t __log_filter_false(IN OUT tt_log_entry_t *entry)
{
    return TT_FALSE;
}

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
        ret = tt_logmgr_append_filter(&lm, TT_LOG_DEBUG, NULL);
        TT_UT_FAIL(ret, "");
        __test_log(&lm);

        ret = tt_logmgr_append_filter(&lm, 200, __log_filter_false);
        TT_UT_FAIL(ret, "");
        __test_log(&lm);

        ret = tt_logmgr_append_filter(&lm, TT_LOG_DEBUG, __log_filter_true);
        TT_UT_SUCCESS(ret, "");
        __test_log(&lm);

        ret = tt_logmgr_append_filter(&lm, TT_LOG_DEBUG, __log_filter_true);
        TT_UT_SUCCESS(ret, "");
        __test_log(&lm);

        ret = tt_logmgr_append_filter(&lm, TT_LOG_DEBUG, __log_filter_false);
        TT_UT_SUCCESS(ret, "");
        __test_log(&lm);
    }

    tt_loglyt_destroy(lyt[TT_LOG_DEBUG]);
    tt_loglyt_destroy(lyt[TT_LOG_INFO]);
    tt_loglyt_destroy(lyt[TT_LOG_WARN]);
    tt_loglyt_destroy(lyt[TT_LOG_ERROR]);
    tt_loglyt_destroy(lyt[TT_LOG_FATAL]);

    tt_logio_destroy(lio);

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

    TT_TEST_CASE_ENTER()
    // test start

    tt_logio_file_attr_default(&a);
    a.log_name = "tttlog";
    a.max_log_size_order = 6; // 64 bytes
    a.log_suffix = TT_LOGFILE_SUFFIX_INDEX;

    tt_fremove(__LIOF_LOG_PATH "tttlog.1");
    tt_fremove(__LIOF_LOG_PATH "tttlog.2");
    tt_fremove(__LIOF_LOG_PATH "tttlog.3");

    lio = tt_logio_file_create(__LIOF_LOG_PATH, __LIOF_ARCH_PATH, &a);
    TT_UT_NOT_NULL(lio, "");

    // 160 bytes
    n = tt_logio_output(lio, "123456789 123456789 123456789 123456789 ", 40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio, "123456789 123456789 123456789 123456789 ", 40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio, "123456789 123456789 123456789 123456789 ", 40);
    TT_UT_EQUAL(n, 40, "");
    n = tt_logio_output(lio, "123456789 123456789 123456789 123456789 ", 40);
    TT_UT_EQUAL(n, 40, "");

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
    TT_UT_EQUAL(ret, TT_END, "");
    tt_fclose(&f);

    tt_logio_destroy(lio);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_log_io_file_date)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_logio_t *lio;
    tt_logio_file_attr_t a;
    tt_result_t ret;
    tt_u32_t n;
    tt_file_t f;
    tt_char_t buf[100];
    tt_dir_t d;
    tt_dirent_t de;

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

    // 40 bytes
    n = tt_logio_output(lio, "123456789 123456789 123456789 123456789 ", 40);
    TT_UT_EQUAL(n, 40, "");
    // can not generate multiple log files, time is short, leading to same file
    // name

    // should be 3 log file
    ret = tt_dopen(&d, __LIOF_LOG_PATH_D, NULL);
    TT_UT_SUCCESS(ret, "");
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
            TT_UT_EQUAL(n, 40, "");
            TT_UT_EQUAL(tt_memcmp(buf,
                                  "123456789 123456789 123456789 123456789 ",
                                  40),
                        0,
                        "");
        } else {
            TT_UT_EQUAL(ret, TT_END, "");
        }
        tt_fclose(&f);
    }
    tt_dclose(&d);
    TT_UT_EQUAL(ret, TT_END, "");

    tt_logio_destroy(lio);

    // test end
    TT_TEST_CASE_LEAVE()
}
