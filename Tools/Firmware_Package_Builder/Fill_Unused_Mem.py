#****************************************************************************************
# @file 		Fill_Unused_Mem.py
# @details 		This file will be use to fill unused memory with 0xFF.
#               Python command to fill unused memory with 0xFF till given offset
#               Command should be in the below given format
#               Fill_Unused_Mem.py" "Path of Project Binary File" "Code Signing Offset in App Image"  
# @note         Given code signing offset should be in hex only.To calculate offset, 
#               refer project CMakeList.txt file
#                
# @copyright 	2022 Eaton Corporation. All Rights Reserved.
#
# *****************************************************************************************

import os
import sys

# Please Enter the file name and end location respectively in the command line
# "Fill_Unused_Mem.py" "Path of Project Binary File" "Code Signing Offset in App Image" 

# Check for command usage
if len(sys.argv) != 3 :
    print("Command Usage format error")
    print("Usage: \"Fill_Unused_Mem.py\" \"Path of Project Binary File\" \"Code Signing Offset in App Image\"")
    quit()

# Check for address validation
#1568768 is starting offset of code sign information in bin file(in decimal) 
#Note : For code signing offset calculation, refer the comment in project CMakeList.txt
if ( hex(int((sys.argv[2]),16)) < hex(0) ) or ( hex(int((sys.argv[2]),16)) > hex(1568768)) :
    print("Invalid Image Size")
    quit()

# Check for correct file name    
if sys.argv[1] != "" :
    print("Provided bin file name : ", sys.argv[1] )
else:
    print("Provided bin file name is incorrect")
    quit()

# Variable initialization    
start_location = 1
end_location = hex(int((sys.argv[2]),16))
data_to_fill = 255
bin_file = sys.argv[1]
bin_file = bytes(bin_file, 'utf-8').decode("unicode_escape")

# Check if file is present or not
if not os.path.isfile(bin_file):
    print("Bin file does not exist!",sys.argv[1])
    quit()

# Open file in read/write mode
f = open(bin_file,"ab")

# Get the size of bin file(in decimal)
size = os.path.getsize( bin_file )
length = int((sys.argv[2]),16) - size

# Logic to fill 0xFF in *.bin file
while start_location < length+1:
    byte_arr = [data_to_fill]
    binary_format = bytearray(byte_arr)
    f.write(binary_format)
    start_location += 1
f.close()
print("Filled unused memory with 0xFF successfully")    
