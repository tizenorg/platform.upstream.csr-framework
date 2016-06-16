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

#include <package-info.h>
#include <app_control.h>

namespace Csr {
namespace Ui {

namespace {
	struct AppControl {
		AppControl() { app_control_create(&handle); }
		~AppControl() { app_control_destroy(handle); }

		app_control_h handle;
	};

	const std::string DEFAULT_URL("https://developer.tizen.org/");
}

Popup::Popup(int buttonN)
{
	// Set win properties.
	m_win = elm_win_add(nullptr, "CSR popup", ELM_WIN_NOTIFICATION);
	elm_win_indicator_opacity_set(m_win, ELM_WIN_INDICATOR_TRANSLUCENT);
	elm_win_borderless_set(m_win, EINA_TRUE);
	elm_win_alpha_set(m_win, EINA_TRUE);
	elm_win_screen_size_get(m_win, NULL, NULL, &m_winW, &m_winH);
	evas_object_size_hint_max_set(m_win, m_winW, m_winH);
	evas_object_size_hint_min_set(m_win, m_winW, m_winH / 4);

	// Set popup properties.
	m_popup = elm_popup_add(m_win);
	setDefaultProperties(m_popup);

	// Wrap objects with box.
	m_box = elm_box_add(m_popup);
	setDefaultProperties(m_box);
	elm_box_padding_set(m_box, 0, 20);
	evas_object_show(m_box);

	m_header = elm_label_add(m_box);
	setDefaultProperties(m_header);
	elm_box_pack_end(m_box, m_header);
	evas_object_show(m_header);

	// Subbox is for icon.
	m_subBox = elm_box_add(m_box);
	setDefaultProperties(m_subBox);
	elm_box_horizontal_set(m_subBox, EINA_TRUE);
	elm_box_padding_set(m_subBox, 20, 0);
	// If icon is not set, it doesn't appear.
	m_icon = elm_icon_add(m_subBox);
	elm_image_resizable_set(m_icon, EINA_FALSE, EINA_FALSE);
	elm_box_pack_end(m_subBox, m_icon);
	evas_object_show(m_icon);

	m_body = elm_label_add(m_subBox);
	setDefaultProperties(m_body);
	elm_box_pack_end(m_subBox, m_body);
	evas_object_show(m_body);

	elm_box_pack_end(m_box, m_subBox);
	evas_object_show(m_subBox);

	// This label is for linking to webview.
	m_hypertext = elm_label_add(m_box);
	setDefaultProperties(m_hypertext);
	setText(m_hypertext, "<a href=><color=#0000FFFF>"
		"More information</color></a>");
	elm_box_pack_end(m_box, m_hypertext);
	evas_object_show(m_hypertext);

	m_footer = elm_label_add(m_box);
	setDefaultProperties(m_footer);
	elm_box_pack_end(m_box, m_footer);
	evas_object_show(m_footer);

	elm_object_content_set(m_popup, m_box);

	// Add buttons dynamically.
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
	evas_object_del(m_win);
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
	setText(m_footer, footer);
}

void Popup::setMessage(const std::string &msg) noexcept
{
	m_message = msg;
}

void Popup::setIcon(const std::string &path) noexcept
{
	if (path.compare(PackageInfo::UNKNOWN) == 0)
		elm_image_file_set(m_icon, DEFAULT_ICON_PATH, NULL);
	else
		elm_image_file_set(m_icon, path.c_str(), NULL);
}

void Popup::run(void)
{
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

void Popup::setDefaultProperties(Evas_Object *obj) noexcept
{
	// Set width as maximum, height as minimum.
	evas_object_size_hint_weight_set(obj, EVAS_HINT_EXPAND, 0);
	// Set width as fill parent, height as center.
	evas_object_size_hint_align_set(obj, EVAS_HINT_FILL, 0.5);
}

void Popup::setText(Evas_Object *obj, const std::string &text) noexcept
{
	// Eable text line-break automatically.
	elm_label_line_wrap_set(obj, ELM_WRAP_WORD);
	elm_object_text_set(obj, text.c_str());
}

void Popup::callbackRegister(Evas_Object *obj, int *type)
{
	evas_object_smart_callback_add(obj, "clicked", btnClickedCb, type);
}

void Popup::callbackRegister(Evas_Object *obj, const std::string &url)
{
	if (url.empty())
		evas_object_smart_callback_add(
			obj, "anchor,clicked", hypertextClickedCb, &DEFAULT_URL);
	else
		evas_object_smart_callback_add(
			obj, "anchor,clicked", hypertextClickedCb, &url);
}

void Popup::hypertextClickedCb(void *data, Evas_Object *, void *)
{
	DEBUG("Launch browser for detailed url.");

	std::string url = *(reinterpret_cast<std::string *>(data));
	std::unique_ptr<AppControl> ac(new AppControl);

	auto ret = app_control_set_operation(ac->handle, APP_CONTROL_OPERATION_VIEW);
	if (ret != APP_CONTROL_ERROR_NONE) {
		WARN("Cannot set app_control operation.");
		return;
	}

	ret = app_control_set_uri(ac->handle, url.c_str());
	if (ret != APP_CONTROL_ERROR_NONE) {
		WARN("Cannot set url to app_control handle.");
		return;
	}

	ret = app_control_send_launch_request(ac->handle, NULL, NULL);
	if (ret != APP_CONTROL_ERROR_NONE) {
		WARN("Cannot launch browser.");
		return;
	}
}

void Popup::btnClickedCb(void *data, Evas_Object *, void *)
{
	response = *(reinterpret_cast<int *>(data));
	elm_exit();
}
} // namespace Ui
} // namespace Csr
