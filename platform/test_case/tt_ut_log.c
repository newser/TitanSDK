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

    TT_TEST_CASE_LIST_DEFINE_END(log_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_LOG, 0, log_case)

    /*
     * interface implementation
     */


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
