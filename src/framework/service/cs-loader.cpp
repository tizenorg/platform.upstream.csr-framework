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
 * @file       csl-oader.cpp
 * @author     Sangsu Choi (sangsu.choi@samsung.com)
 * @version    1.0
 * @brief
 */
#include "service/cs-loader.h"

#include <stdexcept>
#include <dlfcn.h>

#include "common/audit/logger.h"

namespace Csr {

namespace {

int getValueCstr(std::string &value,
				 const std::function<int(const char *)> &getfunc)
{
	const char *cvalue = nullptr;
	auto retval = getfunc(cvalue);

	if (retval == CSRE_ERROR_NONE && cvalue != nullptr)
		value = cvalue;

	return retval;
}

} // namespace anonymous

int CsLoader::globalInit(const std::string &ro_res_dir,
						 const std::string &rw_working_dir)
{
	if (ro_res_dir.empty() || rw_working_dir.empty())
		throw std::invalid_argument("cs loader global init");

	return m_pc.fpGlobalInit(ro_res_dir.c_str(), rw_working_dir.c_str());
}

int CsLoader::globalDeinit()
{
	return m_pc.fpGlobalDeinit();
}

int CsLoader::contextCreate(csre_cs_context_h &c)
{
	return m_pc.fpContextCreate(&c);
}

int CsLoader::contextDestroy(csre_cs_context_h c)
{
	if (c == nullptr)
		throw std::invalid_argument("cs loader context destroy");

	return m_pc.fpContextDestroy(c);
}

int CsLoader::scanData(csre_cs_context_h c,
					   const std::vector<unsigned char> &data,
					   csre_cs_detected_h *pdetected)
{
	if (c == nullptr || data.empty() || pdetected == nullptr)
		throw std::invalid_argument("cs loader scan data");

	return m_pc.fpScanData(c, data.data(), data.size(), pdetected);
}

int CsLoader::scanFile(csre_cs_context_h c, const std::string &filepath,
					   csre_cs_detected_h *pdetected)
{
	if (c == nullptr || filepath.empty() || pdetected == nullptr)
		throw std::invalid_argument("cs loader scan file");

	return m_pc.fpScanFile(c, filepath.c_str(), pdetected);
}

int CsLoader::scanAppOnCloud(csre_cs_context_h c, const std::string &appdir,
							 csre_cs_detected_h *pdetected)
{
	if (c == nullptr || appdir.empty() || pdetected == nullptr)
		throw std::invalid_argument("cs loader scan app on cloud");

	return m_pc.fpScanAppOnCloud(c, appdir.c_str(), pdetected);
}

int CsLoader::getSeverity(csre_cs_detected_h d,
						  csre_cs_severity_level_e *pseverity)
{
	if (d == nullptr || pseverity == nullptr)
		throw std::invalid_argument("cs loader get severity");

	return m_pc.fpGetSeverity(d, pseverity);
}

int CsLoader::getThreatType(csre_cs_detected_h d,
							csre_cs_threat_type_e *pthreat)
{
	if (d == nullptr || pthreat == nullptr)
		throw std::invalid_argument("cs loader get threat type");

	return m_pc.fpGetThreatType(d, pthreat);
}

int CsLoader::getMalwareName(csre_cs_detected_h d, std::string &value)
{
	if (d == nullptr)
		throw std::invalid_argument("cs loader get malware name");

	return getValueCstr(value, [&](const char *cvalue) {
		return m_pc.fpGetMalwareName(d, &cvalue);
	});
}

int CsLoader::getDetailedUrl(csre_cs_detected_h d, std::string &value)
{
	if (d == nullptr)
		throw std::invalid_argument("cs loader get detailed url");

	return getValueCstr(value, [&](const char *cvalue) {
		return m_pc.fpGetDetailedUrl(d, &cvalue);
	});
}

int CsLoader::getTimestamp(csre_cs_detected_h d, time_t *timestamp)
{
	if (d == nullptr || timestamp == nullptr)
		throw std::invalid_argument("cs loader get time stamp");

	return m_pc.fpGetTimestamp(d, timestamp);
}

int CsLoader::getErrorString(int ec, std::string &value)
{
	return getValueCstr(value, [&](const char *cvalue) {
		return m_pc.fpGetErrorString(ec, &cvalue);
	});
}

int CsLoader::getEngineInfo(csre_cs_engine_h &e)
{
	return m_pc.fpGetEngineInfo(&e);
}

int CsLoader::destroyEngine(csre_cs_engine_h e)
{
	return m_pc.fpDestroyEngine(e);
}

int CsLoader::getEngineApiVersion(csre_cs_engine_h e, std::string &value)
{
	if (e == nullptr)
		throw std::invalid_argument("cs loader get error string");

	return getValueCstr(value, [&](const char *cvalue) {
		return m_pc.fpGetEngineApiVersion(e, &cvalue);
	});
}

int CsLoader::getEngineVendor(csre_cs_engine_h e, std::string &value)
{
	if (e == nullptr)
		throw std::invalid_argument("cs loader get engine vendor");

	return getValueCstr(value, [&](const char *cvalue) {
		return m_pc.fpGetEngineVendor(e, &cvalue);
	});
}

int CsLoader::getEngineName(csre_cs_engine_h e, std::string &value)
{
	if (e == nullptr)
		throw std::invalid_argument("cs loader get engine name");

	return getValueCstr(value, [&](const char *cvalue) {
		return m_pc.fpGetEngineName(e, &cvalue);
	});
}

int CsLoader::getEngineVersion(csre_cs_engine_h e, std::string &value)
{
	if (e == nullptr)
		throw std::invalid_argument("cs loader get engine version");

	return getValueCstr(value, [&](const char *cvalue) {
		return m_pc.fpGetEngineName(e, &cvalue);
	});
}

int CsLoader::getEngineDataVersion(csre_cs_engine_h e, std::string &value)
{
	if (e == nullptr)
		throw std::invalid_argument("cs loader get engine version");

	return getValueCstr(value, [&](const char *cvalue) {
		return m_pc.fpGetEngineDataVersion(e, &cvalue);
	});
}

int CsLoader::getEngineLatestUpdateTime(csre_cs_engine_h e, time_t *ptime)
{
	if (e == nullptr || ptime == nullptr)
		throw std::invalid_argument("cs loader get latest update time");

	return m_pc.fpGetEngineLatestUpdateTime(e, ptime);
}

int CsLoader::getEngineActivated(csre_cs_engine_h e,
								 csre_cs_activated_e *pactivated)
{
	if (e == nullptr || pactivated == nullptr)
		throw std::invalid_argument("cs loader get engine activated");

	return m_pc.fpGetEngineActivated(e, pactivated);
}

int CsLoader::getEngineVendorLogo(csre_cs_engine_h e,
								  std::vector<unsigned char> &value)
{
	if (e == nullptr)
		throw std::invalid_argument("cs loader get engine vendor logo");

	unsigned char *cvalue = nullptr;
	unsigned int size = 0;
	auto retval = m_pc.fpGetEngineVendorLogo(e, &cvalue, &size);

	if (retval == CSRE_ERROR_NONE && cvalue != nullptr && size != 0)
		value.assign(cvalue, cvalue + size);

	return retval;
}

CsLoader::CsLoader(const std::string &enginePath)
{
	INFO("Load CS-Engine plugin start. engine path: " << enginePath);

	void *handle = dlopen(enginePath.c_str(), RTLD_LAZY);

	if (handle == nullptr)
		throw std::logic_error(FORMAT("engine dlopen error. path: " << enginePath <<
									  " errno: " << errno));

	m_pc.dlhandle = handle;

	m_pc.fpGlobalInit = reinterpret_cast<FpGlobalInit>(dlsym(handle,
						"csre_cs_global_initialize"));
	m_pc.fpGlobalDeinit = reinterpret_cast<FpGlobalDeinit>(dlsym(handle,
						  "csre_cs_global_deinitialize"));
	m_pc.fpContextCreate = reinterpret_cast<FpContextCreate>(dlsym(handle,
						   "csre_cs_context_create"));
	m_pc.fpContextDestroy = reinterpret_cast<FpContextDestroy>(dlsym(handle,
							"csre_cs_context_destroy"));
	m_pc.fpScanData = reinterpret_cast<FpScanData>(dlsym(handle,
					  "csre_cs_scan_data"));
	m_pc.fpScanFile = reinterpret_cast<FpScanFile>(dlsym(handle,
					  "csre_cs_scan_file"));
	m_pc.fpScanAppOnCloud = reinterpret_cast<FpScanAppOnCloud>(dlsym(handle,
							"csre_cs_scan_app_on_cloud"));
	m_pc.fpGetSeverity = reinterpret_cast<FpGetSeverity>(dlsym(handle,
						 "csre_cs_detected_get_severity"));
	m_pc.fpGetThreatType = reinterpret_cast<FpGetThreatType>(dlsym(handle,
						   "csre_cs_detected_get_threat_type"));
	m_pc.fpGetMalwareName = reinterpret_cast<FpGetMalwareName>(dlsym(handle,
							"csre_cs_detected_get_malware_name"));
	m_pc.fpGetDetailedUrl = reinterpret_cast<FpGetDetailedUrl>(dlsym(handle,
							"csre_cs_detected_get_detailed_url"));
	m_pc.fpGetTimestamp = reinterpret_cast<FpGetTimestamp>(dlsym(handle,
						  "csre_cs_detected_get_timestamp"));
	m_pc.fpGetErrorString = reinterpret_cast <FpGetErrorString>(dlsym(handle,
							"csre_cs_get_error_string"));
	m_pc.fpGetEngineInfo = reinterpret_cast<FpGetEngineInfo>(dlsym(handle,
						   "csre_cs_engine_get_info"));
	m_pc.fpDestroyEngine = reinterpret_cast<FpDestroyEngine>(dlsym(handle,
						   "csre_cs_engine_destroy"));
	m_pc.fpGetEngineApiVersion = reinterpret_cast<FpGetEngineApiVersion>(dlsym(
									 handle, "csre_cs_engine_get_api_version"));
	m_pc.fpGetEngineVendor = reinterpret_cast<FpGetEngineVendor>(dlsym(handle,
							 "csre_cs_engine_get_vendor"));
	m_pc.fpGetEngineName = reinterpret_cast<FpGetEngineName>(dlsym(handle,
						   "csre_cs_engine_get_version"));
	m_pc.fpGetEngineVersion = reinterpret_cast<FpGetEngineVersion>(dlsym(handle,
							  "csre_cs_engine_get_data_version"));
	m_pc.fpGetEngineDataVersion = reinterpret_cast<FpGetEngineDataVersion>(dlsym(
									  handle, "csre_cs_engine_get_data_version"));
	m_pc.fpGetEngineLatestUpdateTime =
		reinterpret_cast<FpGetEngineLatestUpdateTime>(dlsym(handle,
				"csre_cs_engine_get_latest_update_time"));
	m_pc.fpGetEngineActivated = reinterpret_cast<FpGetEngineActivated>(dlsym(handle,
								"csre_cs_engine_get_activated"));
	m_pc.fpGetEngineVendorLogo = reinterpret_cast<FpGetEngineVendorLogo>(dlsym(
									 handle, "csre_cs_engine_get_vendor_logo"));

	if (m_pc.fpGlobalInit == nullptr || m_pc.fpGlobalDeinit == nullptr ||
			m_pc.fpContextCreate == nullptr || m_pc.fpContextDestroy == nullptr ||
			m_pc.fpScanData == nullptr || m_pc.fpScanFile == nullptr  ||
			m_pc.fpScanAppOnCloud == nullptr || m_pc.fpGetSeverity == nullptr ||
			m_pc.fpGetThreatType == nullptr || m_pc.fpGetMalwareName == nullptr ||
			m_pc.fpGetDetailedUrl == nullptr || m_pc.fpGetTimestamp == nullptr ||
			m_pc.fpGetErrorString == nullptr || m_pc.fpGetEngineInfo == nullptr ||
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

CsLoader::~CsLoader()
{
	dlclose(m_pc.dlhandle);
}

}
