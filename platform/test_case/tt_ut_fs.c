/*
 * import header files
 */
#include "tt_unit_test_case_config.h"
#include <unit_test/tt_unit_test.h>

#include <event/tt_event_center.h>
#include <io/tt_file_system.h>
#include <memory/tt_memory_alloc.h>
#include <os/tt_spinlock.h>
#include <timer/tt_time_reference.h>

#include <tt_cstd_api.h>

#include <stdio.h>
#include <time.h>

//#define __perf_vs
#ifdef __perf_vs
#include <uv.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h> /* memset */
#include <sys/stat.h>
#endif

/*
 * local definition
 */

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_fs_basic)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(fs_case)

#ifndef __SIMULATE_FS_AIO_FAIL
#if 1
TT_TEST_CASE("tt_unit_test_fs_basic",
             "testing fs basic",
             tt_unit_test_fs_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,
#endif

#endif

    TT_TEST_CASE_LIST_DEFINE_END(fs_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_FS, 0, fs_case)

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

#if TT_ENV_OS_IS_WINDOWS
#define __SC_TEST_FILE "测试"
#elif TT_ENV_OS_IS_IOS
#define __SC_TEST_FILE "../tmp/测试"
#else
#define __SC_TEST_FILE "测试"
#endif

        TT_TEST_ROUTINE_DEFINE(tt_unit_test_fs_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_file_t tf, tf2;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start
    // setlocale(LC_ALL, "chs");
    tt_fremove(__SC_TEST_FILE);

    // create
    ret = tt_fcreate(__SC_TEST_FILE, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // remove
    ret = tt_fremove(__SC_TEST_FILE);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
