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

#include "common/audit/logger.h"
#include "service/engine-error-converter.h"

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

	int ret = this->getEngineActivated(c, &a);

	if (ret != CSRE_ERROR_NONE)
		toException(ret);

	if (a == CSRE_CS_NOT_ACTIVATED)
		ThrowExc(EngineNotActivated, "engine is not activated yet");
}

int CsLoader::contextCreate(csre_cs_context_h &c)
{
	return this->m_pc.fpContextCreate(&c);
}

int CsLoader::contextDestroy(csre_cs_context_h c)
{
	if (c == nullptr)
		throw std::invalid_argument("cs loader context destroy");

	return this->m_pc.fpContextDestroy(c);
}

int CsLoader::scanData(csre_cs_context_h c,
					   const std::vector<unsigned char> &data,
					   csre_cs_detected_h *pdetected)
{
	if (c == nullptr || data.empty() || pdetected == nullptr)
		throw std::invalid_argument("cs loader scan data");

	this->checkEngineActivated(c);

	return this->m_pc.fpScanData(c, data.data(), data.size(), pdetected);
}

int CsLoader::scanFile(csre_cs_context_h c, const std::string &filepath,
					   csre_cs_detected_h *pdetected)
{
	if (c == nullptr || filepath.empty() || pdetected == nullptr)
		throw std::invalid_argument("cs loader scan file");

	this->checkEngineActivated(c);

	return this->m_pc.fpScanFile(c, filepath.c_str(), pdetected);
}

int CsLoader::scanAppOnCloud(csre_cs_context_h c, const std::string &appdir,
							 csre_cs_detected_h *pdetected)
{
	if (c == nullptr || appdir.empty() || pdetected == nullptr)
		throw std::invalid_argument("cs loader scan app on cloud");

	this->checkEngineActivated(c);

	return this->m_pc.fpScanAppOnCloud(c, appdir.c_str(), pdetected);
}

int CsLoader::getSeverity(csre_cs_detected_h d,
						  csre_cs_severity_level_e *pseverity)
{
	if (d == nullptr || pseverity == nullptr)
		throw std::invalid_argument("cs loader get severity");

	return this->m_pc.fpGetSeverity(d, pseverity);
}

int CsLoader::getMalwareName(csre_cs_detected_h d, std::string &value)
{
	if (d == nullptr)
		throw std::invalid_argument("cs loader get malware name");

	return getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetMalwareName(d, cvalue);
	});
}

int CsLoader::getDetailedUrl(csre_cs_detected_h d, std::string &value)
{
	if (d == nullptr)
		throw std::invalid_argument("cs loader get detailed url");

	return getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetDetailedUrl(d, cvalue);
	});
}

int CsLoader::getErrorString(int ec, std::string &value)
{
	return getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetErrorString(ec, cvalue);
	});
}

int CsLoader::getEngineApiVersion(csre_cs_context_h c, std::string &value)
{
	if (c == nullptr)
		throw std::invalid_argument("cs loader get error string");

	return getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineApiVersion(c, cvalue);
	});
}

int CsLoader::getEngineVendor(csre_cs_context_h c, std::string &value)
{
	if (c == nullptr)
		throw std::invalid_argument("cs loader get engine vendor");

	return getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineVendor(c, cvalue);
	});
}

int CsLoader::getEngineName(csre_cs_context_h c, std::string &value)
{
	if (c == nullptr)
		throw std::invalid_argument("cs loader get engine name");

	return getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineName(c, cvalue);
	});
}

int CsLoader::getEngineVersion(csre_cs_context_h c, std::string &value)
{
	if (c == nullptr)
		throw std::invalid_argument("cs loader get engine version");

	return getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineVersion(c, cvalue);
	});
}

int CsLoader::getEngineDataVersion(csre_cs_context_h c, std::string &value)
{
	if (c == nullptr)
		throw std::invalid_argument("cs loader get engine version");

	return getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineDataVersion(c, cvalue);
	});
}

int CsLoader::getEngineLatestUpdateTime(csre_cs_context_h c, time_t *ptime)
{
	if (c == nullptr || ptime == nullptr)
		throw std::invalid_argument("cs loader get latest update time");

	return this->m_pc.fpGetEngineLatestUpdateTime(c, ptime);
}

int CsLoader::getEngineActivated(csre_cs_context_h c, csre_cs_activated_e *pactivated)
{
	if (c == nullptr || pactivated == nullptr)
		throw std::invalid_argument("cs loader get engine activated");

	auto ret = this->m_pc.fpGetEngineActivated(c, pactivated);

	if (ret == CSRE_ERROR_ENGINE_NOT_ACTIVATED) {
		*pactivated = CSRE_CS_NOT_ACTIVATED;
		return CSRE_ERROR_NONE;
	} else {
		return ret;
	}
}

int CsLoader::getEngineVendorLogo(csre_cs_context_h c, std::vector<unsigned char> &value)
{
	if (c == nullptr)
		throw std::invalid_argument("cs loader get engine vendor logo");

	unsigned char *cvalue = nullptr;
	unsigned int size = 0;
	auto retval = this->m_pc.fpGetEngineVendorLogo(c, &cvalue, &size);

	if (retval == CSRE_ERROR_NONE && cvalue != nullptr && size != 0)
		value.assign(cvalue, cvalue + size);

	return retval;
}

void CsLoader::init(const std::string &enginePath, const std::string &roResDir,
					const std::string &rwWorkingDir)
{
	if (enginePath.empty() || roResDir.empty() || rwWorkingDir.empty())
		ThrowExc(InternalError, "empty string comes in to loader init");

	INFO("Load CS-Engine plugin start. engine path: " << enginePath);

	void *handle = dlopen(enginePath.c_str(), RTLD_LAZY);

	if (handle == nullptr)
		ThrowExc(InternalError, "engine dlopen error. path: " << enginePath <<
				 " errno: " << errno);

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
	this->m_pc.fpGetSeverity = reinterpret_cast<FpGetSeverity>(dlsym(handle,
							   "csre_cs_detected_get_severity"));
	this->m_pc.fpGetMalwareName = reinterpret_cast<FpGetMalwareName>(dlsym(handle,
								  "csre_cs_detected_get_malware_name"));
	this->m_pc.fpGetDetailedUrl = reinterpret_cast<FpGetDetailedUrl>(dlsym(handle,
								  "csre_cs_detected_get_detailed_url"));
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
			this->m_pc.fpGetSeverity == nullptr ||
			this->m_pc.fpGetMalwareName == nullptr ||
			this->m_pc.fpGetDetailedUrl == nullptr ||
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
		ThrowExc(EngineError, "Failed to load funcs from engine library. "
				 "engine path: " << enginePath << "errno: " << errno);
	}

	auto ret = this->m_pc.fpGlobalInit(roResDir.c_str(), rwWorkingDir.c_str());

	if (ret != CSRE_ERROR_NONE) {
		dlclose(this->m_pc.dlhandle);
		this->m_pc.dlhandle = nullptr;
		toException(ret);
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
		ThrowExc(EngineNotExist, "null loader means engine not exist!");

	toException(this->m_loader->contextCreate(this->m_context));
}

CsEngineContext::~CsEngineContext()
{
	toException(this->m_loader->contextDestroy(this->m_context));
}

csre_cs_context_h &CsEngineContext::get(void)
{
	return this->m_context;
}

}
