
# Since this is built as a logical part of the PSP, define the _CFE_PSP_ macro
add_definitions(-D_CFE_PSP_)

# Generic I/O device driver interface module
include_directories(${iodriver_MISSION_DIR}/inc)
add_psp_module(iodriver src/iodriver.c)
