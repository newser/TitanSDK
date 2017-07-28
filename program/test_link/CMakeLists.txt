project(test_link)

cmake_minimum_required(VERSION 3.1.3)

# version number would be changed
set(PLAT_INC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(PLAT_STATIC ${CMAKE_CURRENT_SOURCE_DIR}/static)
set(PLAT_SHARED ${CMAKE_CURRENT_SOURCE_DIR}/shared)
set(PLAT_PROGRAM ${CMAKE_CURRENT_SOURCE_DIR}/program)

# static
add_executable(test_link_static test_link.c)

target_include_directories(test_link_static PRIVATE ${PLAT_INC})

find_library(s_platform NAMES platform PATHS ${PLAT_STATIC} NO_DEFAULT_PATH)
find_library(s_cares NAMES cares PATHS ${PLAT_STATIC} NO_DEFAULT_PATH)
find_library(s_mbedcrypto NAMES mbedcrypto PATHS ${PLAT_STATIC} NO_DEFAULT_PATH)
find_library(s_mbedtls NAMES mbedtls PATHS ${PLAT_STATIC} NO_DEFAULT_PATH)
find_library(s_mbedx509 NAMES mbedx509 PATHS ${PLAT_STATIC} NO_DEFAULT_PATH)
find_library(s_xml NAMES xml PATHS ${PLAT_STATIC} NO_DEFAULT_PATH)
set(STATIC_LIB ${STATIC_LIB} ${s_platform} ${s_cares} ${s_mbedtls} ${s_mbedx509} ${s_mbedcrypto} ${s_xml})
target_link_libraries(test_link_static ${STATIC_LIB})

# shared
add_executable(test_link_shared test_link.c)

target_include_directories(test_link_shared PRIVATE ${PLAT_INC})

find_library(d_platform NAMES platform PATHS ${PLAT_SHARED} ${PLAT_PROGRAM} NO_DEFAULT_PATH)
set(SHARED_LIB ${SHARED_LIB} ${d_platform})
target_link_libraries(test_link_shared ${SHARED_LIB})

# platform specific
if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    target_link_libraries(test_link_static winmm ws2_32 iphlpapi)
    target_link_libraries(test_link_static winmm ws2_32 iphlpapi)
    
    string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
    string(REPLACE "/MDd" "/MTd" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
    target_link_libraries(test_link_static stdc++ m pthread)
    target_link_libraries(test_link_shared stdc++ m pthread)
elseif (CMAKE_SYSTEM_NAME STREQUAL Darwin)
    target_link_libraries(test_link_static stdc++ "-framework CoreFoundation")
    target_link_libraries(test_link_shared stdc++ "-framework CoreFoundation")
    # set rpath??
elseif (CMAKE_SYSTEM_NAME STREQUAL iOS)
    target_link_libraries(test_link_static stdc++ "-framework CoreFoundation")
    target_link_libraries(test_link_shared stdc++ "-framework CoreFoundation")
else ()
    message(FATAL_ERROR "unsupported os: ${${CMAKE_SYSTEM_NAME}}")
endif ()
