@ECHO OFF

start /B /WAIT cmd /c esp32_common\esp-idf\install.bat && esp32_common\esp-idf\export.bat && idf.py build && pause

cmd /k
pause