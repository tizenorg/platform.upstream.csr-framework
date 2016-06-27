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
 * @file       cs-loader.cpp
 * @author     Sangsu Choi (sangsu.choi@samsung.com)
 * @version    1.0
 * @brief
 */
#include "service/cs-loader.h"

#include <stdexcept>
#include <dlfcn.h>

#include <csre-error.h>

#include "common/audit/logger.h"
#include "common/exception.h"

namespace Csr {

namespace {

int getValueCstr(std::string &value, const std::function<int(const char **)> &getfunc)
{
	const char *cvalue = nullptr;
	auto retval = getfunc(&cvalue);

	if (retval == CSRE_ERROR_NONE && cvalue != nullptr)
		value = cvalue;

	return retval;
}

} // namespace anonymous

void CsLoader::checkEngineActivated(csre_cs_context_h c)
{
	csre_cs_activated_e a = CSRE_CS_NOT_ACTIVATED;

	this->getEngineActivated(c, &a);

	if (a == CSRE_CS_NOT_ACTIVATED)
		ThrowExc(CSR_ERROR_ENGINE_NOT_ACTIVATED, "engine is not activated yet");
}

void CsLoader::contextCreate(csre_cs_context_h &c)
{
	this->toException(this->m_pc.fpContextCreate(&c));
}

void CsLoader::contextDestroy(csre_cs_context_h c)
{
	if (c == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "cs loader context destroy");

	this->toException(this->m_pc.fpContextDestroy(c));
}

void CsLoader::scanData(csre_cs_context_h c,
						const std::vector<unsigned char> &data,
						csre_cs_detected_h *pdetected)
{
	if (c == nullptr || data.empty() || pdetected == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "cs loader scan data");

	this->checkEngineActivated(c);

	this->toException(this->m_pc.fpScanData(c, data.data(), data.size(), pdetected));
}

void CsLoader::scanFile(csre_cs_context_h c, const std::string &filepath,
						csre_cs_detected_h *pdetected)
{
	if (c == nullptr || filepath.empty() || pdetected == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "cs loader scan file");

	this->checkEngineActivated(c);

	this->toException(this->m_pc.fpScanFile(c, filepath.c_str(), pdetected));
}

void CsLoader::scanAppOnCloud(csre_cs_context_h c, const std::string &appdir,
							  csre_cs_detected_h *pdetected)
{
	if (c == nullptr || appdir.empty() || pdetected == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "cs loader scan app on cloud");

	this->checkEngineActivated(c);

	this->toException(this->m_pc.fpScanAppOnCloud(c, appdir.c_str(), pdetected));
}

bool CsLoader::scanAppOnCloudSupported(void)
{
	return this->m_pc.fpScanAppOnCloudSupported() == CSRE_CS_SUPPORTED;
}

void CsLoader::getSeverity(csre_cs_detected_h d,
						   csre_cs_severity_level_e *pseverity)
{
	if (d == nullptr || pseverity == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "cs loader get severity");

	this->toException(this->m_pc.fpGetSeverity(d, pseverity));
}

void CsLoader::getMalwareName(csre_cs_detected_h d, std::string &value)
{
	if (d == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "cs loader get malware name");

	this->toException(getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetMalwareName(d, cvalue);
	}));
}

void CsLoader::getDetailedUrl(csre_cs_detected_h d, std::string &value)
{
	if (d == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "cs loader get detailed url");

#ifdef DETAILED_URL_BASE
	DEBUG("get deatiled url with base!");
	std::string name;
	this->getMalwareName(d, name);

	value = std::string(DETAILED_URL_BASE) + name;
#else
	DEBUG("get detailed url with engine getter API!");
	this->toException(getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetDetailedUrl(d, cvalue);
	}));
#endif
}

std::string CsLoader::getErrorString(int ec)
{
	std::string value;

	this->toException(getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetErrorString(ec, cvalue);
	}));

	return value;
}

void CsLoader::getEngineApiVersion(csre_cs_context_h c, std::string &value)
{
	if (c == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "cs loader get error string");

	this->toException(getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineApiVersion(c, cvalue);
	}));
}

void CsLoader::getEngineVendor(csre_cs_context_h c, std::string &value)
{
	if (c == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "cs loader get engine vendor");

	this->toException(getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineVendor(c, cvalue);
	}));
}

void CsLoader::getEngineName(csre_cs_context_h c, std::string &value)
{
	if (c == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "cs loader get engine name");

	this->toException(getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineName(c, cvalue);
	}));
}

