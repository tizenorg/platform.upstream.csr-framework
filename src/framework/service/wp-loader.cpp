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

int WpLoader::globalInit(const std::string &ro_res_dir,
						 const std::string &rw_working_dir)
{
	if (ro_res_dir.empty() || rw_working_dir.empty())
		throw std::invalid_argument("wp loader global init");

	return m_pc.fpGlobalInit(ro_res_dir.c_str(), rw_working_dir.c_str());
}

int WpLoader::globalDeinit()
{
	return m_pc.fpGlobalDeinit();
}

int WpLoader::contextCreate(csre_wp_context_h &c)
{
	return m_pc.fpContextCreate(&c);
}

int WpLoader::contextDestroy(csre_wp_context_h c)
{
	if (c == nullptr)
		throw std::invalid_argument("wp loader context destroy");

	return m_pc.fpContextDestroy(c);
}

int WpLoader::checkUrl(csre_wp_context_h c, const std::string &url,
					   csre_wp_check_result_h *presult)
{
	if (c == nullptr || url.empty() || presult == nullptr)
		throw std::invalid_argument("wp loader check url error");

	return m_pc.fpCheckUrl(c, (const char *)url.c_str(), presult);
}

int WpLoader::getRiskLevel(csre_wp_check_result_h r,
						   csre_wp_risk_level_e *plevel)
{
	if (r == nullptr || plevel == nullptr)
		throw std::invalid_argument("wp loader Get Risk Level error");

	return m_pc.fpGetRiskLevel(r, plevel);
}

int WpLoader::getDetailedUrl(csre_wp_check_result_h r, std::string &value)
{
	if (r == nullptr)
		throw std::invalid_argument("wp loader get detailed url error");

	return getValueCstr(value, [&](const char **cvalue) {
		return m_pc.fpGetDetailedUrl(r, cvalue);
	});
}


int WpLoader::getErrorString(int ec, std::string &value)
{
	return getValueCstr(value, [&](const char **cvalue) {
		return m_pc.fpGetErrorString(ec, cvalue);
	});
}


int WpLoader::getEngineInfo(csre_wp_engine_h &e)
{
	return m_pc.fpGetEngineInfo(&e);
}

int WpLoader::destroyEngine(csre_wp_engine_h e)
{
	return m_pc.fpDestroyEngine(e);
}

int WpLoader::getEngineApiVersion(csre_wp_engine_h e, std::string &value)
{
	if (e == nullptr)
		throw std::invalid_argument("wp loader get error string");

	return getValueCstr(value, [&](const char **cvalue) {
		return m_pc.fpGetEngineApiVersion(e, cvalue);
	});
}

int WpLoader::getEngineVendor(csre_wp_engine_h e, std::string &value)
{
	if (e == nullptr)
		throw std::invalid_argument("wp loader get engine vendor");

	return getValueCstr(value, [&](const char **cvalue) {
		return m_pc.fpGetEngineVendor(e, cvalue);
	});
}

int WpLoader::getEngineName(csre_wp_engine_h e, std::string &value)
{
	if (e == nullptr)
		throw std::invalid_argument("wp loader get engine name");

	return getValueCstr(value, [&](const char **cvalue) {
		return m_pc.fpGetEngineName(e, cvalue);
	});
}

int WpLoader::getEngineVersion(csre_wp_engine_h e, std::string &value)
{
	if (e == nullptr)
		throw std::invalid_argument("wp loader get engine version");

	return getValueCstr(value, [&](const char **cvalue) {
		return m_pc.fpGetEngineName(e, cvalue);
	});
}

int WpLoader::getEngineDataVersion(csre_wp_engine_h e, std::string &value)
{
	if (e == nullptr)
		throw std::invalid_argument("wp loader get engine version");

	return getValueCstr(value, [&](const char **cvalue) {
		return m_pc.fpGetEngineDataVersion(e, cvalue);
	});
}

int WpLoader::getEngineLatestUpdateTime(csre_wp_engine_h e, time_t *ptime)
{
	if (e == nullptr || ptime == nullptr)
		throw std::invalid_argument("wp loader get latest update time");

	return m_pc.fpGetEngineLatestUpdateTime(e, ptime);
}

int WpLoader::getEngineActivated(csre_wp_engine_h e,
								 csre_wp_activated_e *pactivated)
{
	if (e == nullptr || pactivated == nullptr)
		throw std::invalid_argument("wp loader get engine activated");

	return m_pc.fpGetEngineActivated(e, pactivated);
}

int WpLoader::getEngineVendorLogo(csre_wp_engine_h e,
								  std::vector<unsigned char> &value)
{
	if (e == nullptr)
		throw std::invalid_argument("wp loader get engine vendor logo");

	unsigned char *cvalue = nullptr;
	unsigned int size = 0;
	auto retval = m_pc.fpGetEngineVendorLogo(e, &cvalue, &size);

	if (retval == CSRE_ERROR_NONE && cvalue != nullptr && size != 0)
		value.assign(cvalue, cvalue + size);

	return retval;
}

