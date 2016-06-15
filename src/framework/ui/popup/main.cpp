/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * @file        main.cpp
 * @author      Janusz Kozerski (j.kozerski@samsung.com)
 *              Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Popup service main
 */
#include <stdexcept>

#include <vconf.h>
#include <Elementary.h>
#include <Ecore.h>
#include <memory>

#include "common/audit/logger.h"
#include "popup-service.h"

namespace {

void updateLanguage(void)
{
	std::unique_ptr<char> lang(new char);
	std::unique_ptr<char> ret(new char);

	lang.reset(vconf_get_str(VCONFKEY_LANGSET));
	if (lang) {
		setenv("LANG", lang.get(), 1);
		setenv("LC_MESSAGES", lang.get(), 1);
		ret.reset(setlocale(LC_ALL, ""));
		INFO("Set language environment : " << lang.get());

		if (!ret)
			ret.reset(setlocale(LC_ALL, vconf_get_str(VCONFKEY_LANGSET)));
		DEBUG("setlocale() ret : " << ret.get());
	}
}

struct ElmRaii {
	ElmRaii(int argc, char **argv)
	{
		DEBUG("elm_init()");
		elm_init(argc, argv);
	}

	virtual ~ElmRaii()
	{
		DEBUG("elm_shutdown()");
		/* TODO: do shutdown.
		 *   shutdown generates segmentation fault by unknown reason.
		 */
		//      elm_shutdown();
	}
};

} // namespace anonymous

int main(int argc, char **argv)
{
	try {
		Csr::Audit::Logger::setTag("CSR_POPUP");

		INFO("CSR popup service start!");

		/* init/shutdown elm automatically */
		ElmRaii elmRaii(argc, argv);

		updateLanguage();

		Csr::Ui::PopupService service;

		// If timeout time is too small and popup service wakes up repeatedly too quickly,
		// it can be kept out to start by systemd.
		service.start(POPUP_SERVICE_IDLE_TIMEOUT_TIME);

		return 0;
	} catch (const std::exception &e) {
		ERROR("std exception: " << e.what());
		return -1;
	} catch (...) {
		ERROR("Unhandled exception occured!");
		return -1;
	}
}
