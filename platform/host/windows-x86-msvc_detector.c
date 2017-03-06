#include <stdio.h>

#include <windows.h>

// - "option" is defined in environment.ttcm
// - output of __ENTRY_IMPL is defined in tt_environment_def.h or in
//   tt_environment_def_native.h

#define __ENTRY_IMPL(option) int __detect_##option()

// os

__ENTRY_IMPL(TTCM_ENV_OS_DETAIL)
{
    printf("TT_ENV_OS_WINDOWS");
    return 0;
}

__ENTRY_IMPL(TTCM_ENV_OS_VER_DETAIL)
{
    OSVERSIONINFO osvi;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if (GetVersionEx(&osvi)) {
        if (osvi.dwMajorVersion == 6) {
            if (osvi.dwMinorVersion == 0) {
                printf("TT_ENV_OS_VER_WINDOWS_VISTA");
                return 0;
            } else if (osvi.dwMinorVersion == 1) {
                printf("TT_ENV_OS_VER_WINDOWS_7");
                return 0;
            } else if (osvi.dwMinorVersion == 2) {
                printf("TT_ENV_OS_VER_WINDOWS_8");
                return 0;
            }
        }
    }

    printf("TT_ENV_OS_VER_UNKNOWN");
    return 0; // -1?
}

__ENTRY_IMPL(TTCM_ENV_OS_FEATURE_DETAIL)
{
    printf("(0");

    // TT_ENV_OS_FEATURE_64BIT
    if (sizeof(void *) == 4) {
        //
    } else if (sizeof(void *) == 8) {
        printf(" | TT_ENV_OS_FEATURE_64BIT");
    }

    printf(")");
    return 0;
}

// cpu

__ENTRY_IMPL(TTCM_ENV_CPU_DETAIL)
{
    printf("TT_ENV_CPU_X86");
    return 0;
}

__ENTRY_IMPL(TTCM_ENV_CPU_FEATURE_DETAIL)
{
    printf("(0");

    // TT_ENV_OS_FEATURE_64BIT
    if (sizeof(void *) == 4) {
        //
    } else if (sizeof(void *) == 8) {
        printf(" | TT_ENV_CPU_FEATURE_64BIT");
    }

    printf(")");
    return 0;
}

// toolchain

__ENTRY_IMPL(TTCM_ENV_TOOLCHAIN_DETAIL)
{
    printf("TT_ENV_TOOLCHAIN_MSVC");
    return 0;
}

__ENTRY_IMPL(TTCM_ENV_TOOLCHAIN_VER_DETAIL)
{
#if (_MSC_VER == 1600)
    printf("TT_ENV_TOOLCHAIN_MSVC_2010");
    return 0;
#elif (_MSC_VER == 1700)
    printf("TT_ENV_TOOLCHAIN_MSVC_2012");
    return 0;
#elif (_MSC_VER == 1800)
    printf("TT_ENV_TOOLCHAIN_MSVC_2013");
    return 0;
#else
    printf("TT_ENV_TOOLCHAIN_VER_UNKNOWN");
    return 0;
#endif
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        return -1;
    }

#define __ENTRY_START(option)                                                  \
    if (strncmp(argv[1], #option, sizeof(#option) - 1) == 0) {                 \
        return __detect_##option();                                            \
    }
#define __ENTRY_NEXT(option, function)                                         \
    else if (strncmp(argv[1], #option, sizeof(#option) - 1) == 0)              \
    {                                                                          \
        return __detect_##option();                                            \
    }
#define __ENTRY_END()                                                          \
    else                                                                       \
    {                                                                          \
        printf("unknown option: %s\n", argv[1]);                               \
        return -1;                                                             \
    }

    // os
    __ENTRY_START(TTCM_ENV_OS_DETAIL)
    __ENTRY_START(TTCM_ENV_OS_VER_DETAIL)
    __ENTRY_START(TTCM_ENV_OS_FEATURE_DETAIL)

    // cpu
    __ENTRY_START(TTCM_ENV_CPU_DETAIL)
    __ENTRY_START(TTCM_ENV_CPU_FEATURE_DETAIL)

    // toolchain
    __ENTRY_START(TTCM_ENV_TOOLCHAIN_DETAIL)
    __ENTRY_START(TTCM_ENV_TOOLCHAIN_VER_DETAIL)
}
