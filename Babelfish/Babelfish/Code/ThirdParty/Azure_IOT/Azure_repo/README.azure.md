# Azure SDK for PX Green

This respository holds the source 'C' and header files from the Microsoft Azure IoT SDK
that are necessary for the PX Green Babelfish projects that need the IoT features.

This repository will be a submodule of the Babelfish code, and use these files directly.

The files found here are "distilled" from the full distribution of the Azure SDK, restricted to the features necessary for Babelfish. The process for this is found in the Azure_update folder described below.

These files may have been revised slightly for the PX Green builds after being placed here.

## Getting the latest azure-iot-sdk-c image

You will get the source from Azure's GitHub site, and store it in our Artifactory storage.
1. Browse to https://github.com/Azure/azure-iot-sdk-c/tree/master
2. Get the https link to clone the repo, https://github.com/Azure/azure-iot-sdk-c.git 
3. Clone it to your workstation or laptop. (You may have some trouble doing this in the Eaton network.)
4. Git checkout the desired Tag or Branch, eg release-2020-07-19
5. Do a submodule update, being sure to use --init and --recursive. (It will take a while.)
6. Rename the root folder to the name you want, eg azure-iot-sdk-c-2020-07-19.
7. Delete the .github folder, which will reduce the size of the zip file.
8. Zip or 7zip the folder and all its contents.
9. Browse to https://artifactory-prod.tcc.etn.com/artifactory/webapp/#/artifacts/browse/tree/General/PX-Embedded-External/vendor_source/azure
10. Log In to Artifactory
11. Select the 'Deploy' button, and in the dialog that pops up, browse to your zip file, select it, (leave the other options as is), and hit the 'Deploy' operator. This will upload your zip file to Artifactory.
12. Note the new file name and path for the next section.

## Azure Update process

When it is necessary to update the version of the Azure C SDK for Babelfish use, follow these steps:

1. Start on a new branch for this repo, since you will be changing the source and header files.
2. Modify the Artifactory download URL and the Azure version folder name in the \Azure_update\config.txt file with the new appropriate names.
2. Open a normal Windows command prompt and cd to the Azure_update folder. (This PC must have the Windows PowerShell installed.)
3. Run the batchfile, simply with the command 'Azure_Version_Update.bat' (with no arguments). 
   1. It will prompt you for your E# and password, to be able to log into Artifactory for the download.
   2. It will download the latest Azure version from the location you gave it.
   3. It will unzip select parts of it and then copy the folders of interest over to the folders in this repo, overwriting the existing files. This is done using the 7zip executable which is included in that folder.
   4. It will rename some files to avoid conflicts with similarly named files elsewhere in Babelfish.
   5. It will remove some of the files copied over from the distro for unused features (like AMQP, HTTP, Blob support) to minimize the build size.
4. Review the results and see if you need to make any adjustments in the config.txt or Azure_Version_Update.bat due to changes in Azure; for example, sometimes Microsoft moves folders around in Azure.
5. When your batch file is running correctly, if there are a lot of differences and renamed or removed files, 
you might find it easier to remove the c_utility, iothub_client, umock_c, and umqtt folders before running the batch file.
6. Rebuild your product (eg, LTK_Example_App) and fix any new build errors in the files in this repo.
   1. A frequent and recurring problem is that IAR wants all malloc() returns cast to their proper type, so you may have to fix those up.
7. Do a pull request to check in all of the changes that you have made, either in the Azure_update folder or in the source and header files in this repo.
