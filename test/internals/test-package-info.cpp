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
 * @file        test-package-info.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Package info class test
 */
#include <package-info.h>

#include <boost/test/unit_test.hpp>

#include "test-common.h"
#include "test-resource.h"

using namespace Csr;

BOOST_AUTO_TEST_SUITE(PACKAGE_INFO)

BOOST_AUTO_TEST_CASE(get_icon_wgt)
{
	EXCEPTION_GUARD_START

	Test::uninstall_app(TEST_WGT_PKG_ID);
	ASSERT_INSTALL_APP(TEST_WGT_PATH, TEST_WGT_TYPE);

	Ui::PackageInfo info(TEST_WGT_PKG_ID);

	BOOST_MESSAGE(info.getIconPath());

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_icon_tpk)
{
	EXCEPTION_GUARD_START

	Test::uninstall_app(TEST_TPK_PKG_ID);
	ASSERT_INSTALL_APP(TEST_TPK_PATH, TEST_TPK_TYPE);

	Ui::PackageInfo info(TEST_TPK_PKG_ID);

	BOOST_MESSAGE(info.getIconPath());

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_version_wgt)
{
	EXCEPTION_GUARD_START

	Test::uninstall_app(TEST_WGT_PKG_ID);
	ASSERT_INSTALL_APP(TEST_WGT_PATH, TEST_WGT_TYPE);

	Ui::PackageInfo info(TEST_WGT_PKG_ID);

	BOOST_MESSAGE(info.getVersion());

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_version_tpk)
{
	EXCEPTION_GUARD_START

	Test::uninstall_app(TEST_TPK_PKG_ID);
	ASSERT_INSTALL_APP(TEST_TPK_PATH, TEST_TPK_TYPE);

	Ui::PackageInfo info(TEST_TPK_PKG_ID);

	BOOST_MESSAGE(info.getVersion());

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_label_wgt)
{
	EXCEPTION_GUARD_START

	Test::uninstall_app(TEST_WGT_PKG_ID);
	ASSERT_INSTALL_APP(TEST_WGT_PATH, TEST_WGT_TYPE);

	Ui::PackageInfo info(TEST_WGT_PKG_ID);

	BOOST_MESSAGE(info.getLabel());

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_label_tpk)
{
	EXCEPTION_GUARD_START

	Test::uninstall_app(TEST_TPK_PKG_ID);
	ASSERT_INSTALL_APP(TEST_TPK_PATH, TEST_TPK_TYPE);

	Ui::PackageInfo info(TEST_TPK_PKG_ID);

	BOOST_MESSAGE(info.getLabel());

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
