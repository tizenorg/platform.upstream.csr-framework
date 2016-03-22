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
#include "csre/content-screening.h"
#include "csre/content-screening-engine-info.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>

#define API __attribute__((visibility("default")))

#ifdef __cplusplus
extern "C" {
#endif

#define PRIVATE_DB_NAME   "csret_cs_virus_signatures"
#define PRIVATE_LOGO_FILE "vendor_logo.bmp"
#define MAX_FILE_PATH_LEN 256
#define MAX_NAME_LEN      64
#define MAX_VERSION_LEN   32
#define MAX_URL_LEN       256
#define MAX_SIG_LEN       256

#define VENDOR_NAME       "TEST_VENDOR"
#define ENGINE_NAME       "TEST_LOCAL_TCS_ENGINE"
#define ENGINE_VERSION    "0.0.1"

typedef enum  __csret_cs_scan_on_cloud {
	TCSE_SCAN_ON_CLOUD_OFF = 0,
	TCSE_SCAN_ON_CLOUD_ON  = 1
} csret_cs_scan_on_cloud_e;

typedef struct __csret_cs_malware {
	csre_cs_severity_level_e severity;
	csre_cs_threat_type_e threat_type;
	char malware_name[MAX_NAME_LEN];
	char detailed_url[MAX_URL_LEN];
	char signature[MAX_SIG_LEN];
} csret_cs_malware_s;

typedef struct __csret_cs_malware_list {
	struct __csret_cs_malware_list *next;
	csret_cs_malware_s malware;
} csret_cs_malware_list_s;

typedef struct __csret_cs_detected {
	csret_cs_malware_s malware;
	long timestamp;
} csret_cs_detected_s;

typedef struct __csret_cs_detected_list {
	struct __csret_cs_detected_list *next;
	csret_cs_detected_s *detected;
} csret_cs_detected_list_s;

typedef struct __csret_cs_context {
	int scan_on_data;
	csret_cs_detected_list_s *detected_list;
} csret_cs_context_s;

typedef struct __csret_cs_engine {
	char vendor_name[MAX_NAME_LEN];
	char engine_name[MAX_NAME_LEN];
	unsigned char *vendor_logo_image;
	unsigned int image_size;
	char engine_version[MAX_VERSION_LEN];
	char data_version[MAX_VERSION_LEN];
} csret_cs_engine_s;

typedef enum __csret_cs_internal_error {
	CSRET_CS_ERROR_NO_SIGNATURE_FILE    = -0x0101,
	CSRET_CS_ERROR_SIGNARUE_FILE_FORMAT = -0x0102,
	CSRET_CS_ERROR_FILE_IO              = -0x0103
} csret_cs_internal_error_e;

//==============================================================================
// static variables
//==============================================================================
static csret_cs_engine_s       *engine_info = nullptr;
static csret_cs_malware_list_s *virus_sig   = nullptr;

//==============================================================================
// Utilities functions
//==============================================================================

char *csret_cs_extract_value(char *line, const char *key)
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

int csret_cs_read_virus_signatures(const char *path)
{
	// virus_signature file format
	// data_version=1.0.0 // it should be in the first line.
	// # this line is a comment.
	//
	// name=test_malware // this starts a description of a new malware
	// severity=HIGH  // LOW/MEDIUM/HIGH
	// threat_type=MALWARE  // MALWARE/RISKY/GENERIC
	// detailed_url=        // It can be null
	// signature=X5O!P%@AP[4\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*
	//
	// name=test_risk
	// severity=MEDIUM
	// threat_type=RISKY
	// detailed_url=
	// signature=RISKY_MALWARE

	csret_cs_malware_list_s *curr_sig = nullptr;

	FILE *fp = fopen(path, "r");
	if (fp == nullptr)
		return CSRET_CS_ERROR_NO_SIGNATURE_FILE;

	char *line = nullptr;
	size_t len = 0;
	ssize_t read;
	while ((read = getline(&line, &len, fp)) != -1) {
		if (line == nullptr || strlen(line) == 0)
			continue;

		auto value = csret_cs_extract_value(line, "data_version=");

		if (value != nullptr && engine_info != nullptr)
			strncpy(engine_info->data_version, value, sizeof(engine_info->data_version) - 1);

		value = csret_cs_extract_value(line, "name=");

		if (value != nullptr) {
			auto next_sig = (csret_cs_malware_list_s *) calloc(sizeof(csret_cs_malware_list_s), 1);

			if (curr_sig != nullptr)
				curr_sig->next = next_sig;
			else
				virus_sig = next_sig;

			curr_sig = next_sig;
			strncpy(curr_sig->malware.malware_name, value, sizeof(curr_sig->malware.malware_name) - 1);
		}

		value = csret_cs_extract_value(line, "severity=");

		if (value != nullptr) {
			if (strcmp(value, "LOW") == 0)
				curr_sig->malware.severity = CSRE_CS_SEVERITY_LOW;
			else if (strcmp(value, "MEDIUM") == 0)
				curr_sig->malware.severity = CSRE_CS_SEVERITY_MEDIUM;
			else
				curr_sig->malware.severity = CSRE_CS_SEVERITY_HIGH;
		}

		value = csret_cs_extract_value(line, "threat_type=");

		if (value != nullptr) {
			if (strcmp(value, "MALWARE") == 0)
				curr_sig->malware.threat_type = CSRE_CS_THREAT_MALWARE;
			else if (strcmp(value, "RISKY") == 0)
				curr_sig->malware.threat_type = CSRE_CS_THREAT_RISKY;
			else
				curr_sig->malware.threat_type = CSRE_CS_THREAT_GENERIC;
		}

		value = csret_cs_extract_value(line, "detailed_url=");

		if (value != nullptr)
			strncpy(curr_sig->malware.detailed_url, value, sizeof(curr_sig->malware.detailed_url) - 1);

		value = csret_cs_extract_value(line, "signature=");

		if (value != nullptr)
			strncpy(curr_sig->malware.signature, value, sizeof(curr_sig->malware.signature) - 1);
	}

	free(line);
	fclose(fp);
	return CSRE_ERROR_NONE;
}


int csret_cs_read_binary_by_file(FILE *file, unsigned char **data, unsigned int *len)
{
	unsigned char *buffer;
	long int fileLen;
	int read;
	int index = 0;

	if (!file)
		return CSRE_ERROR_FILE_NOT_FOUND;

	//Get file length
	fseek(file, 0, SEEK_END);
	fileLen = ftell(file);

	if (fileLen <= 0)
		return CSRET_CS_ERROR_FILE_IO;

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
		return CSRET_CS_ERROR_FILE_IO;
	}

	*data = buffer;
	*len = fileLen;
	return CSRE_ERROR_NONE;
}

