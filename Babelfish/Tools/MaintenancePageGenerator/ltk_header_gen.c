/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "fw.h"
#include "ltk_header_gen.h"

/* Global variables */
FILE* fOut;
FILE* fIn;
FILE* fVersion;
FILE* fIni;
/* String tables. Generic header generation*/
#define MAX_GEN_TAGS 6
#define MAX_FORMAT_TAGS 2
#define MAX_OFFSET_TAGS 2
#define FIELD_START 0
#define HEADER_INFO 1
#define OFFSET 2
#define LENGTH 3
#define FORMAT 4
#define PADD_VALUE 0xff
#define MIN_FILE_LENGTH_COUNT 1U
#define MAX_FILE_LENGTH_COUNT 10U

const char* Gen_Tags[MAX_GEN_TAGS] =
{
	"<FieldStart>",
	"<HEADERINFO Value=",
	"<Offset>",
	"<Length>",
	"<Format>",
	"</InfoStart>"
};

enum
{
	FORMAT_NONE,
	FORMAT_ASCII,
	FORMAT_VERSION
};

enum
{
	TOP_FW,
	BOTTOM_FW,
} offset;

const char* OFFSET_Tags[MAX_OFFSET_TAGS] =
{
	"TOP_FW",
	"BOTTOM_FW"
};
const char* FORMAT_Tags[MAX_FORMAT_TAGS] =
{
	"NONE",
	"ASCII"
};
typedef union
{
	uint16_t u16;
	uint8_t u8[2];
} SPLIT_UINT16;

struct padd_struct
{
	uint32_t file_length;
	uint32_t file_size;
	uint32_t padding_count;
	uint32_t padding_offset;
	bool padding_flag;
};

/* CRC generation code directly used from fwtool.c */
uint16_t Gen_Crc( const uint8_t* data, unsigned length, uint16_t start )
{
	uint8_t temp;
	SPLIT_UINT16 temp_crc;

	temp_crc.u16 = start;

	while ( length-- )
	{
		temp = ( *data++ ) ^ temp_crc.u8[0];
		temp ^= ( temp << 4 );
		temp_crc.u8[0] = temp_crc.u8[1] ^ ( temp << 3 ) ^ ( temp >> 4 );
		temp_crc.u8[1] = temp ^ ( temp >> 5 );
	}

	return ( temp_crc.u16 );
}

/**
 * @brief: Check whether the field is decimal or not
 * @param[in] uint8_t* data: pointer to the string
 * @param[out] uint8_t return the integer value
 */
uint8_t Is_Decimal( uint8_t data )
{
	return ( ( data >= 0x30 && data <= 0x39 ) ? 1 : 0 );
}

/**
 * @brief: Check whether the field is hex upper or not
 * @param[in] uint8_t* data: pointer to the string
 * @param[out] uint8_t return the integer value
 */
uint8_t Is_Upper_Hex_Char( uint8_t data )
{
	return ( ( data >= 0x41 && data <= 0x46 ) ? 1 : 0 );
}

/**
 * @brief: Check whether the field is hex lower or not
 * @param[in] uint8_t* data: pointer to the string
 * @param[out] uint8_t return the integer value
 */
uint8_t Is_Lower_Hex_Char( uint8_t data )
{
	return ( ( data >= 0x61 && data <= 0x66 ) ? 1 : 0 );
}

/**
 * @brief: Check whether the string contains Hex value or not
 * @param[in] uint8_t* data: pointer to the string
 * @param[out] uint8_t return the integer value
 */
uint8_t Is_Hex_String( const uint8_t* data )
{
	uint8_t return_status = ERROR_VALUE;

	if ( ( data[0] == '0' ) && ( ( data[1] == 'x' ) || ( data[1] == 'X' ) ) )
	{
		return_status = SUCCESS_VALUE;
	}
	return ( return_status );
}

/**
 * @brief: Convert ASCII to INT
 * @param[in] uint8_t* field_val: pointer to the string
 * @param[in] uint16_t field_cnt: field count
 * @param[in] uint16_t* act_cnt: actual field count after conversion
 * @param[out] uint8_t return the integer value
 */
uint8_t Convert_Ascii_To_Int( uint8_t* field_val, uint16_t field_cnt, uint16_t* act_cnt )
{
	uint8_t return_status = SUCCESS_VALUE;
	uint32_t i = 0;

	for ( i = 0; i < field_cnt; i++ )
	{
		if ( Is_Decimal( field_val[i] ) )
		{
			field_val[i] = field_val[i] - 0x30;	/* ASCII To INT conversion */
			*act_cnt = *act_cnt + 1;
		}
		else if ( Is_Upper_Hex_Char( field_val[i] ) )
		{
			field_val[i] = field_val[i] - 0x41 + 10;/* ASCII To INT conversion */
			*act_cnt = *act_cnt + 1;
		}
		else if ( Is_Lower_Hex_Char( field_val[i] ) )
		{
			field_val[i] = field_val[i] - 0x61 + 10;/* ASCII To INT conversion */
			*act_cnt = *act_cnt + 1;
		}

		if ( field_val[i] > 0x0f )
		{
			/*X or x character found */
			if ( ( field_val[i] == 0x78 ) || ( field_val[i] == 0x58 ) )
			{
				continue;
			}
			else
			{
				return_status = ERROR_VALUE;
				printf( "Not valid char=%x\n", field_val[i] );
				break;
			}
		}
	}
	// printf("Return conv=%x\n",return_status);
	return ( return_status );
}

/**
 * @brief: convert offset and length from xml file into integer value from ASCII
 * @param[in] const uint8_t* pointer to the offset or length tag value
 * @param[out] uint32_t return the integer value
 */
uint32_t Offset_Length_Conv( const uint8_t* data )
{
	uint32_t ret_offset_val = 0;
	uint8_t temp_val[16] = {0};
	uint32_t i = 0;
	uint32_t count = 0;
	uint8_t is_string_hex = Is_Hex_String( data );

	if ( is_string_hex == SUCCESS_VALUE )
	{
		data = data + 2;
	}
	while ( *data != '<' )
	{
		temp_val[i] = *data;
		if ( Is_Decimal( temp_val[i] ) )
		{
			temp_val[i] = temp_val[i]
				- 0x30;	/*ASCII To INT conversion*/
		}
		else if ( Is_Upper_Hex_Char( temp_val[i] ) )
		{
			temp_val[i] = temp_val[i]
				- 0x41 + 10;	/*ASCII To INT conversion*/
		}
		else if ( Is_Lower_Hex_Char( temp_val[i] ) )
		{
			temp_val[i] = temp_val[i]
				- 0x61 + 10;	/*ASCII To INT conversion*/
		}

		if ( temp_val[i] > 0x0f )
		{
			return ( EXIT_FAILURE );	// @TODO
			/*Invalid character*/
		}
		data = data + 1;
		i++;
		count++;
	}

	if ( is_string_hex == SUCCESS_VALUE )
	{
		for ( i = 0; i < count; i++ )
		{
			// printf("conv=%x\n",temp_val[i]);
			ret_offset_val = ret_offset_val * 16 + temp_val[i];
			// printf("hex to int=%x\n",decval);
		}
		printf( "hex TO int=%u\n", ret_offset_val );
	}
	else
	{
		for ( i = 0; i < count; i++ )
		{
			ret_offset_val = ret_offset_val * 10 + temp_val[i];
			// printf("conv=%x\n",temp_val[i]);
		}
	}
	return ( ret_offset_val );
}

/**
 * @brief: return the type of format for the field (NONE,ASCII,dotted version)
 * @param[in] const uint8_t* pointer to the format tag value
 * @param[in] uint8_t* dot count for firmware dotted representation
 * @param[out] uint8_t return the format type (ENUM value)
 */
