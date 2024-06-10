/**
 *****************************************************************************************
 * @file CRC_Generator_Main.cpp
 * @details A generic utility to calculate crc and add the same in output file
 *          Refer ReadMe.txt for module overview.
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include <iostream>
#include <fstream>
#include <string>
#include <strings.h>
#include <cstdlib>
#include <conio.h>

#define BIN_ARG "-bin"
#define OUT_ARG "-out"
#define CRC_OFFSET_ARG "-offset"
#define ALGO_ARG "-algo"
#define DATA_START_ARG "-data_start"
#define DATA_LEN_ARG "-data_len"
#define DATA_END_ARG "-data_end"
#define DATA_LOG_ARG "-data_log"

#define HELP_OPTION "-help"
#define SUCCESS_VALUE 0x01
#define ERROR_VALUE 0xFF
#define HELP_VALUE 0xAA
#define MAX_ALGO_TYPE 1

using namespace std;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
unsigned int Get_File_Size( ifstream & );

bool Concat_Files( char*, char* );

bool Write_CRC16( char*, unsigned long int, unsigned long int, unsigned long int );

unsigned short int Gen_Crc16( const char* data, unsigned long int length, unsigned int start );

/* Global variables */
unsigned int data_log_flag = 0U;
typedef union
{
	unsigned short int u16;
	unsigned char u8[2];
} SPLIT_UINT16;

