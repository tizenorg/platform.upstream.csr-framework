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
#include <csre-content-screening.h>
#include <csre-content-screening-engine-info.h>

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <list>
#include <fstream>
#include <iostream>
#include <climits>
#include <ctime>
#include <cstring>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#define API __attribute__((visibility("default")))

#define PRIVATE_DB_NAME   "csret_cs_virus_signatures"
#define PRIVATE_LOGO_FILE "vendor_logo.bmp"

#define VENDOR_NAME       "TEST_VENDOR"
#define ENGINE_NAME       "TEST_LOCAL_TCS_ENGINE"
#define ENGINE_VERSION    "0.0.1"

using RawBuffer = std::vector<unsigned char>;

// TODO(k.tak): make all engine functions exception-safe

struct csret_cs_malware_s {
	csre_cs_severity_level_e severity;
	csre_cs_threat_type_e threat_type;
	std::string name;
	std::string detailed_url;
	std::string signature;
};

struct csret_cs_detected_s {
	csret_cs_malware_s malware;
	time_t timestamp;
};

struct csret_cs_engine_s {
	std::string vendorName;
	std::string engineName;
	std::string apiVersion;
	std::string engineVersion;
	std::string dataVersion;
	RawBuffer logoImage;
	time_t latestUpdate;
};

struct csret_cs_context_s {
	int scan_on_data;
	std::list<csret_cs_detected_s> detected_list;
	csret_cs_engine_s *engine;
};

enum csret_cs_internal_error_e {
	CSRET_CS_ERROR_NO_SIGNATURE_FILE     = -0x0101,
	CSRET_CS_ERROR_SIGNATURE_FILE_FORMAT = -0x0102,
	CSRET_CS_ERROR_FILE_IO               = -0x0103
};

//==============================================================================
// static variables
//==============================================================================
static std::string g_private_db_path;
static std::string g_private_logo_path;
static std::list<csret_cs_malware_s> g_virus_sig;

//==============================================================================
// Utilities functions
//==============================================================================
std::string csret_cs_extract_value(const std::string &line,
								   const std::string &key)
{
	if (line.empty() || key.empty())
		return std::string();

	auto pos = line.find(key);

	if (pos == std::string::npos || pos != 0)
		return std::string();

	return line.substr(key.length());
}

int csret_cs_read_virus_signatures(const std::string &path)
{
	// virus_signature file format
	// data_version=1.0.0 // it should be in the first line.
	// # this line is a comment.
	//
	// name=test_malware // this starts a description of a new malware
	// severity=HIGH  // LOW/MEDIUM/HIGH
	// threat_type=MALWARE  // MALWARE/RISKY/GENERIC
	// detailed_url=http://high.malware.com
	// signature=X5O!P%@AP[4\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*
	//
	// name=test_risk
	// severity=MEDIUM
	// threat_type=RISKY
	// detailed_url=http://medium.malware.com
	// signature=RISKY_MALWARE
	std::ifstream f(path.c_str(), std::ifstream::in);

	if (!f.is_open())
		return CSRET_CS_ERROR_NO_SIGNATURE_FILE;

	std::string line;
	csret_cs_malware_s node;

	while (std::getline(f, line)) {
		if (line.empty() || line[0] == '#')
			continue;

		auto value = csret_cs_extract_value(line, "name=");

		if (!value.empty()) {
			if (!node.name.empty()) {
				g_virus_sig.push_back(node);
				node = csret_cs_malware_s();
			}

			node.name = std::move(value);
			continue;
		}

		value = csret_cs_extract_value(line, "severity=");

		if (!value.empty()) {
			if (value.compare("LOW") == 0)
				node.severity = CSRE_CS_SEVERITY_LOW;
			else if (value.compare("MEDIUM") == 0)
				node.severity = CSRE_CS_SEVERITY_MEDIUM;
			else
				node.severity = CSRE_CS_SEVERITY_HIGH;

			continue;
		}

		value = csret_cs_extract_value(line, "threat_type=");

		if (!value.empty()) {
			if (value.compare("MALWARE") == 0)
				node.threat_type = CSRE_CS_THREAT_MALWARE;
			else if (value.compare("RISKY") == 0)
				node.threat_type = CSRE_CS_THREAT_RISKY;
			else
				node.threat_type = CSRE_CS_THREAT_GENERIC;

			continue;
		}

		value = csret_cs_extract_value(line, "detailed_url=");

		if (!value.empty()) {
			node.detailed_url = std::move(value);
			continue;
		}

		value = csret_cs_extract_value(line, "signature=");

		if (!value.empty())
			node.signature = std::move(value);
	}

	if (!node.name.empty())
		g_virus_sig.push_back(node);

	return CSRE_ERROR_NONE;
}

