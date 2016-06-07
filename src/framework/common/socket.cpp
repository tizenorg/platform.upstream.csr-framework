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
 * @file        socket.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief
 */
#include "common/socket.h"

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>

#include <systemd/sd-daemon.h>

#include "common/audit/logger.h"
#include "common/exception.h"

namespace Csr {

namespace {

int g_sd_listen_fds = -1;

int createSystemdSocket(const std::string &path)
{
	if (g_sd_listen_fds == -1)
		g_sd_listen_fds = ::sd_listen_fds(0);

	if (g_sd_listen_fds < 0)
		ThrowExc(CSR_ERROR_SOCKET, "failed to sd_listen_fds");

	for (int fd = SD_LISTEN_FDS_START; fd < SD_LISTEN_FDS_START + g_sd_listen_fds; ++fd) {
		if (::sd_is_socket_unix(fd, SOCK_STREAM, 1, path.c_str(), 0) > 0) {
			INFO("service's systemd socket found with fd: " << fd);
			return fd;
		}
	}

	ThrowExc(CSR_ERROR_SOCKET, "No useable socket were passed by systemd. path: " << path);
}

} // namespace anonymous

Socket::Socket(SockId sockId, int fd) : m_sockId(sockId), m_fd(fd)
{
	if (this->m_fd < 0)
		ThrowExc(CSR_ERROR_SOCKET, "Socket fd from constructor is invalid!!");
}

Socket::Socket(SockId sockId) : m_sockId(sockId)
{
	this->m_fd = createSystemdSocket(getSockDesc(this->m_sockId).path);
}

Socket::Socket(Socket &&other) noexcept : m_sockId(other.m_sockId), m_fd(other.m_fd)
{
	other.m_fd = 0;
}

Socket &Socket::operator=(Socket &&other) noexcept
{
	if (this == &other)
		return *this;

	this->m_sockId = other.m_sockId;
	this->m_fd = other.m_fd;
	other.m_fd = 0;

	return *this;
}

Socket::~Socket()
{
	if (this->m_fd == 0)
		return;

	INFO("Close socket of fd: " << this->m_fd);
	::close(m_fd);
}

Socket Socket::accept(void) const
{
	int fd = ::accept(this->m_fd, nullptr, nullptr);

	if (fd < 0)
		ThrowExc(CSR_ERROR_SOCKET, "socket on fd[" << this->m_fd << "] accept failed "
				 "with errno: " << errno);

	INFO("Accept client success with fd: " << fd);

	return Socket(this->m_sockId, fd);
}

Socket Socket::connect(SockId sockId)
{
	const auto &path = getSockDesc(sockId).path;

	if (path.size() >= sizeof(sockaddr_un::sun_path))
		ThrowExc(CSR_ERROR_SERVER, "socket path size too long!");

	int fd = ::socket(AF_UNIX, SOCK_STREAM, 0);

	if (fd < 0)
		ThrowExc(CSR_ERROR_SOCKET, "Socket create failed with errno: " << errno);

	sockaddr_un addr;
	addr.sun_family = AF_UNIX;

	strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path));

	if (::connect(fd, reinterpret_cast<sockaddr *>(&addr),
				  sizeof(sockaddr_un)) == -1)
		ThrowExc(CSR_ERROR_SOCKET, "Socket[" << path << "] connect failed "
				 "with errno: " << errno);

	INFO("Connect to CSR server success with fd:" << fd);

	return Socket(sockId, fd);
}

SockId Socket::getSockId(void) const noexcept
{
	return this->m_sockId;
}

int Socket::getFd(void) const noexcept
{
	return this->m_fd;
}

RawBuffer Socket::read(void) const
{
	size_t total = 0;
	size_t size = 0;

	DEBUG("Read data from stream on socket fd[" << this->m_fd << "]");

	auto bytes = ::read(this->m_fd, &size, sizeof(size));
	if (bytes < 0)
		ThrowExc(CSR_ERROR_SOCKET, "Socket data size read failed with errno: " << errno);

	RawBuffer data(size, 0);
	auto buf = reinterpret_cast<char *>(data.data());

	while (total < size) {
		bytes = ::read(this->m_fd, buf + total, size - total);

		if (bytes < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
				continue;
			else
				ThrowExc(CSR_ERROR_SOCKET, "Socket read failed with errno: " << errno);
		}

		total += bytes;
	}

	data.resize(total);

	DEBUG("Read data of size[" << total
		  << "] from stream on socket fd[" << this->m_fd << "] done.");

	return data;
}

void Socket::write(const RawBuffer &data) const
{
	size_t total = 0;

	auto buf = reinterpret_cast<const char *>(data.data());
	auto size = data.size();

	DEBUG("Write data to stream on socket fd[" << this->m_fd << "]");

	auto bytes = ::write(this->m_fd, &size, sizeof(size));
	if (bytes < 0)
		ThrowExc(CSR_ERROR_SOCKET, "Socket data size write failed with errno: " << errno);

	while (total < size) {
		bytes = ::write(this->m_fd, buf + total, size - total);

		if (bytes < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
				continue;
			else
				ThrowExc(CSR_ERROR_SOCKET, "Socket write failed with errno: " << errno);
		}

		total += bytes;
	}

	DEBUG("Write data of size[" << total <<
		  "] to stream on socket fd[" << this->m_fd << "] done.");
}

}
