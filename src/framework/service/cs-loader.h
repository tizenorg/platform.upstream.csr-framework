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
 * @file       cs-loader.h
 * @author     Sangsu Choi (sangsu.choi@samsung.com)
 * @version    1.0
 * @brief
 */
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <ctime>

#include <csre-content-screening-types.h>
#include <csre-content-screening-engine-info.h>

#include "service/iloader.h"

namespace Csr {

class CsLoader : public ILoader {
public:
	CsLoader(const std::string &enginePath, const std::string &roResDir,
			 const std::string &rwWorkingDir);
	virtual ~CsLoader();

	int contextCreate(csre_cs_context_h &);
	int contextDestroy(csre_cs_context_h);
	int scanData(csre_cs_context_h, const std::vector<unsigned char> &,
				 csre_cs_detected_h *);
	int scanFile(csre_cs_context_h, const std::string &, csre_cs_detected_h *);
	int scanAppOnCloud(csre_cs_context_h, const std::string &, csre_cs_detected_h *);

	int getSeverity(csre_cs_detected_h, csre_cs_severity_level_e *);
	int getMalwareName(csre_cs_detected_h, std::string &);
	int getDetailedUrl(csre_cs_detected_h, std::string &);

	int getErrorString(int, std::string &);

	int getEngineApiVersion(csre_cs_context_h, std::string &);
	int getEngineVendor(csre_cs_context_h, std::string &);
	int getEngineName(csre_cs_context_h, std::string &);
	int getEngineVersion(csre_cs_context_h, std::string &);
	int getEngineDataVersion(csre_cs_context_h, std::string &);
	int getEngineLatestUpdateTime(csre_cs_context_h, time_t *);
	int getEngineActivated(csre_cs_context_h, csre_cs_activated_e *);
	int getEngineVendorLogo(csre_cs_context_h, std::vector<unsigned char> &);

private:
	using FpGlobalInit = int(*)(const char *, const char *);
	using FpGlobalDeinit = int(*)();
	using FpContextCreate = int(*)(csre_cs_context_h *);
	using FpContextDestroy = int(*)(csre_cs_context_h);
	using FpScanData = int(*)(csre_cs_context_h, const unsigned char *, size_t,
							  csre_cs_detected_h *);
	using FpScanFile = int(*)(csre_cs_context_h, const char *,
							  csre_cs_detected_h *);
	using FpScanAppOnCloud = int(*)(csre_cs_context_h, const char *, csre_cs_detected_h *);
	using FpGetSeverity = int(*)(csre_cs_detected_h, csre_cs_severity_level_e *);
	using FpGetMalwareName = int(*)(csre_cs_detected_h, const char **);
	using FpGetDetailedUrl = int(*)(csre_cs_detected_h, const char **);
	using FpGetErrorString = int(*)(int, const char **);
	using FpGetEngineApiVersion = int(*)(csre_cs_context_h, const char **);
	using FpGetEngineVendor = int(*)(csre_cs_context_h, const char **);
	using FpGetEngineName = int(*)(csre_cs_context_h, const char **);
	using FpGetEngineVersion = int(*)(csre_cs_context_h, const char **);
	using FpGetEngineDataVersion = int(*)(csre_cs_context_h, const char **);
	using FpGetEngineLatestUpdateTime = int(*)(csre_cs_context_h, time_t *);
	using FpGetEngineActivated = int(*)(csre_cs_context_h, csre_cs_activated_e *);
	using FpGetEngineVendorLogo = int(*)(csre_cs_context_h, unsigned char **,
										 unsigned int *);

	struct PluginContext {
		void *dlhandle;

		FpGlobalInit fpGlobalInit;
		FpGlobalDeinit fpGlobalDeinit;
		FpContextCreate fpContextCreate;
		FpContextDestroy fpContextDestroy;
		FpScanData fpScanData;
		FpScanFile fpScanFile;
		FpScanAppOnCloud fpScanAppOnCloud;
		FpGetSeverity fpGetSeverity;
		FpGetMalwareName fpGetMalwareName;
		FpGetDetailedUrl fpGetDetailedUrl;
		FpGetErrorString fpGetErrorString;
		FpGetEngineApiVersion fpGetEngineApiVersion;
		FpGetEngineVendor fpGetEngineVendor;
		FpGetEngineName fpGetEngineName;
		FpGetEngineVersion fpGetEngineVersion;
		FpGetEngineDataVersion fpGetEngineDataVersion;
		FpGetEngineLatestUpdateTime fpGetEngineLatestUpdateTime;
		FpGetEngineActivated fpGetEngineActivated;
		FpGetEngineVendorLogo fpGetEngineVendorLogo;
	};

	virtual void init(const std::string &, const std::string &, const std::string &) override;
	virtual void deinit(void) override;

	PluginContext m_pc;
};

class CsEngineContext {
public:
	CsEngineContext(const std::shared_ptr<CsLoader> &);
	~CsEngineContext();
	csre_cs_context_h &get(void);

private:
	std::shared_ptr<CsLoader> m_loader;
	csre_cs_context_h m_context;
};

}
