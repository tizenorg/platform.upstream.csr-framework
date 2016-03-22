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

#include "common/audit/logger.h"
#include "popup-service.h"

namespace {

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
//		elm_shutdown();
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

		setlocale(LC_ALL, vconf_get_str(VCONFKEY_LANGSET));

		Csr::Ui::PopupService service("/tmp/." SERVICE_NAME "-popup.socket");

		service.start(0);

		return 0;
	} catch (const std::exception &e) {
		ERROR("std exception: " << e.what());
		return -1;
	} catch (...) {
		ERROR("Unhandled exception occured!");
		return -1;
	}
}
