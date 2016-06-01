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
 * @file        popup-wp-prompt.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */
#include "popup-wp-prompt.h"

namespace Csr {
namespace Ui {

PopupWpPrompt::PopupWpPrompt(const std::string &url, const std::string &risk) noexcept :
	Popup(1), m_url(url), m_risk(risk)
{
}

void PopupWpPrompt::setLayout()
{
	setTitle("Block malicious URL");
	setHeader("This website may harm your phone.");
	setBody(FORMAT(
			"- URL : " << m_url << "<br>" <<
			"- Risk : " << m_risk << "<br><br>" <<
			"More information"));
	setFooter("Accessing to this URL is prohibited to protect your phone.");

	setText(m_buttons[0], "OK");

	m_types.emplace_back(static_cast<int>(CSR_WP_USER_RESPONSE_PROCESSING_DISALLOWED));

	callbackRegister(m_buttons[0], "clicked", &m_types[0]);
}
} // namespace Ui
} // namespace Csr