uint8_t Format_Type_Ret( const uint8_t* data, uint8_t* dot_count )
{
	uint8_t temp_val[16] = {0};
	uint8_t i = 0;
	uint8_t format_type = 0;
	uint8_t count = 0;

	while ( *data != '<' )
	{
		temp_val[i] = *data;
		if ( temp_val[i] == '.' )
		{
			count++;
		}
		data = data + 1;
		i++;
	}

	temp_val[i + 1] = '\0';

	if ( count == 0 )
	{
		for ( i = 0; i < MAX_FORMAT_TAGS; i++ )
		{
			// printf("temp_val=%s,tags=%s\n",);
			if ( strcmp( ( const char* )temp_val, FORMAT_Tags[i] ) == 0 )
			{
				format_type = i;
				break;
			}
			else
			{
				format_type = 0x0F;	/* error value*/
			}
		}
	}
	else
	{
		format_type = FORMAT_VERSION;
		*dot_count = count;
	}

	return ( format_type );
}

/**
 * @brief: copy the ASCII value present in xml file
 * @param[in] uint8_t* gen_ltk_header pointer to the header buffer
 * @param[in] uint8_t* field_val ASCII format value of the field
 * @param[in] uint8_t field_count count of the field to be copied in header
 * @param[in] uint32_t len actual length in the header for the field
 * @param[in] uint32_t offset location in header where field needs to be inserted
 * @param[out] void
 */
void Copy_Ascii( uint8_t* gen_ltk_header, uint8_t* field_val, uint8_t field_count, uint32_t len,
				 uint32_t offset )
{
	uint16_t j;

	for ( j = 0; j < len; j++ )
	{
		gen_ltk_header[offset + j] = field_val[j];
		// printf( "ASCII = %x,%x\n", gen_ltk_header[offset + j],offset );
	}
}

/**
 * @brief: convert the ASCII value present in xml file for the field present in xml
 * @param[in] uint8_t* gen_ltk_header pointer to the header buffer
 * @param[in] uint8_t* field_val ASCII format value of the field
 * @param[in] uint8_t field_count count of the field to be copied in header
 * @param[in] uint32_t len actual length in the header for the field
 * @param[in] uint32_t offset location in header where field needs to be inserted
 * @param[out] uint8_t success or failure
 */
uint8_t Convert_Hex( uint8_t* gen_ltk_header, uint8_t* field_val, uint8_t field_count, uint32_t len,
					 uint32_t offset )
{
	uint8_t is_string_hex = Is_Hex_String( field_val );
	uint8_t exit_status = SUCCESS_VALUE;
	uint16_t actual_cnt = 0;

	if ( is_string_hex == SUCCESS_VALUE )
	{
		// printf("hex string\n");
		const uint8_t REMOVE_HEX_FIELD = 2;
		uint8_t* pool = ( uint8_t* ) ( malloc( field_count - REMOVE_HEX_FIELD ) );
		uint8_t* temp = ( uint8_t* ) ( malloc( field_count - REMOVE_HEX_FIELD ) );
		memset( pool, 0, ( field_count - REMOVE_HEX_FIELD ) );
		memset( temp, 0, ( field_count - REMOVE_HEX_FIELD ) );
		uint32_t req_count;
		uint32_t mod = 0;
		uint32_t i, k;
		if ( ( pool != NULL ) && ( temp != NULL ) )
		{
			memset( pool, 0, ( field_count - REMOVE_HEX_FIELD ) );
			exit_status = Convert_Ascii_To_Int( field_val, field_count, &actual_cnt );

			if ( exit_status == SUCCESS_VALUE )
			{
				req_count = field_count - REMOVE_HEX_FIELD;
				mod = req_count % 2;
				// printf("mod=%x\n",mod);

				for ( i = 0; i < req_count; i++ )
				{
					pool[i] = field_val[field_count - i - 1];
					// printf("pool=%x\n",pool[i]);
				}
				if ( mod )
				{
					for ( k = 0; k < mod; k++ )
					{
						pool[k + i] = 0;
					}
				}
				else
				{
					k = 0;
				}
				int32_t j;
				uint32_t count = 0;
				for ( j = ( req_count + mod - 1 ); j >= 0;)
				{
					if ( j > 0 )
					{
						temp[count] = pool[j - 1]
							| ( pool[j] << 4 );
					}
					else
					{
						temp[count] = pool[j];
					}

					printf( "ProductCode = %x,%x\n", temp[count], count );
					count++;
					j -= 2;
				}

				for ( j = 0; j < count; j++ )
				{
					gen_ltk_header[offset + j] = temp[count - 1 - j];
					printf( "ProductCode = %x,%x\n", gen_ltk_header[offset + j], offset );
				}
			}
		}

		if ( pool != NULL )
		{
			free( pool );
		}

		if ( temp != NULL )
		{
			free( temp );
		}
	}
	else
	{
		int32_t ret_offset_val = 0;
		uint32_t i = 0;
		uint32_t p;
		uint32_t mod = 0;
		uint32_t count = 0;

		uint32_t act_cnt = 0;
		// printf("no hex string\n");
		// printf("DEC len=%x\n",len);
		exit_status = Convert_Ascii_To_Int( field_val, field_count, ( uint16_t* )&count );
		// printf("Count==%x\n",count);
		// printf("status==%x\n",exit_status);

		if ( exit_status == SUCCESS_VALUE )
		{
			for ( i = 0; i < count; i++ )
			{
				ret_offset_val = ret_offset_val * 10 + field_val[i];
				// printf( "Dec=%x\n", field_val[i] );
			}

			// printf("decimal=%x\n",ret_offset_val);

			uint8_t* pool = ( uint8_t* ) ( malloc( 16 * sizeof( uint8_t ) ) );
			uint8_t* temp = ( uint8_t* ) ( malloc( 16 * sizeof( uint8_t ) ) );
			count = 0;
			if ( ( pool != NULL ) && ( temp != NULL ) )
			{
				memset( pool, 0, ( 16 * sizeof( uint8_t ) ) );
				memset( temp, 0, ( 16 * sizeof( uint8_t ) ) );
				i = 0;
				while ( ret_offset_val != 0 )
				{
					int16_t temp = 0;

					temp = ret_offset_val % 16;

					if ( temp < 10 )
					{
						pool[i] = temp + ( int16_t )0x30;
						i++;
					}
					else
					{
						pool[i] = temp + ( int16_t )0x37;
						i++;
					}
					ret_offset_val = ret_offset_val / 16;
					count++;
				}
				mod = count % 2;
				if ( mod )
				{

					for ( i = 0; i < count; i++ )
					{
						temp[i] = pool[count - i - 1];
						// printf("temp=%x,%x\n",temp[i],i);
					}
					for ( p = 0; p < mod; p++ )
					{
						temp[p + i] = 0;
						// printf("temp=%x,%x\n",temp[p+i],p);
					}
				}
				else
				{
					p = 0;
					for ( i = 0; i < count; i++ )
					{
						temp[p + i] = pool[i];
					}
				}
				int32_t j, k;
				// printf("act count=%x\n",count);
				// count = i;
				uint16_t req_count = 0;
				exit_status = Convert_Ascii_To_Int( temp, count, ( uint16_t* )&req_count );
				memset( pool, 0, ( 16 * sizeof( uint8_t ) ) );

				if ( exit_status == SUCCESS_VALUE )
				{
					i = 0;
					for ( k = ( count + mod - 1 ); k >= 0;)
					{
						if ( k > 0 )
						{
							pool[i] += ( temp[k] << 4 ) | temp[k - 1];
						}
						else
						{
							pool[i] += temp[k];
						}
						// printf("convert=%x\n",pool[i]);
						act_cnt++;
						i++;
						k -= 2;
					}

					// printf("act cnt=%x\n",act_cnt);

					for ( j = 0; j < act_cnt; j++ )
					{
						gen_ltk_header[offset + j] = pool[act_cnt - 1 - j];
						// printf("Decto HEx=%x,%x\n",gen_ltk_header[offset + j],j);
					}

					for (; j < len - act_cnt; j++ )
					{
						gen_ltk_header[offset + j] = pool[len - act_cnt - 1 - j];
						// printf("Decto HEx=%x,%x\n",gen_ltk_header[offset + j],j);
					}
				}

			}
			if ( pool != NULL )
			{
				free( pool );
			}

			if ( temp != NULL )
			{
				free( temp );
			}
		}
	}
	return ( exit_status );
}

