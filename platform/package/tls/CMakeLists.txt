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

function(hide_var var value)
  set(${var} ${value} PARENT_SCOPE)
  mark_as_advanced(${var})
endfunction(hide_var)

#
# customization
#

# no need pkcs11 heler lib
hide_var(USE_PKCS11_HELPER_LIBRARY, OFF)

# may add compression support in future
hide_var(ENABLE_ZLIB_SUPPORT OFF)

# no need programs
hide_var(ENABLE_PROGRAMS OFF)

hide_var(UNSAFE_BUILD OFF)

# no need test cases
hide_var(ENABLE_TESTING OFF)

# lib type depends on platform configuration
hide_var(USE_STATIC_MBEDTLS_LIBRARY ON)
hide_var(USE_SHARED_MBEDTLS_LIBRARY OFF)

# platform will decide how to link pthread
hide_var(LINK_WITH_PTHREAD OFF)

# no need to install header files
hide_var(INSTALL_MBEDTLS_HEADERS OFF)

add_subdirectory(${TLS_PATH})

#
# result
#

set(TLS_LIB mbedtls PARENT_SCOPE)
set(X509_LIB mbedx509 PARENT_SCOPE)
set(CRYPTO_LIB mbedcrypto PARENT_SCOPE)
