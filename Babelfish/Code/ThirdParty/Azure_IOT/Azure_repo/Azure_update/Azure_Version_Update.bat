@echo off
rem Run this batch file from a regular windows cmd shell
rem It will call PowerShell as needed.
rem
setlocal enabledelayedexpansion
rem cls
mkdir temp
set /A past_value=0
set /A present_value=0
set /A change=0
set azure_version=""
set temp_azure_zfile=azure-iot-sdk.7z

set start_url=start_url
set end_url=end_url
set start_copy=start_copy
set end_copy=end_copy
set start_rename=start_rename
set end_rename=end_rename
set start_unzip=start_unzip
set end_unzip=end_unzip
set start_deleteme=start_deleteme
set end_deleteme=end_deleteme

echo Starting...
for /f "tokens=*" %%a in (config.txt) do (
  echo Processing token: %%a
  if %%a==%start_url% (

	set /A present_value=1

	set /A change=1

	)
  if %%a==%end_url% (

	set /A present_value=2

	set /A change=1

	)
  if %%a==%start_copy% (

	set /A present_value=3

	set /A change=1

	)
  if %%a==%end_copy% (

	set /A present_value=4

	set /A change=1

	)
  if %%a==%start_rename% (

	set /A present_value=5

	set /A change=1

	)
  if %%a==%end_rename% (

	set /A present_value=6

	set /A change=1

	)
  if %%a==%start_unzip% (

	set /A present_value=7

	set /A change=1

	)
  if %%a==%end_unzip% (

	set /A present_value=8

	set /A change=1

	)
  if %%a==%start_deleteme% (

	set /A present_value=9

	set /A change=1

	)
  if %%a==%end_deleteme% (

	set /A present_value=10

	set /A change=1

	)
  if !present_value!==0 (
    if !past_value!==1 (

		rem Needed to put the Artifactory login and download into a Powershells script
		rem PowerShell -NoProfile -ExecutionPolicy Bypass -Command "& '.\artifactory-download.ps1' '%%a' '.\temp\azure-iot-sdk.zip' "
		echo PowerShell -Command "Invoke-WebRequest -Uri '%%a' -OutFile '.\temp\%temp_azure_zfile%' "
		PowerShell -Command "Invoke-WebRequest -Uri '%%a' -OutFile '.\temp\%temp_azure_zfile%' "
		echo Downloaded to temp\%temp_azure_zfile%
	)
	)
  rem cls	
  if !present_value!==0 (
    if !past_value!==7 (

		rem Save the Azure Version as a local variable
		set azure_version=%%a
		cd temp
		rem echo extracting the zip file 
		rem ..\7z.exe x -aoa azure-iot-sdk.zip
		echo unzipping just specific folders from the Azure zipfile to %azure_version%
		rem -ir!c is recursively extract just files (folder contents) starting with the wildcard 'umqtt', ie, 'umqtt/*'
		rem Have to escape the '!' (bang) char with double carets, since we have enabledelayedexpansion
		..\7z.exe x -aoa -ir^^!c-utility %temp_azure_zfile%
		..\7z.exe x -aoa -ir^^!iothub_client %temp_azure_zfile%
		..\7z.exe x -aoa -ir^^!umqtt %temp_azure_zfile%
		cd ..
		rem Use '!' (bangs) for late variable expansion, since azure_version is initially ""
		dir temp\!azure_version!\ /W
		pause
	)
	)
  if !present_value!==0 (
	if !past_value!==3 (
		rem Divide the line into two tokens
		for /f "tokens=1,2" %%F in ("%%a") do (
			set from_dir=%%F
			set to_dir=%%G
			echo Copying files from temp\!azure_version!\!from_dir! to ..\!to_dir!
			xcopy /e /v /Y temp\!azure_version!\!from_dir! ..\!to_dir!
		)
	  )
	)
  if !present_value!==0 (
	if !past_value!==5 (
		echo Renaming the Files %%a
	    move %%a
	)
	)
  if !present_value!==0 (
	if !past_value!==9 (
		echo Removing the File or Files %%a
	    del %%a
	)
	)
  if !change!==1 (
	set /A past_value=!present_value!
	set /A present_value=0
	set /A change=0
   )
  )
echo Deleting the downloaded and extracted files/folders 
echo Wait.........
pause
@RD /S /Q ".\temp"
echo Modified the Azure C SDK files
