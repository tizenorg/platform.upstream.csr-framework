# Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
# @file        CMakeLists.txt
# @author      Sangwan Kwon (sangwan.kwon@samsung.com)
# @brief
#
SET(POFILES
	as.po
	az.po
	be_BY.po
	bn_BD.po
	bn.po
	en.po
	gu.po
	hi.po
	hy.po
	id.po
	jv.po
	ka.po
	kk.po
	kn.po
	ko_KR.po
	ky_KG.po
	ml.po
	mn_MN.po
	mr.po
	ne.po
	or.po
	pa.po
	ru_RU.po
	si.po
	su_ID.po
	ta.po
	te.po
	tg_TJ.po
	tk_TM.po
	uk.po
	ur.po
	uz.po
	xh.po
	zh_CN.po
	zu.po
	en_US.po
	ms.po

)

SET(MSGFMT "${BIN_DIR}/msgfmt")

FOREACH(pofile ${POFILES})
	SET(pofile ${CMAKE_CURRENT_SOURCE_DIR}/${pofile})
	MESSAGE("PO: ${pofile}")
	GET_FILENAME_COMPONENT(absPofile ${pofile} ABSOLUTE)
	GET_FILENAME_COMPONENT(lang ${absPofile} NAME_WE)
	SET(moFile ${CMAKE_CURRENT_BINARY_DIR}/${lang}.mo)
	ADD_CUSTOM_COMMAND(
		OUTPUT ${moFile}
		COMMAND ${MSGFMT} -o ${moFile} ${absPofile}
		DEPENDS ${absPofile}
	)
	INSTALL(
		FILES ${moFile}
		DESTINATION ${LOCALEDIR}/${lang}/LC_MESSAGES
		RENAME ${SERVICE_NAME}.mo
	)
	SET(moFiles ${moFiles} ${moFile})
ENDFOREACH(pofile)

MESSAGE(".mo files: ${moFiles}")
ADD_CUSTOM_TARGET(po ALL DEPENDS ${moFiles})
