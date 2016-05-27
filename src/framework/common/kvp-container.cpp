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
 * @file        kvp-container.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Key-value pair container with set/get interface
 */
#include "common/kvp-container.h"

#include "common/exception.h"

namespace Csr {

void KvpContainer::set(int, int)
{
	ThrowExc(InternalError, "Not implemented key-value pair interface used!");
}

void KvpContainer::set(int, bool)
{
	ThrowExc(InternalError, "Not implemented key-value pair interface used!");
}

void KvpContainer::set(int, const std::string &)
{
	ThrowExc(InternalError, "Not implemented key-value pair interface used!");
}

void KvpContainer::set(int, const char *)
{
	ThrowExc(InternalError, "Not implemented key-value pair interface used!");
}

void KvpContainer::set(int, time_t)
{
	ThrowExc(InternalError, "Not implemented key-value pair interface used!");
}

void KvpContainer::get(int, int &) const
{
	ThrowExc(InternalError, "Not implemented key-value pair interface used!");
}

void KvpContainer::get(int, bool &) const
{
	ThrowExc(InternalError, "Not implemented key-value pair interface used!");
}

void KvpContainer::get(int, std::string &) const
{
	ThrowExc(InternalError, "Not implemented key-value pair interface used!");
}

void KvpContainer::get(int, const char **) const
{
	ThrowExc(InternalError, "Not implemented key-value pair interface used!");
}

void KvpContainer::get(int, time_t &) const
{
	ThrowExc(InternalError, "Not implemented key-value pair interface used!");
}

}