void CsLoader::getEngineVersion(csre_cs_context_h c, std::string &value)
{
	if (c == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "cs loader get engine version");

	this->toException(getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineVersion(c, cvalue);
	}));
}

void CsLoader::getEngineDataVersion(csre_cs_context_h c, std::string &value)
{
	if (c == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "cs loader get engine version");

	this->toException(getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineDataVersion(c, cvalue);
	}));
}

time_t CsLoader::getEngineLatestUpdateTime(csre_cs_context_h c)
{
	if (c == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "cs loader get latest update time");

	time_t t;

	this->toException(this->m_pc.fpGetEngineLatestUpdateTime(c, &t));

	return t;
}

void CsLoader::getEngineActivated(csre_cs_context_h c, csre_cs_activated_e *pactivated)
{
	if (c == nullptr || pactivated == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "cs loader get engine activated");

	auto ret = this->m_pc.fpGetEngineActivated(c, pactivated);

	if (ret == CSRE_ERROR_ENGINE_NOT_ACTIVATED)
		*pactivated = CSRE_CS_NOT_ACTIVATED;
	else if (ret != CSRE_ERROR_NONE)
		this->toException(ret);
}

void CsLoader::getEngineVendorLogo(csre_cs_context_h c, std::vector<unsigned char> &value)
{
	if (c == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "cs loader get engine vendor logo");

	unsigned char *cvalue = nullptr;
	unsigned int size = 0;
	auto retval = this->m_pc.fpGetEngineVendorLogo(c, &cvalue, &size);

	if (retval == CSRE_ERROR_NONE && cvalue != nullptr && size != 0)
		value.assign(cvalue, cvalue + size);

	this->toException(retval);
}

