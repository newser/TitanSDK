# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# definition
#

set(TLS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls)

#
# customization
#

# no need pkcs11 heler lib
hide(USE_PKCS11_HELPER_LIBRARY BOOL OFF)

# may add compression support in future
hide(ENABLE_ZLIB_SUPPORT BOOL OFF)

# no need programs
#set(ENABLE_PROGRAMS OFF CACHE BOOL "" FORCE)
hide(ENABLE_PROGRAMS BOOL OFF)

hide(UNSAFE_BUILD BOOL OFF)

# no need test cases
hide(ENABLE_TESTING BOOL OFF)

# must be static
hide(USE_STATIC_MBEDTLS_LIBRARY BOOL ON)
hide(USE_SHARED_MBEDTLS_LIBRARY BOOL OFF)

# platform will decide how to link pthread
hide(LINK_WITH_PTHREAD BOOL OFF)

# no need to install header files
hide(INSTALL_MBEDTLS_HEADERS BOOL OFF)

# customize configuration
add_definitions(-DMBEDTLS_CONFIG_FILE="tt_mbedtls_config.h")

add_subdirectory(${TLS_PATH})

#
# result
#

set(TLS_LIB mbedtls PARENT_SCOPE)
set(X509_LIB mbedx509 PARENT_SCOPE)
set(CRYPTO_LIB mbedcrypto PARENT_SCOPE)