/**
 * @brief: convert the ASCII value present in xml for the firmware version
 * @param[in] uint8_t* gen_ltk_header pointer to the header buffer
 * @param[in] uint8_t* field_val ASCII format value of the field
 * @param[in] uint8_t field_count count of the field to be copied in header
 * @param[in] uint32_t len actual length in the header for the field
 * @param[in] uint32_t offset location in header where field needs to be inserted
 * @param[in]  uint8_t* fmt_size actual size for the firmware version to be used in header
 * @param[out] uint8_t suceess or failure
 */
uint8_t Convert_Version( uint8_t* gen_ltk_header, uint8_t* field_val, uint8_t field_count,
						 uint32_t len, uint32_t offset, uint8_t* fmt_size )
{
	uint32_t i, j;
	int16_t ret_offset_val = 0;
	uint32_t dot_count = 0;
	uint32_t total_field_cnt = 0;
	uint8_t pool[256];
	uint8_t exit_status = SUCCESS_VALUE;

	memset( pool, 0, 256 * sizeof( uint8_t ) );

	uint32_t counter = 0;

	j = 0;
	for ( i = 0; i < field_count; i++ )
	{
		if ( Is_Decimal( field_val[i] ) )
		{
			field_val[i] = field_val[i] - 0x30;	/* ASCII To INT conversion */
			pool[counter] = field_val[i];
			// printf("ver=%d,%x\n",pool[counter],field_val[i]);
			counter++;
			total_field_cnt = total_field_cnt + 1;
		}
		else if ( Is_Upper_Hex_Char( field_val[i] ) )
		{
			field_val[i] = field_val[i] - 0x41 + 10;/* ASCII To INT conversion */
			pool[counter] = field_val[i];
			// printf("ver=%d,%x\n",pool[counter],field_val[i]);
			counter++;
			total_field_cnt = total_field_cnt + 1;
		}
		else if ( Is_Lower_Hex_Char( field_val[i] ) )
		{
			field_val[i] = field_val[i] - 0x61 + 10;/* ASCII To INT conversion */
			pool[counter] = field_val[i];
			// printf("ver=%d,%x\n",pool[counter],field_val[i]);
			counter++;
			total_field_cnt = total_field_cnt + 1;
		}

		if ( field_val[i] == '.' )
		{
			// field_val[i] = field_val[i];
			// printf( "j=%x,i=%x\n", j, i );
			for (; j < counter; j++ )
			{
				ret_offset_val = ret_offset_val * 10 + pool[j];
				// printf( "Field=%x\n", pool[j] );
			}
			// printf( "Dec=%x\n", ret_offset_val );
			dot_count = dot_count + 1;
			// printf( "dot=%x\n", dot_count );
			if ( fmt_size[dot_count - 1] == sizeof( uint8_t ) )
			{
				*( ( uint8_t* )( gen_ltk_header + offset ) ) = ret_offset_val;
				ret_offset_val = 0;
				offset += sizeof( uint8_t );
			}
			else if ( fmt_size[dot_count - 1] == sizeof( uint16_t ) )
			{
				*( ( uint16_t* )( gen_ltk_header + offset ) ) = ret_offset_val;
				ret_offset_val = 0;
				offset += sizeof( uint16_t );
			}
		}

		if ( field_val[i] > 0x0f )
		{

			/*dot character found */
			if ( field_val[i] == 0x2E )
			{
				continue;
			}
			else
			{
				exit_status = ERROR_VALUE;
				printf( "Not valid char=%x\n", field_val[i] );
				break;
			}

		}

	}

	// printf( "dot. j=%x,i=%x\n", j, i );
	if ( exit_status == SUCCESS_VALUE )
	{
		for (; j < counter; j++ )
		{
			ret_offset_val = ret_offset_val * 10 + pool[j];
		}
		// printf( "Dec=%x\n", ret_offset_val );
		if ( fmt_size[dot_count] == sizeof( uint8_t ) )
		{
			*( ( uint8_t* )( gen_ltk_header + offset ) ) = ret_offset_val;
			ret_offset_val = 0;
			offset += sizeof( uint8_t );
		}
		else if ( fmt_size[dot_count] == sizeof( uint16_t ) )
		{
			*( ( uint16_t* )( gen_ltk_header + offset ) ) = ret_offset_val;
			ret_offset_val = 0;
			offset += sizeof( uint16_t );
		}
	}
	return ( exit_status );

}

/**
 * @brief: retrieve the field value from the file specified in xml
 * @param[in] uint8_t* gen_ltk_header pointer to the header buffer
 * @param[in] uint8_t* field_val ASCII format value of the field
 * @param[in] uint8_t field_count count of the field to be copied in header
 * @param[in] uint32_t len actual length in the header for the field
 * @param[in] uint32_t offset location in header where field needs to be inserted
 * @param[in]  uint8_t* fmt_size actual size for the firmware version to be used in header
 * @param[out] uint8_t Success or Failure
 */
uint8_t Header_Parser( const uint8_t* header_tag, const uint8_t* file_path, uint8_t* header_val )
{
	static uint8_t return_value = ERROR_VALUE;	/* error value set */
	FILE* fWebini = NULL;
	uint8_t* fbuf = NULL;
	uint32_t file_size = 0;
	uint16_t i = 0;

	uint8_t path[1024] = {0};

	while ( *file_path != '<' )
	{
		path[i] = *file_path;
		file_path = file_path + 1;
		i++;
	}
	path[i + 1] = '\0';
	fWebini = fopen( path, "r+" );
	if ( fWebini != NULL )
	{
		uint16_t i;
		fseek( fWebini, 0, SEEK_END );
		file_size = ftell( fWebini );
		// printf( "parsed file size =%x\n", file_size );
		fbuf = ( uint8_t* ) ( malloc( file_size ) );
		fseek( fWebini, SEEK_SET, 0 );
		// memset( fbuf, 0, file_size);
		if ( fbuf != NULL )
		{
			for ( i = 0; i < file_size; i++ )
			{
				fbuf[i] = fgetc( fWebini );
				if ( feof( fWebini ) )
				{
					printf( "File EOF reached" );
					break;
				}
			}
			fbuf[i] = '\0';
		}
		return_value = 0;
	}
	else
	{
		printf( "file open error\n" );
	}
	// printf("file content=%s\n",fbuf);
	if ( return_value != ERROR_VALUE )
	{
		uint8_t* header_fmt_ver = NULL;

		uint8_t* tmp = ( uint8_t* ) ( strstr( ( const char* )fbuf,
											  ( const char* ) ( header_tag ) ) );

		// printf("file parse=%s\n",tmp);
		if ( tmp != NULL )
		{
			i = 0;
			uint16_t count = 0;
			header_fmt_ver = tmp + strlen( ( const char* ) ( header_tag ) ) + 1;
			while ( *header_fmt_ver != '\n' )
			{
				/* check for space and if found increment pointer*/
				if ( *header_fmt_ver == 0x20 )
				{
					header_fmt_ver++;
					// printf(" space found \n");
				}
				else
				{
					header_val[i] = *header_fmt_ver;
					// printf("file field=%x\n",header_val[i]);
					header_fmt_ver++;
					i++;
					count++;
				}
			}

			return_value = count;
		}
	}

	if ( fbuf != NULL )
	{
		free( fbuf );
	}
	fclose( fWebini );
	return ( return_value );
}

/**
 * @brief: retrieve the file length from the string buffer passed as argument
 * @param[in] uint8_t field_count count of the field to be copied in header
 * @param[in] uint8_t* field_value ASCII format value of the field
 * @param[in] uint32_t generic_header_size size of the generic header
 * @param[in] struct padd_struct* filepadding padding structure to be filled
 * @param[out] uint8_t Success or Failure
 */
uint8_t Get_file_length( uint8_t field_count,  uint8_t* field_value, uint32_t generic_header_size,
						 struct padd_struct* filepadding )
{
	uint8_t i = 0;
	uint32_t count = 0;
	uint8_t exit_status = EXIT_FAILURE;

	filepadding->padding_flag = false;

