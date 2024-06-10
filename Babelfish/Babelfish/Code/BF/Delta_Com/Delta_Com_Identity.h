/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DELTA_COM_IDENTITY_H
#define DELTA_COM_IDENTITY_H
#include "Includes.h"

static const uint8_t DELTA_COM_ID_BLOCK_REVISION = 0U;

struct delta_com_identity_t
{
	uint8_t m_identity_revision;
	uint8_t m_product_compatibility_number;
	uint16_t m_product_code;
	uint16_t m_product_subcode;
	uint32_t m_firmware_revision;
	uint16_t m_hardware_revision;
	uint32_t m_device_serial_number;
	uint16_t m_buffer_size;
	uint16_t m_update_interval_time;
};

struct delta_com_identity_source_t
{
	uint8_t m_identity_revision;
	uint8_t m_product_compatibility_number;
	uint16_t const* m_product_code;
	uint16_t const* m_product_subcode;
	uint32_t const* m_firmware_revision;
	uint16_t const* m_hardware_revision;
	uint32_t const* m_device_serial_number;
	uint16_t m_buffer_size;
	uint16_t m_update_interval_time;
};

typedef uint8_t dcom_identity_error;
static const dcom_identity_error DCOM_IDENTITY_ERROR_SUCCESS = 0U;
static const dcom_identity_error DCOM_IDENTITY_ERROR_FAILURE = 1U;
static const dcom_identity_error DCOM_IDENTITY_ERROR_UPDATE_RATE_HIGH = 2U;
static const dcom_identity_error DCOM_IDENTITY_ERROR_BUFFER_TOO_LARGE = 3U;
static const dcom_identity_error DCOM_IDENTITY_ERROR_COMPATIBILITY_NUMBER = 4U;
static const dcom_identity_error DCOM_IDENTITY_ERROR_IDENTITY_REV = 5U;
static const dcom_identity_error DCOM_IDENTITY_ERROR_PRODUCT_CODE = 6U;
static const dcom_identity_error DCOM_IDENTITY_ERROR_PRODUCT_SUB_CODE = 7U;
static const dcom_identity_error DCOM_IDENTITY_ERROR_FIRMWARE_REV = 8U;

#endif
