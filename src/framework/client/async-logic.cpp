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
 * @file        async-logic.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#include "client/async-logic.h"

#include <cstdint>
#include <utility>

#include "common/exception.h"
#include "common/cs-detected.h"
#include "common/audit/logger.h"

namespace Csr {
namespace Client {

AsyncLogic::AsyncLogic(HandleExt *handle, void *userdata) :
	m_handle(handle), m_userdata(userdata)
{
}

AsyncLogic::~AsyncLogic()
{
}

void AsyncLogic::scanDirs(const StrSet &dirs)
{
	this->scanHelper(CommandId::SCAN_DIRS_ASYNC, dirSet);
}

void AsyncLogic::scanFiles(const StrSet &fileSet)
{
	this->scanHelper(CommandId::SCAN_FILES_ASYNC, fileSet);
}

void AsyncLogic::scanHelper(const CommandId &id, const StrSet &s)
{
	auto ret =
}

} // namespace Client
} // namespace Csr
