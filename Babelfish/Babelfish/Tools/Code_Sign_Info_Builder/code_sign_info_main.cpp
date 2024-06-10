#include <iostream>
#include <fstream>
#include <string>
#include <strings.h>
#include <cstdlib>
#include <conio.h>

#ifndef FVK_KEY_INCL
#define FVK_KEY_INCL
#endif

#define BIN_ARG "-bin"
#define FVK_ARG "-fvkc"
#define FVK_KEY_ARG "-fvk"
#define IVK_ARG "-ivkc"
#define FWSIGN_ARG "-fwsign"
#define INI_ARG "-ini"
#define OUT_ARG "-out"
#define RVK_KEY_ARG "-rvk"
#define CROSS_SIGN_ARG "-CS"

#define HELP_OPTION "-help"
#define SUCCESS_VALUE 0x01
#define ERROR_VALUE 0xFF
#define HELP_VALUE 0xAA

#define FVK_CERT_SIZE 1022
#define IVK_CERT_SIZE 1534
#define FW_SIGN_SIZE 142

#define RVK_KEY_MAX_SIZE 162
#define FVK_KEY_MAX_SIZE 162

using namespace std;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
unsigned int verStringToNum(char* fvk_str);
unsigned int getFileSize(ifstream&);
void appendSignature(char*);
bool concatFiles(char* , char* );
void generateFWPack(char*, char*, char*);
char* SubstringFileName(char*, char *);
bool validateVersionString(char*);
void padBytes(char * op_file_name);
struct code_sign_info_header;
bool Code_header_Update(code_sign_info_header*, char* input_fvk_cert_name, char* input_ivk_cert_name, char * op_file_name, char * fvk_key_name, char * rvk_key_name );
bool Extract_Ini_info(code_sign_info_header* append_header_info,const char* ini_file_name);
unsigned char Convert_Hex( unsigned char* gen_ltk_header, unsigned char* field_val, unsigned char field_count, unsigned int len,
                           unsigned int offset );
/* Global variables */
ifstream fIn;
ofstream fOut;
unsigned char version[4];
char output_file_name[256];
/* struct for code sign info header */
struct code_sign_info_header
{
	unsigned int code_sign_info_identity_mark;
	unsigned short int code_sign_header_version_num;
	unsigned char cross_sign_status;
	unsigned char algo_type;
	unsigned char ecc_curve_type;
	unsigned char key_size;
	unsigned char hash_size;
	unsigned char reserved;
	unsigned char fvk_key[FVK_KEY_MAX_SIZE];
	unsigned short int fvk_key_len;	
	unsigned char FVK[FVK_CERT_SIZE];
	unsigned short int FVK_size;
	unsigned char IVK[IVK_CERT_SIZE];
	unsigned short int IVK_size;
	unsigned char rvk_key[RVK_KEY_MAX_SIZE];
	unsigned short int rvk_key_len;	
};

