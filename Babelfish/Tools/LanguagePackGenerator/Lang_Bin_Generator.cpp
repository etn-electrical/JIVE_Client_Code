/**
 **********************************************************************************************
 * @file            Lang_Bin_Generator.cpp
 *
 * @brief           Create language binary file
 * @details         Get the language header from INI file, get language block information from JSON file  
 * 					and append all languages information in single binary file
 *
 * @copyright       2020 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <strings.h>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <sstream>

/* Macros for input arguments */
#define HEADER_ARG "-header_file"
#define INI_ARG "-ini_path"
#define OUT_ARG "-out"
#define HELP_OPTION "-help"

/* Macros for exit_status */
#define SUCCESS_VALUE 0x01
#define ERROR_VALUE 0xFF
#define HELP_VALUE 0xAA
#define INVALID_VALUE 0xBB

/* Macros for max value check */
#define LG_CODE_MAX_BYTES 8     ///< Maximum bytes for language code, keep it same as in
	                            //RTK_Example\Code\Prod_Spec_REST\Language_Config.h
#define LG_NAME_MAX_BYTES 32    ///< Maximum bytes for language name, keep it same as in
	                            //RTK_Example\Code\Prod_Spec_REST\Language_Config.h
#define JSON_PATH_MAX_BYTES 1024 ///< Maximum bytes for language json file path
#define JSON_FILE_READ_MAX_BYTES 256 ///< Maximum bytes to be read at a time from language json file

using namespace std;

/* Structure to hold specifications of language (Language header) */
struct lg_header_struct_t
{
	unsigned char spec_version;   ///< Version of language specification
	unsigned char id;             ///< Language ID
	unsigned char format;         ///< Language format e.g Jason, XML
	unsigned char compression;    ///< 0:no compression, 1:method 1, 2:method 2
	unsigned char code[LG_CODE_MAX_BYTES]; ///< Language code e.g. "en" for English, "de" for
	//German
	unsigned char name[LG_NAME_MAX_BYTES]; ///< Language name e.g  English, German
	unsigned int length;        ///< Language block length excluding language header
	unsigned int next_lg;       ///< Next language address offset, '0' if not available
};

unsigned int Get_File_Size( ifstream & file );

bool Get_Language_File_Path( unsigned char* json_path, const char* lang_header_ini_file );

bool Get_Language_Header( lg_header_struct_t* lg_header_s, const char* lang_header_ini_file );

bool Write_Info( const char* lang_header_ini_file, ofstream & bin_file );

int lg_header_ptr = 0U;

/**
 * @brief Get size of the file
 * @param[in] file : ifstream object.
 * @return Size of file
 */
unsigned int Get_File_Size( ifstream & file )
{
	file.seekg( 0U, ios::end );
	unsigned int file_size = file.tellg();
	file.seekg( 0U, ios::beg );
	return ( file_size );
}

/**
 * @brief Read path of language json file from ini file having language header
 * @param[in] lang_header_ini_file : Pointer to ini file path string
 * @param[out] json_path : Path of json file
 * @return Success or failure of requested operation.
 */
bool Get_Language_File_Path( unsigned char* json_path, const char* lang_header_ini_file )
{
	bool ret_val = false;

	if ( GetPrivateProfileString( "Language Header Information",  "LANGUAGE_JSON_PATH",
								  NULL, reinterpret_cast<char*>( json_path ), JSON_PATH_MAX_BYTES,
								  lang_header_ini_file ) != 0U )
	{
		cout << "    Language json file Path : " << json_path << endl;
		ret_val = true;
	}

	return ( ret_val );
}

/**
 * @brief Read language header information from ini file
 * @param[in] lang_header_ini_file : Pointer to ini file path string
 * @param[out] lg_header_s : Language header structure pointer
 * @return Success or failure of requested operation.
 */
bool Get_Language_Header( lg_header_struct_t* lg_header_s, const char* lang_header_ini_file )
{
	bool ret_val = false;

	lg_header_s->spec_version =
		static_cast<unsigned char>( GetPrivateProfileInt( "Language Header Information",
														  "Language_Spec_Version", 0U,
														  lang_header_ini_file ) );
	lg_header_s->id =
		static_cast<unsigned char>( GetPrivateProfileInt( "Language Header Information",
														  "Language_ID",
														  0U, lang_header_ini_file ) );
	lg_header_s->format =
		static_cast<unsigned char>( GetPrivateProfileInt( "Language Header Information",  "Format",
														  0U,
														  lang_header_ini_file ) );
	lg_header_s->compression =
		static_cast<unsigned char>( GetPrivateProfileInt( "Language Header Information",
														  "Compression",
														  0U, lang_header_ini_file ) );

	cout << "    Language_Spec_Version : " << static_cast<unsigned int>( lg_header_s->spec_version ) <<
		endl;
	cout << "    Language_ID : " << static_cast<unsigned int>( lg_header_s->id ) << endl;
	cout << "    Format : " << static_cast<unsigned int>( lg_header_s->format ) << endl;
	cout << "    Compression : " << static_cast<unsigned int>( lg_header_s->compression ) << endl;

	/* GetPrivateProfileString(..) returns number of characters copied to the buffer, not including
	   the terminating null character. */
	if ( GetPrivateProfileString( "Language Header Information",  "Language_Code",
								  NULL, reinterpret_cast<char*>( lg_header_s->code ),
								  LG_CODE_MAX_BYTES, lang_header_ini_file ) != 0U )
	{
		if ( GetPrivateProfileString( "Language Header Information",  "Language_Name",
									  NULL, reinterpret_cast<char*>( lg_header_s->name ),
									  LG_NAME_MAX_BYTES, lang_header_ini_file ) != 0U )
		{
			cout << "    Language_Code : " << lg_header_s->code << endl;
			cout << "    Language_Name : " << lg_header_s->name << endl;
			ret_val = true;
		}
	}
	return ( ret_val );
}

