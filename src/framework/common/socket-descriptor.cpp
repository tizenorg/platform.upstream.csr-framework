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
 * @file        socket-descriptor.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Socket path and privileges
 */
#include "common/socket-descriptor.h"

#include <map>

#include "common/exception.h"

namespace Csr {

namespace {

const std::map<SockId, SocketDescriptor> g_sockDescMap {
	{
		SockId::CS,
		SocketDescriptor("/tmp/." SERVICE_NAME "-cs.socket",
						 "http://tizen.org/privilege/antivirus.scan",
						 "csr::csapi")
	},
	{
		SockId::WP,
		SocketDescriptor("/tmp/." SERVICE_NAME "-wp.socket",
						 "http://tizen.org/privilege/antivirus.webprotect",
						 "csr::wpapi")
	},
	{
		SockId::ADMIN,
		SocketDescriptor("/tmp/." SERVICE_NAME "-admin.socket",
						 "http://tizen.org/privilege/antivirus.admin",
						 "csr::adminapi")
	},
	{
		SockId::POPUP,
		SocketDescriptor("/tmp/." SERVICE_NAME "-popup.socket",
						 std::string(),
						 std::string())
	}
};

} // namespace anonymous

SocketDescriptor::SocketDescriptor(const std::string &_path, const std::string &_priv,
								   const std::string &_label) :
	path(_path), privilege(_priv), label(_label) {}

const SocketDescriptor &getSockDesc(const SockId &id)
{
	switch (id) {
	case SockId::CS:
	case SockId::WP:
	case SockId::ADMIN:
	case SockId::POPUP:
		return g_sockDescMap.at(id);

	default:
		ThrowExc(InternalError, "Invalid sock id to descriptor map");
	}
}

}