void CsLoader::init(const std::string &enginePath, const std::string &roResDir,
					const std::string &rwWorkingDir)
{
	if (enginePath.empty() || roResDir.empty() || rwWorkingDir.empty())
		ThrowExc(CSR_ERROR_SERVER, "empty string comes in to loader init");

	INFO("Load CS-Engine plugin start. engine path: " << enginePath);

	void *handle = dlopen(enginePath.c_str(), RTLD_LAZY);

	if (handle == nullptr)
		ThrowExcWarn(CSR_ERROR_ENGINE_NOT_EXIST, "engine dlopen error. path: " <<
					 enginePath << " errno: " << errno);

	this->m_pc.dlhandle = handle;

	this->m_pc.fpGlobalInit = reinterpret_cast<FpGlobalInit>(dlsym(handle,
							  "csre_cs_global_initialize"));
	this->m_pc.fpGlobalDeinit = reinterpret_cast<FpGlobalDeinit>(dlsym(handle,
								"csre_cs_global_deinitialize"));
	this->m_pc.fpContextCreate = reinterpret_cast<FpContextCreate>(dlsym(handle,
								 "csre_cs_context_create"));
	this->m_pc.fpContextDestroy = reinterpret_cast<FpContextDestroy>(dlsym(handle,
								  "csre_cs_context_destroy"));
	this->m_pc.fpScanData = reinterpret_cast<FpScanData>(dlsym(handle,
							"csre_cs_scan_data"));
	this->m_pc.fpScanFile = reinterpret_cast<FpScanFile>(dlsym(handle,
							"csre_cs_scan_file"));
	this->m_pc.fpScanAppOnCloud = reinterpret_cast<FpScanAppOnCloud>(dlsym(handle,
								  "csre_cs_scan_app_on_cloud"));
	this->m_pc.fpScanAppOnCloudSupported = reinterpret_cast<FpScanAppOnCloudSupported>(
										   dlsym(handle,
												 "csre_cs_scan_app_on_cloud_supported"));
	this->m_pc.fpGetSeverity = reinterpret_cast<FpGetSeverity>(dlsym(handle,
							   "csre_cs_detected_get_severity"));
	this->m_pc.fpGetMalwareName = reinterpret_cast<FpGetMalwareName>(dlsym(handle,
								  "csre_cs_detected_get_malware_name"));
	this->m_pc.fpGetErrorString = reinterpret_cast <FpGetErrorString>(dlsym(handle,
								  "csre_cs_get_error_string"));
	this->m_pc.fpGetEngineApiVersion = reinterpret_cast<FpGetEngineApiVersion>(dlsym(
									   handle, "csre_cs_engine_get_api_version"));
	this->m_pc.fpGetEngineVendor = reinterpret_cast<FpGetEngineVendor>(dlsym(handle,
								   "csre_cs_engine_get_vendor"));
	this->m_pc.fpGetEngineName = reinterpret_cast<FpGetEngineName>(dlsym(handle,
								 "csre_cs_engine_get_name"));
	this->m_pc.fpGetEngineVersion = reinterpret_cast<FpGetEngineVersion>(dlsym(handle,
									"csre_cs_engine_get_version"));
	this->m_pc.fpGetEngineDataVersion = reinterpret_cast<FpGetEngineDataVersion>(dlsym(
										handle, "csre_cs_engine_get_data_version"));
	this->m_pc.fpGetEngineLatestUpdateTime =
		reinterpret_cast<FpGetEngineLatestUpdateTime>(dlsym(handle,
				"csre_cs_engine_get_latest_update_time"));
	this->m_pc.fpGetEngineActivated = reinterpret_cast<FpGetEngineActivated>(dlsym(
									  handle, "csre_cs_engine_get_activated"));
	this->m_pc.fpGetEngineVendorLogo = reinterpret_cast<FpGetEngineVendorLogo>(dlsym(
									   handle, "csre_cs_engine_get_vendor_logo"));

	if (this->m_pc.fpGlobalInit == nullptr || this->m_pc.fpGlobalDeinit == nullptr ||
			this->m_pc.fpContextCreate == nullptr ||
			this->m_pc.fpContextDestroy == nullptr ||
			this->m_pc.fpScanData == nullptr || this->m_pc.fpScanFile == nullptr  ||
			this->m_pc.fpScanAppOnCloud == nullptr ||
			this->m_pc.fpScanAppOnCloudSupported == nullptr ||
			this->m_pc.fpGetSeverity == nullptr ||
			this->m_pc.fpGetMalwareName == nullptr ||
			this->m_pc.fpGetErrorString == nullptr ||
			this->m_pc.fpGetEngineApiVersion == nullptr ||
			this->m_pc.fpGetEngineVendor == nullptr ||
			this->m_pc.fpGetEngineName == nullptr ||
			this->m_pc.fpGetEngineVersion == nullptr ||
			this->m_pc.fpGetEngineDataVersion == nullptr ||
			this->m_pc.fpGetEngineLatestUpdateTime == nullptr ||
			this->m_pc.fpGetEngineActivated == nullptr ||
			this->m_pc.fpGetEngineVendorLogo == nullptr) {
		dlclose(this->m_pc.dlhandle);
		this->m_pc.dlhandle = nullptr;
		ThrowExc(CSR_ERROR_ENGINE_NOT_EXIST, "Failed to load funcs from engine library. "
				 "engine path: " << enginePath << " errno: " << errno);
	}

	try {
#ifndef DETAILED_URL_BASE
		this->m_pc.fpGetDetailedUrl = reinterpret_cast<FpGetDetailedUrl>(dlsym(handle,
									  "csre_cs_detected_get_detailed_url"));
		if (this->m_pc.fpGetDetailedUrl == nullptr)
			ThrowExc(CSR_ERROR_ENGINE_NOT_EXIST, "Failed to load detailed_url getter from "
					 "engine lib. engine path: " << enginePath << " errno: " << errno);
#endif

		auto ret = this->m_pc.fpGlobalInit(roResDir.c_str(), rwWorkingDir.c_str());

		if (ret != CSRE_ERROR_NONE)
			this->toException(ret);
	} catch (...) {
		dlclose(this->m_pc.dlhandle);
		this->m_pc.dlhandle = nullptr;
		throw;
	}
}

void CsLoader::deinit()
{
	// ignore return value
	this->m_pc.fpGlobalDeinit();

	if (this->m_pc.dlhandle) {
		dlclose(this->m_pc.dlhandle);
		this->m_pc.dlhandle = nullptr;
	}
}

CsLoader::CsLoader(const std::string &enginePath, const std::string &roResDir,
				   const std::string &rwWorkingDir)
{
	this->init(enginePath, roResDir, rwWorkingDir);
}

CsLoader::~CsLoader()
{
	this->deinit();
}


CsEngineContext::CsEngineContext(const std::shared_ptr<CsLoader> &loader) :
	m_loader(loader), m_context(nullptr)
{
	if (!this->m_loader)
		ThrowExc(CSR_ERROR_ENGINE_NOT_EXIST, "null loader means engine not exist!");

	this->m_loader->contextCreate(this->m_context);
}

CsEngineContext::~CsEngineContext()
{
	try {
		this->m_loader->contextDestroy(this->m_context);
	} catch (...) {
		ERROR("exception in contextDestroy of cs loader");
	}
}

csre_cs_context_h &CsEngineContext::get(void)
{
	return this->m_context;
}

}