/**
 * @brief Write language header (ini) and corresponding language block (json) in output bin file,
 *        single language information is updated at a time.
 * @param[in] lang_header_ini_file : Pointer to ini file path string
 * @param[in] bin_file : output binary file object 
 * @return Success or failure of requested operation.
 */
bool Write_Info( const char* lang_header_ini_file, ofstream & bin_file )
{
	bool ret_status = false;
	bool header_info_ready = false;
	unsigned char json_path[JSON_PATH_MAX_BYTES];
	unsigned int json_file_size = 0U;
	lg_header_struct_t lg_header_s = {};

	cerr << "lang_header_ini_file-->  " << lang_header_ini_file << endl;

	/* Read language header information and JSON file path from ini file  */
	if ( Get_Language_Header( &lg_header_s, lang_header_ini_file ) == true )
	{
		if ( Get_Language_File_Path( json_path, lang_header_ini_file ) == true )
		{
			header_info_ready = true;
			//cout << "INI file (header information) read successfully " << endl;
		}
		else
		{
			cerr << "ERROR : Get Language json file Path failed " << endl;
		}
	}
	else
	{
		cerr << "ERROR : Get Language Header failed" << endl;
	}

	if ( header_info_ready == true )
	{
		ifstream json_file( reinterpret_cast<const char*>( json_path ), ios::in | ios::binary );
		//cout << "json_file get pointer = " << json_file.tellg() << endl;

		if ( !json_file )
		{

			cerr << "ERROR : Cannot open language json file" << endl;
			return ( false );
		}
		else
		{
			unsigned int i = 0U;

			/*  1. Calculate language block size and next language offset.
				2. Write language header in binary file.
				3. Read data from JSON file (JSON_FILE_READ_MAX_BYTES at a time) and write in binary file.
				4. Close JSON file.
			*/
			if ( json_file.is_open() )
			{
				json_file_size = Get_File_Size( json_file );
				lg_header_s.length = json_file_size;
				cout << "    Length of language block : " << lg_header_s.length << endl;
				
				lg_header_s.next_lg = sizeof( lg_header_struct_t ) + lg_header_s.length;
				cout << "    Next language address offset (next_lg) : " << lg_header_s.next_lg << endl;

				unsigned int i = 0U;

				if ( bin_file.is_open() )
				{
					// cout << "Language header pointer = " << bin_file.tellp() << endl;
					bin_file.write( reinterpret_cast<const char*>( &lg_header_s ),
									sizeof( lg_header_struct_t ) );
					cout << "Language header added successfully" << endl;
					lg_header_ptr = bin_file.tellp();
					cout << "bin_file put pointer = " << lg_header_ptr << endl;
		
					unsigned int read_len = JSON_FILE_READ_MAX_BYTES;
					if ( read_len > json_file_size )
					{
						read_len = json_file_size;
					}

					unsigned int i = 0U;
					char json_file_buffer[read_len] = {0U};

					while ( i < json_file_size )
					{
						json_file.read( json_file_buffer, read_len );
						bin_file.write( reinterpret_cast<const char*>( json_file_buffer ),
										read_len );
						i = i + read_len;
						if ( read_len > ( json_file_size - i ) )
						{
							read_len = ( json_file_size - i );
						}
					}
					ret_status = true;
					cout << "Language block added successfully" << endl;
					cout << "bin_file put pointer = " << bin_file.tellp() << endl;
				}
				else
				{
					cerr << " ERROR : Cannot open Binary File " << endl;
				}
				json_file.close();
			}
			else
			{
				cerr << "ERROR : Cannot open language json file " << endl;
			}
		}
	}
	return ( ret_status );
}

/**
 * @brief Update next language address offset( next_lg ) of last language header. 
 * Offset is updated to zero, to indicate that this is last language in language data(binary file) 
 * Call this function after adding all languages in a binary file.
 * @param[in] bin_file : output binary file object 
 * @return Success or failure of requested operation.
 */
