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
# @brief       Make server daemon and client library
#

###############     SERVER     ###################

IF (PLATFORM_VERSION_3)
	MESSAGE("Use Cynara as access control backend")
	SET(AC_BACKEND_REQUIRE cynara-client)
	SET(AC_BACKEND_SRCS framework/service/access-control-cynara.cpp)
ELSE (PLATFORM_VERSION_3)
	MESSAGE("Use Smack as access control backend")
	SET(AC_BACKEND_REQUIRE libsmack)
	SET(AC_BACKEND_SRCS framework/service/access-control-smack.cpp)
ENDIF (PLATFORM_VERSION_3)

PKG_CHECK_MODULES(${TARGET_CSR_SERVER}_DEP
	REQUIRED
	libsystemd-daemon
	sqlite3
	pkgmgr
	pkgmgr-info
	${AC_BACKEND_REQUIRE}
)

SET(${TARGET_CSR_SERVER}_SRCS
	framework/main/csr-main.cpp
	framework/service/logic.cpp
	framework/service/cs-logic.cpp
	framework/service/wp-logic.cpp
	framework/service/em-logic.cpp
	framework/service/type-converter.cpp
	framework/service/server-service.cpp
	framework/service/thread-pool.cpp
	framework/service/core-usage.cpp
	framework/service/fs-utils.cpp
	framework/service/file-system.cpp
	framework/service/app-deleter.cpp
	framework/service/cs-loader.cpp
	framework/service/wp-loader.cpp
	framework/service/engine-error-converter.cpp
	framework/ui/askuser.cpp

	# question and response codes needed on both of
	# csr-server and popup-backend service
	framework/ui/common.cpp
	framework/db/connection.cpp
	framework/db/statement.cpp
	framework/db/manager.cpp

	${AC_BACKEND_SRCS}
)

INCLUDE_DIRECTORIES(
	framework
	${PROJECT_SOURCE_DIR}/src/include/csr
	${PROJECT_SOURCE_DIR}/src/include/csre
	${${TARGET_CSR_SERVER}_DEP_INCLUDE_DIRS}
)

SET_SOURCE_FILES_PROPERTIES(${${TARGET_CSR_SERVER}_SRCS}
	PROPERTIES
		COMPILE_FLAGS "-D_GNU_SOURCE -fPIE"
)

ADD_EXECUTABLE(${TARGET_CSR_SERVER} ${${TARGET_CSR_SERVER}_SRCS})

TARGET_LINK_LIBRARIES(${TARGET_CSR_SERVER}
	${${TARGET_CSR_SERVER}_DEP_LIBRARIES}
	${TARGET_CSR_COMMON}
	-ldl
	-pthread
	-pie
)

INSTALL(TARGETS ${TARGET_CSR_SERVER} DESTINATION ${BIN_DIR})

###############      CLIENT     ###################
PKG_CHECK_MODULES(${TARGET_CSR_CLIENT}_DEP
	REQUIRED
	capi-base-common
)

SET(${TARGET_CSR_CLIENT}_SRCS
	framework/client/async-logic.cpp
	framework/client/canonicalize.cpp
	framework/client/content-screening.cpp
	framework/client/engine-manager.cpp
	framework/client/error.cpp
	framework/client/handle.cpp
	framework/client/handle-ext.cpp
	framework/client/utils.cpp
	framework/client/web-protection.cpp
)

INCLUDE_DIRECTORIES(
	framework
	${PROJECT_SOURCE_DIR}/src/include/csr
	${PROJECT_SOURCE_DIR}/src/include/csre
	${${TARGET_CSR_CLIENT}_DEP_INCLUDE_DIRS}
)

ADD_LIBRARY(${TARGET_CSR_CLIENT} SHARED ${${TARGET_CSR_CLIENT}_SRCS})

SET_TARGET_PROPERTIES(${TARGET_CSR_CLIENT}
	PROPERTIES
		COMPILE_FLAGS "-D_GNU_SOURCE -fvisibility=hidden"
		SOVERSION ${API_VERSION}
		VERSION ${VERSION}
)

TARGET_LINK_LIBRARIES(${TARGET_CSR_CLIENT}
	${${TARGET_CSR_CLIENT}_DEP_LIBRARIES}
	${TARGET_CSR_COMMON}
)

INSTALL(TARGETS ${TARGET_CSR_CLIENT} DESTINATION ${LIB_INSTALL_DIR})

ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(framework)
