/*
 * import header files
 */
#include "tt_unit_test_case_config.h"
#include <unit_test/tt_unit_test.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <os/tt_semaphore.h>
#include <time/tt_timer_manager.h>

#include <stdio.h>

/*
 * local definition
 */

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_timer_basic)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(timer_case)

TT_TEST_CASE("tt_unit_test_timer_basic",
             "testing timer basic",
             tt_unit_test_timer_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE_LIST_DEFINE_END(timer_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_TIMER, 0, timer_case)

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

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_timer_basic)
{
    TT_TEST_CASE_ENTER()
    // test start

    TT_RECORD_INFO("%s", "");

    // test end
    TT_TEST_CASE_LEAVE()
}
