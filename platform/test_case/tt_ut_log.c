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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_log_context)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_log_manager)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(log_case)

TT_TEST_CASE("tt_unit_test_log_context",
             "testing log context",
             tt_unit_test_log_context,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_log_manager",
                 "testing log manager",
                 tt_unit_test_log_manager,
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
    TT_TEST_ROUTINE_DEFINE(tt_unit_test_log_manager)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_log_context)
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
    TT_TEST_CHECK_NOT_EQUAL(lyt, NULL, "");

    lio = tt_logio_std_create(NULL);
    TT_TEST_CHECK_NOT_EQUAL(lio, NULL, "");
    TT_TEST_CHECK_EQUAL(lio->itf->type, TT_LOGIO_STANDARD, "");

    ret = tt_logctx_create(&ctx, TT_LOG_FATAL, lyt, NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");

    tt_logctx_input(&ctx, &entry);

    entry.content = "aabbcc";
    tt_logctx_input(&ctx, &entry);

    ret = tt_logctx_append_io(&ctx, lio);
    TT_TEST_CHECK_SUCCESS(ret, "");

    entry.function = __FUNCTION__;
    tt_logctx_input(&ctx, &entry);

    ret = tt_logctx_append_io(&ctx, lio);
    TT_TEST_CHECK_SUCCESS(ret, "");

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
    TT_TEST_CHECK_FAIL(ret, "");

    ret = tt_logctx_create(&ctx, TT_LOG_DEBUG, NULL, NULL);
    TT_TEST_CHECK_FAIL(ret, "");

    ret = tt_logctx_create(NULL, TT_LOG_DEBUG, lyt, NULL);
    TT_TEST_CHECK_FAIL(ret, "");

    tt_logctx_destroy(NULL);
    tt_logctx_attr_default(NULL);

    ret = tt_logctx_append_io(NULL, lio);
    TT_TEST_CHECK_FAIL(ret, "");

    ret = tt_logctx_append_io(&ctx, NULL);
    TT_TEST_CHECK_FAIL(ret, "");

    ret = tt_logctx_input(NULL, &entry);
    TT_TEST_CHECK_FAIL(ret, "");
    ret = tt_logctx_input(&ctx, NULL);
    TT_TEST_CHECK_FAIL(ret, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

static tt_bool_t __test_log(tt_logmgr_t *lm)
{
    tt_logmgr_input(lm,
                    TT_LOG_DEBUG,
                    __FUNCTION__,
                    __LINE__,
                    "this is [%s] log",
                    tt_g_log_level_name[TT_LOG_DEBUG]);

    tt_logmgr_input(lm,
                    TT_LOG_INFO,
                    __FUNCTION__,
                    __LINE__,
                    "this is [%s] log",
                    tt_g_log_level_name[TT_LOG_INFO]);

    tt_logmgr_input(lm,
                    TT_LOG_WARN,
                    __FUNCTION__,
                    __LINE__,
                    "this is [%s] log",
                    tt_g_log_level_name[TT_LOG_WARN]);

    tt_logmgr_input(lm,
                    TT_LOG_ERROR,
                    __FUNCTION__,
                    __LINE__,
                    "this is [%s] log",
                    tt_g_log_level_name[TT_LOG_ERROR]);

    tt_logmgr_input(lm,
                    TT_LOG_FATAL,
                    __FUNCTION__,
                    __LINE__,
                    "this is [%s] log",
                    tt_g_log_level_name[TT_LOG_FATAL]);

    return TT_TRUE;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_log_manager)
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
    TT_TEST_CHECK_NOT_EQUAL(lyt[TT_LOG_DEBUG], NULL, "");

    lyt[TT_LOG_INFO] = tt_loglyt_pattern_create("${content}\n");
    TT_TEST_CHECK_NOT_EQUAL(lyt[TT_LOG_INFO], NULL, "");

    lyt[TT_LOG_WARN] =
        tt_loglyt_pattern_create("${time} ${level:%-6.6s} ${content}\n");
    TT_TEST_CHECK_NOT_EQUAL(lyt[TT_LOG_WARN], NULL, "");

    lyt[TT_LOG_ERROR] =
        tt_loglyt_pattern_create("${time} ${level:%-6.6s} ${content}\n");
    TT_TEST_CHECK_NOT_EQUAL(lyt[TT_LOG_ERROR], NULL, "");

    lyt[TT_LOG_FATAL] =
        tt_loglyt_pattern_create("${time} ${level:%-6.6s} ${content}\n");
    TT_TEST_CHECK_NOT_EQUAL(lyt[TT_LOG_FATAL], NULL, "");

    lio = tt_logio_std_create(NULL);
    TT_TEST_CHECK_NOT_EQUAL(lio, NULL, "");

    // test
    ret = tt_logmgr_create(&lm, "haniu", NULL);
    TT_TEST_CHECK_SUCCESS(ret, "");
    __test_log(&lm);

    // has lio
    ret = tt_logmgr_append_io(&lm, TT_LOG_LEVEL_NUM, lio);
    TT_TEST_CHECK_SUCCESS(ret, "");
    __test_log(&lm);

    // set layout
    ret = tt_logmgr_set_layout(&lm, TT_LOG_DEBUG, lyt[TT_LOG_DEBUG]);
    TT_TEST_CHECK_SUCCESS(ret, "");
    __test_log(&lm);

    ret = tt_logmgr_set_layout(&lm, TT_LOG_INFO, lyt[TT_LOG_INFO]);
    TT_TEST_CHECK_SUCCESS(ret, "");
    __test_log(&lm);

    ret = tt_logmgr_set_layout(&lm, TT_LOG_WARN, lyt[TT_LOG_WARN]);
    TT_TEST_CHECK_SUCCESS(ret, "");
    __test_log(&lm);

    ret = tt_logmgr_set_layout(&lm, TT_LOG_ERROR, lyt[TT_LOG_ERROR]);
    TT_TEST_CHECK_SUCCESS(ret, "");
    __test_log(&lm);

    ret = tt_logmgr_set_layout(&lm, TT_LOG_FATAL, lyt[TT_LOG_FATAL]);
    TT_TEST_CHECK_SUCCESS(ret, "");
    __test_log(&lm);

    tt_logmgr_set_level(&lm, TT_LOG_DEBUG);
    __test_log(&lm);

    tt_logmgr_set_level(&lm, 255);
    __test_log(&lm);

    tt_logmgr_set_level(&lm, TT_LOG_LEVEL_NUM);
    __test_log(&lm);

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
