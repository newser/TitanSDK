#include <tt_platform.h>

#include "app_unit_test_process.h"

//#include <locale.h>

struct tt_evcenter_s;

int plus(int i)
{
    if (i > 0) {
        return i + plus(i - 1);
    } else {
        return 0;
    }
}

tt_result_t __ut_fiber(IN void *param)
{
    tt_test_framework_init(0);
    tt_test_unit_init(NULL);
    tt_test_unit_run(NULL);
    tt_test_unit_list(NULL);

    tt_task_exit(NULL);

    return TT_SUCCESS;
}

int app_ut_main(int argc, char *argv[])
{
    tt_thread_t *ut_thread;
    int i;
    tt_task_t t;

    printf("%d\n", plus(100));

    // setlocale(LC_ALL, "chs");

    for (i = 0; i < argc; ++i) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    if (argc > 1) {
        if (strcmp(argv[1], "process") == 0) {
            return app_ut_process(argc, argv);
        } else if (strcmp(argv[1], "ipc_stress") == 0) {
            tt_evcenter_t evc;
            tt_evc_attr_t evc_attr;

            if (argc < 3) {
                printf("app unit test ipc need at least 3 args\n");
                return -1;
            }

            // init platform
            tt_platform_init(NULL);

            extern tt_result_t __ipc_cli_oneshot(IN void *param);
            tt_task_create(&t, NULL);
            tt_task_add_fiber(&t, NULL, __ipc_cli_oneshot, NULL, NULL);
            tt_task_run(&t);
            tt_task_wait(&t);
            printf("exiting\n");
            return 0;
        } // haniu
        else {
            printf("unknown process arg: %s\n", argv[1]);

            // must return so as to infinitely creating process
            return -1;
        }
    }

    // init platform
    tt_platform_init(NULL);

    tt_task_create(&t, NULL);
    tt_task_add_fiber(&t, NULL, __ut_fiber, NULL, NULL);
    tt_task_run(&t);
    tt_task_wait(&t);

    return 0;

// tt_page_os_stat_show(0);
// tt_skt_stat_show(0);

#ifdef TT_WINDOWS_CRT_DUMP
    _CrtDumpMemoryLeaks();
#endif

#if TT_ENV_OS_IS_WINDOWS
    while (1) {
#else
    while (0) {
#endif
        tt_sleep(10000);
    }

    return 0;
}