	uint8_t* length = ( uint8_t* ) ( malloc( field_count * sizeof( uint8_t ) ) );

	// copy field value to length variable
	for ( i = 0; i < field_count; i++ )
	{
		length[i] = field_value[i];
	}

	// convert ASCII array to decimal
	exit_status = Convert_Ascii_To_Int( length, field_count, ( uint16_t* )&count );

	if ( exit_status != SUCCESS_VALUE )
	{
		free( length );
		return ( exit_status );
	}

	// Calculate the file length
	for ( i = 0; i < count; i++ )
	{
		filepadding->file_length = filepadding->file_length * 10 + length[i];
	}
	// printf( "File Length\n = %x",  filepadding->file_length);

	free( length );
	return ( exit_status );
}

int main( int argc, const char* argv[] )
{
	uint32_t file_size = 0;	/* in bytes */
	char input_file_name[FILE_NAME_SIZE];
	char output_file_name[FILE_NAME_SIZE];
	uint8_t in_out_same = 0;
	uint8_t exit_status = SUCCESS_VALUE;
	uint16_t generic_header_size = 0;
	uint8_t in_arg_num = 0;
	uint8_t in_xml_arg_num = 0;
	uint8_t out_arg_num = 0;
	uint8_t header_arg_num = 0;
	uint8_t header_arg_default_value = 0;
	uint8_t generic_header_default_value = 0;
	uint16_t n = 0;
	struct padd_struct file_padding = {0};

	/*		printf( " list Input arguments = %d", argc );
	        for ( n = 0; n < argc; n++ )
	        {
	            printf( "%s\n", argv[n] );
	        }*/

	if ( argc == 2 )
	{
		if ( ( strcmp( argv[1], "-help" ) == 0 ) )
		{
			printf( "*********** Usage of generic header utility *************************************\n"
					"*********** ltk_header_gen.exe arg1 arg2 arg3 ***********************************\n"
					"*********** arg1 = input web bin file name **************************************\n"
					"*********** arg2 = output web bin file name *************************************\n"
					"*********** arg3 = web_header_linker_xml file ***********************************\n" );
		}
		exit_status = HELP_OPTION;
	}
	/* Open www.bin for editing */
	else if ( argc >= 4 )
	{

		for ( n = 0; n < argc; n++ )
		{
			// printf( "%s\n", argv[n] );
			if ( strcmp( INPUT_ARG, argv[n] ) == 0 )
			{
				in_arg_num = n + 1;
				// printf("in_arg_num=%x\n",in_arg_num);
			}
			else if ( strcmp( OUTPUT_ARG, argv[n] ) == 0 )
			{
				out_arg_num = n + 1;
				// printf("out_arg_num=%x\n",out_arg_num);
			}
			else if ( strcmp( INPUT_XML, argv[n] ) == 0 )
			{
				in_xml_arg_num = n + 1;
				// printf("in_xml_arg_num=%x\n",in_xml_arg_num);
			}
			else if ( strcmp( INPUT_HEADER_SIZE, argv[n] ) == 0 )
			{
				header_arg_num = n + 1;
				if ( header_arg_num <= argc )
				{
					exit_status = SUCCESS_VALUE;
				}
				else
				{
					exit_status = ERROR_ARG_MISSING;
				}
				// printf("header_arg_num=%x\n",header_arg_num);
			}
			else if ( strcmp( INPUT_HEADER_DEFAULT_VALUE, argv[n] ) == 0 )
			{
				header_arg_default_value = n + 1;
				if ( header_arg_default_value <= argc )
				{
					exit_status = SUCCESS_VALUE;
				}
				else
				{
					exit_status = ERROR_ARG_MISSING;
				}
				// printf("header_arg_default_value=%x\n",header_arg_default_value);
			}
		}

		if ( in_arg_num )
		{
			fIn = fopen( argv[in_arg_num], "rb" );	/* If no input argument is provided try to open www.bin file*/
		}

		// printf( "Opening Default argument file" );
		strcpy( input_file_name, argv[in_arg_num] );
		puts( input_file_name );
		strcpy( output_file_name, argv[out_arg_num] );
		if ( 0 != strcmp( output_file_name, input_file_name ) )
		{
			if ( out_arg_num )
			{
				fOut = fopen( argv[out_arg_num], "wb" );/* Create a file named xxxx as asked by user */
				printf( "out and input are diff\n" );
			}
		}
		else
		{
			fOut = fopen( "web.bin", "wb" );/* Default file named web.bin sahll be created */
			strcpy( output_file_name, "web.bin" );
			in_out_same = 1;/* Input output file name are same */
		}
		puts( output_file_name );
		// printf( "Number of Input arguments = %d ", argc );
		// printf( "Size of LTk header = %d", generic_header_size );
	}
	else
	{
		/* return error as expected argumemnt count is less*/
		exit_status = ERROR_VALUE;
		/*		fIn = fopen( argv[1], "rb" );  Argument 0 is the input file name strcpy( input_file_name, argv[1] );
		        puts( input_file_name );*/
	}

	if ( exit_status == SUCCESS_VALUE )
	{
		if ( NULL == fOut )
		{
			fprintf( stderr, "failed to open file: %s, exiting ...\n", "web.bin" );
			return ( EXIT_FAILURE );// @TODO single return
			/* Unable to open the file */
		}
		if ( NULL == fIn )
		{
			fprintf( stderr, "failed to open file: %s, exiting ...\n", "www.bin" );
			return ( EXIT_FAILURE );// @TODO single return
			/* Unable to open the file */
		}
		else
		{
			/*We have successfully opened up bin file for edit purpose */
			if ( 0U == fseek( fIn, 0, SEEK_END ) )
			{
				file_size = ftell( fIn );
				real_fw_header_ltk* new_ltk_header;
				/* pointer to hold generic header */
				real_fw_header_ltk* gen_ltk_header = NULL;
				uint8_t* fw_header_pos = NULL;
				uint8_t* dest_pool = NULL;
				printf( "\nFileSize=%x\n", file_size );

				/* Dynamic memory allocation for output file */
				uint8_t* pool = ( uint8_t* ) ( malloc( file_size ) );	// @TODO header size
				if ( header_arg_num == 0 )
				{
					generic_header_size = HEADER_SIZE;
				}
				else
				{
					generic_header_size = atoi( argv[header_arg_num] );
					printf( "header size=%x\n", atoi( argv[header_arg_num] ) );
				}
				// printf( "Total destpool=%x\n",
				// file_size + generic_header_size + 2 * sizeof( uint8_t ) );
				uint8_t* pool_ptr_volatile = 0;
				uint8_t bin_header = 0;
				uint16_t crc_checksum = 0;

				if ( NULL == pool )
				{
					/* Unable to allocate Dyanamic memory */
					return ( EXIT_FAILURE );// @TODO single return
				}

				/* Seek the pointer to the strat of the file */
				fseek( fIn, SEEK_SET, 0 );

				/* Read the whole binary file and put the data into dynamically allocated buffer */
				fread( pool, file_size, 1, fIn );

				pool_ptr_volatile = pool + sizeof( fw_header );

				// printf( "CRC = %x\n", crc_checksum );
				printf( "BCM Header Charector  = %x %x\n", *( pool_ptr_volatile ), *( pool_ptr_volatile + 1 ) );
				/* lets calculate the CRC */
				if ( ( ( DATA_START & 0xff ) == ( *( pool_ptr_volatile ) ) ) &&
					 ( ( ( DATA_START >> 8 ) & 0xff ) == ( *( pool_ptr_volatile + 1 ) ) ) )
				{
					/* BCM Header does exist */
					// printf( "BCM Header exists. Replacing it with LTK header.." );
					bin_header = 1;	/* Value = 1 signifies that BCM header exist in the code */
				}
				else
				{
					pool_ptr_volatile = pool + generic_header_size;
					if ( ( ( DATA_START & 0xff ) == ( *( pool_ptr_volatile ) ) ) &&
						 ( ( ( DATA_START >> 8 ) & 0xff ) == ( *( pool_ptr_volatile + 1 ) ) ) )
					{
						bin_header = 2;	/* Value = 2 signifies that LTK header exist in the code and we dont have to do
										   anything */
					}
					else
					{
						bin_header = 0;	/* Value = 0 signifies that No header exist in the code and we have to add LTk
										   header */
					}
				}

				if ( 1 == bin_header )	// BCM header exists
				{
					printf( "BCM Header exists. Replacing it with LTK header..\n" );
					printf( "memorypool=%x\n",
							( file_size - sizeof( fw_header ) + generic_header_size ) );
					/***************Code for Gen XML*******************************/
					fIni = fopen( argv[in_xml_arg_num], "rb" );
					if ( fIni != NULL )
					{
						fseek( fIni, 0, SEEK_END );
						uint32_t xml_file_size = ftell( fIni );
						uint8_t* xml_pool = ( uint8_t* ) ( malloc( xml_file_size + 1 ) );	// @TODO header
						// size
						if ( NULL == xml_pool )
						{
							/* Unable to allocate Dyanamic memory */
							return ( EXIT_FAILURE );// @TODO single return
						}
						gen_ltk_header = ( real_fw_header_ltk* )( malloc( generic_header_size ) );
						fw_header_pos = ( uint8_t* )( ( uint8_t* ) gen_ltk_header + 4 * sizeof( uint8_t ) );
						if ( header_arg_default_value == 0 )
						{
							generic_header_default_value = HEADER_DEFAULT_VALUE;
							memset( gen_ltk_header, generic_header_default_value,
									( generic_header_size ) );
						}
						else
						{
							generic_header_default_value = atoi( argv[header_arg_default_value] );
							printf( "generic_header_default_value=%x\n", generic_header_default_value );
							memset( gen_ltk_header, generic_header_default_value,
									( generic_header_size ) );
						}
						/* Seek the pointer to the strat of the file */
						fseek( fIni, SEEK_SET, 0 );
						fread( xml_pool, xml_file_size, 1, fIni );
						xml_pool[xml_file_size] = '\0';
						uint8_t* ptr = xml_pool;
						uint8_t field_count = 0;
						uint32_t offset = 0;
						uint32_t length = 0;
						uint8_t Format_type = 0;
						uint8_t constant_count = 0;
						uint8_t dot_count = 0;
						uint32_t field_len = file_size - sizeof( fw_header )
							+ generic_header_size;
						uint8_t Field_value[256] = {0};
						uint8_t total_rcv_field_count = 0;
						uint8_t* dot_ptr = NULL;
						uint8_t crc_present = 0;
						uint32_t crc_offset = 0;
						// printf( "Last char=%s\n", ptr );
						while ( *ptr != 0 )
						{
							if ( ( ptr = ( ( uint8_t* ) ( strstr( ( const char* )ptr,
																  ( const char* ) ( Gen_Tags[
																						FIELD_START
																					] ) ) ) ) ) !=
								 NULL )
							{
								field_count++;
								ptr = ptr + strlen( Gen_Tags[FIELD_START] );
							}
							else
							{
								printf( "BREAK WHILE here=%s\n", ptr );
								break;
							}

							ptr = ptr + 1;
						}

						// printf( "Field found =%x\n", field_count );
						uint8_t i;
						ptr = xml_pool;
						for ( i = 0; i < field_count; i++ )
						{
							// printf("Header info=%s\n",ptr);
							if ( ( ptr = ( ( uint8_t* ) ( strstr( ( const char* )ptr,
																  ( const char* ) ( Gen_Tags[
																						HEADER_INFO
																					] ) ) ) ) ) !=
								 NULL )
							{
								ptr = ptr + strlen( Gen_Tags[HEADER_INFO] ) + 1;
								// printf("Header info=%s\n",ptr);
								if ( strncmp( ptr, "Constant", 8 ) == 0 )
								{
									ptr = ptr + strlen( "Constant" ) + 2;

									// printf("DECimal=%s\n",ptr);
									uint16_t k = 0;
									total_rcv_field_count = 0;
									while ( *ptr != '<' )
									{
										Field_value[k] = *ptr;
										// printf("dec=%x\n",Field_value[k]);
										k++;
										ptr++;
										total_rcv_field_count++;
									}

								}
								else
								{
									/* xml values to be parsed here */

									uint8_t header_tag[256] = {0};
									uint16_t k = 0;
									while ( *ptr != '"' )
									{
										header_tag[k] = *ptr;
										ptr = ptr + 1;
										k++;
									}
									header_tag[k + 1] = '\0';
									ptr = ptr + 2;
									if ( strcmp( "CRC", header_tag ) == 0 )
									{
										// printf("header msg=%s\n",header_tag);
										crc_present = 1;
									}
									else if ( strcmp( "MagicEndian", header_tag ) == 0 )
									{
										constant_count++;
										printf( "Magic endian=%x\n", constant_count );
									}
									else if ( strcmp( "FileLength", header_tag ) == 0 )
									{
										if ( strncmp( ptr, "INTERNAL", 8 ) == 0 )
										{
											constant_count++;
											printf( "FileLength=%x\n", constant_count );
										}
										else
										{
											constant_count++;
											total_rcv_field_count = Header_Parser( header_tag, ptr, Field_value );
											/* file length should be in decimal and within the 32bit integer range. File
											   length should not be smaller than the file size */
											if ( ( total_rcv_field_count >= MIN_FILE_LENGTH_COUNT ) &&
												 ( total_rcv_field_count <= MAX_FILE_LENGTH_COUNT ) )
											{
												file_padding.file_size = file_size;
												Get_file_length( total_rcv_field_count, Field_value,
																 generic_header_size,
																 &file_padding );
												file_padding.padding_offset = file_padding.file_size -
													sizeof( fw_header ) + generic_header_size;
												if ( file_padding.file_length > file_padding.padding_offset )
												{
													file_padding.padding_count = file_padding.file_length -
														file_padding.padding_offset;
													field_len = file_size - sizeof( fw_header ) + generic_header_size +
														file_padding.padding_count;
													if ( file_padding.padding_count >= 1U )
													{
														file_padding.padding_flag = true;
													}
												}
											}
										}
									}
									else
									{
										constant_count = 0;
										total_rcv_field_count = Header_Parser( header_tag, ptr,
																			   Field_value );
									}

								}

							}
							else
							{
								printf( "header not found=%s\n", ptr );
							}

							if ( ( ptr = ( ( uint8_t* ) ( strstr( ( const char* )ptr,
																  ( const char* ) ( Gen_Tags[OFFSET] ) ) ) ) )
								 !=
								 NULL )
							{
								ptr = ptr + strlen( Gen_Tags[OFFSET] );
								if ( crc_present == 0 )
								{
									offset = Offset_Length_Conv( ptr );
								}
								else
								{
									uint8_t offset_tag[64] = {0};
									uint16_t k = 0;
									uint8_t* offset_ptr = ptr;
									while ( *ptr != '<' )
									{
										offset_tag[k] = *ptr;
										ptr = ptr + 1;
										k++;
									}
									offset_tag[k + 1] = '\0';
									// printf("Tag offset=%s\n",offset_tag);
									for ( k = 0; k < ( MAX_OFFSET_TAGS + 1 ); k++ )
									{
										if ( strcmp( ( const char* ) ( OFFSET_Tags[k] ),
													 offset_tag ) == 0 )
										{
											// printf("header msg=%s\n,%x",offset_tag,k);
											offset = k;
											break;
										}
									}
									// printf("value of count=%x\n",k);

									if ( k == 3 )
									{
										offset = Offset_Length_Conv( offset_ptr );
										// printf("CRC Ptr=%x\n",offset);
									}
								}
								// printf("offset=%x\n",offset);
							}
							else
							{
								/* OFFSET not found*/
								printf( "offset not found=%s\n", ptr );
								return ( EXIT_FAILURE );
							}

							if ( ( ptr = ( ( uint8_t* ) ( strstr( ( const char* )ptr,
																  ( const char* ) ( Gen_Tags[LENGTH] ) ) ) ) )
								 !=
								 NULL )
							{
								ptr = ptr + strlen( Gen_Tags[LENGTH] );
								length = Offset_Length_Conv( ptr );
								// printf("Length=%x\n",length);
							}
							else
							{
								/* Length not found*/
								printf( "length not found=%s\n", ptr );
								return ( EXIT_FAILURE );
							}

							if ( ( ptr = ( ( uint8_t* ) ( strstr( ( const char* )ptr,
																  ( const char* ) ( Gen_Tags[FORMAT] ) ) ) ) )
								 !=
								 NULL )
							{
								ptr = ptr + strlen( Gen_Tags[FORMAT] );
								uint8_t* ver = ptr;
								Format_type = Format_Type_Ret( ptr, &dot_count );

								if ( dot_count )
								{
									// printf("ver=%s\n",ver);
									uint8_t format_size = 0;
									uint8_t str_count = 0;
									dot_ptr = ( uint8_t* ) ( malloc( dot_count + 1 ) );
									while ( *ver != '/' )
									{
										str_count = str_count + 1;
										if ( ( *ver == '.' ) || ( *ver == '<' ) )
										{
											dot_ptr[format_size] = ( str_count - 1 ) / 2;
											printf( "dot_ptr =%x.%x\n", dot_ptr[format_size],
													format_size );
											format_size = format_size + 1;
											str_count = 0;
										}
										ver = ver + 1;
									}
									dot_ptr[format_size] = ( str_count ) / 2;
									// printf("dot_ptr =%x.%x\n",dot_ptr[format_size],format_size);
								}
								// printf("FORMAT_type=%x, dot= %x\n",Format_type,dot_count);
								dot_count = 0;
							}
							else
							{
								/*Format not found*/
								printf( "Format not found=%s\n", ptr );
								return ( EXIT_FAILURE );
							}

							if ( constant_count == 1 )
							{
								*( ( uint32_t* )( fw_header_pos ) ) = MAGIC_ENDIAN;
							}

							if ( constant_count == 2 )
							{
								*( ( uint32_t* )( fw_header_pos + 4 ) ) = field_len;
							}

							switch ( Format_type )
							{
								case FORMAT_NONE:
									if ( ( constant_count == 0 ) && ( crc_present == 0 ) )
									{
										exit_status = Convert_Hex( fw_header_pos, Field_value,
																   total_rcv_field_count,
																   length, offset );
									}
									break;

								case FORMAT_ASCII:
									Copy_Ascii( fw_header_pos, Field_value, total_rcv_field_count,
												length, offset );
									break;

								case FORMAT_VERSION:
									exit_status = Convert_Version( fw_header_pos, Field_value,
																   total_rcv_field_count,
																   length, offset, dot_ptr );
									break;

								default:
									break;
							}
							memset( Field_value, 0, sizeof( Field_value ) );

							if ( exit_status == ERROR_VALUE )
							{
								break;
							}
							if ( crc_present == 1 )
							{
								// get CRC address here for top, bottom, and address
								crc_offset = offset;
								// printf("CRC offset=%u\n",crc_offset);
								crc_present = 0;
							}

						}

						// printf("Parsing completed\n");
						if ( exit_status == SUCCESS_VALUE )
						{

							uint32_t p = 0;

							fseek( fIn, SEEK_SET, 0 );
							fread( pool, file_size, 1, fIn );

							if ( file_padding.padding_flag == true )
							{
								dest_pool =
									( uint8_t* ) ( malloc( file_size + generic_header_size
														   + file_padding.padding_count + 2 * sizeof( uint8_t ) ) );
							}
							else
							{
								dest_pool =
									( uint8_t* ) ( malloc( file_size + generic_header_size + 2 *
														   sizeof( uint8_t ) ) );
							}
							/* 2 bytes added in dest_pool incase crc needs to be added last */

							uint8_t* crc_ptr = ( uint8_t* )( dest_pool );

							uint8_t* new_pool = dest_pool;
							uint8_t* data_pool = pool;
							uint8_t* new_header = ( uint8_t* ) gen_ltk_header;
							for ( p = 0; p < ( generic_header_size ); p++ )
							{
								*new_pool = *( new_header );
								new_pool++;
								new_header++;
							}

							new_pool = ( dest_pool + generic_header_size );
							for ( p = 0; p < ( file_size - sizeof( fw_header ) ); p++ )
							{
								*new_pool = *( data_pool + sizeof( fw_header ) );
								new_pool++;
								data_pool++;
							}

							if ( file_padding.padding_flag == true )
							{
								new_pool = ( dest_pool + file_padding.padding_offset );
								memset( new_pool, PADD_VALUE, file_padding.padding_count );
								file_size += file_padding.padding_count;
							}


							if ( crc_offset == TOP_FW )
							{
								crc_checksum = Gen_Crc(
									( const uint8_t* ) ( dest_pool + sizeof( crc_info_ltk ) ),
									( file_size - sizeof( fw_header )
									  + generic_header_size - sizeof( crc_info_ltk ) ),
									0 );
								/*BCM header exists*/
								*( ( uint16_t* )( crc_ptr ) ) = crc_checksum;
								printf( "TOPCRC value=%x\n", crc_checksum );
								printf( "header  = %x\n", *( ( uint8_t* )pool_ptr_volatile ) );
								/* Write the data into the file */
								uint32_t write_count = 0;
								write_count = fwrite( dest_pool, 1,
													  ( file_size + generic_header_size -
														sizeof( fw_header ) ), fOut );
								printf( "write count =%x\n", write_count );
							}
							else if ( crc_offset == BOTTOM_FW )
							{
								crc_checksum = Gen_Crc(
									( const uint8_t* ) ( dest_pool + sizeof( crc_info_ltk ) ),
									( file_size - sizeof( fw_header )
									  + generic_header_size - sizeof( crc_info_ltk ) ),
									0 );
								*( ( uint16_t* )( crc_ptr + file_size +
												  generic_header_size -
												  sizeof( fw_header ) ) ) = crc_checksum;
								printf( "BOTCRC value=%x,bottom=%x\n", crc_checksum,
										( file_size + generic_header_size -
										  sizeof( fw_header ) ) );
								printf( "header  = %x\n", *( ( uint8_t* )pool_ptr_volatile ) );
								/* Write the data into the file*/
								uint32_t write_count = 0;
								write_count = fwrite( dest_pool, 1,
													  ( file_size + generic_header_size -
														sizeof( fw_header ) + 2 * sizeof( uint8_t ) ),
													  fOut );
								printf( "write count =%x\n", write_count );
							}
							else
							{
								crc_checksum = Gen_Crc(
									( const uint8_t* ) ( dest_pool + sizeof( crc_info_ltk ) ),
									( file_size - sizeof( fw_header )
									  + generic_header_size - sizeof( crc_info_ltk ) ),
									0 );
								/*BCM header exists*/
								*( ( uint16_t* )( crc_ptr + crc_offset ) ) = crc_checksum;
								printf( "OFFSETCRC value=%x,offset=%x\n", crc_checksum,
										crc_offset );
								printf( "header  = %x\n", *( ( uint8_t* )pool_ptr_volatile ) );
								/* Write the data into the file */
								uint32_t write_count = 0;
								write_count = fwrite( dest_pool, 1,
													  ( file_size + generic_header_size -
														sizeof( fw_header ) ), fOut );
								printf( "write count =%x\n", write_count );
							}

							// uint16_t k;
							// for ( k = 0; k < generic_header_size; k++ )
							// {
							// printf( "gen_header[%x]=%x\n", k, dest_pool[k] );
							// }
							free( pool );
							free( dest_pool );
							free( xml_pool );
							free( dot_ptr );
							free( gen_ltk_header );
							fclose( fIn );
							fclose( fOut );
							fclose( fIni );
						}
						else
						{
							printf( " Exited due to Parsing ERROR \n" );
						}
					}
					else
					{
						printf( "ERROR in file opening \n" );
					}

				}
				else
				{
					printf( "BCM Header does not Exists in the Code\n" );
					printf( "memorypool=%x\n",
							( file_size + generic_header_size ) );
					/***************Code for Gen XML*******************************/
					fIni = fopen( argv[in_xml_arg_num], "rb" );
					if ( fIni != NULL )
					{
						fseek( fIni, 0, SEEK_END );
						uint32_t xml_file_size = ftell( fIni );
						uint8_t* xml_pool = ( uint8_t* ) ( malloc( xml_file_size + 1 ) );	// @TODO header
						// size
						if ( NULL == xml_pool )
						{
							/* Unable to allocate Dyanamic memory */
							return ( EXIT_FAILURE );// @TODO single return
						}
						gen_ltk_header = ( real_fw_header_ltk* )( malloc( generic_header_size ) );
						fw_header_pos = ( uint8_t* )( ( uint8_t* ) gen_ltk_header + 4 * sizeof( uint8_t ) );
						if ( header_arg_default_value == 0 )
						{
							generic_header_default_value = HEADER_DEFAULT_VALUE;
							memset( gen_ltk_header, generic_header_default_value,
									( generic_header_size ) );
						}
						else
						{
							generic_header_default_value = atoi( argv[header_arg_default_value] );
							printf( "generic_header_default_value=%x\n", generic_header_default_value );
							memset( gen_ltk_header, generic_header_default_value,
									( generic_header_size ) );
						}
						/* Seek the pointer to the strat of the file */
						fseek( fIni, SEEK_SET, 0 );
						fread( xml_pool, xml_file_size, 1, fIni );
						xml_pool[xml_file_size] = '\0';
						uint8_t* ptr = xml_pool;
						uint8_t field_count = 0;
						uint32_t offset = 0;
						uint32_t length = 0;
						uint8_t Format_type = 0;
						uint8_t constant_count = 0;
						uint8_t dot_count = 0;
						uint32_t field_len = file_size + generic_header_size;
						uint8_t Field_value[256] = {0};
						uint8_t total_rcv_field_count = 0;
						uint8_t* dot_ptr = NULL;
						uint8_t crc_present = 0;
						uint32_t crc_offset = 0;
						// printf( "Last char=%s\n", ptr );
						while ( *ptr != 0 )
						{
							if ( ( ptr = ( ( uint8_t* ) ( strstr( ( const char* )ptr,
																  ( const char* ) ( Gen_Tags[
																						FIELD_START
																					] ) ) ) ) ) !=
								 NULL )
							{
								field_count++;
								ptr = ptr + strlen( Gen_Tags[FIELD_START] );
							}
							else
							{
								printf( "BREAK WHILE here=%s\n", ptr );
								break;
							}

							ptr = ptr + 1;
						}

						// printf( "Field found =%x\n", field_count );
						uint8_t i;
						ptr = xml_pool;
						for ( i = 0; i < field_count; i++ )
						{
							// printf("Header info=%s\n",ptr);
							if ( ( ptr = ( ( uint8_t* ) ( strstr( ( const char* )ptr,
																  ( const char* ) ( Gen_Tags[
																						HEADER_INFO
																					] ) ) ) ) ) !=
								 NULL )
							{
								ptr = ptr + strlen( Gen_Tags[HEADER_INFO] ) + 1;
								// printf("Header info=%s\n",ptr);
								if ( strncmp( ptr, "Constant", 8 ) == 0 )
								{
									ptr = ptr + strlen( "Constant" ) + 2;

									// printf("DECimal=%s\n",ptr);
									uint16_t k = 0;
									total_rcv_field_count = 0;
									while ( *ptr != '<' )
									{
										Field_value[k] = *ptr;
										// printf("dec=%x\n",Field_value[k]);
										k++;
										ptr++;
										total_rcv_field_count++;
									}

								}
								else
								{
									/* xml values to be parsed here */

									uint8_t header_tag[256] = {0};
									uint16_t k = 0;
									while ( *ptr != '"' )
									{
										header_tag[k] = *ptr;
										ptr = ptr + 1;
										k++;
									}
									header_tag[k + 1] = '\0';
									ptr = ptr + 2;
									if ( strcmp( "CRC", header_tag ) == 0 )
									{
										// printf("header msg=%s\n",header_tag);
										crc_present = 1;
									}
									else if ( strcmp( "MagicEndian", header_tag ) == 0 )
									{
										constant_count++;
										printf( "Magic endian=%x\n", constant_count );
									}
									else if ( strcmp( "FileLength", header_tag ) == 0 )
									{
										if ( strncmp( ptr, "INTERNAL", 8 ) == 0 )
										{
											constant_count++;
											printf( "FileLength=%x\n", constant_count );
										}
										else
										{
											constant_count++;
											total_rcv_field_count = Header_Parser( header_tag, ptr, Field_value );
											/* file length should be in decimal and within the 32bit integer range. File
											   length should not be smaller than the file size */
											if ( ( total_rcv_field_count >= MIN_FILE_LENGTH_COUNT ) &&
												 ( total_rcv_field_count <= MAX_FILE_LENGTH_COUNT ) )
											{
												file_padding.file_size = file_size;
												Get_file_length( total_rcv_field_count, Field_value,
																 generic_header_size,
																 &file_padding );
												file_padding.padding_offset = file_padding.file_size +
													generic_header_size;
												if ( file_padding.file_length > file_padding.padding_offset )
												{
													file_padding.padding_count = file_padding.file_length -
														file_padding.padding_offset;
													field_len = file_size + generic_header_size +
														file_padding.padding_count;
													if ( file_padding.padding_count >= 1U )
													{
														file_padding.padding_flag = true;
													}
												}
											}
										}
									}
									else
									{
										constant_count = 0;
										total_rcv_field_count = Header_Parser( header_tag, ptr,
																			   Field_value );
									}

								}

							}
							else
							{
								printf( "header not found=%s\n", ptr );
							}

							if ( ( ptr = ( ( uint8_t* ) ( strstr( ( const char* )ptr,
																  ( const char* ) ( Gen_Tags[OFFSET] ) ) ) ) )
								 !=
								 NULL )
							{
								ptr = ptr + strlen( Gen_Tags[OFFSET] );
								if ( crc_present == 0 )
								{
									offset = Offset_Length_Conv( ptr );
								}
								else
								{
									uint8_t offset_tag[64] = {0};
									uint16_t k = 0;
									uint8_t* offset_ptr = ptr;
									while ( *ptr != '<' )
									{
										offset_tag[k] = *ptr;
										ptr = ptr + 1;
										k++;
									}
									offset_tag[k + 1] = '\0';
									// printf("Tag offset=%s\n",offset_tag);
									for ( k = 0; k < ( MAX_OFFSET_TAGS + 1 ); k++ )
									{
										if ( strcmp( ( const char* ) ( OFFSET_Tags[k] ),
													 offset_tag ) == 0 )
										{
											// printf("header msg=%s\n,%x",offset_tag,k);
											offset = k;
											break;
										}
									}
									// printf("value of count=%x\n",k);

									if ( k == 3 )
									{
										offset = Offset_Length_Conv( offset_ptr );
										// printf("CRC Ptr=%x\n",offset);
									}
								}
								// printf("offset=%x\n",offset);
							}
							else
							{
								/* OFFSET not found*/
								printf( "offset not found=%s\n", ptr );
								return ( EXIT_FAILURE );
							}

							if ( ( ptr = ( ( uint8_t* ) ( strstr( ( const char* )ptr,
																  ( const char* ) ( Gen_Tags[LENGTH] ) ) ) ) )
								 !=
								 NULL )
							{
								ptr = ptr + strlen( Gen_Tags[LENGTH] );
								length = Offset_Length_Conv( ptr );
								// printf("Length=%x\n",length);
							}
							else
							{
								/* Length not found*/
								printf( "length not found=%s\n", ptr );
								return ( EXIT_FAILURE );
							}

							if ( ( ptr = ( ( uint8_t* ) ( strstr( ( const char* )ptr,
																  ( const char* ) ( Gen_Tags[FORMAT] ) ) ) ) )
								 !=
								 NULL )
							{
								ptr = ptr + strlen( Gen_Tags[FORMAT] );
								uint8_t* ver = ptr;
								Format_type = Format_Type_Ret( ptr, &dot_count );

								if ( dot_count )
								{
									// printf("ver=%s\n",ver);
									uint8_t format_size = 0;
									uint8_t str_count = 0;
									dot_ptr = ( uint8_t* ) ( malloc( dot_count + 1 ) );
									while ( *ver != '/' )
									{
										str_count = str_count + 1;
										if ( ( *ver == '.' ) || ( *ver == '<' ) )
										{
											dot_ptr[format_size] = ( str_count - 1 ) / 2;
											printf( "dot_ptr =%x.%x\n", dot_ptr[format_size],
													format_size );
											format_size = format_size + 1;
											str_count = 0;
										}
										ver = ver + 1;
									}
									dot_ptr[format_size] = ( str_count ) / 2;
									// printf("dot_ptr =%x.%x\n",dot_ptr[format_size],format_size);
								}
								// printf("FORMAT_type=%x, dot= %x\n",Format_type,dot_count);
								dot_count = 0;
							}
							else
							{
								/*Format not found*/
								printf( "Format not found=%s\n", ptr );
								return ( EXIT_FAILURE );
							}

							if ( constant_count == 1 )
							{
								*( ( uint32_t* )( fw_header_pos ) ) = MAGIC_ENDIAN;
							}

							if ( constant_count == 2 )
							{
								*( ( uint32_t* )( fw_header_pos + 4 ) ) = field_len;
							}

							switch ( Format_type )
							{
								case FORMAT_NONE:
									if ( ( constant_count == 0 ) && ( crc_present == 0 ) )
									{
										exit_status = Convert_Hex( fw_header_pos, Field_value,
																   total_rcv_field_count,
																   length, offset );
									}
									break;

								case FORMAT_ASCII:
									Copy_Ascii( fw_header_pos, Field_value, total_rcv_field_count,
												length, offset );
									break;

								case FORMAT_VERSION:
									exit_status = Convert_Version( fw_header_pos, Field_value,
																   total_rcv_field_count,
																   length, offset, dot_ptr );
									break;

								default:
									break;
							}
							memset( Field_value, 0, sizeof( Field_value ) );

							if ( exit_status == ERROR_VALUE )
							{
								break;
							}
							if ( crc_present == 1 )
							{
								// get CRC address here for top, bottom, and address
								crc_offset = offset;
								// printf("CRC offset=%u\n",crc_offset);
								crc_present = 0;
							}

						}

						// printf("Parsing completed\n");
						if ( exit_status == SUCCESS_VALUE )
						{
							uint32_t p = 0;

							fseek( fIn, SEEK_SET, 0 );
							fread( pool, file_size, 1, fIn );

							if ( file_padding.padding_flag == true )
							{
								dest_pool =
									( uint8_t* ) ( malloc( file_size + generic_header_size
														   + file_padding.padding_count + 2 * sizeof( uint8_t ) ) );
							}
							else
							{
								dest_pool =
									( uint8_t* ) ( malloc( file_size + generic_header_size + 2 *
														   sizeof( uint8_t ) ) );
							}
							/* 2 bytes added in dest_pool incase crc needs to be added last */

							uint8_t* crc_ptr = ( uint8_t* )( dest_pool );

							uint8_t* new_pool = dest_pool;
							uint8_t* data_pool = pool;
							uint8_t* new_header = ( uint8_t* ) gen_ltk_header;
							for ( p = 0; p < ( generic_header_size ); p++ )
							{
								*new_pool = *( new_header );
								new_pool++;
								new_header++;
							}

							new_pool = ( dest_pool + generic_header_size );
							for ( p = 0; p < ( file_size ); p++ )
							{
								*new_pool = *( data_pool );
								new_pool++;
								data_pool++;
							}

							if ( file_padding.padding_flag == true )
							{
								new_pool = ( dest_pool + file_padding.padding_offset );
								memset( new_pool, PADD_VALUE, file_padding.padding_count );
								file_size += file_padding.padding_count;
							}


							if ( crc_offset == TOP_FW )
							{
								crc_checksum = Gen_Crc(
									( const uint8_t* ) ( dest_pool + sizeof( crc_info_ltk ) ),
									( file_size + generic_header_size - sizeof( crc_info_ltk ) ),
									0 );
								/*BCM header exists*/
								*( ( uint16_t* )( crc_ptr ) ) = crc_checksum;
								printf( "TOPCRC value=%x\n", crc_checksum );
								printf( "header  = %x\n", *( ( uint8_t* )pool_ptr_volatile ) );
								/* Write the data into the file */
								uint32_t write_count = 0;
								write_count = fwrite( dest_pool, 1,
													  ( file_size + generic_header_size ), fOut );
								printf( "write count =%x\n", write_count );
							}
							else if ( crc_offset == BOTTOM_FW )
							{
								crc_checksum = Gen_Crc(
									( const uint8_t* ) ( dest_pool + sizeof( crc_info_ltk ) ),
									( file_size + generic_header_size - sizeof( crc_info_ltk ) ),
									0 );
								*( ( uint16_t* )( crc_ptr + file_size +
												  generic_header_size /*-
																	     sizeof( fw_header )*/) ) = crc_checksum;
								printf( "BOTCRC value=%x,bottom=%x\n", crc_checksum,
										( file_size + generic_header_size ) );
								printf( "header  = %x\n", *( ( uint8_t* )pool_ptr_volatile ) );
								/* Write the data into the file*/
								uint32_t write_count = 0;
								write_count = fwrite( dest_pool, 1,
													  ( file_size + generic_header_size + 2 * sizeof( uint8_t ) ),
													  fOut );
								printf( "write count =%x\n", write_count );
							}
							else
							{
								crc_checksum = Gen_Crc(
									( const uint8_t* ) ( dest_pool + sizeof( crc_info_ltk ) ),
									( file_size + generic_header_size - sizeof( crc_info_ltk ) ),
									0 );
								/*BCM header exists*/
								*( ( uint16_t* )( crc_ptr + crc_offset ) ) = crc_checksum;
								printf( "OFFSETCRC value=%x,offset=%x\n", crc_checksum,
										crc_offset );
								printf( "header  = %x\n", *( ( uint8_t* )pool_ptr_volatile ) );
								/* Write the data into the file */
								uint32_t write_count = 0;
								write_count = fwrite( dest_pool, 1,
													  ( file_size + generic_header_size ), fOut );
								printf( "write count =%x\n", write_count );
							}

							/*							uint16_t k;
							                            for ( k = 0; k < generic_header_size; k++ )
							                            {
							                                printf( "gen_header[%x]=%x\n", k, dest_pool[k] );
							                            }*/
							free( pool );
							free( dest_pool );
							free( xml_pool );
							free( dot_ptr );
							free( gen_ltk_header );
							fclose( fIn );
							fclose( fOut );
							fclose( fIni );
						}
						else
						{
							printf( " Exited due to Parsing ERROR \n" );
						}
					}
					else
					{
						printf( "ERROR in file opening \n" );
					}

				}

				if ( exit_status == SUCCESS_VALUE )
				{
					uint8_t ret = 0;
					if ( argc >= 4 )/*output file name missing*/
					{
						/*By default we will create www.bin file*/
						remove( argv[1] );
					}
					else// if(0 == strcmp(input_file_name, output_file_name)) /* Input and Output
						// file
						// name
						// are equal /
					{
						if ( 1 == in_out_same )
						{
							uint8_t file_delete_status = 0;
							/*Delete the old file from file system*/
							file_delete_status = remove( input_file_name );

							ret = rename( output_file_name, input_file_name );	/*old name new name*/
							if ( ret == 0 )
							{
								printf( "File renamed successfully" );
							}
							else
							{
								printf( "Error: Unable to rename the file" );
							}
						}
					}
				}
			}
		}
	}
	else
	{
		if ( exit_status == ERROR_VALUE )
		{
			printf( " EXIT - > Expected argument count is less \n" );
		}
		else if ( exit_status == HELP_OPTION )
		{
			printf( " EXIT - > HELP OPTION USED \n" );
		}
		else
		{
			printf( " EXIT - > ARGUMENTS MISSING \n" );
		}
	}
	return ( 0 );
}
