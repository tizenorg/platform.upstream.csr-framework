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
 * @file        popup-wp-notify.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */
#include "popup-wp-notify.h"

namespace Csr {
namespace Ui {

PopupWpNotify::PopupWpNotify(std::string url, std::string risk) noexcept :
	Popup(2), m_url(url), m_risk(risk)
{
}

void PopupWpNotify::setLayout()
{
	setTitle("Block malicious URL");
	setHeader("This website may harm your phone.");
	setBody(FORMAT(
			"- URL : " << m_url << "<br>" <<
			"- Risk : " << m_risk << "<br><br>" <<
			"More information"));
	setFooter("If you really want to process anyway, tap View");

	setText(m_buttons[0], "OK");
	setText(m_buttons[1], "View");

	m_types.emplace_back(static_cast<int>(CSR_WP_USER_RESPONSE_PROCESSING_DISALLOWED));
	m_types.emplace_back(static_cast<int>(CSR_WP_USER_RESPONSE_PROCESSING_ALLOWED));

	callbackRegister(m_buttons[0], "clicked", &m_types[0]);
	callbackRegister(m_buttons[1], "clicked", &m_types[1]);
}
} // namespace Ui
} // namespace Csr
