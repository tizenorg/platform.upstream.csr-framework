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

#include "common/types.h"
#include "csr/content-screening-types.h"

namespace Csr {

class CsContext : public Context {
public:
	enum class Key : int {
		PopupMessage = 0x01, // string

		AskUser      = 0x10, // int
		CoreUsage    = 0x11, // int

		ScanOnCloud  = 0x20, // bool
	};

	CsContext();
	virtual ~CsContext();

	CsContext(IStream &);
	virtual void Serialize(IStream &) const;

	CsContext(CsContext &&);
	CsContext &operator=(CsContext &&);

	CsContext(const CsContext &);
	CsContext &operator=(const CsContext &);

	virtual void set(int, int) override;
	virtual void set(int, const std::string &) override;
	virtual void set(int, const char *) override;
	virtual void set(int, bool) override;

	virtual void get(int, int &) const override;
	virtual void get(int, std::string &) const override;
	virtual void get(int, const char **) const override;
	virtual void get(int, bool &) const override;

private:
	std::string m_popupMessage;
	csr_cs_ask_user_e m_askUser;
	csr_cs_core_usage_e m_coreUsage;
	bool m_isScanOnCloud;
};

}