int main(int argc, char** argv)
{

	unsigned int arg_n = 0U;
	unsigned int in_arg_bin_num = 0U;
	unsigned int in_arg_fvk_num = 0U;
	unsigned int in_arg_fvk_key_num = 0U;
	unsigned int in_arg_ivk_num = 0U;
	unsigned int in_arg_fwsign_num = 0U;
	unsigned int in_arg_ini_num = 0U;
	unsigned int in_arg_out_num = 0;
	unsigned int in_arg_rvk_key_num = 0U;
	unsigned int exit_status = SUCCESS_VALUE;

	bool ret_state = false;
	code_sign_info_header code_sign_info_header_s = {};

	if(argc == 2  )
	{
		if ( strcmp(argv[1],HELP_OPTION) == 0U)
		{
			cerr<<" usage of Code Sign Info builder : "<<endl;
#ifdef FVK_KEY_INCL
			cerr<<"code_sign_info.exe -bin bin_file -fvkc FVK_cert_file -fvk FVK_KEY_file -ivkc IVK_cert_file -ini Code_Sign_INI_file -rvk RVK_KEY_FILE -out Output_file -CS Y"<<endl;
			cerr<<"   1. -bin bin file"<<endl;
			cerr<<"   2. -fvkc FVK cert file "<<endl;
			cerr<<"   3. -fvk FVK key file "<<endl;
			cerr<<"   4. -ivkc IVK cert file "<<endl;
			cerr<<"   5. -ini Code Sign INI file "<<endl;
			cerr<<"   6. -rvk RVK key file "<<endl;
			cerr<<"   7. -out Output_file "<<endl;
#else
			cerr<<"code_sign_info.exe -bin bin_file -fvk FVK_cert_file -ivk IVK_cert_file -ini Code_Sign_INI_file -out Output_file"<<endl;
			cerr<<"   1. -bin bin file"<<endl;
			cerr<<"   2. -fvk FVK cert file "<<endl;
			cerr<<"   3. -ivk IVK cert file "<<endl;
			cerr<<"   4. -ini Code Sign INI file "<<endl;
			cerr<<"   5. -out Output_file "<<endl;
#endif
		}

		exit_status = HELP_VALUE;
	}
	else if ( argc == 17U )
	{
		for( arg_n; arg_n < argc; arg_n++ )
		{
			if ( strcmp(argv[arg_n],BIN_ARG ) == 0U )
			{
				in_arg_bin_num = arg_n + 1;
				cerr<<"bin path found"<<in_arg_bin_num<<endl;
			}
			else if ( strcmp(argv[arg_n], FVK_ARG ) == 0U )
			{
				in_arg_fvk_num = arg_n + 1;
				cerr<<"fvk cert found"<<arg_n<<endl;
			}
			else if ( strcmp(argv[arg_n], FVK_KEY_ARG ) == 0U )
			{
				in_arg_fvk_key_num = arg_n + 1;
				cerr<<"fvk found"<<arg_n<<endl;
			}
			else if ( strcmp(argv[arg_n], IVK_ARG ) == 0U )
			{
				in_arg_ivk_num = arg_n + 1;
				cerr<<"ivk path found"<<in_arg_ivk_num<<endl;
			}
			else if ( strcmp(argv[arg_n], INI_ARG ) == 0U )
			{
				in_arg_ini_num = arg_n + 1;
				cerr<<"ini path found"<<arg_n<<endl;
			}
			else if ( strcmp(argv[arg_n], RVK_KEY_ARG ) == 0U )
			{
				in_arg_rvk_key_num = arg_n + 1;
				cerr<<"rvk found"<<arg_n<<endl;
			}
			else if ( strcmp(argv[arg_n], OUT_ARG ) == 0U )
			{
				in_arg_out_num = arg_n + 1;
				cerr<<"out path found"<<arg_n<<endl;
			}
			else if ( strcmp(argv[arg_n], CROSS_SIGN_ARG ) == 0U )
			{
				if ( ( strcmp(argv[arg_n+1], "1" ) ==0U ))
				{
					code_sign_info_header_s.cross_sign_status =1U;
					cerr<<"Cross Signing enable *****************************************************************"<<arg_n<<endl;
				}
				else
				{
					code_sign_info_header_s.cross_sign_status =0U;
					cerr<<"Cross Signing disable *****************************************************************"<<arg_n<<endl;
				}
				//cerr<<"out path found"<<arg_n<<endl;
			}
		}

	}
	else
	{
		exit_status = ERROR_VALUE;
	}

	if ( exit_status == SUCCESS_VALUE )
	{
		if ( in_arg_bin_num )
		{
			fIn.open( argv[in_arg_bin_num],ios::in | ios::binary);
			if(!fIn)
			{
				cerr<<"opening binary file fails"<<endl;
			}
			else
			{
				unsigned int fw_sig_size = getFileSize(fIn);
				cerr <<"FW Signature File Size = " << dec << fw_sig_size << "Hex: " <<hex << fw_sig_size << endl;

				//strcpy(output_file_name,SubstringFileName(argv[in_arg_bin_num], "Upload_Pack.bin") ) ;
				//strcat(output_file_name,"Code_Sign_Upload_Pack.bin");
				//cout<<"file name"<<output_file_name<<"arg="<<in_arg_bin_num<<endl;
				concatFiles(argv[in_arg_bin_num],argv[in_arg_out_num]);
				/* update code sign info header information*/
				ret_state = Code_header_Update( &code_sign_info_header_s,argv[in_arg_fvk_num],argv[in_arg_ivk_num],argv[in_arg_out_num],argv[in_arg_fvk_key_num],argv[in_arg_rvk_key_num]);
			}
		}
	}
	else
	{
		if ( exit_status == ERROR_VALUE )
		{
			cerr<<" EXIT -> Expected Argument count is less "<<endl;
		}
		else if ( exit_status == HELP_VALUE )
		{
			cout<<" EXIT -> HELP option used "<<endl;
		}
		else
		{
			cerr<<"EXIT -> Argument count missing "<<endl;
		}
		if( exit_status != HELP_VALUE )
		{
			cerr<<"argc="<<argc<<endl;
			for (int i = 0; i < argc; i++)
			{
				cerr<<"argv["<<i+1<<"]"<<argv[i]<<endl;
			}
		}		
	}
//cout<<"FVK Size = "<<fvk_size<<endl;
//cout<<"FVK Version = "<<  fvk_version<<endl;
//getch();
	return 0;
}

