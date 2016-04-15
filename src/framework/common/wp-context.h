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
 * @file        wp-context.h
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief       Wep protection context with dependent options
 */
#pragma once

#include <string>

#include "common/types.h"
#include "csr/web-protection-types.h"

namespace Csr {

class WpContext : public Context {
public:
	enum class Key : int {
		PopupMessage = 0x01, // string
		AskUser      = 0x10, // int
	};

	WpContext();
	virtual ~WpContext();

	WpContext(IStream &);
	virtual void Serialize(IStream &) const override;

	WpContext(WpContext &&);
	WpContext &operator=(WpContext &&);

	WpContext(const WpContext &);
	WpContext &operator=(const WpContext &);

	virtual void set(int, int) override;
	virtual void set(int, const std::string &) override;
	virtual void set(int, const char *) override;

	virtual void get(int, int &) const override;
	virtual void get(int, std::string &) const override;
	virtual void get(int, const char **) const override;

private:
	std::string m_popupMessage;
	csr_wp_ask_user_e m_askUser;
};

} // end of namespace Csr
