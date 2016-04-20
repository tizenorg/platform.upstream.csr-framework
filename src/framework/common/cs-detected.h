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
 * @file        cs-detected.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Content screening detected result
 */
#pragma once

#include <string>

#include "common/types.h"
#include "csr/content-screening-types.h"

namespace Csr {

class CsDetected;
using CsDetectedPtr = std::unique_ptr<CsDetected>;
using CsDetectedList = std::vector<CsDetectedPtr>;

class CsDetected : public Result {
public:
	// key for set/get
	enum class Key : int {
		TargetName   = 0x01, // string
		MalwareName  = 0x02, // string
		DetailedUrl  = 0x03, // string
		PkgId        = 0x04, // string

		Severity     = 0x10, // int
		Threat       = 0x11, // int
		UserResponse = 0x12, // int

		TimeStamp    = 0x20, // time_t

		IsApp        = 0x30  // bool
	};

	CsDetected();
	virtual ~CsDetected();

	CsDetected(IStream &);
	virtual void Serialize(IStream &) const override;

	CsDetected(CsDetected &&);
	CsDetected &operator=(CsDetected &&);

	virtual void set(int, int) override;
	virtual void set(int, bool) override;
	virtual void set(int, const std::string &) override;
	virtual void set(int, const char *) override;
	virtual void set(int, time_t) override;

	virtual void get(int, int &) const override;
	virtual void get(int, bool &) const override;
	virtual void get(int, const char **) const override;
	virtual void get(int, time_t &) const override;

private:
	std::string m_targetName; // file path or app id which contains malware

	std::string m_malwareName;
	std::string m_detailedUrl;
	std::string m_pkgId;
	csr_cs_severity_level_e m_severity;
	csr_cs_threat_type_e m_threat;
	csr_cs_user_response_e m_response;
	bool m_isApp;
	time_t m_ts;
};

}
