/**
 *****************************************************************************************
 * @file
 *
 * @brief Includes definitions for the command Shell.
 *
 * @details
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef SHELL_LIB_H
#define SHELL_LIB_H

/**
 *****************************************************************************************
 * @brief Shell Namespace
 *
 * @section abstract Abstract
 * Shell is a simple command shell to be used for testing and debugging of embedded systems.
 *
 * @section description Description
 * The Shell is a means of providing simple human interface to embedded systems for the purpose of
 * testing and debugging. It utilizes ASCII text via a serial octet stream to a character terminal
 * emulator
 * on a personal workstation (e.g. Putty, Hyperterm, etc.).
 *
 * Only the 7-bit ASCII character set is used. No provision is made for human languages other than
 * US English.
 * In particular, we do NOT support multibyte character sequences, code pages, Unicode UTF-8, etc.
 *
 * Only very basic editing is provided. The backspace key may be used to delete the previous
 * character.
 * The control-X key combination may also be used to abort the entire command line. Note that we do
 * NOT
 * implement terminal emulations such as ANSI/VT-100 or telnet. We do NOT implement advanced
 * features found on
 * Linux or Windows shells such as command recall or autoamtic command suggestion/completion.
 *
 * In principal, any full-duplex octet stream may be used to carry the commands and responses
 * between the user
 * and Shell. The most obvious examples are RS-232 asynchronous serial links or USB CDC virtual COM:
 * ports.
 * Berkley socket tcp streams should also be suitable on platforms where Ethernet MAC/PHY and TCP/IP
 * protocol
 * stack are provided. Links such as CAN, SPI, I2C, USB Bulk should be feasible given appropriate
 * drivers
 * and protocols.
 *
 * It is also conceivable that automated scripts running on host workstations could emulate the
 * behavior of
 * a human user. This is envisioned as a practical means of accomplishing repeatable regression
 * testing.
 *
 * @section CLE CLE Command Line Editor
 * The CLE handles the editing of a stream of characters as typed by a human user at an ASCII
 * terminal keyboard.
 * It presumes only a minimum functionality of the terminal, such as ability to perform backspace
 * and bell.
 *
 * The CLE is implemented by class Shell::Command_Line_Editor:: in files Command_Line_Editor.h and
 * Command_Line_Editor.cpp.
 *
 * @section CLI CLI Command Line Interpreter
 * The CLI parses the command line given by the user, and dispatches it to the corresponding command
 * function.

 * The CLI is implemented by class Shell::Command_Line_Interpreter:: in files
 * Command_Line_Interpreter.h and Command_Line_Interpreter.cpp.
 *
 * @section Shell_Base Shell_Base
 * The Shell is agnostic of the details of the environment, such as whether or not an RTOS is used,
 * etc. The abstract
 * base class Shell::Shell_Base:: is used to encapsulate and hide such platform-specific details.
 * See files Shell_Base.h and
 * Shell_Base.cpp for implementation.
 *
 * @section Command Shell Commands
 * Each command to be executed is derived from Shell::Command_Base::, which is an abstract base
 * class
 * defining the
 * required interface.
 *
 * If the function runs asynchronously, it may be derived from Shell::Async_Command_Base::.
 *
 * @section ExampleCmds Example Commands.
 * Three commands are provided for testing and as application examples. They are "Foo", "Bar" and
 *"Baz".
 *
 * The "Foo" command is implemented in class Shell::FooExampleCommand:: in files FooExampleCommand.h
 * and FooExampleCommand.cpp.
 *
 * The "Bar" command is implemented in class Shell::BarExampleCommand:: in files BarExampleCommand.h
 * and BarExampleCommand.cpp.
 *
 * The "Baz" command is implemented in class Shell::BazExampleAsyncCommand:: in files
 * BazExampleAsyncCommand.h and BazExampleAsyncCommand.cpp.
 *
 *****************************************************************************************
 */

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif

namespace Shell
{

#define UNIT_TEST ( 1 )

#if defined( UNIT_TEST )
#define YOUR_PLATFORM_SPAWN( abc )        abc()
#define YOUR_PLATFORM_KILL( xyz )
#else
#define YOUR_PLATFORM_SPAWN( abc )        abc()
#define YOUR_PLATFORM_KILL( xyz )
#endif

class Shell_Base;
class Command_Line_Editor;
class Command_Line_Interpreter;
class Command_Base;
class Command_Base;

}	// namespace Shell

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>



#include "Shell_Base.h"
#include "Command_Line_Editor.h"
#include "Command_Line_Interpreter.h"
#include "Command_Base.h"
#include "Async_Command_Base.h"

#endif	/* SHELL_LIB_H */

