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
 * @file
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief
 */
#include "csre/web-protection.h"
#include "csre/web-protection-engine-info.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>

#define API __attribute__((visibility("default")))

#define PRIVATE_DB_NAME   "csret_wp_risky_urls"
#define PRIVATE_LOGO_FILE "vendor_logo.bmp"
#define MAX_FILE_PATH_LEN 256
#define MAX_NAME_LEN      64
#define MAX_VERSION_LEN   32
#define MAX_URL_LEN       256

#define VENDOR_NAME       "TEST_VENDOR"
#define ENGINE_NAME       "TEST_LOCAL_TWP_ENGINE"
#define ENGINE_VERSION    "0.0.1"

typedef struct __csret_wp_risky_url {
	char url[MAX_URL_LEN];
	csre_wp_risk_level_e risk_level;
} csret_wp_risky_url_s;

typedef struct __csret_wp_risky_url_list {
	struct __csret_wp_risky_url_list   *next;
	csret_wp_risky_url_s               *risky_url;
} csret_wp_risky_url_list_s;

typedef struct __csret_wp_context {
	csret_wp_risky_url_list_s *detected_list;
} csret_wp_context_s;

typedef struct __csret_wp_engine {
	char           vendor_name[MAX_NAME_LEN];
	char           engine_name[MAX_NAME_LEN];
	unsigned char *vendor_logo_image;
	unsigned int   image_size;
	char           engine_version[MAX_VERSION_LEN];
	char           data_version[MAX_VERSION_LEN];
} csret_wp_engine_s;

typedef enum __csret_wp_internal_error {
	CSRET_WP_ERROR_NO_RISKY_URL_FILE    = -0x0101,
	CSRET_WP_ERROR_FILE_IO              = -0x0103
} csret_wp_internal_error_e;

//==============================================================================
// static variables
//==============================================================================
static csret_wp_engine_s         *engine_info = nullptr;
static csret_wp_risky_url_list_s *risky_urls  = nullptr;

//==============================================================================
// Utilities functions
//==============================================================================
char *csret_wp_extract_value(char *line, const char *key)
{
	if (line == nullptr || key == nullptr)
		return nullptr;

	auto found = strstr(line, key);
	if (found == nullptr)
		return nullptr;

	auto value = found + strlen(key);

	// remove end line char
	for (auto current = found; current && *current; current++) {
		if (*current == '\n') {
			*current = '\0';
			break;
		}
	}

	return value;
}

int csret_wp_read_risky_urls(const char *path)
{
	// csret_wp_risky_urls file format
	// data_version=1.0.0 // it should be in the first line.
	// # this line is a comment.
	//
	// url=highrisky.test.com // this starts a description of a new risky url
	// risk_level=HIGH  // LOW/MEDIUM/HIGH
	//
	// url=midiumrisky.test.com
	// risk_level=MEDIUM
	FILE *fp;
	char *line = nullptr;
	size_t len = 0;
	ssize_t read;
	char *value;
	csret_wp_risky_url_list_s *curr_url = nullptr;
	csret_wp_risky_url_list_s *next_url = nullptr;
	fp = fopen(path, "r");

	if (fp == nullptr)
		return CSRET_WP_ERROR_NO_RISKY_URL_FILE;

	while ((read = getline(&line, &len, fp)) != -1) {
		if (line == nullptr || strlen(line) == 0)
			continue;

		value = csret_wp_extract_value(line, "data_version=");

		if (value != nullptr && engine_info != nullptr)
			snprintf(engine_info->data_version, MAX_VERSION_LEN, "%s", value);

		value = csret_wp_extract_value(line, "url=");

		if (value != nullptr) {
			next_url = (csret_wp_risky_url_list_s *) calloc(sizeof(csret_wp_risky_url_list_s), 1);
			next_url->risky_url = (csret_wp_risky_url_s *) calloc(sizeof(csret_wp_risky_url_s), 1);

			if (curr_url != nullptr) curr_url->next = next_url;
			else                 risky_urls = next_url;

			curr_url = next_url;
			snprintf(curr_url->risky_url->url, MAX_URL_LEN, "%s", value);
		}

		value = csret_wp_extract_value(line, "risk_level=");

		if (value != nullptr) {
			if (strcmp(value, "LOW") == 0)
				curr_url->risky_url->risk_level = CSRE_WP_RISK_LOW;
			else if (strcmp(value, "MEDIUM") == 0)
				curr_url->risky_url->risk_level = CSRE_WP_RISK_MEDIUM;
			else if (strcmp(value, "HIGH") == 0)
				curr_url->risky_url->risk_level = CSRE_WP_RISK_HIGH;
			else
				curr_url->risky_url->risk_level = CSRE_WP_RISK_UNVERIFIED;
		}
	}

	free(line);
	fclose(fp);
	return CSRE_ERROR_NONE;
}


