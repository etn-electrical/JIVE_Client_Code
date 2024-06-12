#include <iostream>
#include <fstream>
#include <string>
#include <strings.h>
#include <cstdlib>
#include <conio.h>
using namespace std;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
 unsigned int verStringToNum(char* fvk_str);
 unsigned int getFileSize(ifstream&);
 void appendSignature(char*);
 bool concatFiles(char* , char* , char* );
 void generateFWPack(char*, char*, char*);
 char* SubstringFileName(char*, char *);
 bool validateVersionString(char*);
 void padBytes(char * op_file_name);

 
 unsigned char version[4];

int main(int argc, char** argv) {

if(argc <= 1  )
{
	 
	cerr<<"FVK Info Options : "<<endl;
	cerr<<"   1.  fvk_info -fwsig"<<endl;
	cerr<<"   2.  fvk_info -fvksig"<<endl;
}
else
{

	if(strcmp(argv[1], "-fwsig") == 0)
	{
		if(argc == 2)
		{
			cout <<"===== Usage -fwsig ====="<<endl;
			cout <<"fvk_info -fwsig <Payload bin file name>"<<endl;
			cout <<"=============== ====="<<endl;
		}
		else if(argc !=3)
		{
			cout<<"Invalid Input arguments -fwsig, 3 arg required"<<endl;
		}
		else
		{
			cerr <<"FW Upload signature" <<endl;
			appendSignature(argv[2]);
			cerr<<"Done with FWSIG "<<endl;
		}
		
	}
	else if (strcmp(argv[1], "-fvksig") == 0)
	{
		if(argc == 2)
		{
			cout <<"===== Usage -fvksig ====="<<endl;
			cout <<"fvk_info -fvksig <FVK Pub Key File > <FVK Version> <FW binary file>"<<endl;
			cout <<"=============== ====="<<endl;
		}
		else if(argc !=5)
		{
			cout<<"Invalid Input arguments fvksig, 5 arg required"<<endl;
		}
		else
		{
			generateFWPack(argv[2], argv[3],argv[4]);
			
		    cerr <<"Done with FVK & signature append" <<endl;
	}
		}
		
	else
	{
		cerr<<"Wrong Key Type" <<endl;
	}		
} 
//cout<<"FVK Size = "<<fvk_size<<endl; 
//cout<<"FVK Version = "<<  fvk_version<<endl;
//getch();
	return 0;
}
 
 void appendSignature(char *input_file)
{  
	if(strlen(input_file) > 256)
	{
		cerr<<"File Name Too long . Should be < 250 characters"<<endl;
	}
	else
	{
	
		char output_file_name[256];
		strcpy(output_file_name,SubstringFileName(input_file, "_payload.bin") ) ;
	    strcat(output_file_name,"_Upload_Pack.bin");
	    cout <<"Upload Pack File = "<< output_file_name<<endl;
	    
		char file_name[256] ;
		//cerr <<input_file <<endl;
		strcpy(file_name,input_file);
	    strcat(file_name,".sig");
	    cerr <<"FW Signature File Name = "<<file_name<<endl;
	    
	    ifstream fvk_sig (file_name, ios::binary);
	   	   
	   	if(!fvk_sig) 
		{
			cerr << " Error opening the FW signature file";
		}   
		else
		{
			unsigned int fw_sig_size = getFileSize(fvk_sig);
		    cerr <<"FW Signature File Size = " << dec << fw_sig_size << "Hex: " <<hex << fw_sig_size << endl;
		    fvk_sig.close();
		
		    ofstream fvk_payload(input_file, ios::out | ios::binary | ios::app);
	    
		    if(!fvk_payload)
		    {
		        cerr << " Error opening the Output File Firmware Upload Pack";	
			}
			else
			{
				fvk_payload.write(reinterpret_cast<const char *>(&fw_sig_size), 2 );
				fvk_payload.close();
			}
		}
		
		if(concatFiles(input_file, file_name, output_file_name))
		{
			padBytes(output_file_name);	
		}
		else
		{
			cerr << " " <<endl;
		}
		
		cerr <<"Removing File :"<<file_name << endl;
		if( remove(file_name) != 0 )
		{
			cerr << "Error deleting file "  << file_name << endl;
		}
	    
	  	else
	  	{
	  		cerr << "Successfully deleted File "  << file_name << endl;
	  	}
	  	
		cerr <<"Removing File :"<<input_file << endl;
		if( remove(input_file) != 0 )
		{
			cerr << "Error deleting file "  << input_file << endl;
		}
	    
	  	else
	  	{
	  		cerr << "Successfully deleted File "  << input_file << endl;
	  	}
	}
}

