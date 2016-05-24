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
 * @file
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief
 */
#include "csre/web-protection.h"
#include "csre/web-protection-engine-info.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <list>
#include <fstream>
#include <iostream>
#include <climits>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define API __attribute__((visibility("default")))

#define PRIVATE_DB_NAME   "csret_wp_risky_urls"
#define PRIVATE_LOGO_FILE "vendor_logo.bmp"

#define VENDOR_NAME       "TEST_VENDOR"
#define ENGINE_NAME       "TEST_LOCAL_TWP_ENGINE"
#define ENGINE_VERSION    "0.0.1"

using RawBuffer = std::vector<unsigned char>;

// TODO(k.tak): make all engine functions exception-safe

struct csret_wp_risky_url_s {
	std::string url;
	csre_wp_risk_level_e risk_level;
	std::string detailed_url;

	csret_wp_risky_url_s() : risk_level(CSRE_WP_RISK_UNVERIFIED) {}
};

struct csret_wp_context_s {
	std::list<csret_wp_risky_url_s> detected_list;
};

struct csret_wp_engine_s {
	std::string vendorName;
	std::string engineName;
	std::string apiVersion;
	std::string engineVersion;
	std::string dataVersion;
	RawBuffer logoImage;
	time_t latestUpdate;
};

enum csret_wp_internal_error_e {
	CSRET_WP_ERROR_NO_RISKY_URL_FILE     = -0x0101,
	CSRET_WP_ERROR_SIGNATURE_FILE_FORMAT = -0x0102,
	CSRET_WP_ERROR_FILE_IO               = -0x0103,
};

//==============================================================================
// static variables
//==============================================================================
static std::string g_resdir;
static std::string g_workingdir;
static std::list<csret_wp_risky_url_s> g_risky_urls;

//==============================================================================
// Utilities functions
//==============================================================================
std::string csret_wp_extract_value(const std::string &line,
								   const std::string &key)
{
	if (line.empty() || key.empty())
		return std::string();

	auto pos = line.find(key);

	if (pos == std::string::npos || pos != 0)
		return std::string();

	return line.substr(key.length());
}

int csret_wp_read_risky_urls(const std::string &path)
{
	// csret_wp_risky_urls file format
	// data_version=1.0.0 // it should be in the first line.
	// # this line is a comment.
	//
	// url=highrisky.test.com // this starts a description of a new risky url
	// risk_level=HIGH  // LOW/MEDIUM/HIGH
	// detailed_url=http://high.risky.com
	//
	// url=mediumrisky.test.com
	// risk_level=MEDIUM
	// detailed_url=http://medium.risky.com
	std::ifstream f(path.c_str());

	if (!f.is_open())
		return CSRET_WP_ERROR_NO_RISKY_URL_FILE;

	std::string line;
	csret_wp_risky_url_s node;

	while (std::getline(f, line)) {
		if (line.empty() || line[0] == '#')
			continue;

		auto value = csret_wp_extract_value(line, "url=");

		if (!value.empty()) {
			if (!node.url.empty()) {
				g_risky_urls.push_back(node);
				node = csret_wp_risky_url_s();
			}

			node.url = std::move(value);
			continue;
		}

		value = csret_wp_extract_value(line, "risk_level=");

		if (!value.empty()) {
			if (value.compare("LOW") == 0)
				node.risk_level = CSRE_WP_RISK_LOW;
			else if (value.compare("MEDIUM") == 0)
				node.risk_level = CSRE_WP_RISK_MEDIUM;
			else if (value.compare("HIGH") == 0)
				node.risk_level = CSRE_WP_RISK_HIGH;
			else
				node.risk_level = CSRE_WP_RISK_UNVERIFIED;

			continue;
		}

		value = csret_wp_extract_value(line, "detailed_url=");

		if (!value.empty())
			node.detailed_url = std::move(value);
	}

	if (!node.url.empty())
		g_risky_urls.push_back(node);

	return CSRE_ERROR_NONE;
}

