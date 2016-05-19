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
 * @file        iresult.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Abstract class for handle all result resources by a single interface
 */
#pragma once

#include "common/serialization.h"

namespace Csr {

class IResult;
using ResultPtr = std::unique_ptr<IResult>;
using ResultList = std::vector<ResultPtr>;
using ResultListPtr = std::unique_ptr<ResultList>;

class IResult : public ISerializable {
public:
	virtual ~IResult() {}
};

}