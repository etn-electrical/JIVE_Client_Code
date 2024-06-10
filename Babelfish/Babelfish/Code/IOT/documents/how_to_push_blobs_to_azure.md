# How to push Blobs to Azure

There is a program called **UploadBlob.exe**, written in C#, that will Base64 encode a binary firmware image and post it to the Azure blob storage. 
This program:

* Reads a configuration file for its input parameters, which detail the metadata for the image as well as practical information for file location, use authentication, and PXWhite adopter and site
* Connects to the indicated Azure IoT Hub with the given credentials
* Uploads the indicated file to Azure Blob Storage
* Returns the UUID for this Blob, which the DLM APIs use to refer to this blob for Firmware Update.

The following sections describe the steps you should follow to upload firmware images to Azure using this program.
You could store other files this way, but the focus in this How To is on Firmware Image files for PXGreen.
You will follow the same procedure for other firmware images as well; Azure does not know or care what the contents of the Blob are,
though we do provide metadata when uploading the Blob to tell ourselves what the contents are.

## 1) Getting the UploadBlob.exe program

1) Download the zip file containing UploadBlob.exe: [UploadBlobUtil.zip](https://confluence-prod.tcc.etn.com/download/attachments/182297417/UploadBlobUtil.zip?version=5&modificationDate=1627568672503&api=v2)
2) Unzip to a folder on your PC (eg, `<some path>\UploadBlobUtil`). There is no installation program; you will just run this file from the command line, from this folder.
3) You will find it helpful to create a subfolder for a specific target, eg `<some path>\UploadBlobUtil\767`. In this folder, you can conveniently keep multiple firmware images and the configuration file(s) for that target.

## 2) Copy your Firmware Images

You will want to use the latest signed .bin image files that you find under Build_Output folder, eg `ltk_sample_app\RTK_Example\Build_Output`.
Copy the following to your target subfolder, eg `<some path>\UploadBlobUtil\767`:

* `Build_Output\<target_processor>\Exe\RTK_Example.signed.bin` (the "App" image)
* `Build_Output\Web\web2.signed.bin` (the Web 2.0 UI image)
* `Build_Output\Language\Language.signed.bin` (the optional Languages image)

Note: For ESP32, you will find .bin image files under build folder, eg `esp32_sample_app\build`.

You will refer to these files in the configuration files you create, one configuration file for each.

## 3) Create the Configuration file for each Image

The program UploadBlob.exe is very simple; it will just look for and process a file which must be named UploadBlob.exe.config, found in the same folder as the .exe program.

However, it is convenient to keep separate copies for each image. The easiest way to do this is to create configuration files named, eg, UploadBlob.exe.config.App, UploadBlob.exe.config.WebUI, and UploadBlob.exe.config.Languages in your subfolder where the binary files are.

Then when you are ready to use one, copy it to the UploadBlobUtil folder and remove the extension (.App, .WebUI, or .Languages), and you will have the one instance of the rightly named file.

The next two sections show an example configuration file and explain what is important about the fields.

### 3a) Example of a Languages Configuration File

The following is a sample UploadBlob.exe.config file, showing possible values for a Languages image.

```xml
<?xml version="1.0" encoding="utf-8"?>
<configuration>
    <startup>
        <supportedRuntime version="v4.0" sku=".NETFramework,Version=v4.8"/>
    </startup>
  <appSettings>
    <!-- Rest Interface URL -->
    <add key="WebUrl" value="https://testiotwebapi.eaton.com/api/v1/"/>
    <!-- add key="WebUrl" value="http://testiotwebapi.eastus.cloudapp.azure.com/api/v1/"/ -->   
    <!-- Users email address -->
    <add key="User" value="UserId_here@eaton.com"/>
    <!-- Users password -->
    <add key="Password" value="password_here"/>
    <!-- Application UUID -->
    <add key="Application" value="e334cb47-2bb8-4e66-98fb-89683a22d293"/>
    <!-- Adopter UUID -->
    <add key="Adopter" value="a5ddbb34-1570-11e7-93ae-92361f002671"/>
    <!-- Image Name -->
    <add key="Name" value="PXGREEN LANGUAGE IMAGE"/>
    <!-- Image Description -->
    <add key="Description" value="French and German; Path: 0/2 Compat#: 4"/>
    <!-- Image Type -->
    <add key="Type" value="PXGREEN LANGUAGE IMAGE"/>
    <!-- Image version -->
    <add key="Version" value="0.0.1"/>
    <!-- Image timestamp -->
    <add key="Timestamp" value="2020-10-22T00:00Z"/>
    <!-- Device model image is Product "GUID" -->
    <add key="Model" value="4660"/>
    <!-- Image format -->
    <add key="Format" value="binary"/>
    <!-- Image crc  -->
    <add key="Crc" value="0"/>
    <!-- Path of file to upload -->
    <add key="File" value="767/Language.signed.bin"/>
   
    <!-- Http request/response timeout in hours-->
    <add key="Timeout" value="1"/>
    
    <!-- add api-key -->
    <add key="apikey" value="d299ea9c20924421a732bc84100ba9b9"/>
    
  </appSettings> 
</configuration>
```

