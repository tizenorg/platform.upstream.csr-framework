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
#include <memory>

#include "common/icontext.h"
#include <csr-web-protection-types.h>

namespace Csr {

struct WpContext;
using WpContextPtr = std::unique_ptr<WpContext>;
using WpContextShPtr = std::shared_ptr<WpContext>;

struct WpContext : public IContext {
	enum class Key : int {
		PopupMessage = 0x01, // string
		AskUser      = 0x10, // int
	};

	WpContext() noexcept;
	virtual ~WpContext() noexcept;

	WpContext(IStream &);
	virtual void Serialize(IStream &) const override;

	WpContext(WpContext &&) = delete;
	WpContext &operator=(WpContext &&) = delete;
	WpContext(const WpContext &) = delete;
	WpContext &operator=(const WpContext &) = delete;

	virtual void set(int, int) override;
	virtual void set(int, const std::string &) override;
	virtual void set(int, const char *) override;

	virtual void get(int, int &) const override;
	virtual void get(int, std::string &) const override;
	virtual void get(int, const char **) const override;

	std::string popupMessage;
	csr_wp_ask_user_e askUser;
};

} // end of namespace Csr
