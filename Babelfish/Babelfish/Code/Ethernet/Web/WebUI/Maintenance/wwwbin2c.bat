cd gzip
..\txt2hex.exe ..\s_maintenance_pages
cd ..\
xxd -i s_maintenance_pages > ..\..\maintenance_pages.c
