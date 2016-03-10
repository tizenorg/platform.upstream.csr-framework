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

IF (DEFINED CSR_FW_SERVER_BUILD)

###############     SERVER     ###################
PKG_CHECK_MODULES(${TARGET_CSR_SERVER}_DEP
    REQUIRED
)

SET(${TARGET_CSR_SERVER}_SRCS
)

INCLUDE_DIRECTORIES(
    framework/service/include
    framework/common/include
    ${${TARGET_CSR_SERVER}_DEP_INCLUDED_DIRS}
)

SET_SOURCE_FILES_PROPERTIES(${${TARGET_CSR_SERVER}_SRCS}
    PROPERTIES
        COMPILE_FLAGS "-D_GNU_SOURCE -fPIE"
)

ADD_EXECUTABLE(${TARGET_CSR_SERVER} ${${TARGET_CSR_SERVER}_SRCS})

SET(${TARGET_CSR_SERVER}_LIST_LINK_LIBRARIES
    ${${TARGET_CSR_SERVER}_DEP_LIBRARIES}
    -ldl     # for dynamic loading engine library
    -pthread
    -pie
)

IF (DEFINED CSR_FW_COMMON_BUILD)
SET(${TARGET_CSR_SERVER}_LIST_LINK_LIBRARIES
    ${${TARGET_CSR_SERVER}_LIST_LINK_LIBRARIES}
    ${TARGET_CSR_COMMON}
)
ENDIF (DEFINED CSR_FW_COMMON_BUILD)

TARGET_LINK_LIBRARIES(${TARGET_CSR_SERVER} ${${TARGET_CSR_SERVER}_LIST_LINK_LIBRARIES})

INSTALL(TARGETS ${TARGET_CSR_SERVER} DESTINATION ${BIN_DIR})

ENDIF (DEFINED CSR_FW_SERVER_BUILD)

###############      CLIENT     ###################
PKG_CHECK_MODULES(${TARGET_CSR_CLIENT}_DEP
    REQUIRED
)

SET(${TARGET_CSR_CLIENT}_SRCS
    framework/client/TCSImpl.c
    framework/client/TWPImpl.c
)

INCLUDE_DIRECTORIES(
    framework/common/include
    framework/client/include
    ${PROJECT_SOURCE_DIR}/src/include/csr
    ${${TARGET_CSR_CLIENT}_DEP_INCLUDE_DIRS}
)

ADD_LIBRARY(${TARGET_CSR_CLIENT} SHARED ${${TARGET_CSR_CLIENT}_SRCS})

SET_TARGET_PROPERTIES(${TARGET_CSR_CLIENT}
    PROPERTIES
        COMPILE_FLAGS "-D_GNU_SOURCE -fvisibility=hidden"
        SOVERSION ${API_VERSION}
        VERSION ${VERSION}
)

SET(${TARGET_CSR_CLIENT}_LIST_LINK_LIBRARIES
    ${${TARGET_CSR_CLIENT}_DEP_LIBRARIES}
)

IF (DEFINED CSR_FW_COMMON_BUILD)
SET(${TARGET_CSR_CLIENT}_LIST_LINK_LIBRARIES
    ${${TARGET_CSR_CLIENT}_LIST_LINK_LIBRARIES}
    ${TARGET_CSR_COMMON}
)
ENDIF (DEFINED CSR_FW_COMMON_BUILD)

TARGET_LINK_LIBRARIES(${TARGET_CSR_CLIENT} ${${TARGET_CSR_CLIENT}_LIST_LINK_LIBRARIES})

INSTALL(TARGETS ${TARGET_CSR_CLIENT} DESTINATION ${LIB_INSTALL_DIR})

ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(framework)
