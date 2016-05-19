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
 * @file        canonicalize.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Canonicalize file and directory paths
 */
#pragma once

#include <string>
#include <memory>

#include "common/types.h"

namespace Csr {
namespace Client {

// based on linux function: realpath
std::string getAbsolutePath(const std::string &path);

// input directory set should contains resolved path only
void canonicalizeDirSet(StrSet &dirset);

} // namespace Client
} // namespace Csr