unsigned int getFileSize(ifstream& file)
{
	file.seekg(0, ios::end);
	int fileSize = file.tellg();
	file.seekg(0,ios::beg);
	return fileSize;
}

char* SubstringFileName(char * bin_file_name, char * findString)
{
	std::string str(bin_file_name);
	std::size_t pos = str.find(findString);
	std::string bin_name = str.substr (0,pos);
	return const_cast<char *>(bin_name.c_str());
}

bool concatFiles(char* input_file_name,  char* op_file_name )
{
	ifstream fvk_file_input (input_file_name, ios::binary);


	if(!fvk_file_input)
	{
		cerr<<"Cannot Open Input File = "<< input_file_name<< endl;
		return false;
	}
	else
	{
		ofstream fvk_file_output (op_file_name, ios::out | ios::binary | ios::ate );

		if(!fvk_file_output)
		{

			cerr<<"Cannot Open Output File"<<endl;
			return false;
		}
		else
		{

			if(fvk_file_input.is_open() && fvk_file_output.is_open())
			{
				char c ;
				//cerr<< input_file_name <<endl;
				while(!fvk_file_input.eof())
				{
					c = fvk_file_input.get();
					if(!fvk_file_input.eof())
						fvk_file_output.put(c);
				}
				cerr<<"Appended  " <<input_file_name <<" Succesfully."<<endl;
			}

			//Close both files
			fvk_file_input.close();
			fvk_file_output.close();

		}
	}
	return true;
}

