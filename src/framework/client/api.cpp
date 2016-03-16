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
 * @file        api.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       csr client C APIs
 */
#include "csr/api.h"
#include "csr/error.h"

#include <functional>
#include <string>
#include <exception>

#include "raw-buffer.h"
#include "message-buffer.h"
#include "connection.h"
#include "socket.h"
#include "command-id.h"
#include "audit/logger.h"

#define API __attribute__((visibility("default")))

static void init_lib(void) __attribute__((constructor));
static void init_lib(void)
{
	// TODO: set log tag more gently? like singleton
	Csr::Audit::Logger::setTag("CSR_CLIENT");
}

namespace {

inline Csr::Connection makeConnection(const std::string &address)
{
	return Csr::Connection(Csr::Socket::connect(address));
}

inline Csr::Connection makeConnection(void)
{
	return makeConnection("/tmp/." SERVICE_NAME ".socket");
}

inline Csr::RawBuffer dispatch(const Csr::Connection &connection, const Csr::RawBuffer &buf)
{
	DEBUG("send rawbuf to server. fd: " << connection.getFd());
	connection.send(buf);

	DEBUG("wait for receive reply from server...");
	return connection.receive();
}

using Encoder = std::function<Csr::MessageBuffer(void)>;
using Decoder = std::function<csr_error_e(Csr::MessageBuffer &&)>;

auto defaultDecoder = [](Csr::MessageBuffer &&msg) {
	csr_error_e retcode = CSR_ERROR_NONE;
	msg.Deserialize(retcode);
	return retcode;
};

auto post = [](Encoder &&encoder, Decoder &&decoder = defaultDecoder) {
	try {
		auto conn = makeConnection();

		INFO("Connection made! Let's dispatch request!");

		auto response = dispatch(conn, encoder().pop());

		DEBUG("Get response!");

		Csr::MessageBuffer msg;
		msg.push(response);

		return decoder(std::move(msg));
	} catch (const std::exception &e) {
		ERROR("std exception occured: " << e.what());
		/* TODO: divide error codes per exception respectively */
		return CSR_ERROR_SERVER;
	} catch (...) {
		ERROR("Unhandled excpeion occured in post!");
		return CSR_ERROR_UNKNOWN;
	}
};

inline std::string toStlString(const char *cstr)
{
	return (cstr == nullptr) ? std::string() : std::string(cstr);
}

} // namespace anonymous

API
int csr_file_scan(const char *filepath)
{
	DEBUG("csr_file_scan API start!");
	return post([&]() {
			return Csr::MessageBuffer::Serialize(Csr::CommandId::FILE_SCAN, toStlString(filepath));
		});
}

API
int csr_file_judge(const char *filepath, int judge)
{
	DEBUG("csr_file_judge API start!");
	return post([&]() {
			return Csr::MessageBuffer::Serialize(Csr::CommandId::FILE_JUDGE, toStlString(filepath), judge);
		});
}
