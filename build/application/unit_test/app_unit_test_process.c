#include "app_unit_test_process.h"

#include <tt_cstd_api.h>

int app_ut_process(int argc, char *argv[])
{
    if (argc < 3) {
        printf("app ut process should have at least 3 args\n");
        return -1;
    }

    if (strcmp(argv[2], "proc1") == 0) {
        printf("test case: proc1\n");
        return 0;
    } else if (strcmp(argv[2], "proc_to") == 0) {
        printf("test case: proc time out\n");
#if TT_ENV_OS_IS_WINDOWS
        Sleep(3000);
#else
        sleep(1);
#endif
        return 0;
    } else if (strcmp(argv[2], "proc_exit") == 0) {
        tt_u8_t ec;
        ec = atoi(argv[3]);

        printf("test case: proc exit[%d]\n", ec);
#if TT_ENV_OS_IS_WINDOWS
        ExitThread((UINT)ec);
#else
        exit(ec);
#endif
        return 0;
    } else if (strcmp(argv[2], "proc_args") == 0) {
        int i;
        if (argc != 16) {
            printf("argc != 16\n");
#if TT_ENV_OS_IS_WINDOWS
            Sleep(50000);
#else
            sleep(5);
#endif
            exit(TT_PROCESS_EXIT_FAILURE);
        }
        for (i = 3; i < 16; ++i) {
            if ((strlen(argv[i]) != 1) ||
                (argv[i][0] != ((i > 9 ? 'a' + i - 10 : '0' + i)))) {
                printf("argv[%d][0] != %c\n",
                       i,
                       ((i > 9 ? 'a' + i - 10 : '0' + i)));
#if TT_ENV_OS_IS_WINDOWS
                Sleep(50000);
#else
                sleep(5);
#endif
                exit(TT_PROCESS_EXIT_FAILURE);
            }
        }
#if TT_ENV_OS_IS_WINDOWS
        ExitThread((UINT)105);
#else
        exit(105);
#endif
        return 0;
    }

    printf("argc: %d, argv[0]: %s, argv[2]: %s", argc, argv[0], argv[2]);
    return 0;
}
