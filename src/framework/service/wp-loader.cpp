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
 * @file       wp-loader.cpp
 * @author     Sangsu Choi (sangsu.choi@samsung.com)
 * @version    1.0
 * @brief
 */
#include "service/wp-loader.h"

#include <stdexcept>
#include <dlfcn.h>

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

void WpLoader::checkEngineActivated(csre_wp_context_h c)
{
	csre_wp_activated_e a = CSRE_WP_NOT_ACTIVATED;

	this->getEngineActivated(c, &a);

	if (a == CSRE_WP_NOT_ACTIVATED)
		ThrowExc(CSR_ERROR_ENGINE_NOT_ACTIVATED, "engine is not activated yet");
}

void WpLoader::contextCreate(csre_wp_context_h &c)
{
	this->toException(this->m_pc.fpContextCreate(&c));
}

void WpLoader::contextDestroy(csre_wp_context_h c)
{
	if (c == nullptr)
		throw std::invalid_argument("wp loader context destroy");

	this->toException(this->m_pc.fpContextDestroy(c));
}

void WpLoader::checkUrl(csre_wp_context_h c, const std::string &url,
					   csre_wp_check_result_h *presult)
{
	if (c == nullptr || url.empty() || presult == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "wp loader check url error");

	this->checkEngineActivated(c);

	this->toException(this->m_pc.fpCheckUrl(c, (const char *)url.c_str(), presult));
}

void WpLoader::getRiskLevel(csre_wp_check_result_h r, csre_wp_risk_level_e *plevel)
{
	if (r == nullptr || plevel == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "wp loader Get Risk Level error");

	this->toException(this->m_pc.fpGetRiskLevel(r, plevel));
}

void WpLoader::getDetailedUrl(csre_wp_check_result_h r, std::string &value)
{
	if (r == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "wp loader get detailed url error");

	this->toException(getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetDetailedUrl(r, cvalue);
	}));
}


std::string WpLoader::getErrorString(int ec)
{
	std::string value;

	this->toException(getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetErrorString(ec, cvalue);
	}));

	return value;
}

void WpLoader::getEngineApiVersion(csre_wp_context_h c, std::string &value)
{
	if (c == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "wp loader get error string");

	this->toException(getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineApiVersion(c, cvalue);
	}));
}

void WpLoader::getEngineVendor(csre_wp_context_h c, std::string &value)
{
	if (c == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "wp loader get engine vendor");

	this->toException(getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineVendor(c, cvalue);
	}));
}

void WpLoader::getEngineName(csre_wp_context_h c, std::string &value)
{
	if (c == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "wp loader get engine name");

	this->toException(getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineName(c, cvalue);
	}));
}

void WpLoader::getEngineVersion(csre_wp_context_h c, std::string &value)
{
	if (c == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "wp loader get engine version");

	this->toException(getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineVersion(c, cvalue);
	}));
}

void WpLoader::getEngineDataVersion(csre_wp_context_h c, std::string &value)
{
	if (c == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "wp loader get engine version");

	this->toException(getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineDataVersion(c, cvalue);
	}));
}

void WpLoader::getEngineLatestUpdateTime(csre_wp_context_h c, time_t *ptime)
{
	if (c == nullptr || ptime == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "wp loader get latest update time");

	this->toException(this->m_pc.fpGetEngineLatestUpdateTime(c, ptime));
}

void WpLoader::getEngineActivated(csre_wp_context_h c, csre_wp_activated_e *pactivated)
{
	if (c == nullptr || pactivated == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "wp loader get engine activated");

	auto ret = this->m_pc.fpGetEngineActivated(c, pactivated);

	if (ret == CSRE_ERROR_ENGINE_NOT_ACTIVATED)
		*pactivated = CSRE_WP_NOT_ACTIVATED;
	else if (ret != CSRE_ERROR_NONE)
		this->toException(ret);
}

void WpLoader::getEngineVendorLogo(csre_wp_context_h c, std::vector<unsigned char> &value)
{
	if (c == nullptr)
		ThrowExc(CSR_ERROR_INVALID_PARAMETER, "wp loader get engine vendor logo");

	unsigned char *cvalue = nullptr;
	unsigned int size = 0;
	auto retval = this->m_pc.fpGetEngineVendorLogo(c, &cvalue, &size);

	if (retval == CSRE_ERROR_NONE && cvalue != nullptr && size != 0)
		value.assign(cvalue, cvalue + size);

	this->toException(retval);
}

