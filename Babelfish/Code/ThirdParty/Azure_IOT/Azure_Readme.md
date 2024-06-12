# Notes on building Azure for Babelfish

## Repository for Azure C Device SDK

The Azure code is kept in a separate git repository, and included as a
submodule here, under the `Azure_repo` folder.

## Checking out the Azure repository

Since Azure is in a git submodule, after cloning your top level product,
you will want to get the Babelfish repository and its submodules (Azure,
and the Eaton IoT Device SDK), so use this command:

`git submodule update --init --recursive`

The recursive option will check out the submodules of Babelfish.

## Configuring for the Azure build

Normally Azure is built with CMake, and you tell it which options to configure it with and without. It is compiled as C code.

While we could insert the build options in the C/C++ Preprocessor list of 
the IAR Project file, that is somewhat harder to maintain.

So instead, we will revise Azure's iothub_transport_ll.h header to include a file which specifies the product-specific configuration; this file is named  `iot_device_platform.h`, and it is found in the `Babelfish\Code\BF\IOT` folder. (This same file specifies the configuration for the Eaton IoT Device SDK.)

## The Babelfish Adapter files

The files in the `Azure_ltk_adapters` folder provide the bridge between Azure code
and the hardware-specific code of the product using Azure for IoT.