WpLoader::WpLoader(const std::string &enginePath)
{
	INFO("Load wp-Engine plugin start. engine path: " << enginePath);

	void *handle = dlopen(enginePath.c_str(), RTLD_LAZY);

	if (handle == nullptr)
		throw std::logic_error(FORMAT("engine dlopen error. path: " << enginePath <<
									  " errno: " << errno));

	m_pc.dlhandle = handle;

	m_pc.fpGlobalInit = reinterpret_cast<FpGlobalInit>(dlsym(handle,
						"csre_wp_global_initialize"));
	m_pc.fpGlobalDeinit = reinterpret_cast<FpGlobalDeinit>(dlsym(handle,
						  "csre_wp_global_deinitialize"));
	m_pc.fpContextCreate = reinterpret_cast<FpContextCreate>(dlsym(handle,
						   "csre_wp_context_create"));
	m_pc.fpContextDestroy = reinterpret_cast<FpContextDestroy>(dlsym(handle,
							"csre_wp_context_destroy"));
	m_pc.fpCheckUrl = reinterpret_cast<FpCheckUrl>(dlsym(handle,
					  "csre_wp_check_url"));
	m_pc.fpGetRiskLevel = reinterpret_cast<FpGetRiskLevel>(dlsym(handle,
						  "csre_wp_result_get_risk_level"));
	m_pc.fpGetDetailedUrl = reinterpret_cast<FPGetDetailedUrl>(dlsym(handle,
							"csre_wp_result_get_detailed_url"));
	m_pc.fpGetErrorString = reinterpret_cast <FpGetErrorString>(dlsym(handle,
							"csre_wp_get_error_string"));
	m_pc.fpGetEngineInfo = reinterpret_cast<FpGetEngineInfo>(dlsym(handle,
						   "csre_wp_engine_get_info"));
	m_pc.fpDestroyEngine = reinterpret_cast<FpDestroyEngine>(dlsym(handle,
						   "csre_wp_engine_destroy"));
	m_pc.fpGetEngineApiVersion = reinterpret_cast<FpGetEngineApiVersion>(dlsym(
									 handle, "csre_wp_engine_get_api_version"));
	m_pc.fpGetEngineVendor = reinterpret_cast<FpGetEngineVendor>(dlsym(handle,
							 "csre_wp_engine_get_vendor"));
	m_pc.fpGetEngineName = reinterpret_cast<FpGetEngineName>(dlsym(handle,
						   "csre_wp_engine_get_name"));
	m_pc.fpGetEngineVersion = reinterpret_cast<FpGetEngineVersion>(dlsym(handle,
							  "csre_wp_engine_get_version"));
	m_pc.fpGetEngineDataVersion = reinterpret_cast<FpGetEngineDataVersion>(dlsym(
									  handle, "csre_wp_engine_get_data_version"));
	m_pc.fpGetEngineLatestUpdateTime =
		reinterpret_cast<FpGetEngineLatestUpdateTime>(dlsym(handle,
				"csre_wp_engine_get_latest_update_time"));
	m_pc.fpGetEngineActivated = reinterpret_cast<FpGetEngineActivated>(dlsym(handle,
								"csre_wp_engine_get_activated"));
	m_pc.fpGetEngineVendorLogo = reinterpret_cast<FpGetEngineVendorLogo>(dlsym(
									 handle, "csre_wp_engine_get_vendor_logo"));

	if (m_pc.fpGlobalInit == nullptr || m_pc.fpGlobalDeinit == nullptr ||
			m_pc.fpContextCreate == nullptr || m_pc.fpContextDestroy == nullptr ||
			m_pc.fpCheckUrl == nullptr || m_pc.fpGetRiskLevel == nullptr ||
			m_pc.fpGetDetailedUrl == nullptr || m_pc.fpGetErrorString == nullptr ||
			m_pc.fpGetEngineInfo == nullptr || m_pc.fpDestroyEngine == nullptr ||
			m_pc.fpGetEngineApiVersion == nullptr || m_pc.fpGetEngineVendor == nullptr ||
			m_pc.fpGetEngineName == nullptr || m_pc.fpGetEngineVersion == nullptr ||
			m_pc.fpGetEngineDataVersion == nullptr ||
			m_pc.fpGetEngineLatestUpdateTime == nullptr ||
			m_pc.fpGetEngineActivated == nullptr || m_pc.fpGetEngineVendorLogo == nullptr) {
		dlclose(handle);
		throw std::runtime_error(FORMAT("Failed to load funcs from engine library. "
										"engine path: " << enginePath << "errno: " <<
										errno));
	}
}

WpLoader::~WpLoader()
{
	dlclose(m_pc.dlhandle);
}

WpEngineContext::WpEngineContext(std::shared_ptr<WpLoader> &loader) :
	m_loader(loader), m_context(nullptr)
{
	if (m_loader == nullptr)
		throw std::logic_error("invalid argument. shouldn't be null.");

	auto ret = m_loader->contextCreate(m_context);

	if (ret != CSRE_ERROR_NONE)
		throw std::runtime_error(FORMAT("get engine context by loader. ret: " << ret));
}

WpEngineContext::~WpEngineContext()
{
	auto ret = m_loader->contextDestroy(m_context);

	if (ret != CSRE_ERROR_NONE)
		throw std::runtime_error(FORMAT("destroy engine context by loader. ret: " <<
										ret));
}

csre_wp_context_h &WpEngineContext::get(void)
{
	return m_context;
}

WpEngineInfo::WpEngineInfo(std::shared_ptr<WpLoader> &loader) :
	m_loader(loader), m_info(nullptr)
{
	if (m_loader == nullptr)
		throw std::logic_error("invalid argument. shouldn't be null.");

	auto ret = m_loader->getEngineInfo(m_info);

	if (ret != CSRE_ERROR_NONE)
		throw std::runtime_error(FORMAT("get engine info by loader. ret: " << ret));
}

WpEngineInfo::~WpEngineInfo()
{
	auto ret = m_loader->destroyEngine(m_info);

	if (ret != CSRE_ERROR_NONE)
		throw std::runtime_error(FORMAT("destroy engine info by loader. ret: " << ret));
}

csre_wp_engine_h &WpEngineInfo::get(void)
{
	return m_info;
}

}
