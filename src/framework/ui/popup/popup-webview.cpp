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
 * @file        popup-webview.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */
#include "popup-webview.h"

namespace Csr {
namespace Ui {

Webview::Webview(const std::string &url) : m_url(url)
{
}

void Webview::winCloseCb(void *, Evas_Object *obj, void *)
{
	evas_object_del(obj);
}

void Webview::closeCb(void *data, Evas_Object *, void *)
{
	//TODO(sangwan.kwon) Check memory leak.
	Evas_Object *win = reinterpret_cast<Evas_Object *>(data);
	evas_object_del(win);
}

void Webview::run(void)
{
	this->m_win = elm_win_add(NULL, "CSR detailed page", ELM_WIN_DIALOG_BASIC);
	eext_object_event_callback_add(
		m_win, EEXT_CALLBACK_BACK, winCloseCb, NULL);

	int width, height;
	elm_win_screen_size_get(this->m_win, NULL, NULL, &width, &height);
	evas_object_move(this->m_win, 0, 0);

	this->m_winW = width * 0.9;
	this->m_winH = height * 0.9;
	evas_object_resize(this->m_win, this->m_winW, this->m_winH);
	this->m_bg = elm_bg_add(this->m_win);

	elm_bg_color_set(this->m_bg, 255, 255, 255);
	evas_object_size_hint_align_set(this->m_bg, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_win_resize_object_add(this->m_win, this->m_bg);
	evas_object_show(this->m_bg);

	this->m_box = elm_box_add(this->m_win);
	evas_object_size_hint_align_set(this->m_box, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_min_set(this->m_box, this->m_winW, this->m_winH);
	elm_win_resize_object_add(this->m_win, this->m_box);
	evas_object_show(this->m_box);

	this->m_webview = ewk_view_add(this->m_win);
	evas_object_size_hint_align_set(m_win, EVAS_HINT_FILL, EVAS_HINT_FILL);
	ewk_view_url_set(this->m_win, this->m_url.c_str());
	elm_box_pack_end(this->m_box, this->m_webview);
	evas_object_show(this->m_webview);

	this->m_subBox = elm_box_add(this->m_box);
	elm_box_horizontal_set(this->m_subBox, EINA_TRUE);
	evas_object_size_hint_align_set(this->m_webview, EVAS_HINT_FILL, 1.0);
	elm_box_pack_end(this->m_box, this->m_subBox);
	evas_object_show(this->m_subBox);

	this->m_label = elm_label_add(this->m_box);
	elm_object_text_set(this->m_label, "Close");
	evas_object_size_hint_align_set(this->m_label, 1.0, 0.5);
	elm_box_pack_end(this->m_subBox, this->m_label);
	evas_object_show(this->m_label);

	this->m_check = elm_check_add(this->m_subBox);
	evas_object_size_hint_align_set(this->m_check, 1.0, 0.5);
	elm_box_pack_end(this->m_subBox, this->m_check);
	evas_object_smart_callback_add(
		this->m_check, "changed", closeCb, this->m_win);
	evas_object_show(this->m_check);

	int bottomH = 0, labelH, checkH;
	evas_object_geometry_get(this->m_label, NULL, NULL, NULL, &labelH);
	evas_object_geometry_get(this->m_check, NULL, NULL, NULL, &checkH);
	bottomH = labelH > checkH ? labelH : checkH;
	evas_object_size_hint_min_set(
		this->m_webview, this->m_winW, this->m_winH - bottomH);
	evas_object_show(this->m_win);
}

} // namespace Ui
} // namespace Csr
