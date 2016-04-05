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
# @brief       build sample engine of csr web protection
#
PKG_CHECK_MODULES(${TARGET_CSR_WP_ENGINE_SAMPLE}_DEP
	REQUIRED
)

INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/src/include
	.
	${${TARGET_CSR_WP_ENGINE_SAMPLE}_DEP_INCLUDE_DIRS}
)

SET(${TARGET_CSR_WP_ENGINE_SAMPLE}_SRCS
	sample-engine.cpp
)

ADD_LIBRARY(${TARGET_CSR_WP_ENGINE_SAMPLE} SHARED ${${TARGET_CSR_WP_ENGINE_SAMPLE}_SRCS})

SET_TARGET_PROPERTIES(${TARGET_CSR_WP_ENGINE_SAMPLE}
	PROPERTIES
		COMPILE_FLAGS "-D_GNU_SOURCE -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(${TARGET_CSR_WP_ENGINE_SAMPLE}
	${${TARGET_CSR_WP_ENGINE_SAMPLE_DEP}_LIBRARIES}
)

INSTALL(TARGETS ${TARGET_CSR_WP_ENGINE_SAMPLE} DESTINATION ${SAMPLE_ENGINE_DIR})
INSTALL(DIRECTORY resources/ DESTINATION ${SAMPLE_ENGINE_RW_WORKING_DIR})
