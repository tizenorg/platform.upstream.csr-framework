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
 * @file        popup-webview.h
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include <string>

#include <Elementary.h>
#include <EWebKit.h>
#include <ewk_view.h>
#include <efl_extension.h>

#include "common/audit/logger.h"

namespace Csr {
namespace Ui {

class Webview {
public:
	Webview(const std::string &url);
	virtual ~Webview() = default;

	Webview(const Webview &) = delete;
	Webview &operator=(const Webview &) = delete;
	Webview(Webview &&) = delete;
	Webview &operator=(Webview &&) = delete;

	void run(void);

	static void winCloseCb(void *data, Evas_Object *, void *);
	static void closeCb(void *data, Evas_Object *, void *);

private:
	Evas_Object *m_win;
	Evas_Object *m_box;
	Evas_Object *m_subBox;
	Evas_Object *m_webview;
	Evas_Object *m_label;
	Evas_Object *m_check;
	Evas_Object *m_bg;
	std::string m_url;
	int m_winW;
	int m_winH;
};

} // namespace Ui
} // namespace Csr
