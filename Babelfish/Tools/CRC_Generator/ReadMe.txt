This is the command line tool for generating crc.
The tool accepts,
	-Binary file path as input containing the data for which crc is calculated.
	-Output binary file path in which generated crc is updated. Utility creates new file if the file doesn not exists. If file exists then it gets overwritten.
		Utility copies the data from input bin file to ouput bin file, calculates and writes crc based on offset parameters.
	-Offset parameters i.e crc offset, data start offset, length of data. 
		Length parameter is optional, if length is zero or not given then length is calulated as (bin file size - start offset)
	-Algorithm type
		'1' for crc16
		'2' for crc32 (currently not inplemented)
	-Data log flag, if data_log parameter is '1' then data buffer on which crc is calculated will be printed on console
		
Command Line Usage:
-------------------
CRC_Generator.exe -bin "<input_file_path>" -out "<output_file_path>" -offset <crc location> -algo <crc algorithm type> -data_start <offset to the start of data> -data_log Flag to enable/disable data log -data_len <length of data>

Examples:
---------
CRC_Generator.exe -bin "..\..\..\build\esp32_sample_app.bin" -out "..\..\..\build\esp32_sample_app_crc.bin" -offset 4 -algo 1 -data_start 3 -data_log 0 -data_len 20
CRC_Generator.exe -bin esp32_sample_app.bin -out esp32_sample_app_crc.bin -offset 10 -algo 1 -data_start 15 -data_log 1 -data_len 30


To enable data log and save logs in txt file
CRC_Generator.exe -bin esp32_sample_app.bin -out esp32_sample_app_crc.bin -offset 15 -algo 1 -data_start 30 -data_log 1 > log.txt

Help:
-----
CRC_Generator.exe -help

Note:
-----
First byte in binary file starts from zero.

Tested below senarios for this utility:
---------------------------------------
1. Input bin file should not be changed
2. CRC should be calculated and updated at correct offset in output binary file
3. Data length should be calculated based on input file size and start address when data len is either '0' or not given
4. Display error when any of the input offsets and data length is invalid
5. File path and name of Input/output files should not be same
6. Less number of arguments
7. Display help menu
8. Data length is given but other arguments are missing