int csret_cs_read_binary(const char *path, unsigned char **data, unsigned int *len)
{
	FILE *file = fopen(path, "rb");
	return csret_cs_read_binary_by_file(file, data, len);
}

int csret_cs_read_binary_by_fd(int file_descriptor, unsigned char **data, unsigned int *len)
{
	FILE *file = fdopen(file_descriptor, "rb");
	return csret_cs_read_binary_by_file(file, data, len);
}

long csret_cs_get_timestamp()
{
	unsigned long time_in_micros;
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	time_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
	return time_in_micros;
}

int csret_cs_init_engine(const char *root_dir)
{
	int ret = CSRE_ERROR_NONE;
	char logo_file_name[MAX_FILE_PATH_LEN] = {0, };
	engine_info = (csret_cs_engine_s *) calloc(sizeof(csret_cs_engine_s), 1);

	if (engine_info == nullptr)
		return CSRE_ERROR_OUT_OF_MEMORY;

	snprintf(engine_info->vendor_name, MAX_NAME_LEN, "%s", VENDOR_NAME);
	snprintf(engine_info->engine_name, MAX_NAME_LEN, "%s", ENGINE_NAME);
	snprintf(engine_info->engine_version, MAX_VERSION_LEN, "%s", ENGINE_VERSION);
	snprintf(logo_file_name, MAX_FILE_PATH_LEN, "%s/%s", root_dir, PRIVATE_LOGO_FILE);
	ret = csret_cs_read_binary(logo_file_name, &(engine_info->vendor_logo_image),
							&(engine_info->image_size));

	if (ret == CSRE_ERROR_FILE_NOT_FOUND) {
		engine_info->vendor_logo_image = nullptr;
		engine_info->image_size = 0;
		ret = CSRE_ERROR_NONE;
	}

	return ret;
}

