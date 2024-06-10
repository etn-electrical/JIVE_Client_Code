/**
 *****************************************************************************************
 * @file	Rest_Config.h
 * @details This is a rest specific configuration file with all required defines
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef REST_CONFIG_H
#define REST_CONFIG_H

/* Enable the appropriate macro depending on the type of authorization either Digest/Basic */

#define   __DIGEST_ACCESS_AUTH__     1		/* Define this for Digest Access Authentication*/

/* The authorization string shall be used to specify the authorization header which will be
   sent back to client. Generally whenever browser detects 401 response along with
   Digest or Basic response header, it generates popup to enter credentials.
   However UI developer may want to genarte its own popup in such cases and not to use popup
   provided by browser. In such case the server will send 401 response, however the Auth_header
   will be custom in agreement with UI owner.
 */
#define CUSTOM_AUTH "Auth_TK"
#ifdef __DIGEST_ACCESS_AUTH__
#define AUTH_STRING "Digest"
#define CUSTOM_AUTH_STRING "TK_Digest"
#endif

/* PX Green recommends value of HTTP_POOL_TOTAL_BUFFER_SIZE to be minimum 6KB. this value is used to reserve RAM to
   handle multiple REST resources at time, it must be in multiple of 1024 only */
#define HTTP_POOL_TOTAL_BUFFER_SIZE ( 6 * 1024U )// 6KB

/* Destination buffer size used in Rest.cpp - recommended value is 255 */
#define MIN_DEST_BUFFER_SIZE 255U

/* Format_Handler module converts raw data to formated string and vice versa
   Length of formatted string changes as per format type.
   Following macro will allow adaptors to configure buffer length.
   This buffer is created to store formated string and immediately deleted after
   formated string passed to next layer.
   Internally 2 bytes get added into FORMAT_HANDLER_BUFFER_SIZE for indexing purpose.
   This space will be consumed from web server module's reserved space.
   Note: Consumption will be in multiple of HTTP_POOL_BLOCK_SIZE (defined in httpd_common.h). Thus adopter
   should select FORMAT_HANDLER_BUFFER_SIZE wisely to consume only required space.
    Example 1 : When FORMAT_HANDLER_BUFFER_SIZE = 254U and HTTP_POOL_BLOCK_SIZE = 128U then
                Required space = FORMAT_HANDLER_BUFFER_SIZE + 2 bytes = 256U
                Actual space given = 256U ( total 2 HTTP_POOL_BLOCK_SIZES will be required to accomodate 256 bytes need
                   )
    Example 2 : When FORMAT_HANDLER_BUFFER_SIZE = 256U and HTTP_POOL_BLOCK_SIZE = 128U then
                Required space = FORMAT_HANDLER_BUFFER_SIZE + 2 bytes = 258U
                Actual space given = 384U ( total 3 HTTP_POOL_BLOCKS will be required to accomodate 258 bytes need )

    Conclusion: Adopter should configure values like 256U only if formatted string size is greater than 254U.
 */
#define FORMAT_HANDLER_BUFFER_SIZE 254U

/* Allocate 128 bytes of buffer for Modbus on REST */
#define MODBUS_ON_REST_BUFFER_SIZE 128U

/*
   MAX_FW_UPGRADE_ENCODED_PACKET_SIZE defines the max number of encrypted and/or encoded text data bytes device will be
      able to handle in each firmware upgrade chunk.
   When using default configuration (no encoding and no encryption), the value should be at-least twice the size of hex
      packet data. However, This is recommended to set this to minimum 2048.
   This memory is taken out from existing HTTP memory pool only(no separate memory is allocated ). This must be in
      multiples of 128 bytes.
 */
#define MAX_FW_UPGRADE_ENCODED_PACKET_SIZE 2048U

/*
   MAX_FW_UPGRADE_DECODED_PACKET_SIZE defines the max number of plain (Hex) data bytes device will be able to handle in
      each firmware upgrade chunk.
   When using default configuration (no encoding and no encryption), This is recommended to set this to minimum 1536U.
   This memory is taken out from existing HTTP memory pool only(no separate memory is allocated ). This must be in
      multiples of 128 bytes.
 */
#define MAX_FW_UPGRADE_DECODED_PACKET_SIZE 1536U

/*
    WEB_PAGE_READ_BUFF_SIZE defines the maximum size of the Web UI data buffer that will be transmitted to the server.
    WEB_PAGE_READ_BUFF_SIZE will only be used when Web UI is placed in External memory. For external memory need to
    uncomment the WEBUI_ON_EXTERNL_FLASH macro in the file common_define_config.xcl
    WEB_PAGE_READ_BUFF_SIZE must be in multiples of 128 bytes. For 512 Bytes HTTP page takes areound 30 sec to load
       completly
 */
#ifdef WEBUI_ON_EXT_FLASH
#define WEB_PAGE_READ_BUFF_SIZE 1536U
#endif

#endif