void generateFWPack(char* fvk, char* fvk_version, char* fw_bin)
{
	unsigned int reserved = 0xFFFFFFFF;
	char file_name[256] ;
	strcpy(file_name,fvk);
	strcat(file_name,".sig");
	cerr <<"FVK Signature File Name = "<<file_name<<endl;
	
	char file_name_extract[256] ;
	strcpy(file_name_extract,SubstringFileName(fw_bin,".bin") ) ;
	strcat(file_name_extract,"_payload.bin");
	cerr <<"Payload File = "<<file_name_extract<<endl;
	if(validateVersionString(fvk_version))
	{
		cerr <<"String Version "<< fvk_version << " Successfully validated"<<endl;	
		if (verStringToNum(fvk_version) < 256)
		{ 
			cerr<<"Version String to Integer :" << hex << (unsigned int)version[0] <<" . "<< (unsigned int)version[1] <<" . "<< (unsigned int)version[2] <<" . "<< (unsigned int)version[3]<<endl;
		}
		else
		{
			cerr<< "Failed Version to Num " << endl;
		}
	}
	else
	{
		cerr << "Invalid Version String" <<endl;
	}
	
	ifstream fvk_file_input (fvk, ios::binary);
	ifstream fvk_sig (file_name, ios::binary);
	
	if(!fvk_file_input)
	{
		cerr<<"Cannot Open FVK File "<<fvk <<endl;
	}
	
	else if(!fvk_sig)
	{
		cerr<<"Cannot Open Signature File = "<<file_name<<endl;
	}
	
	else
	{
		ofstream fvk_file_header ("fvk_header.bin", ios::out | ios::binary);
			
    	unsigned int fvk_size = getFileSize(fvk_file_input);
	    unsigned int fvk_sig_size = getFileSize(fvk_sig);
	   	    
	    cerr <<"FVK File Size = " << dec << fvk_size << endl;
	    cerr <<"FVK Signature Size = " << dec << fvk_sig_size << endl;
	   
	    
		if(!fvk_file_header)
		{
			cerr<<"Cannot Open Header File fvk_header.bin"<<endl;
		}	
		else
		{
			fvk_file_header.write(reinterpret_cast<const char *>(&version[0]), 4 );
			fvk_file_header.write(reinterpret_cast<const char *>(&fvk_size), 2 );
			fvk_file_header.write(reinterpret_cast<const char *>(&reserved), 2 );
		
		
			if(fvk_file_input.is_open() && fvk_file_header.is_open())
			{
					char c ;
					while(!fvk_file_input.eof())
					{
						c = fvk_file_input.get();
						if(!fvk_file_input.eof())
						 fvk_file_header.put(c);
					}
			}
		    
			fvk_file_header.write(reinterpret_cast<const char *>(&fvk_sig_size), 2 );
			
		   if (fvk_sig.is_open())
		   {
		   	    char c ;
				while(!fvk_sig.eof())
				{
					c = fvk_sig.get();
					if(!fvk_sig.eof())
					   fvk_file_header.put(c);
				}
		   }
		 }
			//Close both files
			fvk_file_input.close();
			fvk_file_header.close();
			fvk_sig.close();
			concatFiles(fw_bin, "fvk_header.bin",file_name_extract);
			remove("fvk_header.bin");
			cerr <<"Removing File :"<<file_name << endl;
	     	remove(file_name);
			//appendSignature();
	}
}


