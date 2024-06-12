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
#define T_FVK_ARG "-tfvkc"
#define IVK_ARG "-ivk"
#define FWSIGN_ARG "-fwsign"
#define INI_ARG "-ini"
#define OUT_ARG "-out"

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
unsigned int Ver_String_To_Num(char* fvk_str);
unsigned int Get_File_Size(ifstream&);
bool Concat_Files(char* , char* );
void Generate_Fw_Pack(char*, char*, char*);
char* Substring_File_Name(char*, char *);
bool Validate_Version_String(char*);
void Pad_Bytes(char * op_file_name); 
struct code_sign_info_header;
struct append_sign;
bool Add_Extended_Sign( char* input_fw_sign_name, char * op_file_name,unsigned int *trusted_fw_sign_size );
bool Calculate_Append_CRC(unsigned int trusted_fw_sign_size, char *file_name, char *fvk_file_name );
unsigned short int Gen_Crc( const char* data, unsigned long int length, unsigned int start );
/* Global variables */
ifstream fIn; 
ofstream fOut;
 unsigned char version[4];
char output_file_name[256];
typedef union
{
	unsigned short int u16;
	unsigned char u8[2];
} SPLIT_UINT16;
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
	unsigned char fw_sign[FW_SIGN_SIZE];
	unsigned short int fw_sig_len;	
	unsigned char reserve[2];	
	unsigned short int crc;
};



