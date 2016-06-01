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
 * @file        popup-cs-prompt-data.h
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include "popup.h"

namespace Csr {
namespace Ui {

class PopupCsPromptData : public Popup {
public:
	PopupCsPromptData(const std::string &severity, const std::string &malName) noexcept;
	virtual ~PopupCsPromptData() = default;

	PopupCsPromptData(const PopupCsPromptData &) = delete;
	PopupCsPromptData &operator=(const PopupCsPromptData &) = delete;
	PopupCsPromptData(PopupCsPromptData &&) = delete;
	PopupCsPromptData &operator=(PopupCsPromptData &&) = delete;

private:
	void setLayout() override;

	std::string m_severity;
	std::string m_malName;
};
} // namespace Ui
} // namespace Csr