int csret_cs_read_binary(const std::string &path, RawBuffer &buffer)
{
	std::ifstream f(path.c_str(), std::ios::binary);

	if (!f.is_open()) {
		buffer.clear();
		return CSRE_ERROR_NONE;
	}

	f.seekg(0, f.end);
	auto len = f.tellg();
	if (len == -1) {
		buffer.clear();
		return CSRET_CS_ERROR_FILE_IO;
	}

	f.seekg(0, f.beg);

	buffer.resize(len, 0);
	f.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
	if (!f) {
		buffer.clear();
		return CSRET_CS_ERROR_FILE_IO;
	}

	return CSRE_ERROR_NONE;
}

time_t csret_cs_get_timestamp()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return tv.tv_sec;
}

int csret_cs_init_engine(csret_cs_engine_s **pengine)
{
	if (pengine == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	auto ptr = new csret_cs_engine_s;

	ptr->vendorName = VENDOR_NAME;
	ptr->engineName = ENGINE_NAME;
	ptr->apiVersion = CSRE_CS_API_VERSION;
	ptr->engineVersion = ENGINE_VERSION;
	ptr->dataVersion = ENGINE_VERSION;

	int ret = csret_cs_read_binary(g_private_logo_path, ptr->logoImage);

	if (ret != CSRE_ERROR_NONE) {
		delete ptr;
		return ret;
	}

	struct stat attrib;
	::memset(&attrib, 0x00, sizeof(attrib));
	ret = ::stat(g_private_db_path.c_str(), &attrib);
	if (ret != 0) {
		delete ptr;
		return CSRET_CS_ERROR_NO_SIGNATURE_FILE;
	}

	ptr->latestUpdate = attrib.st_mtime;

	*pengine = ptr;

	return CSRE_ERROR_NONE;
}

int csret_cs_compare_data(const RawBuffer &data, const std::string &needle)
{
	if (data.size() < needle.length())
		return -1;

	for (size_t i = 0; i < data.size() - needle.length(); i++) {
		bool isMatched = true;

		for (size_t j = 0; j < needle.length(); j++) {
			if (data[i + j] != static_cast<unsigned char>(needle[j])) {
				isMatched = false;
				break;
			}
		}

		if (isMatched)
			return 0; // matched
	}

	return -1;
}

int csret_cs_detect_malware(csret_cs_context_s *context, const RawBuffer &data,
							csret_cs_detected_s **pdetected)
{
	if (context == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (g_virus_sig.empty())
		return CSRE_ERROR_ENGINE_NOT_ACTIVATED;

	for (auto &item : g_virus_sig) {
		if (csret_cs_compare_data(data, item.signature) != 0)
			continue;

		csret_cs_detected_s detected;
		detected.malware.severity = item.severity;
		detected.malware.threat_type = item.threat_type;
		detected.malware.name = item.name;
		detected.malware.detailed_url = item.detailed_url;
		detected.timestamp = csret_cs_get_timestamp();

		context->detected_list.push_back(detected);
		*pdetected = &context->detected_list.back();

		return CSRE_ERROR_NONE;
	}

	*pdetected = nullptr;

	return CSRE_ERROR_NONE;
}

//==============================================================================
// Main function related
//==============================================================================
API
int csre_cs_global_initialize(const char *ro_res_dir, const char *rw_working_dir)
{
	if (ro_res_dir == nullptr || rw_working_dir == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	g_private_db_path = std::string(rw_working_dir) + "/" PRIVATE_DB_NAME;
	g_private_logo_path = std::string(ro_res_dir) + "/" PRIVATE_LOGO_FILE;

	g_virus_sig.clear();

	return csret_cs_read_virus_signatures(g_private_db_path);
}

API
int csre_cs_global_deinitialize()
{
	return CSRE_ERROR_NONE;
}

API
int csre_cs_context_create(csre_cs_context_h *phandle)
{
	if (phandle == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	if (g_virus_sig.empty())
		return CSRE_ERROR_INVALID_HANDLE; // not yet initialized

	csret_cs_engine_s *engine = nullptr;
	auto ret = csret_cs_init_engine(&engine);
	if (ret != CSRE_ERROR_NONE)
		return ret;

	auto context = new (std::nothrow) csret_cs_context_s;
	if (context == nullptr)
		return CSRE_ERROR_OUT_OF_MEMORY;

	context->engine = engine;

	*phandle = reinterpret_cast<csre_cs_context_h>(context);

	return CSRE_ERROR_NONE;
}

API
int csre_cs_context_destroy(csre_cs_context_h handle)
{
	auto context = reinterpret_cast<csret_cs_context_s *>(handle);

	if (context == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (context->engine != nullptr)
		delete context->engine;

	delete context;

	return CSRE_ERROR_NONE;
}

API
int csre_cs_scan_data(csre_cs_context_h handle,
					  const unsigned char *data,
					  size_t length,
					  csre_cs_detected_h *pdetected)
{
	auto context = reinterpret_cast<csret_cs_context_s *>(handle);

	if (context == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	RawBuffer vec(data, data + length);
	csret_cs_detected_s *detected = nullptr;
	int ret = csret_cs_detect_malware(context, vec, &detected);

	if (ret != CSRE_ERROR_NONE)
		return ret;

	*pdetected = reinterpret_cast<csre_cs_detected_h>(detected);

	return ret;
}

API
int csre_cs_scan_file(csre_cs_context_h handle,
					  const char *file_path,
					  csre_cs_detected_h *pdetected)
{
	if (file_path == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	if (handle == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	auto context = reinterpret_cast<csret_cs_context_s *>(handle);

	RawBuffer vec;
	int ret = csret_cs_read_binary(file_path, vec);

	if (ret != CSRE_ERROR_NONE)
		return ret;

	csret_cs_detected_s *detected = nullptr;
	ret = csret_cs_detect_malware(context, vec, &detected);

	if (ret != CSRE_ERROR_NONE)
		return ret;

	*pdetected = reinterpret_cast<csre_cs_detected_h>(detected);

	return CSRE_ERROR_NONE;
}

API
int csre_cs_scan_app_on_cloud_supported()
{
	return CSRE_CS_SUPPORTED;
}

API
int csre_cs_scan_app_on_cloud(csre_cs_context_h handle,
							  const char *app_root_dir,
							  csre_cs_detected_h *pdetected)
{
	if (handle == nullptr || app_root_dir == nullptr || pdetected == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	csre_cs_detected_h detected = nullptr;
	csre_cs_detected_h most_detected = nullptr;
	csre_cs_severity_level_e most = CSRE_CS_SEVERITY_LOW;

	std::unique_ptr<DIR, std::function<int(DIR *)>> dirp(opendir(app_root_dir),
			closedir);

	if (!dirp) {
		// silently skip the subdirectory in app_root_dir that unable to open because of
		// having no permission
		*pdetected = nullptr;
		return CSRE_ERROR_NONE;
	}

	struct dirent entry;
	struct dirent *result;

	while (readdir_r(dirp.get(), &entry, &result) == 0 && result != nullptr) {
		std::string fullpath(app_root_dir);
		std::string filename(entry.d_name);
		fullpath += "/";
		fullpath += filename;

		int ret = CSRE_ERROR_NONE;

		if (entry.d_type & (DT_REG | DT_LNK))
			ret = csre_cs_scan_file(handle, fullpath.c_str(), &detected);
		else if ((entry.d_type & DT_DIR)
				 && filename.compare("..") != 0
				 && filename.compare(".") != 0)
			ret = csre_cs_scan_app_on_cloud(handle, fullpath.c_str(), &detected);
		else
			continue;

		if (ret != CSRE_ERROR_NONE)
			return ret;

		if (detected == nullptr)
			continue;

		csre_cs_severity_level_e s = CSRE_CS_SEVERITY_LOW;
		ret = csre_cs_detected_get_severity(detected, &s);

		if (ret != CSRE_ERROR_NONE)
			return ret;

		if (most_detected == nullptr || s > most) {
			most_detected = detected;
			most = s;
		}
	}

	*pdetected = most_detected;

	return CSRE_ERROR_NONE;
}

//==============================================================================
// Result related
//==============================================================================
API
int csre_cs_detected_get_severity(csre_cs_detected_h detected,
								  csre_cs_severity_level_e *pseverity)
{
	if (detected == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (pseverity == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	auto pdetected = reinterpret_cast<csret_cs_detected_s *>(detected);
	*pseverity = pdetected->malware.severity;
	return CSRE_ERROR_NONE;
}

API
int csre_cs_detected_get_threat_type(csre_cs_detected_h detected,
									 csre_cs_threat_type_e *pthreat_type)
{
	if (detected == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (pthreat_type == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	auto pdetected = reinterpret_cast<csret_cs_detected_s *>(detected);
	*pthreat_type = pdetected->malware.threat_type;
	return CSRE_ERROR_NONE;
}

API
int csre_cs_detected_get_malware_name(csre_cs_detected_h detected,
									  const char **malware_name)
{
	if (detected == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (malware_name == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	auto pdetected = reinterpret_cast<csret_cs_detected_s *>(detected);
	*malware_name = pdetected->malware.name.c_str();
	return CSRE_ERROR_NONE;
}

API
int csre_cs_detected_get_detailed_url(csre_cs_detected_h detected,
									  const char **detailed_url)
{
	if (detected == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (detailed_url == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	auto pdetected = reinterpret_cast<csret_cs_detected_s *>(detected);
	*detailed_url = pdetected->malware.detailed_url.c_str();
	return CSRE_ERROR_NONE;
}

//==============================================================================
// Engine information related
//==============================================================================
API
int csre_cs_engine_get_vendor(csre_cs_context_h context, const char **vendor)
{
	if (context == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (vendor == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	auto eng = reinterpret_cast<csret_cs_context_s *>(context)->engine;

	*vendor = eng->vendorName.c_str();

	return CSRE_ERROR_NONE;
}

API
int csre_cs_engine_get_name(csre_cs_context_h context, const char **name)
{
	if (context == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (name == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	auto eng = reinterpret_cast<csret_cs_context_s *>(context)->engine;

	*name = eng->engineName.c_str();

	return CSRE_ERROR_NONE;
}

API
int csre_cs_engine_get_vendor_logo(csre_cs_context_h context,
								   unsigned char **logo_image,
								   unsigned int *image_size)
{
	if (context == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (logo_image == nullptr || image_size == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	auto eng = reinterpret_cast<csret_cs_context_s *>(context)->engine;

	auto s = eng->logoImage.size();

	if (s > UINT_MAX - 1)
		return CSRET_CS_ERROR_FILE_IO;

	*image_size = static_cast<unsigned int>(s);
	*logo_image = eng->logoImage.data();

	return CSRE_ERROR_NONE;
}

API
int csre_cs_engine_get_version(csre_cs_context_h context, const char **version)
{
	if (context == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (version == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	auto eng = reinterpret_cast<csret_cs_context_s *>(context)->engine;

	*version = eng->engineVersion.c_str();

	return CSRE_ERROR_NONE;
}

API
int csre_cs_engine_get_data_version(csre_cs_context_h context, const char **version)
{
	if (context == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (version == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	auto eng = reinterpret_cast<csret_cs_context_s *>(context)->engine;

	*version = eng->dataVersion.c_str();

	return CSRE_ERROR_NONE;
}

API
int csre_cs_engine_get_latest_update_time(csre_cs_context_h context, time_t *time)
{
	if (context == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (time == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	auto eng = reinterpret_cast<csret_cs_context_s *>(context)->engine;

	*time = eng->latestUpdate;

	return CSRE_ERROR_NONE;
}

API
int csre_cs_engine_get_activated(csre_cs_context_h context,
								 csre_cs_activated_e *pactivated)
{
	(void) context;

	if (pactivated == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	if (g_virus_sig.empty())
		*pactivated = CSRE_CS_NOT_ACTIVATED;
	else
		*pactivated = CSRE_CS_ACTIVATED;

	return CSRE_ERROR_NONE;
}

API
int csre_cs_engine_get_api_version(csre_cs_context_h context, const char **version)
{
	if (context == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (version == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	auto eng = reinterpret_cast<csret_cs_context_s *>(context)->engine;

	*version = eng->apiVersion.c_str();

	return CSRE_ERROR_NONE;
}


//==============================================================================
// Error related
//==============================================================================
API
int csre_cs_get_error_string(int error_code, const char **string)
{
	if (string == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	switch (error_code) {
	case CSRET_CS_ERROR_NO_SIGNATURE_FILE:
		*string = "CSRET_CS_ERROR_NO_SIGNATURE_FILE";
		break;

	case CSRET_CS_ERROR_SIGNATURE_FILE_FORMAT:
		*string = "CSRET_CS_ERROR_SIGNATURE_FILE_FORMAT";
		break;

	case CSRET_CS_ERROR_FILE_IO:
		*string = "CSRET_CS_ERROR_FILE_IO";
		break;

	default:
		*string = "UNDEFINED ERROR CODE";
		break;
	}

	return CSRE_ERROR_NONE;
}
