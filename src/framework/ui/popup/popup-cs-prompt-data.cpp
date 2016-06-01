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
 * @file        popup-cs-notify-data.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */
#include "popup-cs-notify-data.h"

namespace Csr {
namespace Ui {

PopupCsNotifyData::PopupCsNotifyData(
	std::string severity, std::string malName) noexcept :
		Popup(1),
		m_severity(severity),
		m_malName(malName)
{
}

void PopupCsNotifyData::setLayout()
{
	setTitle("Malware detected");
	setHeader("Malware which is harm your phone is detected.");
	setBody(FORMAT(
			"- Risk : " << m_severity << " (" << m_malName << ")" <<
			"<br><br>" << "More information"));
	setFooter("Processing is prohibited to protect your phone.");

	setText(m_buttons[0], "OK");

	m_types.emplace_back(static_cast<int>(CSR_CS_PROCESSING_DISALLOWED));

	callbackRegister(m_buttons[0], "clicked", &m_types[0]);
}
} // namespace Ui
} // namespace Csr
