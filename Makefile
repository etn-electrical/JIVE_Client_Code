#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := esp32_sample_app

# Added this macro as this is required to build Babelfish code only
# ESP32 is still using FreeRTOS version 8.0.2 
CPPFLAGS += -DFREERTOS_V10

#EXTRA_COMPONENT_DIRS = $(IDF_PATH)/examples/common_components/protocol_examples_common

include $(IDF_PATH)/make/project.mk