bool Code_header_Update(code_sign_info_header* append_header_info, char* input_fvk_cert_name,char* input_ivk_cert_name, char * op_file_name, char * fvk_key_name, char * rvk_key_name )
{

//	Extract_Ini_info(append_header_info,"Code_Sign_Info_header.ini");
	append_header_info->code_sign_info_identity_mark = 0x4598FE21;
	append_header_info->code_sign_header_version_num =0x0001;
	append_header_info->algo_type =1;
	append_header_info->ecc_curve_type =23;
	append_header_info->key_size =1;
	append_header_info->hash_size =2;
	append_header_info->reserved =0xFF;
	ifstream fvk_file_input (input_fvk_cert_name, ios::binary);
	ifstream ivk_file_input (input_ivk_cert_name, ios::binary);
	unsigned short int fvk_size = getFileSize( fvk_file_input );
	cerr<<"FVk file name"<<input_fvk_cert_name<<endl;
	cerr<<"iVk file name"<<input_ivk_cert_name<<endl;
	cerr <<"Fvk File Size = " << dec << fvk_size << "Hex: " <<hex << fvk_size << endl;
	unsigned short int ivk_size = getFileSize( ivk_file_input );
	cerr <<"IVK File Size = " << dec << ivk_size << "Hex: " <<hex << ivk_size << endl;
	ifstream fvk_key_file_input (fvk_key_name, ios::binary);
	unsigned short int fvk_key_size = 0U;
	if( fvk_key_name != NULL )
	{	
		cerr<<"fvk key file name"<<fvk_key_name<<endl;
		
		fvk_key_size = getFileSize( fvk_key_file_input );
	}

	ifstream rvk_key_file_input (rvk_key_name, ios::binary);
	unsigned short int rvk_key_size = 0U;
	if( rvk_key_name != NULL )
	{	
		cerr<<"rvk key file name"<<rvk_key_name<<endl;
		
		rvk_key_size = getFileSize( rvk_key_file_input );
	}
	unsigned char padding_val = 0xFF;
	if(!fvk_file_input && !ivk_file_input)
	{
		cerr<<"Cannot Open Input File = "<< input_fvk_cert_name<<"second ivk="<<input_ivk_cert_name<< endl;
		return false;
	}
	else
	{
		ofstream key_file_output (op_file_name, ios::out | ios::binary | ios::ate | ios::app );
		cerr<<"size11="<<key_file_output.tellp()<<endl;

		if(!key_file_output)
		{

			cerr<<"Cannot Open Output File"<<endl;
			return false;
		}
		else
		{
			unsigned int i =0;

			/* add fvk key */
			if(fvk_key_file_input.is_open() && key_file_output.is_open())
			{
				unsigned char c=0;
				
				while(!fvk_key_file_input.eof())
				{
					c = fvk_key_file_input.get();
					if(!fvk_key_file_input.eof())
					{
						append_header_info->fvk_key[i]=c;
						i++;
					}

				}
				for( i; i<(FVK_KEY_MAX_SIZE ); i++)
				{
					append_header_info->fvk_key[i]=padding_val;
				}
				append_header_info->fvk_key_len = fvk_key_size;
				cerr<<"Appended fvk key " <<fvk_key_name <<" Succesfully."<<endl;
				cerr<<"i count =  " <<i <<" Succesfully."<<endl;
				cerr<<"fvk_key_size="<<fvk_key_size<<endl;
			}

			/* append fvk cert in fw */
			if(fvk_file_input.is_open() && key_file_output.is_open())
			{
				unsigned char c=0;
				i=0;
				while(!fvk_file_input.eof())
				{
					c = fvk_file_input.get();
					if(!fvk_file_input.eof())
					{
						append_header_info->FVK[i]=c;
						i++;
					}

				}
				for( i; i<(FVK_CERT_SIZE ); i++)
				{
					append_header_info->FVK[i]=padding_val;
				}
				append_header_info->FVK_size = fvk_size;
				cerr<<"Appended fvk cert " <<input_fvk_cert_name <<" Succesfully."<<endl;
				cerr<<"i count =  " <<i <<" Succesfully."<<endl;
				cerr<<"fvk_size =  " <<fvk_size <<" Succesfully."<<endl;
			}
			
				/* append ivk cert in fw */
			if(ivk_file_input.is_open() && key_file_output.is_open())
			{
				unsigned char c=0;
				i=0;
				while(!ivk_file_input.eof())
				{
					c = ivk_file_input.get();
					if(!ivk_file_input.eof())
					{
						append_header_info->IVK[i]=c;
						i++;
					}

				}
				for( i; i<(IVK_CERT_SIZE ); i++)
				{
					append_header_info->IVK[i]=padding_val;
				}
				append_header_info->IVK_size = ivk_size;
				cerr<<"Appended fvk " <<input_ivk_cert_name <<" Succesfully."<<endl;
				cerr<<"i count =  " <<i <<" Succesfully."<<endl;
				cerr<<"ivk_size =  " <<ivk_size <<" Succesfully."<<endl;
			}
			
			/* add rvk key in fw */
			if(rvk_key_file_input.is_open() && key_file_output.is_open())
			{
				unsigned char c=0;
				i=0;
				while(!rvk_key_file_input.eof())
				{
					c = rvk_key_file_input.get();
					if(!rvk_key_file_input.eof())
					{
						append_header_info->rvk_key[i]=c;
						i++;
					}

				}
				for( i; i<(FVK_KEY_MAX_SIZE ); i++)
				{
					append_header_info->rvk_key[i]=padding_val;
				}
				append_header_info->rvk_key_len = rvk_key_size;
				cerr<<"Appended fvk key " <<rvk_key_name <<" Succesfully."<<endl;
				cerr<<"i count =  " <<i <<" Succesfully."<<endl;
				cerr<<"rvk_key_size =  " <<rvk_key_size <<" Succesfully."<<endl;
			}
			key_file_output.write( reinterpret_cast< const char*>(append_header_info),sizeof(code_sign_info_header));
			cerr<<"size21="<<key_file_output.tellp()<<endl;

			//Close all files
			fvk_file_input.close();
			ivk_file_input.close();
			key_file_output.close();
			if( fvk_key_name != NULL )
			{
				fvk_key_file_input.close();
			}
			if( rvk_key_name != NULL )
			{
				rvk_key_file_input.close();
			}

		}
	}

}
bool Extract_Ini_info(code_sign_info_header* append_header_info,const char* ini_file_name)
{
	bool status = false;
	ifstream ini_file_input (ini_file_name, ios::in);
	if(!ini_file_input)
	{
		cout<<"cannot open ini file"<<endl;
	}
	return status;
}

/**
 * @brief: convert the ASCII value present in xml file for the field present in xml
 * @param[in] unsigned char* gen_ltk_header pointer to the header buffer
 * @param[in] unsigned char* field_val ASCII format value of the field
 * @param[in] unsigned char field_count count of the field to be copied in header
 * @param[in] unsigned int len actual length in the header for the field
 * @param[in] unsigned int offset location in header where field needs to be inserted
 * @param[out] unsigned char success or failure
 */
unsigned char Convert_Hex( unsigned char* gen_ltk_header, unsigned char* field_val, unsigned char field_count, unsigned int len,
                           unsigned int offset )
{
	unsigned char exit_status = SUCCESS_VALUE;
	unsigned short int actual_cnt = 0;
	const unsigned char REMOVE_HEX_FIELD = 2;
}