int main( int argc, char** argv )
{

	unsigned int arg_n = 0U;
	unsigned int in_arg_bin_num = 0U;
	unsigned int in_arg_out_num = 0U;
	unsigned int in_arg_offset_num = 0U;
	unsigned int in_arg_algo_num = 0U;
	unsigned int in_arg_start_num = 0U;
	unsigned int in_arg_len_num = 0U;
	unsigned int in_arg_log_num = 0U;

	unsigned int exit_status = SUCCESS_VALUE;
	bool ret_state = false;

	cerr << "argc=" << argc << endl;

	if ( argc == 2 )
	{
		if ( strcmp( argv[1], HELP_OPTION ) == 0U )
		{
			cerr << " Usage of CRC Generator : " << endl;
			cerr	<<
				" CRC_Generator.exe -bin bin file -out output file -offset crc offset -algo algorithm type -data_start data start offset -data_log Flag to enable data log -data_len data length "
					<< endl;
			cerr << "   1. -bin bin file" << endl;
			cerr << "   2. -out output file " << endl;
			cerr << "   3. -offset crc offset " << endl;
			cerr << "   4. -algo algorithm type " << endl;
			cerr << "   5. -data_start data start offset " << endl;
			cerr << "   6. -data_log Flag to enable data log " << endl;
			cerr << "   7. -data_len data length (optional) " << endl;
		}

		exit_status = HELP_VALUE;
	}
	else if ( ( argc == 13U ) || ( argc == 15U ) )
	{
		for ( arg_n; arg_n < argc; arg_n++ )
		{
			if ( strcmp( argv[arg_n], BIN_ARG ) == 0U )
			{
				in_arg_bin_num = arg_n + 1;
				cerr << "Bin found at " << in_arg_bin_num << endl;
			}
			else if ( strcmp( argv[arg_n], OUT_ARG ) == 0U )
			{
				in_arg_out_num = arg_n + 1;
				cerr << "Out path found at " << in_arg_out_num << endl;
			}
			else if ( strcmp( argv[arg_n], CRC_OFFSET_ARG ) == 0U )
			{
				in_arg_offset_num = arg_n + 1;
				cerr << "CRC offset found at " << in_arg_offset_num << endl;
			}
			else if ( strcmp( argv[arg_n], ALGO_ARG ) == 0U )
			{
				in_arg_algo_num = arg_n + 1;
				cerr << "Algorithm type found at " << in_arg_algo_num << endl;
			}
			else if ( strcmp( argv[arg_n], DATA_START_ARG ) == 0U )
			{
				in_arg_start_num = arg_n + 1;
				cerr << "Data start found at " << in_arg_start_num << endl;
			}
			else if ( strcmp( argv[arg_n], DATA_LOG_ARG ) == 0U )
			{
				in_arg_log_num = arg_n + 1;
				cerr << "Data log found at " << in_arg_log_num << endl;
			}
			else if ( strcmp( argv[arg_n], DATA_LEN_ARG ) == 0U )
			{
				in_arg_len_num = arg_n + 1;
				cerr << "Data length found at " << in_arg_len_num << endl;
			}
		}
		/* data_len is optional, therefore this check is required in case data_len is provided but other argument is
		   missing */
		if ( ( in_arg_bin_num == 0U ) || ( in_arg_out_num == 0U ) || ( in_arg_offset_num == 0U ) ||
			 ( in_arg_algo_num == 0U ) || ( in_arg_start_num == 0U ) || ( in_arg_log_num == 0U ) )
		{
			exit_status = ERROR_VALUE;
		}
	}
	else
	{
		exit_status = ERROR_VALUE;
	}

	if ( exit_status == SUCCESS_VALUE )
	{
		if ( ( in_arg_bin_num ) && ( in_arg_out_num ) )
		{
			cerr << "Input bin file path : " << argv[in_arg_bin_num] << endl;
			cerr << "Output bin file path : " << argv[in_arg_out_num] << endl;
			if ( strcmp( argv[in_arg_bin_num], argv[in_arg_out_num] ) == 0U )
			{
				cerr << "Invalid file path, input/output file path and file name is same" << endl;
			}
			else
			{
				ifstream fIn;
				fIn.open( argv[in_arg_bin_num], ios::in | ios::binary );
				if ( !fIn )
				{
					cerr << "Opening binary file failed" << endl;
				}
				else
				{
					bool ret_state = false;
					bool param_valid = false;
					unsigned long int algo_type = 0U;
					unsigned long int crc_offset = 0U;
					unsigned long int start = 0U;
					unsigned long int length = 0U;

					unsigned int bin_size = Get_File_Size( fIn );
					cerr << "Input bin file size = " << dec << bin_size << " Hex: " << hex << bin_size << endl;
					fIn.close();

					/* Copy input bin file to the output bin file */
					ret_state = Concat_Files( argv[in_arg_bin_num], argv[in_arg_out_num] );

					if ( ret_state == true )
					{
						/* Validate offset parameters */
						param_valid = true;

						/* Algorithm type, CRC16 : 1, CRC32 : 2 */
						algo_type = strtoul( argv[in_arg_algo_num], NULL, 0 );
						cerr << "Algorithm type = " << dec << algo_type << " Hex: " << hex << algo_type << endl;
						if ( algo_type > MAX_ALGO_TYPE )
						{
							param_valid = false;
							cerr << "Invalid algorithm type" << endl;
						}

						/* Offset from the start of file where crc should be updated */
						crc_offset = strtoul( argv[in_arg_offset_num], NULL, 0 );
						cerr << "CRC offset = " << dec << crc_offset << " Hex: " << hex << crc_offset << endl;
						if ( crc_offset >= bin_size )
						{
							param_valid = false;
							cerr << "Invalid crc offset" << endl;
						}

						/* Start address and length of data on which crc is calculated */
						start = strtoul( argv[in_arg_start_num], NULL, 0 );
						cerr << "Start offset = " << dec << start << " Hex: " << hex << start << endl;
						if ( start >= bin_size )
						{
							param_valid = false;
							cerr << "Invalid start offset" << endl;
						}

						/* Data log flag, this flag is used to enable or disable printing the data buffer on which crc
						   is calculated */
						data_log_flag = strtoul( argv[in_arg_log_num], NULL, 0 );
						cerr << "Data log flag = " << dec << data_log_flag << " Hex: " << hex << data_log_flag << endl;
						if ( 0U > data_log_flag > 1U )
						{
							param_valid = false;
							cerr << "Invalid data log value" << endl;
						}

						/* Length parameter is optional, if not available then length is caludated based on bin file
						   size */
						if ( in_arg_len_num == 0U )
						{
							length = bin_size - start;
						}
						else
						{
							length = strtoul( argv[in_arg_len_num], NULL, 0 );
							if ( length == 0U )
							{
								length = bin_size - start;
							}
						}
						cerr << "Data length = " << dec << length << " Hex: " << hex << length << endl;
						if ( length > bin_size )
						{
							param_valid = false;
							cerr << "Invalid data length" << endl;
						}
						else if ( ( start + length ) > bin_size )
						{
							param_valid = false;
							cerr << "Invalid data length or start offset" << endl;
						}
					}
					else
					{
						cerr << "Failed to copy input bin file to the output bin file" << endl;
					}

					if ( param_valid == true )
					{
						if ( algo_type == 1U )
						{
							/* Get data from output bin file into data buffer, calculate crc on data buffer and write
							   crc to output bin file */
							Write_CRC16( argv[in_arg_out_num], crc_offset, start, length );
						}
						else
						{
							cerr << "Invalid CRC algorithm type" << endl;
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
			cerr << " EXIT -> Expected Argument count is less " << endl;
		}
		else if ( exit_status == HELP_VALUE )
		{
			cout << " EXIT -> HELP option used " << endl;
		}
		else
		{
			cerr << "EXIT -> Argument count missing " << endl;
		}

		if ( exit_status != HELP_VALUE )
		{
			cerr << "argc=" << argc << endl;
			for ( int i = 0; i < argc; i++ )
			{
				cerr << "argv[" << i + 1 << "]" << argv[i] << endl;
			}
		}
	}
	return ( 0U );
}

unsigned int Get_File_Size( ifstream& file )
{
	file.seekg( 0, ios::end );
	int fileSize = file.tellg();

	file.seekg( 0, ios::beg );
	return ( fileSize );
}

bool Concat_Files( char* input_file_name, char* op_file_name )
{
	bool ret_state = false;
	ifstream input_file( input_file_name, ios::binary );

	if ( !input_file )
	{
		cerr << "Cannot Open Input File = " << input_file_name << endl;
	}
	else
	{
		ofstream output_file( op_file_name, ios::out | ios::binary | ios::ate );

		if ( !output_file )
		{
			cerr << "Cannot Open Output File" << endl;
		}
		else
		{

			if ( input_file.is_open() && output_file.is_open() )
			{
				char c;
				while ( !input_file.eof() )
				{
					c = input_file.get();
					if ( !input_file.eof() )
					{
						output_file.put( c );
					}
				}
				cerr << "Copied  " << input_file_name << " Succesfully." << endl;
				ret_state = true;
			}
			// Close both files
			input_file.close();
			output_file.close();

		}
	}
	return ( ret_state );
}

bool Write_CRC16( char* output_bin_name, unsigned long int crc_offset,  unsigned long int start,
				  unsigned long int length )
{
	bool ret_state = false;
	fstream bin_file( output_bin_name, ios::in | ios::out | ios::binary | ios::ate | ios::app );

	if ( !bin_file )
	{
		cerr << "Cannot Open output file = " << output_bin_name << endl;
	}
	else
	{
		char* data_buffer = new char[length];
		if ( !data_buffer )
		{
			cerr << "Memory allocation failed for data buffer" << endl;
		}
		else
		{
			/* Set the position of the pointer to start of data on which crc is calculated */
			bin_file.clear();
			bin_file.seekp( start, ios::beg );
			/* Verify position of pointer */
			unsigned long int cur_ptr = bin_file.tellg();
			cout << "Current position of pointer = " << dec << cur_ptr << endl;

			unsigned long int i = 0U;
			unsigned char c;
			/* Read character from bin file and put into data buffer */
			for ( i; i < length; i++ )
			{
				c = bin_file.get();

				if ( data_log_flag == 1U )
				{

					printf( " %x ", c );// Enable this log to verify data on which crc is calculated
				}
				if ( !bin_file.eof() )
				{
					data_buffer[i] = c;
				}
			}
			/* Check current position of pointer */
			cur_ptr = bin_file.tellg();
			cout << "Current position of pointer = " << dec << cur_ptr << endl;
			bin_file.close();

			/* Calculate crc on data buffer */
			unsigned short int crc = 0U;
			crc = Gen_Crc16( data_buffer, length, 0U );
			cout << " Calculated CRC = " << dec << crc << " Hex : " << hex << crc << endl;

			/* Write 16 bit crc in output bin file */
			ofstream out( output_bin_name, ios::in | ios::out | ios::binary );
			if ( !out )
			{
				cerr << "Cannot Open output file to write crc " << output_bin_name << endl;
			}
			else
			{
				out.seekp( crc_offset, ios::beg );
				out.write( reinterpret_cast<const char*>( &crc ), sizeof( unsigned short int ) );
				out.close();
				cout << "Updated CRC to output file \"" << output_bin_name << "\" Succesfully" << endl;
				ret_state = true;
			}
			delete[] data_buffer;
		}

	}
	return ( ret_state );
}

/* CRC generation code directly used from fwtool.c */
unsigned short int Gen_Crc16( const char* data, unsigned long int length, unsigned int start )
{
	unsigned char temp;
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