void WpLoader::init(const std::string &enginePath, const std::string &roResDir,
					const std::string &rwWorkingDir)
{
	if (enginePath.empty() || roResDir.empty() || rwWorkingDir.empty())
		ThrowExc(CSR_ERROR_SERVER, "empty string comes in to loader init");

	INFO("Load WP-Engine plugin start. engine path: " << enginePath);

	void *handle = dlopen(enginePath.c_str(), RTLD_LAZY);

	if (handle == nullptr)
		ThrowExcWarn(CSR_ERROR_ENGINE_NOT_EXIST, "engine dlopen error. path: " <<
					 enginePath << " errno: " << errno);

	this->m_pc.dlhandle = handle;

	this->m_pc.fpGlobalInit = reinterpret_cast<FpGlobalInit>(dlsym(handle,
							  "csre_wp_global_initialize"));
	this->m_pc.fpGlobalDeinit = reinterpret_cast<FpGlobalDeinit>(dlsym(handle,
								"csre_wp_global_deinitialize"));
	this->m_pc.fpContextCreate = reinterpret_cast<FpContextCreate>(dlsym(handle,
								 "csre_wp_context_create"));
	this->m_pc.fpContextDestroy = reinterpret_cast<FpContextDestroy>(dlsym(handle,
								  "csre_wp_context_destroy"));
	this->m_pc.fpCheckUrl = reinterpret_cast<FpCheckUrl>(dlsym(handle,
							"csre_wp_check_url"));
	this->m_pc.fpGetRiskLevel = reinterpret_cast<FpGetRiskLevel>(dlsym(handle,
								"csre_wp_result_get_risk_level"));
	this->m_pc.fpGetDetailedUrl = reinterpret_cast<FPGetDetailedUrl>(dlsym(handle,
								  "csre_wp_result_get_detailed_url"));
	this->m_pc.fpGetErrorString = reinterpret_cast <FpGetErrorString>(dlsym(handle,
								  "csre_wp_get_error_string"));
	this->m_pc.fpGetEngineApiVersion = reinterpret_cast<FpGetEngineApiVersion>(dlsym(
									   handle, "csre_wp_engine_get_api_version"));
	this->m_pc.fpGetEngineVendor = reinterpret_cast<FpGetEngineVendor>(dlsym(handle,
								   "csre_wp_engine_get_vendor"));
	this->m_pc.fpGetEngineName = reinterpret_cast<FpGetEngineName>(dlsym(handle,
								 "csre_wp_engine_get_name"));
	this->m_pc.fpGetEngineVersion = reinterpret_cast<FpGetEngineVersion>(dlsym(handle,
									"csre_wp_engine_get_version"));
	this->m_pc.fpGetEngineDataVersion = reinterpret_cast<FpGetEngineDataVersion>(dlsym(
										handle, "csre_wp_engine_get_data_version"));
	this->m_pc.fpGetEngineLatestUpdateTime =
		reinterpret_cast<FpGetEngineLatestUpdateTime>(dlsym(handle,
				"csre_wp_engine_get_latest_update_time"));
	this->m_pc.fpGetEngineActivated = reinterpret_cast<FpGetEngineActivated>(dlsym(
									  handle, "csre_wp_engine_get_activated"));
	this->m_pc.fpGetEngineVendorLogo = reinterpret_cast<FpGetEngineVendorLogo>(dlsym(
									   handle, "csre_wp_engine_get_vendor_logo"));

	if (this->m_pc.fpGlobalInit == nullptr || this->m_pc.fpGlobalDeinit == nullptr ||
			this->m_pc.fpContextCreate == nullptr ||
			this->m_pc.fpContextDestroy == nullptr ||
			this->m_pc.fpCheckUrl == nullptr || this->m_pc.fpGetRiskLevel == nullptr ||
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
		ThrowExc(CSR_ERROR_ENGINE_NOT_EXIST, "Failed to load funcs from engine library. "
				 "engine path: " << enginePath << "errno: " << errno);
	}

	auto ret = this->m_pc.fpGlobalInit(roResDir.c_str(), rwWorkingDir.c_str());

	if (ret != CSRE_ERROR_NONE) {
		dlclose(this->m_pc.dlhandle);
		this->m_pc.dlhandle = nullptr;
		this->toException(ret, false);
	}
}

void WpLoader::deinit()
{
	// ignore return value
	this->m_pc.fpGlobalDeinit();

	if (this->m_pc.dlhandle) {
		dlclose(this->m_pc.dlhandle);
		this->m_pc.dlhandle = nullptr;
	}
}

WpLoader::WpLoader(const std::string &enginePath, const std::string &roResDir,
				   const std::string &rwWorkingDir)
{
	this->init(enginePath, roResDir, rwWorkingDir);
}

WpLoader::~WpLoader()
{
	this->deinit();
}


WpEngineContext::WpEngineContext(const std::shared_ptr<WpLoader> &loader) :
	m_loader(loader), m_context(nullptr)
{
	if (!this->m_loader)
		ThrowExc(CSR_ERROR_ENGINE_NOT_EXIST, "null loader means engine not exist!");

	this->m_loader->contextCreate(this->m_context);
}

WpEngineContext::~WpEngineContext()
{
	try {
		this->m_loader->contextDestroy(this->m_context);
	} catch (...) {
		ERROR("exception in contextDestroy of wp loader");
	}
}

csre_wp_context_h &WpEngineContext::get(void)
{
	return this->m_context;
}

}
