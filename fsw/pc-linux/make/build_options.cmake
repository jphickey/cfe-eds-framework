# This indicates where to install target binaries created during the build
set(INSTALL_SUBDIR "eeprom1")

# Some upper-level code may be gated on _LINUX_OS_ being defined
set(OSAL_C_FLAGS "${OSAL_C_FLAGS} -D_LINUX_OS_")

# These are additional definitions from the old build system
# NONE of these should really be required and they assume quite a bit 
# more than they should (bit order, processor type, etc)
foreach(FLAG "_EL" "ENDIAN=_EL" "SOFTWARE_LITTLE_BIT_ORDER" "__ix86__" 
           "_ix86_" "posix" "X86PC" "_REENTRANT" "_EMBED_" "OS_DEBUG_LEVEL=3")
  set(OSAL_C_FLAGS "${OSAL_C_FLAGS} -D${FLAG}")
endforeach()

# TEMPORARY HACK until the native 64-bit fixes are moved in -
# force any "native" build to use the -m32 switch to gcc
if (NOT NATIVE_64_WORKS AND "${SIMULATION}" STREQUAL "native")
  set(OSAL_C_FLAGS "${OSAL_C_FLAGS} -m32")
endif()
