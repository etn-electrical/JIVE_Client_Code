#####################################
# libiot_sdk makefile for Linux
#####################################

ifndef TOP
	TOP:=$(CURDIR)/../../../..
	export TOP
endif

# Set name of application.
MODULENAME:=libetn_iot_sdk
# We want to build this as a statically linked library
PROJECT_IS_ARLIB:=y

# If not release code, enable Info and Debug logging 
ifeq ($(CONFIG_DEVEL),y)
	CPPFLAGS+= -DIOT_ENABLE_INFO_LOGGING -DIOT_ENABLE_DEBUG_LOGGING
else
	# Define NO_LOGGING for Azure 
	CPPFLAGS+= -DNO_LOGGING
endif
# In all cases (regardless of DEBUG or NDEBUG), enable IoT Device SDK Error logging
CPPFLAGS+= -DIOT_ENABLE_ERROR_LOGGING

ifeq ($(CONFIG_SW_PRODUCT),edgex)
	# Nothing to set here yet
else
	# And set the define that distinguishes this Red build from Green
	CPPFLAGS+= -DPX_RED_DEVICE_BUILD 
endif

# Select the protocol we want for the IoT Device SDK; default to AMQP over Websockets:
ifeq ($(CONFIG_EATON_IOT_MQTT_WS),y)
	CPPFLAGS+= -DIOT_INCLUDE_MQTT_WEBSOCKETS
else
	CPPFLAGS+= -DIOT_INCLUDE_AMQP_WEBSOCKETS
endif

# Add the SAS Token Lifetime, for the IoT SDK, if configured to a non-standard value
# Examples for quick testing: target is refresh at 4min 3sec = 243sec, 
# For MQTT, 243 is 80% of 304sec, so use 304
# For AMQP, refresh will be 30s less than lifetime, 243 + 30 = 273sec, so use 273.
ifneq ($(CONFIG_IOT_SAS_TOKEN_REFRESH_SECS),)
CPPFLAGS+= -DIOT_SAS_TOKEN_REFRESH_SECS=$(CONFIG_IOT_SAS_TOKEN_REFRESH_SECS)
endif

# Configuration for Azure:
# Give the path to the Azure headers (base)
CPPFLAGS+=-I$(TOOLKIT_VENDOR_INCLUDE)/azureiot
# Don't use more specific paths like this, due to too-common file names like "strings.h"
#CPPFLAGS+=-I$(TOOLKIT_VENDOR_INCLUDE)/azure/iothub_client/azure_c_shared_utility

ifeq ($(CONFIG_IOT_ENABLE_BLOB_UPLOAD),y)
CPPFLAGS+= -DWANT_BASE64_ENCODING
else
# To save space and remove unused functionality, set DONT_USE_UPLOADTOBLOB (http-based)
CPPFLAGS+= -DDONT_USE_UPLOADTOBLOB
endif

# May set IOT_ENABLE_VERBOSE_LOGGING for CURLOPT_VERBOSE; only applies when using HTTP
# Possible operation options are OPTION_SAS_TOKEN_REFRESH_TIME,
# 		OPTION_EVENT_SEND_TIMEOUT_SECS, and OPTION_C2D_KEEP_ALIVE_FREQ_SECS.

## This library doesn't have a unit or performance test
CONFIG_UNIT_TEST:=n
CONFIG_PERF_TEST:=n

## LIBRARIES for inclusion. None for this static library
LIBS:=
TESTLIBS:=

# Use Eaton generic project Makefile
include $(PROJECT_MK_LOCATION)/project.mk

# These files not logically guaranteed to exist but they are never generated.
# Don't waste time with implicit rules.
$(PROJECT_MK_LOCATION)/project.mk Makefile: ;
