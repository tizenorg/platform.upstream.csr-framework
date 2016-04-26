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
 * @file        popup.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#include "popup.h"

#include <stdexcept>

#include "common/audit/logger.h"

namespace Csr {
namespace Ui {

Popup::Popup()
{
	Evas_Object *win = elm_win_add(nullptr, "CSR popup", ELM_WIN_NOTIFICATION);
	elm_win_indicator_opacity_set(win, ELM_WIN_INDICATOR_TRANSLUCENT);
	elm_win_borderless_set(win, EINA_TRUE);
	elm_win_alpha_set(win, EINA_TRUE);

	Evas_Object *popup = elm_popup_add(win);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	m_win = win;
	m_popup = popup;
}

Popup::~Popup()
{
}

void Popup::stop()
{
	if (m_popup)
		evas_object_del(m_popup);

	if (m_win)
		evas_object_del(m_win);

	elm_exit();
}

void Popup::start()
{
	evas_object_show(m_win);
	evas_object_show(m_popup);

	DEBUG("Popup UI event loop start!");

	elm_run();
}

void Popup::fillText(const std::string &title, const std::string &content)
{
	elm_object_part_text_set(m_popup, "title,text", title.c_str());
	elm_object_text_set(m_popup, content.c_str());
}

Evas_Object *Popup::addButton(const std::string &part)
{
	Evas_Object *button = elm_button_add(m_popup);
	elm_object_style_set(button, "bottom");
	elm_object_part_content_set(m_popup, part.c_str(), button);
	evas_object_show(button);

	return button;
}

}
}
