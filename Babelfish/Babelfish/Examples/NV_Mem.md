\example NV_Mem
\brief    The sample code to which tells  how to use the NV_Mem class.

\par Board Settings  
 \details  Please do the following setting on EasyMXPro Board to run successfully the sample code.
 \n        1. Connect the FRAM chip  to the board and initialize the FRAM appropriately.

 \par Configurations 
      Not Applicable
      
\par Sample Code Listing

~~~~{.cpp}

#include "NV_Mem.h"



// init function
void  NV_Mem_Examplecode(void)
{ 

// Location/Address to be used
uint32_t loc_address;
//Length to be used
uint32_t length;
loc_address = 0xa0;
length = 5;

uint8_t write_array[5];
uint8_t read_array[5];
using namespace NV;
//Erasing the location to be written at
NV_Mem::Erase(loc_address,length);
//Values to be loaded
write_array[0]=56;
write_array[1]=55;
write_array[2]=45;
write_array[3]=34;
write_array[4]=12;
//Reading the values from the location
NV_Mem::Read(&read_array[0],loc_address,length);
//Writing the values to the location
NV_Mem::Write(&write_array[0],loc_address,length);
//Reading the loaded values
NV_Mem::Read(&read_array[0],loc_address,length);
//Erasing the values in the read array
NV_Mem::Erase(loc_address,length);
//Reading the values in  the read array.It must be reset to 0 .
NV_Mem::Read(&read_array[0],loc_address,length);
}
~~~~
