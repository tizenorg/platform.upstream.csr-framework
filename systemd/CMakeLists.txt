CONFIGURE_FILE(${SERVICE_NAME}.service.in ${SERVICE_NAME}.service @ONLY)
CONFIGURE_FILE(${SERVICE_NAME}.socket.in ${SERVICE_NAME}.socket @ONLY)
CONFIGURE_FILE(${SERVICE_NAME}-popup.service.in ${SERVICE_NAME}-popup.service @ONLY)
CONFIGURE_FILE(${SERVICE_NAME}-popup.socket.in ${SERVICE_NAME}-popup.socket @ONLY)

INSTALL(
	FILES
		${SERVICE_NAME}.service
		${SERVICE_NAME}.socket
		${SERVICE_NAME}-popup.service
		${SERVICE_NAME}-popup.socket
	DESTINATION ${SYSTEMD_UNIT_DIR}
)