### 3b) Configuration File Fields

Some of the fields are free text, but the contents of most are significant, as explained here.
If you want to see how some of the fields are mapped into the UpdateFirmware request, 
check the [Design page here](https://confluence-prod.tcc.etn.com/display/LTK/Design+of+FUS+over+IoT#DesignofFUSoverIoT-MappingofDLMUpdateFirmwareandUploadBlobparameterstoBabelfishparameters).

* **WebUrl** points to the IoT Hub that your product uses; the blob storage is associated only with this hub, so if you should need the same firmware in two different hubs, you will have to push it to both and keep both image UUIDs.
* **User** and **Password** are the ones you use for Swagger or PX White API access
* **Application** and **Adopter** are per your Hub assignments.
  * These are the fields you are most likely to get wrong, and have your blob upload refused.
  * From /api/v1/devices/{id} (in Swagger), with your device UUID, you can get your site-id
  * Use that with /api/v1/sites/{id} to get your Adopter ID 
  * You will have to ask your Adopter Admin for the Application ID to use here
* **Name** is a free text name for this Blob
* **Description** is free text, except that it must contain "Compat#: X" where X is the image's "Compatibility Number".
  * Normally an update should not be done for a firmware image whose Compatibility Number is lower than that of the currently running firmware.
* **Type** is, ideally, signficant and ought to match the Codepack's `<Image><Description>` text. 
  * But this has proven too changeable to validate at the time of the UpdateFirmware command.
* **Version** is the Image Version, which ought to be the same as the Codepack's `<Image><Ver>`
  * Important metadata for informative purposes, but there is no good way to validate it algorithmically in the device
* **Timestamp** is the time at which the binary file was built
  * Also informative but otherwise unused
* **Model** is the Codepack's `<Product><ProductGuid>`, a number given in text form
* **Format** is always "binary", for Base64 encoded image data
* **Crc** is a useful but unused concept; set to 0 since it is not used.
* **File** of course names the .bin file that you are uploading.
* **Timeout** puts an upper bound on the time to upload, but normally all uploads happen in seconds, not hours. Use a value of 1.
* **apikey** apikey of Swagger or PX White API access need to ge integrated.

## 4) Running the program

The UploadBlob.exe program is very simple to run.

1) Open a Command console or Windows Powershell session
2) Cd to the folder where you unzipped the executable
3) Simply run `.\UploadBlob.exe`, with no arguments.
   1) The program will get its operating configuration from the file named UploadBlob.exe.config
4) Answer the question about Uploading or Downloading with a '1' for Uploading.
5) When it is done transferring, copy the Blob ID that is returned as a UUID.
   1) It will return a line like `Firmware uploaded successfully. Time Elapsed: 00:00:35.8687618, Image Id: {"id":"b1090594-acb3-4ff8-8b77-a9051b592429"}`
   2) Copy the UUID after the `"id":`; this is your blob's UUID.
6) Hit return to exit the program.

Note that if you run the program again with the same Name and Description, even if your file contents have changed,
you will replace the existing blob with the new contents but keep the same Blob UUID.

## 5) Saving the Blob UUID

Quite simply: Do It!

You will need it to be able to schedule a Firmware Update for this image.

