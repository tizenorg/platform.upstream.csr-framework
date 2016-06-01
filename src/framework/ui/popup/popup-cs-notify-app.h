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
 * @file        popup-cs-notify-app.h
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include "popup.h"

namespace Csr {
namespace Ui {

class PopupCsNotifyApp : public Popup {
public:
	PopupCsNotifyApp(const std::string &appName, const std::string &version,
		const std::string &risk, const std::string &malName) noexcept;
	virtual ~PopupCsNotifyApp() = default;

	PopupCsNotifyApp(const PopupCsNotifyApp &) = delete;
	PopupCsNotifyApp &operator=(const PopupCsNotifyApp &) = delete;
	PopupCsNotifyApp(PopupCsNotifyApp &&) = delete;
	PopupCsNotifyApp &operator=(PopupCsNotifyApp &&) = delete;


private:
	void setLayout() override;

// TODO(sangwan.kwon) Insert icon to layout.
	std::string m_appName;
	std::string m_version;
	std::string m_risk;
	std::string m_malName;
};
} // namespace Ui
} // namespace Csr
