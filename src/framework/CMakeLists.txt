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
PKG_CHECK_MODULES(${TARGET_CSR_COMMON}_DEP
	REQUIRED
	dlog
	libsystemd-daemon
)

SET(${TARGET_CSR_COMMON}_SRCS
	common/audit/console-sink.cpp
	common/audit/dlog-sink.cpp
	common/audit/logger.cpp
	common/binary-queue.cpp
	common/connection.cpp
	common/cs-types.cpp
	common/dispatcher.cpp
	common/mainloop.cpp
	common/message-buffer.cpp
	common/service.cpp
	common/socket.cpp
	common/wp-types.cpp
)

INCLUDE_DIRECTORIES(
	common
	${${TARGET_CSR_COMMON}_DEP_INCLUDE_DIRS}
)

ADD_LIBRARY(${TARGET_CSR_COMMON} SHARED ${${TARGET_CSR_COMMON}_SRCS})

SET_TARGET_PROPERTIES(${TARGET_CSR_COMMON}
	PROPERTIES
		COMPILE_FLAGS "-D_GNU_SOURCE -fvisibility=default" # TODO: visibility hidden
		SOVERSION ${API_VERSION}
		VERSION ${VERSION}
)

TARGET_LINK_LIBRARIES(${TARGET_CSR_COMMON}
	${${TARGET_CSR_COMMON}_DEP_LIBRARIES}
)

INSTALL(TARGETS ${TARGET_CSR_COMMON} DESTINATION ${LIB_INSTALL_DIR})

ADD_SUBDIRECTORY(ui)
