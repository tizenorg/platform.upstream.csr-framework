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
 * @file        em-context.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Engine manager context
 */
#pragma once

#include "common/icontext.h"
#include <csr-engine-manager.h>

namespace Csr {

struct EmContext;
using EmContextShPtr = std::shared_ptr<EmContext>;

struct EmContext : public IContext {
	enum class Key : int {
		EngineId = 0x01 // int
	};

	EmContext();
	virtual ~EmContext();

	EmContext(IStream &);
	virtual void Serialize(IStream &) const override;

	EmContext(EmContext &&) = delete;
	EmContext &operator=(EmContext &&) = delete;
	EmContext(const EmContext &) = delete;
	EmContext &operator=(const EmContext &) = delete;

	virtual void set(int, int) override;
	virtual void get(int, int &) const override;

	csr_engine_id_e engineId;
};

}