int csret_wp_read_binary(const std::string &path, RawBuffer &buffer)
{
	std::ifstream f(path.c_str(), std::ios::binary);

	if (!f.is_open()) {
		buffer.clear();
		return CSRE_ERROR_NONE;
	}

	f.seekg(0, f.end);
	auto len = f.tellg();
	f.seekg(0, f.beg);

	buffer.resize(len, 0);
	f.read(reinterpret_cast<char *>(buffer.data()), buffer.size());

	if (!f) {
		buffer.clear();
		return CSRET_WP_ERROR_FILE_IO;
	}

	return CSRE_ERROR_NONE;
}

csret_wp_engine_s *csret_wp_init_engine()
{
	auto ptr = new csret_wp_engine_s;

	ptr->vendorName = VENDOR_NAME;
	ptr->engineName = ENGINE_NAME;
	ptr->apiVersion = CSRE_WP_API_VERSION;
	ptr->engineVersion = ENGINE_VERSION;
	ptr->dataVersion = ENGINE_VERSION;

	int ret = csret_wp_read_binary(g_resdir + "/" PRIVATE_LOGO_FILE,
								   ptr->logoImage);

	if (ret != CSRE_ERROR_NONE) {
		delete ptr;
		return nullptr;
	}

	struct stat attrib;

	stat(PRIVATE_DB_NAME, &attrib);

	ptr->latestUpdate = attrib.st_mtime;

	return ptr;
}


