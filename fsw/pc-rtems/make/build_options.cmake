# This indicates where to install target binaries created during the build
set(INSTALL_SUBDIR "eeprom")

SET(OSAL_LINK_LIBS cexp spencer_regexp tecla pmbfd pmelf ${OSAL_LINK_LIBS})
