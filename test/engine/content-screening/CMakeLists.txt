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
# @brief       build sample engine of content screening
#
PKG_CHECK_MODULES(${TARGET_CSR_CS_ENGINE}_DEP
	REQUIRED
)

INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/src/include/csr
	${PROJECT_SOURCE_DIR}/src/include/csre
	.
	${${TARGET_CSR_CS_ENGINE}_DEP_INCLUDE_DIRS}
)

SET(${TARGET_CSR_CS_ENGINE}_SRCS
	sample-engine.cpp
)

ADD_LIBRARY(${TARGET_CSR_CS_ENGINE} SHARED ${${TARGET_CSR_CS_ENGINE}_SRCS})

SET_TARGET_PROPERTIES(${TARGET_CSR_CS_ENGINE}
	PROPERTIES
		COMPILE_FLAGS "-D_GNU_SOURCE -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(${TARGET_CSR_CS_ENGINE}
	${${TARGET_CSR_CS_ENGINE_DEP}_LIBRARIES}
)

INSTALL(TARGETS ${TARGET_CSR_CS_ENGINE} DESTINATION ${ENGINE_DIR})
INSTALL(DIRECTORY resources/ DESTINATION ${ENGINE_RW_WORKING_DIR})