//==============================================================================
// Main function related
//==============================================================================
API
int csre_wp_global_initialize(const char *ro_res_dir,
							  const char *rw_working_dir)
{
	if (ro_res_dir == nullptr || rw_working_dir == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	g_resdir = ro_res_dir;
	g_workingdir = rw_working_dir;

	g_risky_urls.clear();

	int ret = csret_wp_read_risky_urls(g_workingdir + "/" PRIVATE_DB_NAME);

	if (ret != CSRE_ERROR_NONE && ret != CSRET_WP_ERROR_NO_RISKY_URL_FILE)
		return ret;


	return CSRE_ERROR_NONE;
}

API
int csre_wp_global_deinitialize()
{
	return CSRE_ERROR_NONE;
}

API
int csre_wp_context_create(csre_wp_context_h *phandle)
{
	if (phandle == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	if (g_risky_urls.empty())
		return CSRE_ERROR_INVALID_HANDLE; // not yet initialized

	auto context = new csret_wp_context_s;

	*phandle = reinterpret_cast<csre_wp_context_h>(context);

	return CSRE_ERROR_NONE;
}

API
int csre_wp_context_destroy(csre_wp_context_h handle)
{
	auto context = reinterpret_cast<csret_wp_context_s *>(handle);

	if (context == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	delete context;

	return CSRE_ERROR_NONE;
}

API
int csre_wp_check_url(csre_wp_context_h handle, const char *url,
					  csre_wp_check_result_h *presult)
{
	auto context = reinterpret_cast<csret_wp_context_s *>(handle);

	if (context == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (url == nullptr || presult == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	std::string in_url(url);

	csret_wp_risky_url_s detected;
	detected.url = in_url;

	// check url from static risky urls list
	for (auto &item : g_risky_urls) {
		if (in_url.find(item.url) == std::string::npos)
			continue;

		detected.risk_level = item.risk_level;
		detected.detailed_url = item.detailed_url;

		break;
	}

	context->detected_list.push_back(detected);
	*presult = reinterpret_cast<csre_wp_check_result_h>
			   (&context->detected_list.back());

	return CSRE_ERROR_NONE;
}


//==============================================================================
// Result related
//==============================================================================
API
int csre_wp_result_get_risk_level(csre_wp_check_result_h result,
								  csre_wp_risk_level_e *plevel)
{
	if (result == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (plevel == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	auto detected = reinterpret_cast<csret_wp_risky_url_s *>(result);

	*plevel = detected->risk_level;

	return CSRE_ERROR_NONE;
}

API
int csre_wp_result_get_detailed_url(csre_wp_check_result_h result,
									const char **detailed_url)
{
	if (result == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (detailed_url == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	auto detected = reinterpret_cast<csret_wp_risky_url_s *>(result);

	*detailed_url = detected->detailed_url.c_str();

	return CSRE_ERROR_NONE;
}

//==============================================================================
// Engine information related
//==============================================================================
API
int csre_wp_engine_get_info(csre_wp_engine_h *pengine)
{
	if (pengine == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	auto ptr = csret_wp_init_engine();
	*pengine = reinterpret_cast<csre_wp_engine_h>(ptr);

	return CSRE_ERROR_NONE;
}

API
int csre_wp_engine_destroy(csre_wp_engine_h engine)
{
	if (engine == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	delete reinterpret_cast<csret_wp_engine_s *>(engine);

	return CSRE_ERROR_NONE;
}

API
int csre_wp_engine_get_vendor(csre_wp_engine_h engine, const char **vendor)
{
	auto eng = reinterpret_cast<csret_wp_engine_s *>(engine);

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (vendor == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*vendor = eng->vendorName.c_str();

	return CSRE_ERROR_NONE;
}

API
int csre_wp_engine_get_name(csre_wp_engine_h engine, const char **name)
{
	auto eng = reinterpret_cast<csret_wp_engine_s *>(engine);

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (name == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*name = eng->engineName.c_str();

	return CSRE_ERROR_NONE;
}

API
int csre_wp_engine_get_vendor_logo(csre_wp_engine_h engine,
								   unsigned char **vendor_logo_image,
								   unsigned int *image_size)
{
	auto eng = reinterpret_cast<csret_wp_engine_s *>(engine);

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (vendor_logo_image == nullptr || image_size == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	auto s = eng->logoImage.size();

	if (s > UINT_MAX - 1)
		return CSRET_WP_ERROR_FILE_IO;

	*image_size = static_cast<unsigned int>(s);
	*vendor_logo_image = eng->logoImage.data();

	return CSRE_ERROR_NONE;
}

API
int csre_wp_engine_get_version(csre_wp_engine_h engine, const char **version)
{
	auto eng = reinterpret_cast<csret_wp_engine_s *>(engine);

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (version == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*version = eng->engineVersion.c_str();

	return CSRE_ERROR_NONE;
}

API
int csre_wp_engine_get_data_version(csre_wp_engine_h engine,
									const char **version)
{
	auto eng = reinterpret_cast<csret_wp_engine_s *>(engine);

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (version == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*version = eng->dataVersion.c_str();

	return CSRE_ERROR_NONE;
}


API
int csre_wp_engine_get_latest_update_time(csre_wp_engine_h engine, time_t *time)
{
	auto eng = reinterpret_cast<csret_wp_engine_s *>(engine);

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (time == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*time = eng->latestUpdate;
	return CSRE_ERROR_NONE;
}


API
int csre_wp_engine_get_activated(csre_wp_engine_h engine,
								 csre_wp_activated_e *pactivated)
{
	auto eng = reinterpret_cast<csret_wp_engine_s *>(engine);

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (pactivated == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	if (g_risky_urls.empty())
		*pactivated = CSRE_WP_NOT_ACTIVATED;
	else
		*pactivated = CSRE_WP_ACTIVATED;

	return CSRE_ERROR_NONE;
}

API
int csre_wp_engine_get_api_version(csre_wp_engine_h engine,
								   const char **version)
{
	auto eng = reinterpret_cast<csret_wp_engine_s *>(engine);

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (version == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*version = eng->apiVersion.c_str();

	return CSRE_ERROR_NONE;
}

//==============================================================================
// Error related
//==============================================================================
API
int csre_wp_get_error_string(int error_code, const char **string)
{
	if (string == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	switch (error_code) {
	case CSRET_WP_ERROR_NO_RISKY_URL_FILE:
		*string = "CSRET_WP_ERROR_NO_RISKY_URL_FILE";
		break;

	case CSRET_WP_ERROR_SIGNATURE_FILE_FORMAT:
		*string = "CSRET_WP_ERROR_SIGNATURE_FILE_FORMAT";

	case CSRET_WP_ERROR_FILE_IO:
		*string = "CSRET_WP_ERROR_FILE_IO";
		break;

	default:
		*string = "UNDEFINED ERROR CODE";
		break;
	}

	return CSRE_ERROR_NONE;
}
