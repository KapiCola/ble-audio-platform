cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(codec_test)

target_sources(app PRIVATE src/main.c)

target_sources(app PRIVATE src/i2c_helpers.c)

target_sources(app PRIVATE src/pmic.c)

target_sources(app PRIVATE src/codec_sgtl5000.c)
