# SBLCP_local_terminal_GUI_esp32_code
The code is for the esp32 dev kit board to use with the SBLCP_local_terminal_GUI (https://github.com/etn-electrical/SBLCP_local_terminal_GUI/tree/main)

This code inherits the breaker code because it uses some functions from the breaker firmware code (Babelfish).

The main files that deals with the client side of the communication is:
<br>Code/app_main.cpp ==> This is the main function, the program starts here.
<br>Code/udp_client_for_GUI.c ==> This is where all the client code lives.
<br>Code/udp_client_for_GUI.h ==> The header file for the client code.
<br>Code/SBLCP/SBLCP.h ==> Some struct definition lives here. I included this file so that I won't redefine them.

The CMakeLists.txt is also modified to accomodate my client code (udp_client_for_GUI.c).
