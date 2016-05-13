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
 * @file        test-canonicalize.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       file / directory set canonicalize unit test
 */
#include <client/canonicalize.h>

#include <system_error>
#include <cerrno>
#include <unistd.h>

#include <boost/test/unit_test.hpp>

#include "test-common.h"

using namespace Csr;

namespace {

class ScopedChDir {
public:
	ScopedChDir(const std::string &dirpath)
	{
		if (getcwd(cdbuf, PATH_MAX + 1) == nullptr)
			throw std::system_error(errno, std::system_category(), "getcwd failed");

		if (::chdir(dirpath.c_str()) == -1)
			throw std::system_error(errno, std::system_category(),
									dirpath + " chdir failed");
	}

	~ScopedChDir()
	{
		if (::chdir(cdbuf) == -1)
			throw std::system_error(errno, std::system_category(),
									std::string(cdbuf) + " chdir failed");
	}

private:
	char cdbuf[PATH_MAX + 1];
};

} // namespace anonymous

BOOST_AUTO_TEST_SUITE(CANONICALIZE)

BOOST_AUTO_TEST_CASE(get_absolute_path_file)
{
	EXCEPTION_GUARD_START

	ScopedChDir chdir("/usr/bin");

	std::string absolutePath = "/usr/bin/csr-server";

	ASSERT_IF(Client::getAbsolutePath("/usr/bin/csr-server"), absolutePath);
	ASSERT_IF(Client::getAbsolutePath("/usr/bin/../bin/csr-server"), absolutePath);
	ASSERT_IF(Client::getAbsolutePath("/usr/bin/./csr-server"), absolutePath);
	ASSERT_IF(Client::getAbsolutePath("csr-server"), absolutePath);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(get_absolute_path_dir)
{
	EXCEPTION_GUARD_START

	ScopedChDir chdir("/usr/bin");

	std::string absolutePath = "/usr/bin";

	ASSERT_IF(Client::getAbsolutePath("/usr/bin/"), absolutePath);
	ASSERT_IF(Client::getAbsolutePath("."), absolutePath);
	ASSERT_IF(Client::getAbsolutePath("/usr/bin/../bin"), absolutePath);
	ASSERT_IF(Client::getAbsolutePath("/usr/bin/./"), absolutePath);
	ASSERT_IF(Client::getAbsolutePath("../bin/"), absolutePath);

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(dir_set_1)
{
	EXCEPTION_GUARD_START

	ScopedChDir chdir("/usr/bin");

	StrSet dirset;

	dirset.insert(Client::getAbsolutePath("/var/lib"));
	dirset.insert(Client::getAbsolutePath("/bin"));
	dirset.insert(Client::getAbsolutePath("/"));
	dirset.insert(Client::getAbsolutePath("/opt"));
	dirset.insert(Client::getAbsolutePath("/usr/bin"));

	Client::canonicalizeDirSet(dirset);

	ASSERT_IF(dirset.size(), std::size_t(1));
	ASSERT_IF(dirset.count("/"), std::size_t(1));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_CASE(dir_set_2)
{
	EXCEPTION_GUARD_START

	ScopedChDir chdir("/usr/bin");

	StrSet dirset;

	dirset.insert(Client::getAbsolutePath("/usr/share"));
	dirset.insert(Client::getAbsolutePath("/usr/lib"));
	dirset.insert(Client::getAbsolutePath("/usr"));
	dirset.insert(Client::getAbsolutePath("/usr/etc"));
	dirset.insert(Client::getAbsolutePath("/opt/share"));
	dirset.insert(Client::getAbsolutePath("/opt/dbspace"));

	Client::canonicalizeDirSet(dirset);

	ASSERT_IF(dirset.size(), std::size_t(3));
	ASSERT_IF(dirset.count("/usr"), std::size_t(1));
	ASSERT_IF(dirset.count("/opt/share"), std::size_t(1));
	ASSERT_IF(dirset.count("/opt/dbspace"), std::size_t(1));

	EXCEPTION_GUARD_END
}

BOOST_AUTO_TEST_SUITE_END()
