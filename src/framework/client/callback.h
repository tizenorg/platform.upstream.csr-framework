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
 * @file        callback.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       CSR callback container for async operations
 */
#pragma once

#include <utility>
#include <stdexcept>

#include <csr-content-screening-types.h>

namespace Csr {

struct Callback {
	enum class Id : int {
		OnCompleted,
		OnCancelled,
		OnError,
		OnScanned,
		OnDetected
	};

	Callback() {}

	Callback(const Callback &other) :
		onScanned(other.onScanned),
		onDetected(other.onDetected),
		onCompleted(other.onCompleted),
		onCancelled(other.onCancelled),
		onError(other.onError)
	{
	}

	Callback &operator=(const Callback &other)
	{
		onScanned = other.onScanned;
		onDetected = other.onDetected;
		onCompleted = other.onCompleted;
		onCancelled = other.onCancelled,
		onError = other.onError;
		return *this;
	}

	std::function<void(void *, const char *)> onScanned;
	std::function<void(void *, csr_cs_malware_h)> onDetected;
	std::function<void(void *)> onCompleted;
	std::function<void(void *)> onCancelled;
	std::function<void(void *, int)> onError;
};

}
