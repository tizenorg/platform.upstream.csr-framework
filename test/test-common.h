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
 * @file        test-common.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Common utilities for test
 */
#pragma once

#include <sstream>
#include <iostream>
#include <ios>
#include <functional>
#include <typeinfo>
#include <string>
#include <cstring>
#include <cerrno>
#include <system_error>
#include <unistd.h>

#include <boost/test/unit_test.hpp>

#include <csr-content-screening.h>
#include <csr-web-protection.h>

#include <csre-content-screening.h>
#include <csre-web-protection.h>

#ifndef __FILENAME__
#define __FILENAME__ (::strrchr(__FILE__, '/') ? ::strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define TOSTRING(ITEMS)                                                         \
	(dynamic_cast<std::ostringstream &>(std::ostringstream().seekp(             \
										0, std::ios_base::cur) << ITEMS)).str()

#define ASSERT_IF_MSG(value, expected, msg) \
	Test::_assert(value, expected, __FILENAME__, __func__, __LINE__, true, TOSTRING(msg))

#define WARN_IF_MSG(value, expected, msg) \
	Test::_assert(value, expected, __FILENAME__, __func__, __LINE__, false, TOSTRING(msg))

#define ASSERT_IF(value, expected) \
	Test::_assert(value, expected, __FILENAME__, __func__, __LINE__, true, "")

#define WARN_IF(value, expected) \
	Test::_assert(value, expected, __FILENAME__, __func__, __LINE__, false, "")

#define ASSERT_SUCCESS(value) \
	Test::_assert(value, CSR_ERROR_NONE, __FILENAME__, __func__, __LINE__, true, "")

#define WARN_SUCCESS(value) \
	Test::_assert(value, CSR_ERROR_NONE, __FILENAME__, __func__, __LINE__, false, "")

#define ASSERT_INSTALL_APP(path, type)                       \
	BOOST_REQUIRE_MESSAGE(Test::install_app(path, type),     \
						  "Failed to install app: " << path)

#define ASSERT_UNINSTALL_APP(path)                             \
	BOOST_REQUIRE_MESSAGE(Test::uninstall_app(path),           \
						  "Failed to uninstall app: " << path)

#define EXCEPTION_GUARD_START Test::exceptionGuard([&] {
#define EXCEPTION_GUARD_END   });

#define CHECK_IS_NULL(ptr)     BOOST_REQUIRE(ptr == nullptr)
#define CHECK_IS_NOT_NULL(ptr) BOOST_REQUIRE(ptr != nullptr)

namespace Test {

template <typename T, typename U>
void _assert(const T &value, const U &expected, const std::string &filename,
			 const std::string &funcname, unsigned int line, bool isAssert,
			 const std::string &msg)
{
	if (isAssert)
		BOOST_REQUIRE_MESSAGE(value == expected,
							  "[" << filename << " > " << funcname << " : " << line <<
							  "]" << " returned[" << value << "] expected[" << expected <<
							  "] " << msg);
	else
		BOOST_WARN_MESSAGE(value == expected,
							  "[" << filename << " > " << funcname << " : " << line <<
							  "]" << " returned[" << value << "] expected[" << expected <<
							  "] " << msg);
}

template <>
void _assert<csr_error_e, csr_error_e>(const csr_error_e &value,
									   const csr_error_e &expected,
									   const std::string &filename,
									   const std::string &funcname,
									   unsigned int line,
									   bool isAssert,
									   const std::string &msg);

template <>
void _assert<csr_error_e, int>(const csr_error_e &value,
							   const int &expected,
							   const std::string &filename,
							   const std::string &funcname,
							   unsigned int line,
							   bool isAssert,
							   const std::string &msg);

template <>
void _assert<int, csr_error_e>(const int &value,
							   const csr_error_e &expected,
							   const std::string &filename,
							   const std::string &funcname,
							   unsigned int line,
							   bool isAssert,
							   const std::string &msg);

template <>
void _assert<const char *, const char *>(const char * const &value,
										 const char * const &expected,
										 const std::string &filename,
										 const std::string &funcname,
										 unsigned int line,
										 bool isAssert,
										 const std::string &msg);

template <>
void _assert<char *, const char *>(char * const &value,
								   const char * const &expected,
								   const std::string &filename,
								   const std::string &funcname,
								   unsigned int line,
								   bool isAssert,
								   const std::string &msg);

template <>
void _assert<const char *, char *>(const char * const &value,
								   char * const &expected,
								   const std::string &filename,
								   const std::string &funcname,
								   unsigned int line,
								   bool isAssert,
								   const std::string &msg);

template <>
void _assert<char *, char *>(char * const &value,
							 char * const &expected,
							 const std::string &filename,
							 const std::string &funcname,
							 unsigned int line,
							 bool isAssert,
							 const std::string &msg);

template <>
void _assert<const char *, std::string>(const char * const &value,
										const std::string &expected,
										const std::string &filename,
										const std::string &funcname,
										unsigned int line,
										bool isAssert,
										const std::string &msg);

template <>
void _assert<char *, std::string>(char * const &value,
								  const std::string &expected,
								  const std::string &filename,
								  const std::string &funcname,
								  unsigned int line,
								  bool isAssert,
								  const std::string &msg);

void exceptionGuard(const std::function<void()> &);

std::string capi_ec_to_string(csr_error_e ec);
std::string capi_ec_to_string(int ec);

void make_dir(const char *dir);
void make_dir_assert(const char *dir);
void copy_file(const char *src_file, const char *dest_file);
void copy_file_assert(const char *src_file, const char *dest_file);
void touch_file(const char *file);
void touch_file_assert(const char *file);
void remove_file(const char *file);
void remove_file_assert(const char *file);
bool is_file_exist(const char *file);
bool install_app(const char *app_path, const char *app_type);
bool uninstall_app(const char *pkg_id);
void initialize_db();

struct ScopedCstr {
	char *ptr;

