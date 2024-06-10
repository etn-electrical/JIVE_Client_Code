cd gzip
..\txt2hex.exe ..\s_default_page
cd ..\
xxd -i s_default_page > ..\..\default_page.c
