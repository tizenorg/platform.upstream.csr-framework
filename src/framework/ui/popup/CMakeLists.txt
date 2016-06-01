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
#
# @file     CMakeLists.txt
# @author   Janusz Kozerski (j.kozerski@samsung.com)
# @version  1.0
#
PKG_CHECK_MODULES(${TARGET_CSR_POPUP}_DEP
	REQUIRED
	elementary
	libsystemd-daemon
	vconf
)

INCLUDE_DIRECTORIES(
	${${TARGET_CSR_POPUP}_DEP_INCLUDE_DIRS}
	.
	${PROJECT_SOURCE_DIR}/src/framework
)

SET(${TARGET_CSR_POPUP}_SRCS
	main.cpp
	logic.cpp
	popup.cpp
	popup-wp-prompt.cpp
	popup-wp-notify.cpp
	popup-cs-prompt-app.cpp
	popup-cs-notify-app.cpp
	popup-cs-prompt-file.cpp
	popup-cs-notify-file.cpp
	popup-cs-prompt-data.cpp
	popup-cs-notify-data.cpp
	popup-service.cpp
	${PROJECT_SOURCE_DIR}/src/framework/ui/common.cpp
)

SET_SOURCE_FILES_PROPERTIES(${${TARGET_CSR_POPUP}_SRCS}
	PROPERTIES
		COMPILE_FLAGS "-D_GNU_SOURCE -fPIE"
		LINK_FLAGS "-pie"
)

ADD_EXECUTABLE(${TARGET_CSR_POPUP} ${${TARGET_CSR_POPUP}_SRCS})

TARGET_LINK_LIBRARIES(${TARGET_CSR_POPUP}
	${${TARGET_CSR_POPUP}_DEP_LIBRARIES}
	${TARGET_CSR_COMMON}
)

INSTALL(TARGETS ${TARGET_CSR_POPUP} DESTINATION ${BIN_DIR})
