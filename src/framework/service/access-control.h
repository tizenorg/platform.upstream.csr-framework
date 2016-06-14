/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/*
 * @file        access-control.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include "common/connection.h"
#include "common/socket-descriptor.h"

namespace Csr {

void hasPermission(const ConnShPtr &conn);

// for commands which has different privilege from
// socket descriptor map information. Needed privilege and label should be used
// associated with input sockId.
// e.g., JUDGE_STATUS dispatched from SockId::CS but it's SockId::ADMIN privileged.
// Throw exception (PermDenied) if it's denied.
void hasPermission(const ConnShPtr &conn, SockId sockId);

}
