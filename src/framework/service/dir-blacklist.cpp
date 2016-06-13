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
 * @file        dir-blacklist.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       directory blacklist to keep out of scanning
 */
#include "service/dir-blacklist.h"

#include <cstring>

namespace {

const size_t DIR_BLACK_LIST_SIZE = 3;
const char *DIR_BLACK_LIST[DIR_BLACK_LIST_SIZE] = {
	"/proc",
	"/sys",
	"/usr"
};

} // namespace anonymous

namespace Csr {

bool isInBlackList(const std::string &path)
{
	for (size_t i = 0; i < DIR_BLACK_LIST_SIZE; ++i) {
		const auto &black = DIR_BLACK_LIST[i];

		if (path == black)
			return true;

		auto black_size = strlen(black);
		auto path_size = path.size();

		// path is in black dir
		if (path_size > black_size && path[black_size] == '/' &&
			path.compare(0, black_size, black) == 0)
			return true;
	}

	return false;
}

}
