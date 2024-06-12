/**
 **************************************************************************************************
 *  @file
 *
 *	@details Image_Header.h provides base class and APIs to access firmware and code
 * sign information block
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef IMAGE_HEADER_H
#define IMAGE_HEADER_H

#include "Includes.h"
#include "FUS_Defines.h"
#include "Uberloader_Data.h"
#include "NV_Ctrl.h"

namespace BF_FUS
{


static const uint16_t VERSION_INFO_SIZE = 16U;
static const uint16_t PRODUCT_CODE_SIZE = 32U;
static const uint16_t FW_HDR_RESERVED_BYTES = 54U;
static const uint16_t IMAGE_HEADER_RESERVED_BYTES = 100U;

class Image_Header
{
	public:

		/**
		 * @brief Structure containing Firmware header components and firmware start location
		 */
		/*typedef struct fw_header_s
		   {
		 * @brief Magic number header; also helps define endianness.
		    uint32_t magic_endian;

		 * @brief Length of the data including FW header and FW
		    uint32_t data_length;

		 * @brief nv address for boot loader to receive message from application.
		    uint32_t msg_for_boot_nvadd;

		 * @brief Version of this header format; right now only one is defined.
		 * Any field past this one is defined by this format.
		    uint16_t header_format_version;

		 * @brief Type of fw (application, web, host, etc)
		    uint16_t image_guid;

		 * @brief Version of this FW file.
		    uint8_t version_info[VERSION_INFO_SIZE];

		 * @brief Product for which FW is intended. Note-aligned to 8 bytes.
		    uint8_t product_code[PRODUCT_CODE_SIZE];

		 * @brief Compatibility number of the image, all images in system should have same
		       compatibility number.
		    uint16_t compatibility_num;

		 * @brief Reserved header space
		    uint8_t reserved[58];

		   } fw_header;///< total header size(including crc at start) becomes 128 bytes.

		   typedef struct crc_info_s
		   {
		    uint16_t crc;
		    uint16_t reserved;
		   } crc_info;

		   typedef struct real_fw_header_s
		   {
		    crc_info fw_crc;	///< crc to validate firmware package components
		    fw_header f;
		    prodinfo_for_boot_s prod;
		 * @brief Payload data.
		    uint8_t data[4];
		   } real_fw_header;

		 */
		Image_Header();

		fus_op_status_t Read_Header( uint8_t* buff, uint16_t* buff_size, NV_Ctrl* nv_ctrl );

		fus_op_status_t Magic_Endian( uint8_t* buff_size, uint16_t* buff_length, NV_Ctrl* nv_ctrl );

		fus_op_status_t Msg_For_Boot_Nvadd( uint8_t* buff_size, uint16_t* buff_length, NV_Ctrl* nv_ctrl );

		fus_op_status_t Header_Format_Version( uint8_t* buff_size, uint16_t* buff_length, NV_Ctrl* nv_ctrl );

		fus_op_status_t Image_Id( uint8_t* buff_size, uint16_t* buff_length, NV_Ctrl* nv_ctrl );

		fus_op_status_t Version_Info( uint8_t* buff_size, uint16_t* buff_length, NV_Ctrl* nv_ctrl );

		fus_op_status_t Product_Code( uint8_t* buff_size, uint16_t* buff_length, NV_Ctrl* nv_ctrl );

		fus_op_status_t Compatibility_Num( uint8_t* buff_size, uint16_t* buff_length, NV_Ctrl* nv_ctrl );

		fus_op_status_t Crc( uint8_t* buff_size, uint16_t* buff_length, NV_Ctrl* nv_ctrl );

		/**
		 * @brief                   Returns compatibility number of given firmware type (app/web)
		 * @param type              Firmware type (App or Web)
		 * @param comp_num          Reference of compatibility number variable
		 * @return                  Status of the operation
		 */
		fus_op_status_t Get_Compatibility_Num( uint16_t* comp_num );

		/**
		 * @brief                   Updates compatibility number of given firmware type (app/web)
		 * @param type              Firmware type (App or Web)
		 * @param comp_num          Value of compatibility number
		 * @return                  Status of the operation
		 */
		fus_op_status_t Update_Compatibility_Num( uint16_t comp_num );

		/**
		 *  @brief Destructor.
		 *  @details It will get called when object of Image_Header_Control goes out of scope or
		 * deleted.
		 *  @return None.
		 */
		~Image_Header( void )
		{}

	private:
		uint16_t m_comp_num;
};

}


#endif	// IMAGE_HEADER_H
