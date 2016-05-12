CONFIGURE_FILE(${SERVICE_NAME}.service.in ${SERVICE_NAME}.service @ONLY)
CONFIGURE_FILE(${SERVICE_NAME}-cs.socket.in ${SERVICE_NAME}-cs.socket @ONLY)
CONFIGURE_FILE(${SERVICE_NAME}-wp.socket.in ${SERVICE_NAME}-wp.socket @ONLY)
CONFIGURE_FILE(${SERVICE_NAME}-admin.socket.in ${SERVICE_NAME}-admin.socket @ONLY)
CONFIGURE_FILE(${SERVICE_NAME}-popup.service.in ${SERVICE_NAME}-popup.service @ONLY)
CONFIGURE_FILE(${SERVICE_NAME}-popup.socket.in ${SERVICE_NAME}-popup.socket @ONLY)

INSTALL(
	FILES
		${SERVICE_NAME}.service
		${SERVICE_NAME}-cs.socket
		${SERVICE_NAME}-wp.socket
		${SERVICE_NAME}-admin.socket
		${SERVICE_NAME}-popup.service
		${SERVICE_NAME}-popup.socket
	DESTINATION ${SYSTEMD_UNIT_DIR}
)
