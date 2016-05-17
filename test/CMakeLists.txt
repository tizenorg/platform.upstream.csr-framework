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
# @brief       build test program of csr-framework
#
FIND_PACKAGE(Threads REQUIRED)
ADD_DEFINITIONS("-DBOOST_TEST_DYN_LINK")
ADD_DEFINITIONS("-DTEST_TARGET=\"${TEST_TARGET}\"")

################### make test common library ##################
PKG_CHECK_MODULES(${TARGET_CSR_TEST_COMMON}_DEP
	REQUIRED
	glib-2.0
	pkgmgr
	pkgmgr-info
)

SET(${TARGET_CSR_TEST_COMMON}_SRCS
	test-common.cpp
	colour_log_formatter.cpp
)

INCLUDE_DIRECTORIES(
	SYSTEM
	${${TARGET_CSR_TEST_COMMON}_DEP_INCLUDE_DIRS}
)

INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/src/include
	.
)

ADD_LIBRARY(${TARGET_CSR_TEST_COMMON} SHARED ${${TARGET_CSR_TEST_COMMON}_SRCS})

SET_TARGET_PROPERTIES(${TARGET_CSR_TEST_COMMON}
	PROPERTIES
		COMPILE_FLAGS "-D_GNU_SOURCE -fvisibility=default"
)

TARGET_LINK_LIBRARIES(${TARGET_CSR_TEST_COMMON}
	${${TARGET_CSR_TEST_COMMON}_DEP_LIBRARIES}
	${TARGET_CSR_CLIENT}
)

INSTALL(TARGETS ${TARGET_CSR_TEST_COMMON} DESTINATION ${LIB_INSTALL_DIR})
#################################################################

###################### make api test program ####################
PKG_CHECK_MODULES(${TARGET_CSR_TEST}_DEP
	REQUIRED
)

SET(${TARGET_CSR_TEST}_SRCS
	test-api-content-screening.cpp
	test-api-content-screening-async.cpp
	test-api-engine-manager.cpp
	test-api-web-protection.cpp
	test-api-engine-content-screening.cpp
	test-api-engine-web-protection.cpp
	test-main.cpp
	test-helper.cpp
)

INCLUDE_DIRECTORIES(
	SYSTEM
	${${TARGET_CSR_TEST}_DEP_INCLUDE_DIRS}
)

INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/src/include
	.
)

ADD_EXECUTABLE(${TARGET_CSR_TEST} ${${TARGET_CSR_TEST}_SRCS})

TARGET_LINK_LIBRARIES(${TARGET_CSR_TEST}
	${TARGET_CSR_TEST_COMMON}
	${TARGET_CSR_CLIENT}
	${TARGET_CSR_CS_ENGINE_SAMPLE}
	${TARGET_CSR_WP_ENGINE_SAMPLE}
	${${TARGET_CSR_TEST}_DEP_LIBRARIES}
	-lboost_unit_test_framework
	-ldl
)

INSTALL(TARGETS ${TARGET_CSR_TEST} DESTINATION ${BIN_DIR})
INSTALL(DIRECTORY resources/ DESTINATION ${TEST_DIR})

ADD_SUBDIRECTORY(internals)
ADD_SUBDIRECTORY(popup)
ADD_SUBDIRECTORY(thread-pool)
#################################################################
