# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.    See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.    You may obtain a copy of the License at
#
#         http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# this file only helps environment.cmake to construct environment name,
# more things like complier flags, sysroot path are defined in
# android-ndk.cmake

# system
set(CMAKE_SYSTEM_NAME Android)

# api version
set(CMAKE_SYSTEM_VERSION 21)

# abi: "armeabi" "armeabi-v7a" "arm64-v8a" "x86" "x86_64"
set(CMAKE_ANDROID_ARCH_ABI x86)

# ndk path
set(CMAKE_ANDROID_NDK "/Users/haniu/Library/Android/sdk/ndk-bundle")
