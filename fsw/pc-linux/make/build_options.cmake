# This indicates where to install target binaries created during the build
set(INSTALL_SUBDIR "eeprom1")

# Some upper-level code may be gated on _LINUX_OS_ being defined
set(OSAL_C_FLAGS "${OSAL_C_FLAGS} -D_LINUX_OS_")

# TEMPORARY HACK until the native 64-bit fixes are moved in -
# force any "native" build to use the -m32 switch to gcc
if (NOT NATIVE_64_WORKS AND "${SIMULATION}" STREQUAL "native")
  set(OSAL_C_FLAGS "${OSAL_C_FLAGS} -m32")
endif()

