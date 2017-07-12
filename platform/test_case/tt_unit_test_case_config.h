/*
 * copyright (c) 2011, haon
 * all rights reserved.
 *
 * name: tt_unit_test_case_config.h
 *
 * description:
 * this file defines some macros for test cases for
 * convinience
 */

#ifndef __TT_UNIT_TEST_CASE_CONFIG__
#define __TT_UNIT_TEST_CASE_CONFIG__

/*
 * import header files
 */

/*
 * macro definition
 */
#define TT_UNIT_TEST_MEM_POOL_FAST (1 << 0)
#define TT_UNIT_TEST_MSG_FAST (1 << 1)
#define TT_UNIT_TEST_THREAD_FAST (1 << 2)
#define TT_UNIT_TEST_ALG_FAST (1 << 3)
#define TT_UNIT_TEST_TIMER_FAST (1 << 4)
#define TT_UNIT_TEST_MSG_THREAD_FAST (1 << 5)

#if 0
#define TT_UNIT_TEST_CASE_FAST                                                 \
    (TT_UNIT_TEST_MEM_POOL_FAST | TT_UNIT_TEST_MSG_FAST |                      \
     TT_UNIT_TEST_ALG_FAST | TT_UNIT_TEST_TIMER_FAST |                         \
     TT_UNIT_TEST_THREAD_FAST)
#else
#define TT_UNIT_TEST_CASE_FAST 0
#endif

/*
 * type definition
 */

/*
 * global variants
 */

/*
 * interface declaration
 */

#endif /* __TT_UNIT_TEST_CASE_CONFIG__ */
