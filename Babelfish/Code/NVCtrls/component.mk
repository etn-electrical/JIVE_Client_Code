#
# Main Makefile. This is basically the same as a component makefile.
#
# Here Manually including the Lib files for compilation to exclude some files from compilation.
#This has added for avoiding the Ram_hybrid (IAR dependency) files to exclude from compilation & keep it in folder
#
COMPONENT_OBJS :=  Defines.o Bit_List.o Button.o CRC16.o Exception.o Input.o Input_Neg.o Input_Pos.o Linked_List.o Output.o Output_Array.o Output_Neg.o Output_Pos.o Ram_Handler.o Ram_Hybrid_ESP32.o Ram_Static.o Rand_Num.o Remote_Input.o Remote_Output.o Scratch_Ram.o Services.o StdLib_MV.o Timers_Lib.o Toolkit_Revision.o
COMPONENT_ADD_INCLUDEDIRS := . 