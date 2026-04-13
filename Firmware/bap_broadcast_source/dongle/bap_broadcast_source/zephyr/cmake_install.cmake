# Install script for directory: C:/ncs/v3.1.1/zephyr

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Zephyr-Kernel")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/ncs/toolchains/c1a76fddb2/opt/zephyr-sdk/arm-zephyr-eabi/bin/arm-zephyr-eabi-objdump.exe")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/zephyr/arch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/zephyr/lib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/zephyr/soc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/zephyr/boards/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/zephyr/subsys/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/zephyr/drivers/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/nrf/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/hostap/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/mcuboot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/mbedtls/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/trusted-firmware-m/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/cjson/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/azure-sdk-for-c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/cirrus-logic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/openthread/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/memfault-firmware-sdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/canopennode/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/chre/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/lz4/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/nanopb/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/zscilib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/cmsis/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/cmsis-dsp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/cmsis-nn/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/cmsis_6/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/fatfs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/hal_nordic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/hal_st/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/hal_tdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/hal_wurthelektronik/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/liblc3/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/libmetal/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/littlefs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/loramac-node/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/lvgl/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/mipi-sys-t/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/nrf_wifi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/open-amp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/percepio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/picolibc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/segger/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/tinycrypt/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/uoscore-uedhoc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/zcbor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/nrfxlib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/nrf_hw_models/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/modules/connectedhomeip/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/zephyr/kernel/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/zephyr/cmake/flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/zephyr/cmake/usage/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Inzynierka_V2/Kody_testowe/bap_broadcast_source/dongle/bap_broadcast_source/zephyr/cmake/reports/cmake_install.cmake")
endif()

