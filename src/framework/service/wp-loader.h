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
 * @file       wp-loader.h
 * @author     Sangsu Choi (sangsu.choi@samsung.com)
 * @version    1.0
 * @brief
 */
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <ctime>

#include <csre-web-protection-types.h>
#include <csre-web-protection-engine-info.h>

#include "service/iloader.h"

namespace Csr {

class WpLoader : public ILoader {
public:
	WpLoader(const std::string &enginePath, const std::string &roResDir,
			 const std::string &rwWorkingDir);
	virtual ~WpLoader();

	int contextCreate(csre_wp_context_h &);
	int contextDestroy(csre_wp_context_h);
	int checkUrl(csre_wp_context_h handle, const std::string &,
				 csre_wp_check_result_h *);
	int getRiskLevel(csre_wp_check_result_h, csre_wp_risk_level_e *);
	int getDetailedUrl(csre_wp_check_result_h, std::string &);
	int getErrorString(int, std::string &);
	int getEngineApiVersion(csre_wp_context_h, std::string &);
	int getEngineVendor(csre_wp_context_h, std::string &);
	int getEngineName(csre_wp_context_h, std::string &);
	int getEngineVersion(csre_wp_context_h, std::string &);
	int getEngineDataVersion(csre_wp_context_h, std::string &);
	int getEngineLatestUpdateTime(csre_wp_context_h, time_t *);
	int getEngineActivated(csre_wp_context_h, csre_wp_activated_e *);
	int getEngineVendorLogo(csre_wp_context_h, std::vector<unsigned char> &);

private:
	using FpGlobalInit = int(*)(const char *, const char *);
	using FpGlobalDeinit = int(*)();
	using FpContextCreate = int(*)(csre_wp_context_h *);
	using FpContextDestroy = int(*)(csre_wp_context_h);
	using FpCheckUrl = int(*)(csre_wp_context_h , const char *,
							  csre_wp_check_result_h *);
	using FpGetRiskLevel = int(*)(csre_wp_check_result_h, csre_wp_risk_level_e *);
	using FPGetDetailedUrl = int(*)(csre_wp_check_result_h, const char **);
	using FpGetErrorString = int(*)(int, const char **);
	using FpGetEngineApiVersion = int(*)(csre_wp_context_h, const char **);
	using FpGetEngineVendor = int(*)(csre_wp_context_h, const char **);
	using FpGetEngineName = int(*)(csre_wp_context_h, const char **);
	using FpGetEngineVersion = int(*)(csre_wp_context_h, const char **);
	using FpGetEngineDataVersion = int(*)(csre_wp_context_h, const char **);
	using FpGetEngineLatestUpdateTime = int(*)(csre_wp_context_h, time_t *);
	using FpGetEngineActivated = int(*)(csre_wp_context_h, csre_wp_activated_e *);
	using FpGetEngineVendorLogo = int(*)(csre_wp_context_h, unsigned char **,
										 unsigned int *);

	struct PluginContext {
		void *dlhandle;

		FpGlobalInit fpGlobalInit;
		FpGlobalDeinit fpGlobalDeinit;
		FpContextCreate fpContextCreate;
		FpContextDestroy fpContextDestroy;
		FpCheckUrl fpCheckUrl;
		FpGetRiskLevel fpGetRiskLevel;
		FPGetDetailedUrl fpGetDetailedUrl;
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

	void checkEngineActivated(csre_wp_context_h);

	PluginContext m_pc;
};

class WpEngineContext {
public:
	WpEngineContext(const std::shared_ptr<WpLoader> &);
	~WpEngineContext();
	csre_wp_context_h &get(void);

private:
	std::shared_ptr<WpLoader> m_loader;
	csre_wp_context_h m_context;
};

}
