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

IF (DEFINED CSR_FW_COMMON_BUILD)

PKG_CHECK_MODULES(${TARGET_CSR_COMMON}_DEP
    REQUIRED
)

SET(${TARGET_CSR_COMMON}_SRCS
)

INCLUDE_DIRECTORIES(
    common/include
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

INSTALL(TARGETS ${TARGET_CSR_COMMON} DESTINATION ${LIB_ISNTALL_DIR})

ENDIF (DEFINED CSR_FW_COMMON_BUILD)
