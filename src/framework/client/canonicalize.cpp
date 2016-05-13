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
 * @file        canonicalize.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Canonicalize file and directory paths
 */
#include "client/canonicalize.h"

#include <utility>
#include <cstdlib>
#include <cerrno>

#include "common/exception.h"

namespace Csr {
namespace Client {

std::string getAbsolutePath(const std::string &path)
{
	auto resolved = realpath(path.c_str(), nullptr);
	if (resolved == nullptr) {
		int err = errno;
		if (err == ENOENT)
			ERROR("File not found: " << path);
		else if (err == EACCES)
			ERROR("Perm denied for a component of the path prefix: " << path);

		ThrowExc(FileSystemError, "Failed to get resolved path: " << path <<
				 " errno: " << err);
	}

	std::string resolvedStr(resolved);

	free(resolved);

	return resolvedStr;
}

void canonicalizeDirSet(StrSet &dirset)
{
	if (dirset.size() < 2)
		return;

	for (auto it = dirset.begin(); it != dirset.end(); ++it) {
		auto itsub = it;
		++itsub;
		while (true) {
			if (itsub == dirset.end())
				break;

			auto itl = it->length();
			auto itsubl = itsub->length();

			if (itl + 1 >= itsubl || // to short to be sub-directory
				itsub->compare(0, itl, *it) != 0 || // prefix isn't matched
				(*it != "/" && itsub->at(itl) != '/')) { // has '/' at the end of prefix
				++itsub;
				continue;
			}

			itsub = dirset.erase(itsub);
		}
	}
}

}
}
