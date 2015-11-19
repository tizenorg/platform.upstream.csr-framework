/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __TIZEN_CORE_CSRFW_DOC_H__
#define __TIZEN_CORE_CSRFW_DOC_H__

/**
 *@defgroup CAPI_CSRFW CSR Framework
 *@brief These APIs provide checking content screening and blocking access of malicious web site.
 *@ingroup CAPI_SECURITY_FRAMEWORK
 *@section CAPI_CSR_FRAMEWORK_OVERVIEW Overview
 *<table>
 *<tr><th>API Prefixes</th><th>Description</th></tr>
 *<tr><td>@ref CAPI_CSRFW_TCS_MODULE</td><td>Providing APIs for content screening</td></tr>
 *<tr><td>@ref CAPI_CSRFW_TWP_MODULE</td><td>Providing APIs for block an access of malicious web site API</td></tr>
 *</table>
 *
 * It proposes the security framework on Tizen platform. Tizen content security
 * framework will be responsible for passing the security API calls to security plug-in, which
 * could be provided by security vendor plug-in. The framework is also responsible for
 * error handling when there is no security vendor plug-in installed.
 *
 * @image html csr_framework_overview.png
 *
 * @section CAPI_CSR_FRAMEWORK_SOLUTION_DESCRIPTION Solution description
 *
 * Both content security framework and security plug-in are shared libraries in Tizen case. All
 * the code is loaded in the application memory space. The authentication of those libraries will
 * be covered by Tizen certificate process.
 * Content security framework (libsecfw.so) will be linked directly to system component which is
 * invoking the security API, while the security plug-in will be loaded in the runtime and
 * installed along with security application package. The security application package should be
 * signed with a trusted certificate which indicates that the package is authorized to carry
 * security plug-in and is ready for use. And as a security consideration, Tizen installer will
 * check this package whenever the application gets installed.
 *
 * @section CAPI_CSR_FRAMEWORK_CONTENT_SECURITY Content security framework
 *
 * API standaridizing:
 *  - Content Security Framework will provide a set of APIs to other system modules with security
 * features. Currently we have site engine and anti-virus engine API defined in this framework.
 * Please refer to Tizen content screening and site engine API specification for detail.
 * Each security vendor who wants to add their plug-in to Tizen platform, need to provide a
 * plug-in library which conforms to the Framework API which we defined in the framework
 * above their own engines.
 *
 * Plug-in managerment:
 *  - Content security framework is responsible for plug-in loading/reloading. It will always try to
 * load the new plug-in from "/opt/usr/share/sec_plugin/libengine.so" when content security
 * framework is reinitialized by library open API call, in this case it is tcs_library_open(). This
 * is saying that the newly installed security plug-in will be loaded only when
 * tcs_library_open() gets called. During the tcs_library_close() and tcs_library_open(), the
 * caller will keep using the old security plug-in until it close the library and reopen it.
 *
 * Error handling:
 *  - Content security framework will return not implemented error code to caller if there is no
 * plug-in found at "/opt/usr/share/sec_plugin".
 *
 * Concurrent Scan Support:
 *  - The TCS security vendor plug-in must support concurrent scan in multi-tasking, so that
 * Tizen component can have multiple threads to scan content concurrently.
 */

#endif /* __TIZEN_CORE_CSRFW_DOC_H__ */
