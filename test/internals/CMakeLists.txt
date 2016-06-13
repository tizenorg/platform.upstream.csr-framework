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
	pkgmgr-info
)

IF (WITH_SAMPLE_ENGINE)
	ADD_DEFINITIONS("-DWITH_SAMPLE_ENGINE")
	SET(SAMPLE_ENGINE_LINKED_SRCS
		test-cs-loader.cpp
		test-wp-loader.cpp
		test-api-engine-content-screening.cpp
		test-api-engine-web-protection.cpp
	)
ENDIF (WITH_SAMPLE_ENGINE)

SET(${TARGET_CSR_INTERNAL_TEST}_SRCS
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
	${CSR_FW_SRC_PATH}/service/engine-error-converter.cpp
	${CSR_FW_SRC_PATH}/service/dir-blacklist.cpp
	${CSR_FW_SRC_PATH}/ui/popup/package-info.cpp

	test-db.cpp
	test-cpucore.cpp
	test-file-system.cpp
	test-package-info.cpp

	test-main.cpp

	${SAMPLE_ENGINE_LINKED_SRCS}
)

INCLUDE_DIRECTORIES(
	SYSTEM
	${${TARGET_CSR_INTERNAL_TEST}_DEP_INCLUDE_DIRS}
)

INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/src/include/csr
	${PROJECT_SOURCE_DIR}/src/include/csre
	${CSR_FW_SRC_PATH}
	${CSR_FW_SRC_PATH}/ui/popup
	${CSR_TEST_SRC_PATH}
)

ADD_EXECUTABLE(${TARGET_CSR_INTERNAL_TEST} ${${TARGET_CSR_INTERNAL_TEST}_SRCS})

SET_TARGET_PROPERTIES(${TARGET_CSR_INTERNAL_TEST}
	PROPERTIES
		COMPILE_FLAGS "-D_GNU_SOURCE -fPIE"
		LINK_FLAGS "-pie -Wl,-rpath,${ENGINE_DIR}"
)

IF (WITH_SAMPLE_ENGINE)
TARGET_LINK_LIBRARIES(${TARGET_CSR_INTERNAL_TEST}
	${TARGET_CSR_TEST_COMMON}
	${TARGET_CSR_COMMON}
	${TARGET_CSR_CS_ENGINE}
	${TARGET_CSR_WP_ENGINE}
	${${TARGET_CSR_INTERNAL_TEST}_DEP_LIBRARIES}
	-lboost_unit_test_framework
	-ldl
)
ELSE (WITH_SAMPLE_ENGINE)
TARGET_LINK_LIBRARIES(${TARGET_CSR_INTERNAL_TEST}
	${TARGET_CSR_TEST_COMMON}
	${TARGET_CSR_COMMON}
	${${TARGET_CSR_INTERNAL_TEST}_DEP_LIBRARIES}
	-lboost_unit_test_framework
	-ldl
)
ENDIF (WITH_SAMPLE_ENGINE)

INSTALL(TARGETS ${TARGET_CSR_INTERNAL_TEST} DESTINATION ${BIN_DIR})
