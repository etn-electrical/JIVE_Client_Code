#
# Main Makefile. This is basically the same as a component makefile.
#
# Here Manually including the Lib files for compilation to exclude some files from compilation.
#This has added for avoiding the Ram_hybrid (IAR dependency) files to exclude from compilation & keep it in folder
#
COMPONENT_ADD_INCLUDEDIRS := .