int csret_wp_csret_wp_read_binary_by_file(FILE *file, unsigned char **data, unsigned int *len)
{
	unsigned char *buffer;
	long int fileLen;
	int read;
	int index = 0;

	if (!file)
		return CSRET_WP_ERROR_FILE_IO;

	//Get file length
	fseek(file, 0, SEEK_END);
	fileLen = ftell(file);

	if (fileLen <= 0)
		return CSRET_WP_ERROR_FILE_IO;

	fseek(file, 0, SEEK_SET);
	//Allocate memory
	buffer = (unsigned char *)calloc(fileLen + 1, 1);

	if (!buffer)
		return CSRE_ERROR_OUT_OF_MEMORY;

	//Read file contents into buffer
	while ((read = fread(buffer + index, 1, fileLen, file)) > 0)
		index += read;

	fclose(file);

	if (index != fileLen) {
		free(buffer);
		return CSRET_WP_ERROR_FILE_IO;
	}

	*data = buffer;
	*len = fileLen;
	return CSRE_ERROR_NONE;
}

int csret_wp_read_binary(const char *path, unsigned char **data, unsigned int *len)
{
	FILE *file = fopen(path, "rb");
	return csret_wp_csret_wp_read_binary_by_file(file, data, len);
}


int csret_wp_init_engine(const char *root_dir)
{
	int ret = CSRE_ERROR_NONE;
	char logo_file_name[MAX_FILE_PATH_LEN] = {0, };
	engine_info = (csret_wp_engine_s *) calloc(sizeof(csret_wp_engine_s), 1);

	if (engine_info == nullptr)
		return CSRE_ERROR_OUT_OF_MEMORY;

	snprintf(engine_info->vendor_name, MAX_NAME_LEN, "%s", VENDOR_NAME);
	snprintf(engine_info->engine_name, MAX_NAME_LEN, "%s", ENGINE_NAME);
	snprintf(engine_info->engine_version, MAX_VERSION_LEN, "%s", ENGINE_VERSION);
	snprintf(logo_file_name, MAX_FILE_PATH_LEN, "%s/%s", root_dir, PRIVATE_LOGO_FILE);
	ret = csret_wp_read_binary(logo_file_name, &(engine_info->vendor_logo_image),
							&(engine_info->image_size));

	if (ret == CSRET_WP_ERROR_FILE_IO) {
		engine_info->vendor_logo_image = nullptr;
		engine_info->image_size = 0;
		ret = CSRE_ERROR_NONE;
	}

	return ret;
}


