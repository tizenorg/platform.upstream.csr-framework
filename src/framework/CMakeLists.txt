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
# @brief       Make common library for both of server and client
#

IF (PLATFORM_VERSION_3)
	MESSAGE("Use Cynara as access control backend")
	SET(AC_BACKEND_COMMON_REQUIRE cynara-creds-socket)
	SET(AC_BACKEND_COMMON_SRCS common/credential-cynara.cpp)
ELSE (PLATFORM_VERSION_3)
	MESSAGE("Use Smack as access control backend")
	SET(AC_BACKEND_COMMON_REQUIRE libsmack)
	SET(AC_BACKEND_COMMON_SRCS common/credential-smack.cpp)
ENDIF (PLATFORM_VERSION_3)

PKG_CHECK_MODULES(${TARGET_CSR_COMMON}_DEP
	REQUIRED
	dlog
	libsystemd-daemon
	${AC_BACKEND_COMMON_REQUIRE}
)

SET(${TARGET_CSR_COMMON}_SRCS
	common/audit/console-sink.cpp
	common/audit/dlog-sink.cpp
	common/audit/logger.cpp
	common/binary-queue.cpp
	common/connection.cpp
	common/cs-context.cpp
	common/cs-detected.cpp
	common/exception.cpp
	common/wp-context.cpp
	common/wp-result.cpp
	common/em-context.cpp
	common/kvp-container.cpp
	common/dispatcher.cpp
	common/mainloop.cpp
	common/service.cpp
	common/socket.cpp
	common/socket-descriptor.cpp

	${AC_BACKEND_COMMON_SRCS}
)

INCLUDE_DIRECTORIES(
	./
	${PROJECT_SOURCE_DIR}/src/include/csr
	${PROJECT_SOURCE_DIR}/src/include/csre
	${${TARGET_CSR_COMMON}_DEP_INCLUDE_DIRS}
)

ADD_LIBRARY(${TARGET_CSR_COMMON} SHARED ${${TARGET_CSR_COMMON}_SRCS})

SET_TARGET_PROPERTIES(${TARGET_CSR_COMMON}
	PROPERTIES
		COMPILE_FLAGS "-D_GNU_SOURCE -fvisibility=hidden"
		SOVERSION ${API_VERSION}
		VERSION ${VERSION}
)

TARGET_LINK_LIBRARIES(${TARGET_CSR_COMMON}
	${${TARGET_CSR_COMMON}_DEP_LIBRARIES}
)

INSTALL(TARGETS ${TARGET_CSR_COMMON} DESTINATION ${LIB_INSTALL_DIR})

ADD_SUBDIRECTORY(ui)
