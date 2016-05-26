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
# @brief       build test program of csr-popup
#
PKG_CHECK_MODULES(${TARGET_CSR_POPUP_TEST}_DEP
	REQUIRED
)

SET(${TARGET_CSR_POPUP_TEST}_SRCS
	${PROJECT_SOURCE_DIR}/src/framework/ui/askuser.cpp
	${PROJECT_SOURCE_DIR}/src/framework/ui/common.cpp
	test-main.cpp
	test-popup.cpp
)

INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/src/include/csr
	${PROJECT_SOURCE_DIR}/src/include/csre
	${PROJECT_SOURCE_DIR}/src/framework
	${PROJECT_SOURCE_DIR}/test
	.
	${${TARGET_CSR_POPUP_TEST}_DEP_INCLUDE_DIRS}
)

ADD_EXECUTABLE(${TARGET_CSR_POPUP_TEST} ${${TARGET_CSR_POPUP_TEST}_SRCS})

TARGET_LINK_LIBRARIES(${TARGET_CSR_POPUP_TEST}
	${TARGET_CSR_TEST_COMMON}
	${TARGET_CSR_COMMON}
	-lboost_unit_test_framework
	-ldl
)

INSTALL(TARGETS ${TARGET_CSR_POPUP_TEST} DESTINATION ${BIN_DIR})