//==============================================================================
// Main function related
//==============================================================================
API
int csre_wp_context_create(const char *engine_root_dir, csre_wp_context_h *phandle)
{
	int ret = CSRE_ERROR_NONE;
	char url_file[MAX_FILE_PATH_LEN] = {0, };

	if (phandle == nullptr || engine_root_dir == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	csret_wp_context_s *context = (csret_wp_context_s *)calloc(sizeof(csret_wp_context_s), 1);

	if (context == nullptr)
		return CSRE_ERROR_OUT_OF_MEMORY;

	if (engine_info == nullptr) {
		ret = csret_wp_init_engine(engine_root_dir);

		if (ret != CSRE_ERROR_NONE)
			return ret;
	}

	if (risky_urls == nullptr) {
		snprintf(url_file, MAX_FILE_PATH_LEN, "%s/%s", engine_root_dir, PRIVATE_DB_NAME);
		ret = csret_wp_read_risky_urls(url_file);

		if (ret != CSRE_ERROR_NONE && ret != CSRET_WP_ERROR_NO_RISKY_URL_FILE)
			return ret;
	}

	*phandle = (csre_wp_context_h) context;
	return CSRE_ERROR_NONE;
}

API
int csre_wp_context_destroy(csre_wp_context_h handle)
{
	if (handle == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	csret_wp_context_s *context = (csret_wp_context_s *)handle;
	csret_wp_risky_url_list_s *curr = nullptr;
	csret_wp_risky_url_list_s *prev = nullptr;
	curr = context->detected_list;

	while (curr != nullptr) {
		if (curr->risky_url != nullptr)
			free(curr->risky_url);

		prev = curr;
		curr = curr->next;
		free(prev);
	}

	free(context);
	return CSRE_ERROR_NONE;
}

API
int csre_wp_check_url(csre_wp_context_h handle, const char *url, csre_wp_check_result_h *presult)
{
	int ret = CSRE_ERROR_NONE;
	char *risky_url = nullptr;
	csret_wp_risky_url_s *detected = nullptr;

	if (handle == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (url == nullptr || presult == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	if (risky_urls == nullptr)
		return CSRE_ERROR_ENGINE_NOT_ACTIVATED;

	csret_wp_context_s *context = (csret_wp_context_s *)handle;
	// create new detected
	detected = (csret_wp_risky_url_s *)calloc(sizeof(csret_wp_risky_url_s), 1);

	if (detected == nullptr)
		return CSRE_ERROR_OUT_OF_MEMORY;

	snprintf(detected->url, MAX_URL_LEN, "%s", url);
	// set detected into context
	csret_wp_risky_url_list_s *last = (csret_wp_risky_url_list_s *) calloc(sizeof(csret_wp_risky_url_list_s), 1);

	if (last == nullptr) {
		free(detected);
		return CSRE_ERROR_OUT_OF_MEMORY;
	}

	last->risky_url = detected;
	csret_wp_risky_url_list_s *curr = context->detected_list;

	while (curr != nullptr && curr->next != nullptr) curr = curr->next; // move to the last

	curr = last;
	// check url
	csret_wp_risky_url_list_s *curr_url = risky_urls;

	while (curr_url != nullptr) {
		risky_url = curr_url->risky_url->url;

		if (strstr(url, risky_url) != nullptr) { // found
			detected->risk_level = curr_url->risky_url->risk_level;
			break; // return the first risky url info in test engine
		}

		curr_url = curr_url->next;
	}

	if (detected->risk_level == 0)
		detected->risk_level = CSRE_WP_RISK_UNVERIFIED;

	*presult = (csre_wp_check_result_h) detected;
	return ret;
}


//==============================================================================
// Result related
//==============================================================================
API
int csre_wp_result_get_risk_level(csre_wp_check_result_h result, csre_wp_risk_level_e *plevel)
{
	csret_wp_risky_url_s *detected = nullptr;

	if (result == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (plevel == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	detected = (csret_wp_risky_url_s *) result;
	*plevel = detected->risk_level;
	return CSRE_ERROR_NONE;
}

//==============================================================================
// Engine information related
//==============================================================================
API
int csre_wp_engine_get_info(csre_wp_engine_h *pengine)
{
	if (pengine == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*pengine = (csre_wp_engine_h)engine_info;
	return CSRE_ERROR_NONE;
}

API
int csre_wp_engine_get_vendor(csre_wp_engine_h engine, const char **vendor)
{
	csret_wp_engine_s *eng = (csret_wp_engine_s *) engine;

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (vendor == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*vendor = eng->vendor_name;
	return CSRE_ERROR_NONE;
}

API
int csre_wp_engine_get_name(csre_wp_engine_h engine, const char **name)
{
	csret_wp_engine_s *eng = (csret_wp_engine_s *) engine;

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (name == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*name = eng->engine_name;
	return CSRE_ERROR_NONE;
}

API
int csre_wp_engine_get_vendor_logo(csre_wp_engine_h engine, unsigned char **vendor_logo_image,
								   unsigned int *image_size)
{
	csret_wp_engine_s *eng = (csret_wp_engine_s *) engine;

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (vendor_logo_image == nullptr || image_size == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*vendor_logo_image = eng->vendor_logo_image;
	*image_size = eng->image_size;
	return CSRE_ERROR_NONE;
}

API
int csre_wp_engine_get_version(csre_wp_engine_h engine, const char **version)
{
	csret_wp_engine_s *eng = (csret_wp_engine_s *) engine;

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (version == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*version = eng->engine_version;
	return CSRE_ERROR_NONE;
}

API
int csre_wp_engine_get_data_version(csre_wp_engine_h engine, const char **version)
{
	csret_wp_engine_s *eng = (csret_wp_engine_s *) engine;

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (version == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*version = eng->data_version;
	return CSRE_ERROR_NONE;
}

API
int csre_wp_engine_get_activated(csre_wp_engine_h engine, csre_wp_activated_e *pactivated)
{
	csret_wp_engine_s *eng = (csret_wp_engine_s *) engine;

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (pactivated == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	if (risky_urls == nullptr)
		*pactivated = CSRE_WP_NOT_ACTIVATED;
	else
		*pactivated = CSRE_WP_ACTIVATED;

	return CSRE_ERROR_NONE;
}

API
int csre_wp_engine_get_api_version(csre_wp_engine_h engine, const char **version)
{
	csret_wp_engine_s *eng = (csret_wp_engine_s *) engine;

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (version == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*version = CSRE_WP_API_VERSION;
	return CSRE_ERROR_NONE;
}

//==============================================================================
// Error related
//==============================================================================
API
int csre_wp_get_error_string(int error_code, const char **string)
{
	if (string == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	switch (error_code) {
	case CSRET_WP_ERROR_NO_RISKY_URL_FILE:
		*string = "CSRET_WP_ERROR_NO_RISKY_URL_FILE";
		break;

	case CSRET_WP_ERROR_FILE_IO:
		*string = "CSRET_WP_ERROR_FILE_IO";
		break;

	default:
		*string = "UNDEFINED ERROR CODE";
		break;
	}

	return CSRE_ERROR_NONE;
}
