This command line tool should be used to create the codepack file for Secure Firmware Upgrade.
The tool accepts INI file(/s) as input containing details about Product, Processor & Images along with path of hex file.

Command Line Usage:
-------------------
LTKCodePackGen.exe silent createcodepackfile inifolder="<path>" xmlfile="<codepack file path>"


Example - 1: 
------------
In this example 'inifolder' is containing single INI file with information for LTK Main Processor and Web Firmware Image details.

Command Line:
-------------
LTKCodePackGen.exe silent createcodepackfile inifolder=".\LTK INI" xmlfile=".\LTK_CodePack.xml"


Example - 2:
------------
In this example 'inifolder' is containing multiple INI files with information for LTK Main Processor and Host Processor,
along with details of Web and App Firmware.

Command Line:
-------------
LTKCodePackGen.exe silent createcodepackfile inifolder=".\LTK INI Multi" xmlfile=".\LTK_CodePack.xml"


Note:
-----
Please refer to 'patb\tools' for the source code of this tool