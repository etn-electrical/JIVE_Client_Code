#!/usr/bin/python

'''
Purpose: To parse the debug output from the Babelfish code's TermIO output
and determine which allocations or re-allocations do not have matching free statements.

Eg, for this output:
     3 socketio_dowork [20:57:13] Info: Have received 2920 bytes
     3 Ram #:0, Ptr: 200174b0, Size Alloc: 928, Percent Used: 58
     3 Ram #:0, Ptr: 20017aa0, Size Alloc: 12, Percent Used: 58
    ...
     3 Ram #:0, Ptr: 20017aa0
    ...
The first allocation does not have a matching free, but the second does.
The first will be reported; the second will be ignored.

Usage:

'''


import sys
import os

# Our global dictionary of pointer references.
# Each entry will be the pointer and its allocation line number
pointer_dict = {}

def pointer_check(line, line_number):
    """
    Given the line of heap debug output, get the pointer and determine if
    this line indicates the allocation of memory or its freeing.
    Eg,
         3 Ram #:0, Ptr: 20017aa0, Size Alloc: 12, Percent Used: 58
         3 Ram #:0, Ptr: 20017aa0
         3 Ram #:0, Ptr: 20024690, Size Realloc: 1024, Percent Used: 69
    The first line is an allocation at pointer 20017aa0, the second frees it, third is a reallocation

    If it is an allocation, add it to our pointer dictionary;
    if a free operation, remove it (if found) from our dictionary.
    :param line: The text of the line from our heap debug log
    :param line_number: The line number where this line occurs in the log file.
    :return: True on success (almost always).
                False, in the case where the line doesn't make sense
    """

    global pointer_dict
    # Find the pointer value
    index = line.find('Ptr: ')
    if index < 1:
        print("No Pointer found in line #{}: {}".format(line_number, line))
        return False
    # Advance beyond the "Ptr: " text
    index += 5
    end_index = line.find(',', index)
    if end_index > 1:
        pointer = line[index:end_index]
    else:
        # Line doesn't end in comma
        pointer = line[index:]

    # Now determine if this line was for an alloc or realloc, else free operation
    if 'Alloc' in line or 'Realloc' in line:
        # Add (or update) this entry
        pointer_dict[pointer] = line_number
        # print('Added {} at l# {}'.format(pointer, line_number))
    else:
        if pointer in pointer_dict:
            del pointer_dict[pointer]
            # print('Removed {}'.format(pointer))
        else:
            print('Did not find allocation for {} at l# {}'.format(pointer, line_number))
    return True


def create_leak_list( leak_list ):
    """
    Given the pointer dictionary contents, build a list of the line numbers of all the remaining entries,
    which would be those where the allocation did not have a matching free,
    which suggests that they are leaks.
    :param leak_list: The list, empty on entry, which we will out with line numbers of potential leaks
    :return: True on success (always)
    """

    global pointer_dict
    for line_number in pointer_dict.values():
        leak_list.append(line_number)
    print("Found {} possible leaks".format(len(leak_list)))
    return True


#################################################
## Main script program begins here:
#################################################

if len(sys.argv) < 3:
    print("Usage: " + __file__ + " <File Folder> <LogFile> \n")
    print("       Where the File Folder contains the input LogFile\n")
    print("       and where the output OUT_LogFile will be written to.\n")
    exit(1)

file_folder = sys.argv[1]
log_file = file_folder + '\\' + sys.argv[2]
temp_filename = file_folder + '\TEMP_' + sys.argv[2]
out_filename = file_folder + '\OUT_' + sys.argv[2]

exit_code = 0
# First, read the LogFile, and find instances where the #Ram statements are not
# at the beginning of the line, and break those lines
try:

    with open(log_file, 'r') as src:
        print("  Opened file {0} for prescanning.".format(log_file))
        with open(temp_filename, 'w') as tfile:
            for line in src:
                line = line.rstrip()
                index = line.find(' 3 Ram #')
                if index > 1:
                    tfile.write(line[0:index -1] + "\n")
                    tfile.write(line[index:] + "\n")
                else:
                    tfile.write(line + "\n")
        print("  Wrote file {0}.".format(temp_filename))

    # Now the scanning pass: for each "Ram #" line, pass it to the function
    # that adds and removes the entries from our pointer list
    with open(temp_filename, 'r') as src:
        print("  Opened file {0} for pointer scanning.".format(temp_filename))
        line_number = 0
        for line in src:
            line = line.rstrip()
            line_number += 1
            if 'Ram #' in line:
                pointer_check(line, line_number)

    # Now generate the list of line numbers to retain (which look like leaks)
    leak_list = []
    create_leak_list(leak_list)

    # Now generate the output; drop any "Ram #" lines not in our list,
    # but do include any in the list, along with the non-heap lines
    with open(temp_filename, 'r') as src:
        print("  Opened file {0} for rescanning.".format(temp_filename))
        with open(out_filename, 'w') as ofile:
            line_number = 0
            for line in src:
                line = line.rstrip()
                line_number += 1
                if 'Ram #' in line:
                    if line_number in leak_list:
                        marked_line = "###_LEAK_#### " + line + "\n"
                        ofile.write(marked_line)
                    # Else we silently drop this heap log line
                else:
                    ofile.write(line + "\n")
        print("  Wrote debug and leak suspects to file {0}.".format(out_filename))


except IOError:
    print("ERROR: Did not find LogFile: {0}".format(log_file))
    exit(1)

print( "SUCCESS: wrote output to {0}".format(out_filename))
exit( exit_code )

