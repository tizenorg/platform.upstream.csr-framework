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
# @brief       build test program of csr internal unit test
#
FIND_PACKAGE(Threads REQUIRED)
ADD_DEFINITIONS("-DBOOST_TEST_DYN_LINK")

SET(CSR_TEST_SRC_PATH ${PROJECT_SOURCE_DIR}/test)
SET(CSR_FW_SRC_PATH ${PROJECT_SOURCE_DIR}/src/framework)

PKG_CHECK_MODULES(${TARGET_CSR_INTERNAL_TEST}_DEP
	REQUIRED
	sqlite3
	glib-2.0
	pkgmgr
	pkgmgr-info
)

SET(${TARGET_CSR_INTERNAL_TEST}_SRCS
	${CSR_TEST_SRC_PATH}/colour_log_formatter.cpp
	${CSR_TEST_SRC_PATH}/test-common.cpp

	${CSR_FW_SRC_PATH}/common/exception.cpp

	${CSR_FW_SRC_PATH}/db/connection.cpp
	${CSR_FW_SRC_PATH}/db/statement.cpp
	${CSR_FW_SRC_PATH}/db/manager.cpp
	${CSR_FW_SRC_PATH}/service/core-usage.cpp
	${CSR_FW_SRC_PATH}/service/fs-utils.cpp
	${CSR_FW_SRC_PATH}/service/file-system.cpp
	${CSR_FW_SRC_PATH}/service/app-deleter.cpp
	${CSR_FW_SRC_PATH}/service/cs-loader.cpp
	${CSR_FW_SRC_PATH}/service/wp-loader.cpp
	${CSR_FW_SRC_PATH}/client/canonicalize.cpp

	test-db.cpp
	test-cpucore.cpp
	test-file-system.cpp
	test-cs-loader.cpp
	test-wp-loader.cpp
	test-canonicalize.cpp

	test-main.cpp
)

INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/src/include
	${CSR_FW_SRC_PATH}
	${CSR_TEST_SRC_PATH}
	${${TARGET_CSR_INTERNAL_TEST}_DEP_INCLUDE_DIRS}
)

ADD_EXECUTABLE(${TARGET_CSR_INTERNAL_TEST} ${${TARGET_CSR_INTERNAL_TEST}_SRCS})

TARGET_LINK_LIBRARIES(${TARGET_CSR_INTERNAL_TEST}
	${TARGET_CSR_COMMON}
	${${TARGET_CSR_INTERNAL_TEST}_DEP_LIBRARIES}
	-lboost_unit_test_framework
	-ldl
)

INSTALL(TARGETS ${TARGET_CSR_INTERNAL_TEST} DESTINATION ${BIN_DIR})