bool concatFiles(char* input_file_name, char* sig_file_name, char* op_file_name )
{
	ifstream fvk_file_input (input_file_name, ios::binary);
	ifstream fvk_sig (sig_file_name, ios::binary);
	
	if(!fvk_file_input)
	{
		cerr<<"Cannot Open Input File = "<< input_file_name<< endl;
		return false;
	}
	else if(!fvk_sig)
	{
		cerr<<"Cannot Open Signature File = "<<sig_file_name<<endl;
		return false;
	}
	else
	{
		ofstream fvk_file_output (op_file_name, ios::out | ios::binary | ios::ate);
		
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
				
		   if (fvk_sig.is_open())
		   {
		   	    char c ;
			//	cerr<< sig_file_name <<endl;
				while(!fvk_sig.eof())
				{
					c = fvk_sig.get();
					if(!fvk_sig.eof())
				       fvk_file_output.put(c);
				}
				cerr<<"Appended  " <<sig_file_name <<" Succesfully."<<endl;
			 }
				   
					//Close both files
			fvk_file_input.close();
			fvk_file_output.close();
			fvk_sig.close();
	 }
   }
		return true;
}

bool is_number(const std::string& s)
{
	//cerr<<"Is Number ="<<s.c_str()<<endl;
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

int subStringToNum(std::string str)
{
	//cerr<<"SubString ="<<str.c_str()<<endl;
	int value = 65535;
	if( is_number(str))
	{
		value = atoi(str.c_str());
		//cerr <<"Value = "<< hex <<value << endl;
	}
	return value;
}

 unsigned int verStringToNum(char* fvk_str)
 {
 	std::string str (fvk_str);
 	
 	bool success = false;
 	int value = 65535;
 	for ( char idx = 3; idx >= 0; idx--)
 	{
 		std::size_t pos = str.find(".");
 		std::string str1 = str.substr (0,pos);
 		//cerr<<"str1 = "<<str1.c_str()<<endl;
 		value = subStringToNum(str.substr (0,pos));
 		
 		if(	value > 0xFF)
 		{
 			success = false;
 			break; 
		}
		else
		{
            version[idx] = value;
        }
 		str = str.substr (pos+1); 
 		
	}
	return value;
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

bool validateVersionString(char* ver_str)
{
	bool success = true;
	unsigned int found_count = 0;
	std::string str(ver_str);
	std::size_t pos = 0;
	unsigned int str_len = strlen(ver_str);

	while(pos <= str_len && found_count < 3)
	{
		 pos = str.find(".");
		if (pos > str_len)
		{
			success = false;
			break;
		}
		else 
		{
			found_count ++;
		}	
	//	cerr << "validateVersionString : Pos = "<< pos <<endl;
	//    cerr << "validateVersionString : String = "<< str.c_str() <<endl;
	//    cerr << "validateVersionString : String Length = "<< strlen(str.c_str()) <<endl;
	//   cerr << "Found Count = "<< dec << found_count <<endl;
		if(!isdigit(str.c_str()[0]))
	  	{
	  		success = false;
	  		cout << "First Character is Non numeric " << str.c_str()[0] <<endl;
			break;
    	}
		str = str.substr (pos +1);

	}
	//cout <<str.c_str()<<endl;
	if(found_count == 3 && !isdigit(str.c_str()[0]) )
	{
		cerr<<"Null String"<< endl;
		success = false;
	}
	//cerr << "Success = "<< dec << success <<endl;
	return success;
}

void padBytes(char * op_file_name)
{
	ifstream fvk_file_input (op_file_name, ios::binary| ios::ate);
	unsigned int file_size =  fvk_file_input.tellg();
	unsigned char reminder = 0;
	ofstream fvk_file_output (op_file_name, ios::out | ios::binary| ios::app);
	if((reminder = file_size%4) != 0)
	{
		reminder = 4 - reminder;
		for(unsigned char idx = 0; idx < reminder; idx++)
		 fvk_file_output.put(0xFF);
		 cerr << "Added "  <<dec << (unsigned int)reminder << " Pad Bytes"<<endl;
	}
	
}

