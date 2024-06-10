This command line tool should be used to create language binary image.
The tool accepts INI file(/s) path as input containing details about language specifications information (language header) and 
json file path in which language block information is stored.

Command Line Usage:
-------------------
Lang_Bin_Generator.exe -ini_path "<ini_folder_path>" -out "<output_bin_name>"


Example - 1: 
------------

Command Line:
-------------
Lang_Bin_Generator.exe -ini_path ".\Language_INI\\" -out "..\..\Build_Output\Language\Language_data.bin"

Note:
-----
This tool will search for all ini files in "<ini_folder_path>". Each ini file should have header information of one language.