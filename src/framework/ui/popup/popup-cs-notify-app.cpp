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
 * @file        popup-cs-notify-app.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */
#include "popup-cs-notify-app.h"

namespace Csr {
namespace Ui {

PopupCsNotifyApp::PopupCsNotifyApp(std::string name, std::string version,
	std::string risk, std::string malName) noexcept :
		Popup(2),
		m_appName(name),
		m_version(version),
		m_risk(risk),
		m_malName(malName)
{
}

void PopupCsNotifyApp::setLayout()
{
	setTitle("Malware detected");
	setHeader("Malware which is harm your phone is detected.");
	setBody(FORMAT(
			"App name : " << m_appName << "<br>" <<
			"Version : " << m_version << "<br>" <<
			"Risk : " << m_risk << " (" << m_malName << ")" <<
			"<br><br>" << "More information"));
	setFooter("Tap Uninstall to uninstall infected"
		"application and protect your phone.");

	setText(m_buttons[0], "OK");
	setText(m_buttons[1], "Uninstall");

	m_types.emplace_back(static_cast<int>(CSR_CS_PROCESSING_DISALLOWED));
	m_types.emplace_back(static_cast<int>(CSR_CS_PROCESSING_ALLOWED));

	callbackRegister(m_buttons[0], "clicked", &m_types[0]);
	callbackRegister(m_buttons[1], "clicked", &m_types[1]);
}
} // namespace Ui
} // namespace Csr
