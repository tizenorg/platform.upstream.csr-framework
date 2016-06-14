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
 * @file        wp-logic.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Web protection logic
 */
#include "service/wp-logic.h"

#include <utility>

#include "common/audit/logger.h"
#include "common/exception.h"
#include "service/type-converter.h"
#include "ui/askuser.h"
#include <csr-error.h>

namespace Csr {

WpLogic::WpLogic(const std::shared_ptr<WpLoader> &loader,
				 const std::shared_ptr<Db::Manager> &db) :
	m_loader(loader), m_db(db)
{
	if (!this->m_db)
		ThrowExc(CSR_ERROR_DB, "DB init failed.");

	if (this->m_loader) {
		WpEngineContext wpEngineContext(this->m_loader);
		this->m_loader->getEngineDataVersion(wpEngineContext.get(), this->m_dataVersion);
	}
}

RawBuffer WpLogic::checkUrl(const WpContext &context, const std::string &url)
{
	if (this->m_db->getEngineState(CSR_ENGINE_WP) != CSR_STATE_ENABLE)
		ThrowExc(CSR_ERROR_ENGINE_DISABLED, "engine is disabled");

	WpEngineContext engineContext(this->m_loader);
	auto &c = engineContext.get();

	csre_wp_check_result_h result;
	this->m_loader->checkUrl(c, url.c_str(), &result);

	auto wr = convert(result);

	DEBUG("checking level.. prepare for asking user");

	switch (wr.riskLevel) {
	case CSR_WP_RISK_UNVERIFIED:
		DEBUG("url[" << url << "] risk level is unverified");
		break;

	case CSR_WP_RISK_LOW:
		DEBUG("url[" << url << "] risk level is low");
		break;

	case CSR_WP_RISK_MEDIUM:
		DEBUG("url[" << url << "] risk level is medium. let's ask user to process.");
		wr.response = getUserResponse(context, url, wr);
		break;

	case CSR_WP_RISK_HIGH:
		DEBUG("url[" << url << "] risk level is high. let's notify user to deny url.");
		wr.response = getUserResponse(context, url, wr);
		break;

	default:
		ThrowExc(CSR_ERROR_SERVER, "Invalid level: " << static_cast<int>(wr.riskLevel));
	}

	return BinaryQueue::Serialize(CSR_ERROR_NONE, wr).pop();
}

csr_wp_user_response_e WpLogic::getUserResponse(const WpContext &c, const std::string &url,
		const WpResult &wr)
{
	if (c.askUser == CSR_WP_ASK_USER_NO)
		return CSR_WP_USER_RESPONSE_USER_NOT_ASKED;

	Ui::CommandId cid;

	if (wr.riskLevel == CSR_WP_RISK_MEDIUM)
		cid = Ui::CommandId::WP_PROMPT;
	else
		cid = Ui::CommandId::WP_NOTIFY;

	Ui::UrlItem item;
	item.url = url;
	item.risk = wr.riskLevel;

	Ui::AskUser askUser;
	return askUser.wp(cid, c.popupMessage, item);
}

WpResult WpLogic::convert(csre_wp_check_result_h &r)
{
	DEBUG("convert engine result handle to WpResult start");

	WpResult wr;
	csre_wp_risk_level_e elevel;

	this->m_loader->getDetailedUrl(r, wr.detailedUrl);
	this->m_loader->getRiskLevel(r, &elevel);
	wr.riskLevel = Csr::convert(elevel);

	return wr;
}

}
