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
 * @file        csre-content-screening-types.h
 * @author      Dongsun Lee (ds73.lee@samsung.com)
 * @version     1.0
 * @brief       Types for content-screening engine
 */
#ifndef __CSRE_CONTENT_SCREENING_TYPES_H_
#define __CSRE_CONTENT_SCREENING_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Content screening engine API version.
 */
#define CSRE_CS_API_VERSION "1.0"

/**
 * CSR CS engine's context handle.
 */
typedef struct csre_cs_context_s *csre_cs_context_h;

/**
 * CSR CS engine's detected malware info handle.
 */
typedef struct csre_cs_detected_s *csre_cs_detected_h;

/**
 * @brief Severity level of an detected malware
 */
typedef enum {
	CSRE_CS_SEVERITY_LOW    = 0x01,  /**< Low Severity. Process with a warning log. */
	CSRE_CS_SEVERITY_MEDIUM = 0x02,  /**< Medium Severity. Prompt the user before processing. Ask the user if the user wants the application to process the data. */
	CSRE_CS_SEVERITY_HIGH   = 0x03   /**< High Severity. Do not process the data and prompt user for removal. */
} csre_cs_severity_level_e;

/**
 * @brief the type of a threat detected
 */
typedef enum {
	CSRE_CS_THREAT_MALWARE = 0x00,  /**< Malware. */
	CSRE_CS_THREAT_RISKY   = 0x01,  /**< It's not a malware but still risky. */
	CSRE_CS_THREAT_GENERIC = 0x02   /**< Generic threat */
} csre_cs_threat_type_e;

typedef enum {
	CSRE_CS_NOT_SUPPORTED  = 0x00,  /**< Cloud scan is not supported */
	CSRE_CS_SUPPORTED      = 0x01,  /**< Cloud scan is supported */
} csre_cs_cloud_supported_e;

#ifdef __cplusplus
}
#endif

#endif
