#include <stdio.h>

#include <sys/utsname.h>

// - "option" is defined in environment.ttcm
// - output of __ENTRY_IMPL is defined in tt_environment_def.h or in
//   tt_environment_def_native.h

#define __ENTRY_IMPL(option) int __detect_##option()

// os

__ENTRY_IMPL(PLATFORM_ENV_OS_DETAIL)
{
    printf("TT_ENV_OS_LINUX");
    return 0;
}

__ENTRY_IMPL(PLATFORM_ENV_OS_VER_DETAIL)
{
    struct utsname name;
    int v, v1, v2, v3;

    if (uname(&name) < 0) {
        printf("TT_ENV_OS_VER_UNKNOWN");
        return 0; // -1?
    }

    v1 = v2 = v3 = 0;
    if (sscanf(name.release, "%d.%d.%d", &v1, &v2, &v3) <= 0) {
        printf("TT_ENV_OS_VER_UNKNOWN");
        return 0; // -1?
    }

    v = (v1) << 16 | (v2) << 8 | (v3);
    printf("0x%x", v);
    return 0;
}

__ENTRY_IMPL(PLATFORM_ENV_OS_FEATURE_DETAIL)
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

__ENTRY_IMPL(PLATFORM_ENV_CPU_DETAIL)
{
    printf("TT_ENV_CPU_X86");
    return 0;
}

__ENTRY_IMPL(PLATFORM_ENV_CPU_FEATURE_DETAIL)
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

__ENTRY_IMPL(PLATFORM_ENV_TOOLCHAIN_DETAIL)
{
    printf("TT_ENV_TOOLCHAIN_GNU");
    return 0;
}

__ENTRY_IMPL(PLATFORM_ENV_TOOLCHAIN_VER_DETAIL)
{
    int v = (__GNUC__ << 16) | (__GNUC_MINOR__ << 8) | 0;
    printf("0x%x", v);
    return 0;
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
    __ENTRY_START(PLATFORM_ENV_OS_DETAIL)
    __ENTRY_START(PLATFORM_ENV_OS_VER_DETAIL)
    __ENTRY_START(PLATFORM_ENV_OS_FEATURE_DETAIL)

    // cpu
    __ENTRY_START(PLATFORM_ENV_CPU_DETAIL)
    __ENTRY_START(PLATFORM_ENV_CPU_FEATURE_DETAIL)

    // toolchain
    __ENTRY_START(PLATFORM_ENV_TOOLCHAIN_DETAIL)
    __ENTRY_START(PLATFORM_ENV_TOOLCHAIN_VER_DETAIL)
}
