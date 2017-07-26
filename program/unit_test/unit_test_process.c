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
        tt_sleep(3000);
        return 0;
    } else if (strcmp(argv[2], "proc_exit") == 0) {
        tt_u8_t ec;
        ec = atoi(argv[3]);

        printf("test case: proc exit[%d]\n", ec);
        tt_process_exit(ec);
        return 0;
    } else if (strcmp(argv[2], "proc_args") == 0) {
        int i;
        if (argc != 16) {
            printf("argc != 16\n");
            tt_sleep(5000);
            tt_process_exit(TT_PROCESS_EXIT_FAILURE);
        }
        for (i = 3; i < 16; ++i) {
            if ((strlen(argv[i]) != 1) ||
                (argv[i][0] != ((i > 9 ? 'a' + i - 10 : '0' + i)))) {
                printf("argv[%d][0] != %c\n",
                       i,
                       ((i > 9 ? 'a' + i - 10 : '0' + i)));
                tt_sleep(5000);
                tt_process_exit(TT_PROCESS_EXIT_FAILURE);
            }
        }
        tt_process_exit(105);
        return 0;
    }

    printf("argc: %d, argv[0]: %s, argv[2]: %s", argc, argv[0], argv[2]);
    return 0;
}
