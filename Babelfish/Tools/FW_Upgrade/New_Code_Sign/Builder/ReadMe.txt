code_sign_builder_signer.bat is the main utility and sign the image using local or server sign. below utilities are used by the main utility.


1) 
code_sign_info.exe -bin %bin_path_407%web_Upload_Pack.bin -fvk %local_cert_path%fvk.der -ivk %local_cert_path%IVK.der -ini Code_Sign_Info_header.ini -out outfile

2) 
Sign_Image.bat %bin_path_407%web_Code_Sign_Upload_Pack.bin

3) 
append_sign_info.exe -bin %bin_path_407%web_Code_Sign_Upload_Pack.bin -fwsign fw_sign.bin -out outfile

4)
append_cross_sign_info.exe -bin bin file -fwsign trusted fw signature file -tfvkc trusted fvk cert file