int csret_cs_compare_data(const unsigned char *data, unsigned int data_len,
					   const char *virus_signature, unsigned int sig_len)
{
	unsigned int i, j;

	if (data_len < sig_len)
		return -1;

	for (i = 0; i <= (data_len - sig_len); i++) {
		for (j = 0; j < sig_len; j++) {
			if (data[i + j] == (unsigned char) virus_signature[j])
				continue;
			else
				break;
		}

		if (j == sig_len)
			return 0; // matched
	}

	return -1;
}

int csret_cs_detect_malware(csret_cs_context_s *context, const unsigned char *data, unsigned int length,
						 csret_cs_detected_s **pdetected)
{
	char *virus_signature = nullptr;
	csret_cs_detected_s *detected = nullptr;
	int ret = CSRE_ERROR_NONE;

	if (context == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (data == nullptr || pdetected == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	if (virus_sig == nullptr)
		return CSRE_ERROR_ENGINE_NOT_ACTIVATED;

	// comare data with virus signature
	csret_cs_malware_list_s *curr_sig = virus_sig;

	while (curr_sig != nullptr) {
		virus_signature = curr_sig->malware.signature;

		if (csret_cs_compare_data(data, length, virus_signature, strlen(virus_signature)) == 0) { // detected
			//printf("..csret_cs_detect_malware: detected signature=%s\n", virus_signature);
			// create new detected
			detected = (csret_cs_detected_s *)calloc(sizeof(csret_cs_detected_s), 1);

			if (detected == nullptr)
				return CSRE_ERROR_OUT_OF_MEMORY;

			// set detected into context
			csret_cs_detected_list_s *last = (csret_cs_detected_list_s *) calloc(sizeof(csret_cs_detected_list_s), 1);

			if (last == nullptr) {
				free(detected);
				return CSRE_ERROR_OUT_OF_MEMORY;
			}

			last->detected = detected;
			csret_cs_detected_list_s *curr = context->detected_list;

			while (curr != nullptr && curr->next != nullptr) curr = curr->next; // move to the last

			curr = last;
			// set values into detected
			detected->malware.severity = curr_sig->malware.severity;
			detected->malware.threat_type = curr_sig->malware.threat_type;
			snprintf(detected->malware.malware_name, MAX_NAME_LEN, "%s", curr_sig->malware.malware_name);
			snprintf(detected->malware.detailed_url, MAX_URL_LEN, "%s", curr_sig->malware.detailed_url);
			detected->timestamp = csret_cs_get_timestamp();
			break; // return the first malware in test engine.
		}

		curr_sig = curr_sig->next;
	}

	// set detected into context
	csret_cs_detected_list_s *last = (csret_cs_detected_list_s *) calloc(sizeof(csret_cs_detected_list_s), 1);

	if (last == nullptr)
		return CSRE_ERROR_OUT_OF_MEMORY;

	last->detected = detected;
	csret_cs_detected_list_s *curr = context->detected_list;

	while (curr != nullptr && curr->next != nullptr) curr = curr->next; // move to the last

	curr = last;
	*pdetected = detected;

	return ret;
}

//==============================================================================
// Main function related
//==============================================================================

API
int csre_cs_context_create(const char *engine_root_dir, csre_cs_context_h *phandle)
{
	int ret = CSRE_ERROR_NONE;
	char sig_file[MAX_FILE_PATH_LEN] = {0, };

	if (phandle == nullptr || engine_root_dir == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	csret_cs_context_s *context = (csret_cs_context_s *)calloc(sizeof(csret_cs_context_s), 1);

	if (context == nullptr)
		return CSRE_ERROR_OUT_OF_MEMORY;

	if (engine_info == nullptr) {
		ret = csret_cs_init_engine(engine_root_dir);

		if (ret != CSRE_ERROR_NONE)
			return ret;
	}

	if (virus_sig == nullptr) {
		snprintf(sig_file, sizeof(sig_file), "%s/%s", engine_root_dir, PRIVATE_DB_NAME);
		ret = csret_cs_read_virus_signatures(sig_file);

		if (ret != CSRE_ERROR_NONE && ret != CSRET_CS_ERROR_NO_SIGNATURE_FILE)
			return ret;
	}

	*phandle = (csre_cs_context_h) context;
	return CSRE_ERROR_NONE;
}

API
int csre_cs_context_destroy(csre_cs_context_h handle)
{
	if (handle == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	csret_cs_context_s *context = (csret_cs_context_s *)handle;
	csret_cs_detected_list_s *curr = nullptr;
	csret_cs_detected_list_s *prev = nullptr;
	curr = context->detected_list;

	while (curr != nullptr) {
		if (curr->detected != nullptr)
			free(curr->detected);

		prev = curr;
		curr = curr->next;
		free(prev);
	}

	free(context);
	return CSRE_ERROR_NONE;
}

API
int csre_cs_set_scan_on_cloud(csre_cs_context_h handle)
{
	if (handle == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	csret_cs_context_s *context = (csret_cs_context_s *)handle;
	context->scan_on_data = TCSE_SCAN_ON_CLOUD_ON;
	return CSRE_ERROR_NONE;
}

API
int csre_cs_scan_data(csre_cs_context_h handle,
					  const unsigned char *data,
					  size_t length,
					  csre_cs_detected_h *pdetected)
{
	int ret = CSRE_ERROR_NONE;
	csret_cs_detected_s *detected = nullptr;

	if (handle == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	csret_cs_context_s *context = (csret_cs_context_s *)handle;

	if (context->scan_on_data == TCSE_SCAN_ON_CLOUD_ON) {
		//ignored in this engine implementation.
	}

	ret = csret_cs_detect_malware(context, data, length, &detected);

	if (ret != CSRE_ERROR_NONE)
		return ret;

	*pdetected = (csre_cs_detected_h) detected;
	return ret;
}

API
int csre_cs_scan_file(csre_cs_context_h handle,
					  const char *file_path,
					  csre_cs_detected_h *pdetected)
{
	int fd = open(file_path, O_RDONLY);

	if (fd < 0)
		return CSRE_ERROR_FILE_NOT_FOUND;

	return csre_cs_scan_file_by_fd(handle, fd, pdetected);
}

API
int csre_cs_scan_file_by_fd(csre_cs_context_h handle,
							int file_descriptor,
							csre_cs_detected_h *pdetected)
{
	csret_cs_detected_s *detected = nullptr;
	unsigned char *data;
	unsigned int data_len;
	int ret = CSRE_ERROR_NONE;

	if (handle == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (file_descriptor < 0)
		return CSRE_ERROR_INVALID_PARAMETER;

	csret_cs_context_s *context = (csret_cs_context_s *)handle;

	if (context->scan_on_data == TCSE_SCAN_ON_CLOUD_ON) {
		//ignored in this engine implementation.
	}

	ret = csret_cs_read_binary_by_fd(file_descriptor, &data, &data_len);
	if (ret != CSRE_ERROR_NONE)
		return ret;

	ret = csret_cs_detect_malware(context, data, data_len, &detected);

	if (ret != CSRE_ERROR_NONE) {
		if (data)
			free(data);
		return ret;
	}

	*pdetected = (csre_cs_detected_h) detected;

	return CSRE_ERROR_NONE;
}

//==============================================================================
// Result related
//==============================================================================
API
int csre_cs_detected_get_severity(csre_cs_detected_h detected, csre_cs_severity_level_e *pseverity)
{
	csret_cs_detected_s *pdetected = nullptr;

	if (detected == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (pseverity == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	pdetected = (csret_cs_detected_s *) detected;
	*pseverity = pdetected->malware.severity;
	return CSRE_ERROR_NONE;
}

API
int csre_cs_detected_get_threat_type(csre_cs_detected_h detected,
									 csre_cs_threat_type_e *pthreat_type)
{
	csret_cs_detected_s *pdetected = nullptr;

	if (detected == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (pthreat_type == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	pdetected = (csret_cs_detected_s *) detected;
	*pthreat_type = pdetected->malware.threat_type;
	return CSRE_ERROR_NONE;
}

API
int csre_cs_detected_get_malware_name(csre_cs_detected_h detected, const char **malware_name)
{
	csret_cs_detected_s *pdetected = nullptr;

	if (detected == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (malware_name == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	pdetected = (csret_cs_detected_s *) detected;
	*malware_name = pdetected->malware.malware_name;
	return CSRE_ERROR_NONE;
}

API
int csre_cs_detected_get_detailed_url(csre_cs_detected_h detected, const char **detailed_url)
{
	csret_cs_detected_s *pdetected = nullptr;

	if (detected == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (detailed_url == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	pdetected = (csret_cs_detected_s *) detected;
	*detailed_url = pdetected->malware.detailed_url;
	return CSRE_ERROR_NONE;
}

API
int csre_cs_detected_get_timestamp(csre_cs_detected_h detected, long *timestamp)
{
	csret_cs_detected_s *pdetected = nullptr;

	if (detected == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (timestamp == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	pdetected = (csret_cs_detected_s *) detected;
	*timestamp = pdetected->timestamp;
	return CSRE_ERROR_NONE;
}

//==============================================================================
// Engine information related
//==============================================================================
API
int csre_cs_engine_get_info(csre_cs_engine_h *pengine)
{
	if (pengine == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*pengine = (csre_cs_engine_h)engine_info;
	return CSRE_ERROR_NONE;
}

API
int csre_cs_engine_get_vendor(csre_cs_engine_h engine, const char **vendor)
{
	csret_cs_engine_s *eng = (csret_cs_engine_s *) engine;

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (vendor == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*vendor = eng->vendor_name;
	return CSRE_ERROR_NONE;
}

API
int csre_cs_engine_get_name(csre_cs_engine_h engine, const char **engine_name)
{
	csret_cs_engine_s *eng = (csret_cs_engine_s *) engine;

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (engine_name == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*engine_name = eng->engine_name;
	return CSRE_ERROR_NONE;
}

API
int csre_cs_engine_get_vendor_logo(csre_cs_engine_h engine, unsigned char **vendor_logo_image,
								   unsigned int *image_size)
{
	csret_cs_engine_s *eng = (csret_cs_engine_s *) engine;

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (vendor_logo_image == nullptr || image_size == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*vendor_logo_image = eng->vendor_logo_image;
	*image_size = eng->image_size;
	return CSRE_ERROR_NONE;
}

API
int csre_cs_engine_get_version(csre_cs_engine_h engine, const char **version)
{
	csret_cs_engine_s *eng = (csret_cs_engine_s *) engine;

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (version == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*version = eng->engine_version;
	return CSRE_ERROR_NONE;
}

API
int csre_cs_engine_get_data_version(csre_cs_engine_h engine, const char **version)
{
	csret_cs_engine_s *eng = (csret_cs_engine_s *) engine;

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (version == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*version = eng->data_version;
	return CSRE_ERROR_NONE;
}

API
int csre_cs_engine_get_activated(csre_cs_engine_h engine, csre_cs_activated_e *pactivated)
{
	csret_cs_engine_s *eng = (csret_cs_engine_s *) engine;

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (pactivated == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	if (virus_sig == nullptr)
		*pactivated = CSRE_CS_NOT_ACTIVATED;
	else
		*pactivated = CSRE_CS_ACTIVATED;

	return CSRE_ERROR_NONE;
}

API
int csre_cs_engine_get_api_version(csre_cs_engine_h engine, const char **version)
{
	csret_cs_engine_s *eng = (csret_cs_engine_s *) engine;

	if (eng == nullptr)
		return CSRE_ERROR_INVALID_HANDLE;

	if (version == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	*version = CSRE_CS_API_VERSION;
	return CSRE_ERROR_NONE;
}


//==============================================================================
// Error related
//==============================================================================
API
int csre_cs_get_error_string(int error_code, const char **string)
{
	if (string == nullptr)
		return CSRE_ERROR_INVALID_PARAMETER;

	switch (error_code) {
	case CSRET_CS_ERROR_NO_SIGNATURE_FILE:
		*string = "CSRET_CS_ERROR_NO_SIGNATURE_FILE";
		break;

	case CSRET_CS_ERROR_SIGNARUE_FILE_FORMAT:
		*string = "CSRET_CS_ERROR_SIGNARUE_FILE_FORMAT";
		break;

	case CSRET_CS_ERROR_FILE_IO:
		*string = "CSRET_CS_ERROR_FILE_IO";
		break;

	default:
		*string = "UNDEFINED ERROR CODE";
		break;
	}

	return CSRE_ERROR_NONE;
}

#ifdef __cplusplus
}
#endif
