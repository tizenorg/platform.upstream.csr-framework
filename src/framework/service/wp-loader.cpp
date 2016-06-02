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
#include "service/engine-error-converter.h"

namespace Csr {

namespace {

int getValueCstr(std::string &value,
				 const std::function<int(const char **)> &getfunc)
{
	const char *cvalue = nullptr;
	auto retval = getfunc(&cvalue);

	if (retval == CSRE_ERROR_NONE && cvalue != nullptr)
		value = cvalue;

	return retval;
}

} // namespace anonymous

int WpLoader::contextCreate(csre_wp_context_h &c)
{
	return this->m_pc.fpContextCreate(&c);
}

int WpLoader::contextDestroy(csre_wp_context_h c)
{
	if (c == nullptr)
		throw std::invalid_argument("wp loader context destroy");

	return this->m_pc.fpContextDestroy(c);
}

int WpLoader::checkUrl(csre_wp_context_h c, const std::string &url,
					   csre_wp_check_result_h *presult)
{
	if (c == nullptr || url.empty() || presult == nullptr)
		throw std::invalid_argument("wp loader check url error");

	return this->m_pc.fpCheckUrl(c, (const char *)url.c_str(), presult);
}

int WpLoader::getRiskLevel(csre_wp_check_result_h r,
						   csre_wp_risk_level_e *plevel)
{
	if (r == nullptr || plevel == nullptr)
		throw std::invalid_argument("wp loader Get Risk Level error");

	return this->m_pc.fpGetRiskLevel(r, plevel);
}

int WpLoader::getDetailedUrl(csre_wp_check_result_h r, std::string &value)
{
	if (r == nullptr)
		throw std::invalid_argument("wp loader get detailed url error");

	return getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetDetailedUrl(r, cvalue);
	});
}


int WpLoader::getErrorString(int ec, std::string &value)
{
	return getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetErrorString(ec, cvalue);
	});
}

int WpLoader::getEngineApiVersion(csre_wp_context_h c, std::string &value)
{
	if (c == nullptr)
		throw std::invalid_argument("wp loader get error string");

	return getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineApiVersion(c, cvalue);
	});
}

int WpLoader::getEngineVendor(csre_wp_context_h c, std::string &value)
{
	if (c == nullptr)
		throw std::invalid_argument("wp loader get engine vendor");

	return getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineVendor(c, cvalue);
	});
}

int WpLoader::getEngineName(csre_wp_context_h c, std::string &value)
{
	if (c == nullptr)
		throw std::invalid_argument("wp loader get engine name");

	return getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineName(c, cvalue);
	});
}

int WpLoader::getEngineVersion(csre_wp_context_h c, std::string &value)
{
	if (c == nullptr)
		throw std::invalid_argument("wp loader get engine version");

	return getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineVersion(c, cvalue);
	});
}

int WpLoader::getEngineDataVersion(csre_wp_context_h c, std::string &value)
{
	if (c == nullptr)
		throw std::invalid_argument("wp loader get engine version");

	return getValueCstr(value, [&](const char **cvalue) {
		return this->m_pc.fpGetEngineDataVersion(c, cvalue);
	});
}

int WpLoader::getEngineLatestUpdateTime(csre_wp_context_h c, time_t *ptime)
{
	if (c == nullptr || ptime == nullptr)
		throw std::invalid_argument("wp loader get latest update time");

	return this->m_pc.fpGetEngineLatestUpdateTime(c, ptime);
}

int WpLoader::getEngineActivated(csre_wp_context_h c,
								 csre_wp_activated_e *pactivated)
{
	if (c == nullptr || pactivated == nullptr)
		throw std::invalid_argument("wp loader get engine activated");

	return this->m_pc.fpGetEngineActivated(c, pactivated);
}

int WpLoader::getEngineVendorLogo(csre_wp_context_h c,
								  std::vector<unsigned char> &value)
{
	if (c == nullptr)
		throw std::invalid_argument("wp loader get engine vendor logo");

	unsigned char *cvalue = nullptr;
	unsigned int size = 0;
	auto retval = this->m_pc.fpGetEngineVendorLogo(c, &cvalue, &size);

	if (retval == CSRE_ERROR_NONE && cvalue != nullptr && size != 0)
		value.assign(cvalue, cvalue + size);

	return retval;
}

void WpLoader::init(const std::string &enginePath, const std::string &roResDir,
					const std::string &rwWorkingDir)
{
	if (enginePath.empty() || roResDir.empty() || rwWorkingDir.empty())
		ThrowExc(InternalError, "empty string comes in to loader init");

	INFO("Load WP-Engine plugin start. engine path: " << enginePath);

	void *handle = dlopen(enginePath.c_str(), RTLD_LAZY);

	if (handle == nullptr)
		ThrowExc(InternalError, "engine dlopen error. path: " << enginePath <<
				 " errno: " << errno);

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
		ThrowExc(EngineNotExist, "null loader means engine not exist!");

	toException(this->m_loader->contextCreate(this->m_context));
}

WpEngineContext::~WpEngineContext()
{
	toException(this->m_loader->contextDestroy(this->m_context));
}

csre_wp_context_h &WpEngineContext::get(void)
{
	return this->m_context;
}

}
