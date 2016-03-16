# Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
# @file        CMakeLists.txt
# @author      Kyungwook Tak (k.tak@samsung.com)
# @brief       csr-framework project cmake file on the top
#
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(${SERVICE_NAME})

INCLUDE(FindPkgConfig)

SET(CMAKE_C_FLAGS_PROFILING    "-g -O0 -pg -Wp,-U_FORTIFY_SOURCE")
SET(CMAKE_CXX_FLAGS_PROFILING  "-g -std=c++0x -O0 -pg -Wp,-U_FORTIFY_SOURCE")
SET(CMAKE_C_FLAGS_DEBUG        "-g -O0 -ggdb -Wp,-U_FORTIFY_SOURCE")
SET(CMAKE_CXX_FLAGS_DEBUG      "-g -std=c++0x -O0 -ggdb -Wp,-U_FORTIFY_SOURCE")
SET(CMAKE_C_FLAGS_RELEASE      "-g -O2 -DNDEBUG")
SET(CMAKE_CXX_FLAGS_RELEASE    "-g -std=c++0x -O2 -DNDEBUG")
SET(CMAKE_C_FLAGS_CCOV         "-g -O2 --coverage")
SET(CMAKE_CXX_FLAGS_CCOV       "-g -std=c++0x -O2 --coverage")

ADD_DEFINITIONS("-fPIC")
ADD_DEFINITIONS("-Werror")
ADD_DEFINITIONS("-Wall")
ADD_DEFINITIONS("-Wextra")

STRING(REGEX MATCH "([^.]*)" API_VERSION "${VERSION}")
ADD_DEFINITIONS("-DSERVICE_NAME=\"${SERVICE_NAME}\"")
ADD_DEFINITIONS("-DINCLUDE_INSTALL_DIR=\"${INCLUDE_INSTALL_DIR}\"")
ADD_DEFINITIONS("-DBIN_DIR=\"${BIN_DIR}\"")

IF (CMAKE_BUILD_TYPE MATCHES "DEBUG")
	ADD_DEFINITIONS("-DTIZEN_DEBUG_ENABLE")
	ADD_DEFINITIONS("-DBUILD_TYPE_DEBUG")
ENDIF (CMAKE_BUILD_TYPE MATCHES "DEBUG")

SET(TARGET_CSR_SERVER ${SERVICE_NAME}-server)
SET(TARGET_CSR_CLIENT ${SERVICE_NAME}-client)
SET(TARGET_CSR_COMMON ${SERVICE_NAME}-common)
SET(TARGET_CSR_TEST ${SERVICE_NAME}-test)

CONFIGURE_FILE(packaging/${SERVICE_NAME}.manifest.in ${SERVICE_NAME}.manifest @ONLY)
CONFIGURE_FILE(packaging/${SERVICE_NAME}-client.manifest.in ${SERVICE_NAME}-client.manifest @ONLY)
CONFIGURE_FILE(packaging/${SERVICE_NAME}-common.manifest.in ${SERVICE_NAME}-common.manifest @ONLY)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(pkgconfig)
ADD_SUBDIRECTORY(systemd)
ADD_SUBDIRECTORY(test)
