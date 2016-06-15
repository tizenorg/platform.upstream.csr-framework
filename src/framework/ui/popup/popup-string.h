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
 * @file        popup-string.h
 * @author      Sangwan Kwon (sangwan.kwon@samsung.comm)
 * @version     1.0
 * @brief       string set for supporting multilingual
 */
#pragma once

namespace Csr {
namespace Ui {

#define __(str) dgettext(SERVICE_NAME, str)

#define WP_TITLE __("IDS_ST_HEADER_BLOCK_MALICIOUS_WEBPAGE")
#define WP_NOTIFY_HEADER __("IDS_ST_POP_THE_FOLLOWING_WEBPAGE_MAY_HARM_YOUR_PHONE_C")
#define WP_NOTIFY_FOOTER __("IDS_ST_POP_TO_PROTECT_YOUR_PHONE_THIS_WEBPAGE_CANT_BE_VIEWED")
#define WP_PROMPT_HEADER __("IDS_ST_POP_THE_FOLLOWING_WEBPAGE_MAY_HARM_YOUR_PHONE_C")
#define WP_PROMPT_FOOTER __("IDS_ST_POP_TAP_VIEW_TO_GO_TO_THE_WEBPAGE_ANYWAY")

#define CS_TITLE __("IDS_ST_HEADER_MALWARE_DETECTED")
#define CS_NOTIFY_APP_HEADER \
	__("IDS_ST_POP_THE_FOLLOWING_ITEM_MAY_HARM_YOUR_PHONE_C")
#define CS_NOTIFY_APP_FOOTER \
	__("IDS_ST_POP_TAP_PS_TO_UNINSTALL_THE_INFECTED_APP_AND_PROTECT_YOUR_PHONE")
#define CS_PROMPT_APP_HEADER \
	__("IDS_ST_POP_THE_FOLLOWING_ITEM_MAY_HARM_YOUR_PHONE_C")
#define CS_PROMPT_APP_FOOTER \
	__("IDS_ST_POP_TAP_OPEN_TO_OPEN_THE_ITEM_THIS_TIME_ONLY_TAP_CANCEL_TO_CLOSE_THIS_POP_UP_AND_NOT_OPEN_THE_ITEM")

#define CS_NOTIFY_FILE_HEADER \
	__("IDS_ST_POP_THE_FOLLOWING_ITEM_MAY_HARM_YOUR_PHONE_C")
#define CS_NOTIFY_FILE_FOOTER \
	__("IDS_ST_POP_TAP_DELETE_TO_DELETE_THE_INFECTED_FILE_AND_PROTECT_YOUR_PHONE")
#define CS_PROMPT_FILE_HEADER \
	__("IDS_ST_POP_THE_FOLLOWING_ITEM_MAY_HARM_YOUR_PHONE_C")
#define CS_PROMPT_FILE_FOOTER \
	__("IDS_ST_POP_TAP_OPEN_TO_OPEN_THE_ITEM_THIS_TIME_ONLY_TAP_CANCEL_TO_CLOSE_THIS_POP_UP_AND_NOT_OPEN_THE_ITEM")

// TODO(UX team) Get did below
#define CS_NOTIFY_DATA_HEADER "Malware which may harm your phone is detected."
#define CS_NOTIFY_DATA_FOOTER "Processing is prohibited to protect your phone"
#define CS_PROMPT_DATA_HEADER "Malware which may harm your phone is detected."
#define CS_PROMPT_DATA_FOOTER \
	__("IDS_ST_POP_TAP_OPEN_TO_OPEN_THE_ITEM_THIS_TIME_ONLY_TAP_CANCEL_TO_CLOSE_THIS_POP_UP_AND_NOT_OPEN_THE_ITEM")

#define LABEL_APP_NAME           __("IDS_ST_POP_APP_NAME_C")
#define LABEL_FILE_NAME          __("IDS_ST_POP_VERSION_C")
#define LABEL_FILE_PATH          __("IDS_ST_POP_FILE_PATH_C_PS")
#define LABEL_VERSION            __("IDS_ST_POP_FILE_PATH_C_PS")
#define LABEL_URL                __("IDS_ST_POP_URL_C_PS")
#define LABEL_RISK               __("IDS_ST_POP_RISK_C")
#define LABEL_RISK_LEVEL_HIGH    __("IDS_ST_POP_HIGH_M_RISK_LEVEL")
#define LABEL_RISK_LEVEL_MEDIUM  __("IDS_ST_POP_MEDIUM_M_RISK_LEVEL")
#define LABEL_RISK_LEVEL_LOW     __("IDS_ST_POP_LOW_M_RISK_LEVEL")
#define LABEL_MORE_INFO          __("IDS_ST_BUTTON_MORE_INFO")

#define BTN_OPEN      __("IDS_ST_BUTTON_OPEN")
#define BTN_OK        __("IDS_ST_BUTTON_OK_ABB6")
#define BTN_CANCEL    __("IDS_ST_BUTTON_CANCEL")
#define BTN_UNINSTALL __("IDS_ST_BUTTON_UNINSTALL_ABB5")
#define BTN_DELETE    __("IDS_ST_BUTTON_DELETE_ABB3")
#define BTN_VIEW      __("IDS_ST_BUTTON_VIEW_ABB5")
#define BTN_BLOCK     __("IDS_ST_BUTTON_BLOCK_ABB4")
#define BTN_IGNORE    __("IDS_SA_BUTTON_IGNORE_ABB_CHN")

} // namespace Ui
} // namespace Csr
