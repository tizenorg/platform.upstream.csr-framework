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
 * @file        exception.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       custom exceptions which are thrown only on server side
 */
#pragma once

#include "common/exception.h"

namespace Csr {

// exceptions listed here are only thrown in server side.
using PermDenied         = DerivedException<CSR_ERROR_ENGINE_PERMISSION>;
using DbFailed           = DerivedException<CSR_ERROR_DB>;
using RemoveFailed       = DerivedException<CSR_ERROR_REMOVE_FAILED>;
using FileChanged        = DerivedException<CSR_ERROR_FILE_CHANGED>;
using EngineError        = DerivedException<CSR_ERROR_ENGINE_INTERNAL>;
using EngineNotActivated = DerivedException<CSR_ERROR_ENGINE_NOT_ACTIVATED>;
using EngineDisabled     = DerivedException<CSR_ERROR_ENGINE_DISABLED>;

}