int main(int argc, char** argv) {
	
	unsigned int arg_n = 0U;
	unsigned int in_arg_bin_num = 0U;
	unsigned int in_arg_fwsign_num = 0U;
	unsigned int in_arg_out_num = 0;
	unsigned int in_arg_tfvk_num = 0;
	unsigned int exit_status = SUCCESS_VALUE;
	bool ret_state = false;
	unsigned int trusted_fw_sign_size = 0U;
	cerr<<"argc="<<argc<<endl;
if(argc == 2  )
{
	if ( strcmp(argv[1],HELP_OPTION) == 0U)
	{
		cerr<<" usage of Code Sign Info builder : "<<endl;
		cerr<<"append_cross_sign_info.exe -bin bin file -fwsign trusted fw signature file -tfvkc trusted fvk cert file "<<endl;
		cerr<<"   1. -bin bin file"<<endl;
		cerr<<"   2. -fwsign trusted fw signature file "<<endl;
		cerr<<"   3. -out output file "<<endl;
		cerr<<"   4. -tfvkc trusted fvk cert file "<<endl;
	}
	
	exit_status = HELP_VALUE;
}
else if ( argc == 7U )
{
	for( arg_n;arg_n < argc; arg_n++ )
	{
		if ( strcmp(argv[arg_n],BIN_ARG ) == 0U )
		{
			in_arg_bin_num = arg_n + 1;
			cerr<<"bin found"<<in_arg_bin_num<<endl;
		}
		else if ( strcmp(argv[arg_n], FWSIGN_ARG ) == 0U )
		{
			in_arg_fwsign_num = arg_n + 1;
			cerr<<"fwsign found"<<in_arg_fwsign_num<<endl;
		}
		else if ( strcmp(argv[arg_n], T_FVK_ARG ) == 0U )
		{
			in_arg_tfvk_num = arg_n + 1;
			cerr<<"out path found"<<in_arg_tfvk_num<<endl;
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
			unsigned int fw_size = Get_File_Size(fIn);
		    cerr <<" cross sign FW  File Size = " << dec << fw_size << "Hex: " <<hex << fw_size << endl;
			/* add trusted fw sign info header information*/
			ret_state = Add_Extended_Sign( argv[in_arg_fwsign_num], argv[in_arg_bin_num], &trusted_fw_sign_size);
			/* add CRC */
			ret_state = Calculate_Append_CRC( trusted_fw_sign_size,argv[in_arg_bin_num],argv[in_arg_tfvk_num] );
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
 
unsigned int Get_File_Size(ifstream& file)
{
	file.seekg(0, ios::end);
    int fileSize = file.tellg();
    file.seekg(0,ios::beg);
    return fileSize;
}

char* Substring_File_Name(char * bin_file_name, char * findString)
{
		std::string str(bin_file_name);
		std::size_t pos = str.find(findString);
		std::string bin_name = str.substr (0,pos);
		return const_cast<char *>(bin_name.c_str());
}

bool Concat_Files(char* input_file_name,  char* op_file_name )
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

 bool Add_Extended_Sign( char *input_fw_sign_name, char *op_file_name, unsigned int * trusted_fw_sign_size )
 {
 	ifstream fw_sign_file_input (input_fw_sign_name, ios::binary);
	unsigned short int fw_sign_size = Get_File_Size( fw_sign_file_input );
	*trusted_fw_sign_size = fw_sign_size;
	cerr<<" cross Fw sign file name"<<input_fw_sign_name<<endl;

	cerr <<" cross Fw sign File Size = " << dec << fw_sign_size << "Hex: " <<hex << fw_sign_size << endl;
	
	unsigned char padding_val = 0xFF;
	char *fw_sign_p = new char[fw_sign_size];
	if(!fw_sign_file_input)
	{
		cerr<<"Cannot Open Input File = "<< input_fw_sign_name<<"second ivk="<<input_fw_sign_name<< endl;
		return false;
	}
	else
	{
		ofstream key_file_output (op_file_name, ios::out | ios::binary | ios::ate | ios::app );
		
		if(!key_file_output)
		{
		
			cerr<<"Cannot Open Output File"<<endl;
			return false;
		}
		else
		{
			unsigned short int i =0;
			
			if(fw_sign_file_input.is_open() && key_file_output.is_open())
			{
				unsigned char c=0;
				key_file_output.write( reinterpret_cast< const char*>(&fw_sign_size),sizeof(unsigned short int));
				while(!fw_sign_file_input.eof())
				{
					c = fw_sign_file_input.get();
					if(!fw_sign_file_input.eof())	
				 	{
				 		fw_sign_p[i]=c;
				 		i++;
				 	}
				 
				}
				cerr<<"Appended fw_sign " <<input_fw_sign_name <<" Succesfully."<<endl;
				cerr<<"i count =  " <<i <<" Succesfully."<<endl;
			}

			key_file_output.write( reinterpret_cast< const char*>(fw_sign_p),fw_sign_size);
				   
					//Close all files
			delete[] fw_sign_p;
			fw_sign_file_input.close();
			key_file_output.close();

  		}
    }
	
 }
 
bool Calculate_Append_CRC( unsigned int trusted_fw_sign_size, char *input_fw_sign_name, char *fvk_cert_name )
{
	fstream fw_file(input_fw_sign_name, ios::in | ios::out | ios::binary | ios::ate | ios::app );
	ifstream fvk_cert_file_input (fvk_cert_name, ios::binary);
	unsigned short int fvk_cert_size = Get_File_Size( fvk_cert_file_input );
	cerr<<"crc fw_sign " <<input_fw_sign_name <<" Succesfully."<<endl;
	if(!fw_file)
	{
		cerr<<"Cannot Open Input File = "<< input_fw_sign_name<< endl;
		return false;
	}
	unsigned long int pos = (fvk_cert_size + trusted_fw_sign_size + 2*(sizeof(unsigned short int)));
	cerr<<"pos =  " <<dec<<pos<<endl;
	cerr<<"fvk_cert_size =  " <<dec<<fvk_cert_size<<endl;
	cerr<<"trusted_fw_sign_size =  " <<dec<<trusted_fw_sign_size<<endl;
	unsigned long int newp = fw_file.tellg();
	cerr<<"newp =  " <<dec<<newp<<endl;
	unsigned long int ptr = newp - pos;
	cerr<<"ptr =  " <<dec<<ptr<<endl;
	fw_file.seekp(ptr, ios::beg);
	unsigned long int curp = fw_file.tellg();
	cerr<<"cur =  " <<dec<<curp<<endl;	
	char *arr = new char[pos];
	unsigned char c;
//	ofstream fvk_file_output ("temp.bin", ios::out | ios::binary | ios::ate );
	if(!arr)
	{
		cerr<<"memory fail"<<endl;
	}
	unsigned long int i =0;
	for(i;i<pos;i++)
	{
		c = fw_file.get();
		if(!fw_file.eof())
		{
//			fvk_file_output.put(c);
			arr[i]=c;
		}

	}

	cout<<"i = "<<dec<<i<<endl;
	unsigned short int crc = 0;
	crc=Gen_Crc(arr,pos,0);
	cout<<" cross sign CRC="<<hex<<crc<<endl;

	curp = fw_file.tellg();
	cout<<"cur =  " <<dec<<curp<<endl;

	fw_file.write( reinterpret_cast< const char*>(&crc),sizeof(unsigned short int));
	fw_file.close();
	fvk_cert_file_input.close();
	//	fvk_file_output.close();


delete[] arr;
	return true;
}

/* CRC generation code directly used from fwtool.c */
unsigned short int Gen_Crc( const char* data, unsigned long int length, unsigned int start )
{
	unsigned char temp;
	SPLIT_UINT16 temp_crc;
//	cout<<"size="<<sizeof(temp_crc);
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
