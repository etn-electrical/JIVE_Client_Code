/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef __BASE_64_H__
#define __BASE_64_H__

// faster, but uses 256 bytes as a static lookup table
// #define BASE64_DECODING_USE_TABLE_LOOKUP





enum
{
	BASE64_DECODING_ERR_OK = 0,
	BASE64_DECODING_ERR_WRONG_INPUT_LENGTH,
	BASE64_DECODING_ERR_WRONG_INPUT_CHAR
};

enum
{
	PADDING_0 = 0U,
	PADDING_1,
	PADDING_2,
};

class Base64
{
	public:
		// inputs:
		// pOriginal: input data pointer to be encoded
		// uInputLength: length of the input data to be encoded
		// pRes: points to the region where the data is stored
		// offset: location of the first octet to be encoded
		// input data is pOriginal[offset .. offset+uInputLength-1]
		// return: number of bytes in the results of encoding
		// the number of bytes shall be 4*ceiling(uInputLength/3)
		static uint32_t Encode( const uint8_t* pOriginal, uint32_t uInputLength, uint8_t* pRes, uint32_t offset = 0 );

		// inputs:
		// pEncoded: input data to be decoded
		// uInputLength: length of input data to be decoded (a multiple of 4)
		// pDecoded: pointer to the result (decoded octet array)
		// uOutputLength: pointer to the location to store decoded length
		// offset: location of the first character to be decoded
		// input data to be decoded is pEncoded[offset .. offset+uInputLength-1]
		// the output length should be uInputLength/4*3,
		// -1 if pEncoded[] ends with "="
		// -2 if pEncoded[] ends with "=="
		// return: error code
		static uint8_t Decode( const uint8_t* pEncoded, uint32_t uInputLength, uint8_t* pDecoded,
							   uint32_t* uOutputLength, uint32_t offset = 0 );

	private:
		static const uint8_t enc_table[65];
#ifdef BASE64_DECODING_USE_TABLE_LOOKUP
		static const uint8_t dec_table[256];
#endif	// BASE64_DECODING_USE_TABLE_LOOKUP
};

#endif	/* __BASE_64_H__ */
