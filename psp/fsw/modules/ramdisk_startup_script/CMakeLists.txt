# This is a simple module that installs a statically-linked CFE startup
# script into the RAMDISK before starting CFE.

add_psp_module(ramdisk_startup_script cfe_psp_ramdisk_startup_script.c)
