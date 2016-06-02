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
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */
#include "popup.h"

namespace Csr {
namespace Ui {

Popup::Popup(int buttonN)
{
	m_win = elm_win_add(nullptr, "CSR popup", ELM_WIN_NOTIFICATION);
	elm_win_indicator_opacity_set(m_win, ELM_WIN_INDICATOR_TRANSLUCENT);
	elm_win_borderless_set(m_win, EINA_TRUE);
	elm_win_alpha_set(m_win, EINA_TRUE);

	m_popup = elm_popup_add(m_win);
	evas_object_size_hint_weight_set(m_popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	m_box = elm_box_add(m_popup);

	m_header = elm_label_add(m_box);
	evas_object_size_hint_align_set(m_header, EVAS_HINT_FILL, 0);
	elm_box_pack_end(m_box, m_header);
	evas_object_show(m_header);

	m_body = elm_label_add(m_box);
	evas_object_size_hint_align_set(m_body, EVAS_HINT_FILL, 0);
	elm_box_pack_end(m_box, m_body);
	evas_object_show(m_body);

	m_hypertext = elm_label_add(m_box);
	elm_object_text_set(m_hypertext,"<color=#0000FFFF>"
		"    More information</color>");
	evas_object_size_hint_align_set(m_hypertext, EVAS_HINT_FILL, 0);
	elm_box_pack_end(m_box, m_hypertext);
	evas_object_show(m_hypertext);

	m_footer = elm_label_add(m_box);
	evas_object_size_hint_align_set(m_footer, EVAS_HINT_FILL, 0);
	elm_box_pack_end(m_box, m_footer);
	evas_object_show(m_footer);

	elm_object_content_set(m_popup, m_box);

	for(int i=1 ; i <= buttonN; i++) {
		std::string id("button" + std::to_string(i));
		Evas_Object *button = elm_button_add(m_popup);
		elm_object_style_set(button, "bottom");
		elm_object_part_content_set(m_popup, id.c_str(), button);
		evas_object_show(button);

		m_buttons.emplace_back(button);
	}

	evas_object_show(m_popup);
	evas_object_show(m_win);
}

Popup::~Popup()
{
	for (auto &obj : m_objects)
		evas_object_del(obj);
}

void Popup::setHeader(const std::string &header) noexcept
{
	setText(m_header, header);
}

void Popup::setBody(const std::string &body) noexcept
{
	setText(m_body, body);
}

void Popup::setFooter(const std::string &footer) noexcept
{
	setText(m_footer, "<br>" + footer);
}

void Popup::setMessage(const std::string &msg) noexcept
{
	m_message = msg;
}

void Popup::run(void)
{
	m_objects.emplace_back(m_header);
	m_objects.emplace_back(m_body);
	m_objects.emplace_back(m_hypertext);
	m_objects.emplace_back(m_footer);
	m_objects.emplace_back(m_box);

	for (auto &btn : m_buttons)
		m_objects.emplace_back(btn);

	m_objects.emplace_back(m_popup);
	m_objects.emplace_back(m_win);

	elm_run();
}

int Popup::response = -1;

RawBuffer Popup::getResult(void)
{
	return BinaryQueue::Serialize(response).pop();
}

void Popup::setTitle(const std::string &title) noexcept
{
	elm_object_part_text_set(m_popup, "title,text", title.c_str());
}

void Popup::setText(Evas_Object *obj, const std::string &text) noexcept
{
	elm_object_text_set(obj, text.c_str());
}

void Popup::callbackRegister(Evas_Object *obj, const char *event, int *type)
{
	evas_object_smart_callback_add(obj, event, btnClickedCb, type);
}

void Popup::btnClickedCb(void *data, Evas_Object *, void *)
{
	response = *(reinterpret_cast<int *>(data));
	elm_exit();
}
} // namespace Ui
} // namespace Csr
