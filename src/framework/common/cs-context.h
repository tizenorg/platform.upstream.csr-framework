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
 * @file        cs-context.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Content screening context with dependent options
 */
#pragma once

#include <string>
#include <memory>

#include <csr-content-screening-types.h>

#include "common/macros.h"
#include "common/icontext.h"

namespace Csr {

struct CsContext;
using CsContextPtr = std::unique_ptr<CsContext>;
using CsContextShPtr = std::shared_ptr<CsContext>;

struct API CsContext : public IContext {
	enum class Key : int {
		PopupMessage = 0x01, // string

		AskUser      = 0x10, // int
		CoreUsage    = 0x11, // int

		ScanOnCloud  = 0x20, // bool
		ScannedCbRegistered = 0x21,
	};

	CsContext() noexcept;
	virtual ~CsContext() = default;

	CsContext(IStream &);
	virtual void Serialize(IStream &) const override;

	CsContext(CsContext &&) = delete;
	CsContext &operator=(CsContext &&) = delete;
	CsContext(const CsContext &) = delete;
	CsContext &operator=(const CsContext &) = delete;

	virtual void set(int, int) override;
	virtual void set(int, const std::string &) override;
	virtual void set(int, const char *) override;
	virtual void set(int, bool) override;

	virtual void get(int, int &) const override;
	virtual void get(int, std::string &) const override;
	virtual void get(int, const char **) const override;
	virtual void get(int, bool &) const override;

	std::string popupMessage;
	csr_cs_ask_user_e askUser;
	csr_cs_core_usage_e coreUsage;
	bool isScanOnCloud;
	bool isScannedCbRegistered;
};

}
