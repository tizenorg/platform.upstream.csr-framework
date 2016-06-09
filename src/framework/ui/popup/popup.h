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
 * @file        popup.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include <Elementary.h>
#include <string>
#include <vector>
#include <memory>

#include <csr-content-screening-types.h>
#include <csr-web-protection-types.h>

#include "common/audit/logger.h"
#include "common/binary-queue.h"
#include "popup-webview.h"

namespace Csr {
namespace Ui {

/*
 * --------------------
 * |      title       |
 * ========box=========
 * | content(header)  |
 * |------subBox------|
 * |icon|content(body)|
 * |------------------|
 * | -hypertext       |
 * |                  |
 * | content(footer)  |
 * ====================
 * |     button(N)    |
 * --------------------
 */

class Popup {
public:
	Popup(int buttonN);
	virtual ~Popup();

	void run(void);
	RawBuffer getResult(void);
	void setMessage(const std::string &msg) noexcept;

	Popup(const Popup &) = delete;
	Popup &operator=(const Popup &) = delete;
	Popup(Popup &&) = delete;
	Popup &operator=(Popup &&) = delete;

	void setTitle(const std::string &title) noexcept;
	void setHeader(const std::string &header) noexcept;
	void setBody(const std::string &body) noexcept;
	void setFooter(const std::string &footer) noexcept;
	void setIcon(const std::string &path) noexcept;
	void setText(Evas_Object *obj, const std::string &text) noexcept;

	void callbackRegister(Evas_Object *obj, int *type);
	void callbackRegister(Evas_Object *obj, std::string *url);
	static void btnClickedCb(void *data, Evas_Object *, void *);

	std::vector<Evas_Object *> m_objects;
	std::vector<Evas_Object *> m_buttons;
	Evas_Object *m_hypertext;
	std::vector<int> m_types;

private:
	Evas_Object *m_win;
	Evas_Object *m_popup;
	Evas_Object *m_box;
	Evas_Object *m_subBox;
	Evas_Object *m_header;
	Evas_Object *m_body;
	Evas_Object *m_footer;
	Evas_Object *m_icon;

	std::string m_message;
	std::string m_iconPath;
	std::string m_hypertextUrl;

	static int response;
};

} // namespace Ui
} // namespace Csr
