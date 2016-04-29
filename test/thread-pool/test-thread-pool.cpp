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
 * @file        test-thread-pool.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       CSR threadpool test
 */
#include "service/thread-pool.h"

#include <iostream>
#include <string>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <ctime>
#include <boost/test/unit_test.hpp>

using namespace std::chrono;

namespace {

auto exceptionGuard = [&](std::function<void()> && func)
{
	try {
		func();
	} catch (...) {
		BOOST_REQUIRE_MESSAGE(0, "exception shouldn't be thronw.");
	}
};

const static long long int TaskSleepUnit = 200; // millisec
const static long long int PoolLogicUnit = 10; // millisec
high_resolution_clock::time_point _start;
long long int _expected;
std::mutex _m;

// times in milliseconds unit
inline void START_TIME(void)
{
	_expected = 0;
	_start = high_resolution_clock::now();
}

inline long long int END_TIME(void)
{
	return duration_cast<milliseconds>(high_resolution_clock::now() -
									   _start).count();
}

inline void INC_EXPECTED_TIME(long long int t)
{
	std::lock_guard<std::mutex> l(_m);
	_expected += t;
}

inline void CHECK_TIME(void)
{
	std::lock_guard<std::mutex> l(_m);

	BOOST_MESSAGE("Elapsed time[" << END_TIME() << "]. "
				  "Expected scope: (" << _expected << ", " << _expected + PoolLogicUnit << ")");

	BOOST_REQUIRE_MESSAGE(END_TIME() < _expected + PoolLogicUnit,
						  "Too much time elapsed");
	BOOST_REQUIRE_MESSAGE(END_TIME() >= _expected, "Too less time elapsed");
}

void runStaticPool(size_t cnt)
{
	START_TIME();

	exceptionGuard([&]() {
		INC_EXPECTED_TIME(TaskSleepUnit);
		auto task = [&]() {
			std::this_thread::sleep_for(milliseconds(TaskSleepUnit));
		};

		Csr::ThreadPool pool(cnt, cnt);

		BOOST_REQUIRE_MESSAGE(pool.size() == cnt,
							  "Thread pool isn't initialized well. "
							  "Pool size[" << pool.size() << "] and correct size[" << cnt << "]");

		for (size_t i = 0; i < cnt; i++)
			pool.submit(task);

		/* thread joins in thread-pool dtor */
	});

	CHECK_TIME();
}

void runDynamicPool(size_t min, size_t max)
{
	BOOST_REQUIRE(min < max);

	START_TIME();

	exceptionGuard([&]() {
		INC_EXPECTED_TIME(TaskSleepUnit);
		auto task = [&]() {
			std::this_thread::sleep_for(milliseconds(TaskSleepUnit));
		};

		Csr::ThreadPool pool(min, max);

		BOOST_REQUIRE_MESSAGE(pool.size() == min,
							  "Thread pool isn't initialized well. "
							  "Pool size[" << pool.size() << "] and corret min size[" << min << "]");

		// Task assigned to already existing workers
		for (size_t i = 0; i < min; i++)
			pool.submit(task);

		// Task assigned new worker which is dynamically added to pool
		for (size_t i = min; i < max; i++)
			pool.submit(task);

		// wait for expected time to tasks done
		// additional time for thread pool logic running time
		INC_EXPECTED_TIME(PoolLogicUnit);
		std::this_thread::sleep_for(milliseconds(TaskSleepUnit + PoolLogicUnit));
		BOOST_REQUIRE_MESSAGE(pool.size() == min,
							  "To dtor idle threads in pool doesn't work well. "
							  "Pool size[" << pool.size() << "] shouldn't exceed given min[" << min << "]");

		// make all(maximum) workers busy
		INC_EXPECTED_TIME(TaskSleepUnit);

		for (size_t i = 0; i < max; i++)
			pool.submit(task);

		INC_EXPECTED_TIME(TaskSleepUnit);
		pool.submit(task); // One more task than maximum workers at the time
		BOOST_REQUIRE_MESSAGE(pool.size() == max,
							  "Upper bound to make thread dynamically to pool doesn't work well. "
							  "Pool size[" << pool.size() << "] shouldn't exceed given max[" << max << "]");
	});

	CHECK_TIME();
}

}

BOOST_AUTO_TEST_SUITE(THREADPOOL)

BOOST_AUTO_TEST_CASE(fixed)
{
	for (size_t i = 1; i <= 13; i += 2) {
		BOOST_MESSAGE("Fixed ThreadPool size: " << i);
		runStaticPool(i);
	}
}

BOOST_AUTO_TEST_CASE(dynamic)
{
	for (size_t min = 1; min <= 7; min += 2) {
		for (size_t max = min + 2; max <= 13; max += 2) {
			BOOST_MESSAGE("Dynamic ThreadPool size: " << min << " ~ " << max);
			runDynamicPool(min, max);
		}
	}
}

BOOST_AUTO_TEST_SUITE_END()