	ScopedCstr() : ptr(nullptr) {}
	~ScopedCstr()
	{
		if (ptr)
			free(ptr);
	}
};

class ScopedChDir {
public:
	ScopedChDir(const std::string &dirpath)
	{
		if (::getcwd(cdbuf, PATH_MAX + 1) == nullptr)
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

template <typename T>
class Context {
public:
	Context() : m_context(nullptr)
	{
		BOOST_REQUIRE_MESSAGE(0, "Type[" << typeid(T).name()
							  << "] isn't specialized for context template");
	}

	virtual ~Context()
	{
		BOOST_REQUIRE_MESSAGE(0, "Type[" << typeid(T).name()
							  << "] isn't specialized for context template");
	}

	T get(void) const
	{
		BOOST_REQUIRE_MESSAGE(0, "Type[" << typeid(T).name()
							  << "] isn't specialized for context template");

		return nullptr;
	}

private:
	T m_context;
};

template <>
class Context<csr_cs_context_h> {
public:
	Context() : m_context(nullptr)
	{
		ASSERT_SUCCESS(csr_cs_context_create(&m_context));
		BOOST_REQUIRE(m_context != nullptr);
	}

	virtual ~Context()
	{
		ASSERT_SUCCESS(csr_cs_context_destroy(m_context));
	}

	csr_cs_context_h get(void) const
	{
		return m_context;
	}

private:
	csr_cs_context_h m_context;
};

template <>
class Context<csr_wp_context_h> {
public:
	Context() : m_context(nullptr)
	{
		ASSERT_SUCCESS(csr_wp_context_create(&m_context));
		BOOST_REQUIRE(m_context != nullptr);
	}

	virtual ~Context()
	{
		ASSERT_SUCCESS(csr_wp_context_destroy(m_context));
	}

	csr_wp_context_h get(void) const
	{
		return m_context;
	}

private:
	csr_wp_context_h m_context;
};

template <>
class Context<csre_cs_context_h> {
public:
	Context() : m_context(nullptr)
	{
		ASSERT_SUCCESS(csre_cs_context_create(&m_context));
		BOOST_REQUIRE(m_context != nullptr);
	}

	virtual ~Context()
	{
		ASSERT_SUCCESS(csre_cs_context_destroy(m_context));
	}

	csre_cs_context_h get(void) const
	{
		return m_context;
	}

private:
	csre_cs_context_h m_context;
};

template <>
class Context<csre_wp_context_h> {
public:
	Context() : m_context(nullptr)
	{
		ASSERT_SUCCESS(csre_wp_context_create(&m_context));
		BOOST_REQUIRE(m_context != nullptr);
	}

	virtual ~Context()
	{
		ASSERT_SUCCESS(csre_wp_context_destroy(m_context));
	}

	csre_wp_context_h get(void) const
	{
		return m_context;
	}

private:
	csre_wp_context_h m_context;
};

} // namespace Test
