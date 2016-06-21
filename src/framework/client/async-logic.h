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
 * @file        async-logic.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include "common/types.h"
#include "client/handle-ext.h"

namespace Csr {
namespace Client {

class AsyncLogic {
public:
	AsyncLogic(HandleExt *handle, void *userdata);
	virtual ~AsyncLogic();

	void scanFiles(const StrSet &files);
	void scanDirs(const StrSet &dirs);

	void stop(void);

private:
	void scanHelper(const CommandId &id, const StrSet &s);

	HandleExt *m_handle; // for registering results for auto-release
	void *m_userdata;
};

}
}