bool Update_Language_Info_End_Offset( ofstream & bin_file )
{
	bool ret_status = false;
	unsigned int offset = 0U;
	if ( bin_file.is_open() )
	{
		// cout << "lg_header_ptr = " << lg_header_ptr << endl;
		bin_file.seekp( lg_header_ptr - sizeof( offset ) );
		// cout << "bin_file put pointer = " << bin_file.tellp() << endl;
		bin_file.write( reinterpret_cast<char*>( &offset ), sizeof( offset ) );
		// cout << "Language End offset updated to zero at " << bin_file.tellp() << endl;
		ret_status = true;
	}
	else
	{
		cerr << " ERROR : Cannot open Binary File to updated end offset " << endl;
	}
	return ( ret_status );
}

/**
 * @brief Search for ini file in lang_header_ini_path, add language information if ini file is found.
 * @param[in] lang_header_ini_path : Folder path where all language ini files are stored
 * @param[in] bin_file : output binary file object 
 * @return Success or failure of requested operation.
 */
bool Add_Language_Info( const char* lang_header_ini_path, ofstream & bin_file )
{
	bool ret_status = false;
	DIR* di;
	struct dirent* dir;
	char *file_name, *file_extension;
	int retn;
	bool lg_info_updated = false;

	di = opendir( lang_header_ini_path );
	if ( di )
	{
		while ( ( dir = readdir( di ) ) != NULL )
		{
			file_name = strtok( dir->d_name, "." );
			file_extension = strtok( NULL, "." );
			if ( file_extension != NULL )
			{
				retn = strcmp( file_extension, "ini" );
				if ( retn == 0 )
				{
					stringstream ss;
					ss << lang_header_ini_path << file_name << ".ini";
					string str = ss.str();
					const char* ini_file = str.c_str();
					
					if ( Write_Info( ini_file, bin_file ) == true )
					{
						lg_info_updated = true;
						cout << 
						"Language info added successfully ------------------------------------------------------------- \n"
								<< endl;
						ret_status = true;
					}
					else
					{
						ret_status = false;
						cerr << "ERROR : Language info write failed " << endl;
						break;
					}
				}
			}
		}
		if ( ( ret_status == true ) && ( lg_info_updated == true ) )
		{
			if ( Update_Language_Info_End_Offset( bin_file ) != true )
			{
				ret_status = false;
			}
		}
		closedir( di );
	}
	else
	{
		cout << "ERROR : Cannot open language header ini file" << endl;
	}
	return ( ret_status );
}

int main( int argc, char** argv )
{

	unsigned int arg_n = 0U;
	unsigned int in_arg_ini_num = 0U;
	unsigned int in_arg_out_num = 0U;
	unsigned int exit_status = SUCCESS_VALUE;

	bool ret_state = false;

	/*
	   cout << "You have entered " << argc
	     << " arguments:" << "\n";

	   for (int i = 0; i < argc; ++i)
	    cout << argv[i] << "\n";
	 */
	if ( argc == 2 )
	{
		if ( strcmp( argv[1], HELP_OPTION ) == 0U )
		{
			cout << " usage of Language bin generator : " << endl;
			cout << " Lang_Bin_Generator.exe -ini Language_Header_INI_file -out Output_file" << endl;
			cout << "   1. -ini_path Language Header INI file path " << endl;
			cout << "   2. -out Output binary file " << endl;
		}

		exit_status = HELP_VALUE;
	}
	else if ( argc == 5U )
	{
		for ( arg_n; arg_n < argc; arg_n++ )
		{
			if ( strcmp( argv[arg_n], INI_ARG ) == 0U )
			{
				in_arg_ini_num = arg_n + 1;
				cout << "ini path found at arg " << arg_n << endl;
			}
			else if ( strcmp( argv[arg_n], OUT_ARG ) == 0U )
			{
				in_arg_out_num = arg_n + 1;
				cout << "out path found at arg " << arg_n << endl;
			}
		}
		if ( ( in_arg_ini_num == 0U ) || ( in_arg_out_num == 0U ) )
		{
			exit_status = INVALID_VALUE;
		}
	}
	else
	{
		exit_status = ERROR_VALUE;
	}

	if ( exit_status == SUCCESS_VALUE )
	{
		/* Delete the out binary file is exists  */
		if ( remove( argv[in_arg_out_num] ) != 0U )
		{
			//cerr<<"ERROR : out bin file does not exists"<<endl;
		}
		else
		{
			//cout << "out bin file deleted " << endl;
		}

		ofstream bin_file( argv[in_arg_out_num], ios::out | ios::binary );
		cout << "bin_file put pointer = " << bin_file.tellp() << endl;

		if ( !bin_file )
		{

			cerr << "ERROR : Cannot Open Output bin File" << endl;
			return ( false );
		}
		else
		{
			Add_Language_Info( argv[in_arg_ini_num], bin_file );
			bin_file.close();
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
		else if ( exit_status == INVALID_VALUE )
		{
			cout << " EXIT -> Invalid arguments " << endl;
		}
		else
		{
			cerr << "EXIT -> Argument count missing " << endl;
		}
		if ( exit_status != HELP_VALUE )
		{
			cerr << "argc=" << argc << endl;
			for ( int i = 0U; i < argc; i++ )
			{
				cerr << "argv[" << i + 1U << "]" << argv[i] << endl;
			}
		}
	}
	return ( 0U );
}
