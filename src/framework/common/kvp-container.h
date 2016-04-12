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
 * @file        kvp-container.h
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       Key-value pair container with set/get interface
 */
#pragma once

#include <string>
#include <ctime>

namespace Csr {

// set/get key-value pairs. Key should be defined in each derived classes
class KvpContainer {
public:
	virtual ~KvpContainer();

	virtual void set(int, int);
	virtual void set(int, bool);
	virtual void set(int, const std::string &);
	virtual void set(int, const char *);
	virtual void set(int, time_t);

	virtual void get(int, int &) const;
	virtual void get(int, bool &) const;
	virtual void get(int, std::string &) const;
	virtual void get(int, const char **) const; // For not copy to string lifecycle within itself.
	virtual void get(int, time_t &) const;
};

}
