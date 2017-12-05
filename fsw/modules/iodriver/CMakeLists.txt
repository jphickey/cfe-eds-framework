
# Generic I/O device driver interface module
include_directories(${iodriver_MISSION_DIR}/inc)
add_psp_module(iodriver src/iodriver.c)
