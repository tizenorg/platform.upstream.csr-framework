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
 * @file        common.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Common data structs for popup-service and csr-server
 */
#include "ui/common.h"

namespace Csr {
namespace Ui {

FileItem::FileItem(IStream &stream)
{
	int int_severity;
	Deserializer<int>::Deserialize(stream, int_severity);
	severity = static_cast<csr_cs_severity_level_e>(int_severity);

	int int_threat;
	Deserializer<int>::Deserialize(stream, int_threat);
	threat = static_cast<csr_cs_threat_type_e>(int_threat);

	Deserializer<std::string>::Deserialize(stream, filepath);
}

void FileItem::Serialize(IStream &stream) const
{
	Serializer<int>::Serialize(stream, static_cast<int>(severity));
	Serializer<int>::Serialize(stream, static_cast<int>(threat));
	Serializer<std::string>::Serialize(stream, filepath);
}

UrlItem::UrlItem(IStream &stream)
{
	int int_risk;
	Deserializer<int>::Deserialize(stream, int_risk);
	risk = static_cast<csr_wp_risk_level_e>(int_risk);

	Deserializer<std::string>::Deserialize(stream, url);
}

void UrlItem::Serialize(IStream &stream) const
{
	Serializer<int>::Serialize(stream, static_cast<int>(risk));
	Serializer<std::string>::Serialize(stream, url);
}

}
}
