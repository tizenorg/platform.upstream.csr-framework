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
 * @file        askuser.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Ask user and get response to handle screening result
 */
#include "ui/askuser.h"

#include "common/raw-buffer.h"
#include "common/message-buffer.h"
#include "common/connection.h"
#include "common/socket.h"
#include "common/audit/logger.h"

namespace Csr {
namespace Ui {

namespace {

inline Csr::Connection makeConnection(const std::string &address)
{
	DEBUG("Try to make connection to csr-popup service");

	return Csr::Connection(Csr::Socket::connect(address));
}

inline Csr::Connection makeConnection(void)
{
	return makeConnection("/tmp/." SERVICE_NAME "-popup.socket");
}

inline Csr::RawBuffer dispatch(const Csr::Connection &connection, const Csr::RawBuffer &buf)
{
	DEBUG("Try to dispatch to csr-popup service");

	connection.send(buf);

	DEBUG("Wait for response from csr-popup service");

	return connection.receive();
}

Response ask(const RawBuffer &buf)
{
	auto conn = makeConnection();

	auto reply = dispatch(conn, buf);

	Csr::MessageBuffer msg;
	msg.push(reply);

	int int_response;
	msg.Deserialize(int_response);
	return static_cast<Response>(int_response);
}

ResponseMap askMultiple(const RawBuffer &buf)
{
	auto conn = makeConnection();

	auto reply = dispatch(conn, buf);

	Csr::MessageBuffer msg;
	msg.push(reply);

	ResponseMap responseMap;
	msg.Deserialize(responseMap);

	return responseMap;
}

} // namespace Csr::Ui::anonymous

AskUser::AskUser()
{
}

AskUser::~AskUser()
{
}


Response AskUser::fileSingle(const std::string &message, const FileItem &item) const
{
	auto inbuf = Csr::MessageBuffer::Serialize(
			static_cast<int>(CommandId::FILE_SINGLE),
			message,
			item).pop();

	return ask(inbuf);
}

ResponseMap AskUser::fileMultiple(const std::string &message, const FileItems &items) const
{
	auto inbuf = Csr::MessageBuffer::Serialize(
			static_cast<int>(CommandId::FILE_MULTIPLE),
			message,
			items).pop();

	return askMultiple(inbuf);
}

Response AskUser::urlSingle(const std::string &message, const UrlItem &item) const
{
	auto inbuf = Csr::MessageBuffer::Serialize(
			static_cast<int>(CommandId::URL_SINGLE),
			message,
			item).pop();

	return ask(inbuf);
}

ResponseMap AskUser::urlMultiple(const std::string &message, const UrlItems &items) const
{
	auto inbuf = Csr::MessageBuffer::Serialize(
			static_cast<int>(CommandId::URL_MULTIPLE),
			message,
			items).pop();

	return askMultiple(inbuf);
}

} // namespace Ui
} // namespace Csr
