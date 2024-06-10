# How to Detect Leaks in Babelfish Code

This helper describes a means whereby you can instrument your code to provide the appropriate output
to detect and help you narrow down to the source of leaks.

This is not done using IAR's C-Run runtime checking, since that unfortunately permits only very small
applications with the basic license we have for it.
Nor does it use Coverity (though that is highly recommended) or Valgrind (don't know how to apply that here).

This is done with simple debug logging and a python script to reduce the output to a probably list of targets.

## Steps before you build

1. Enable TermIO output for your build, and enable the TermIO.log file to capture its output, as we will need that for analysis.
2. Enable RAM_TERM_PRINT (and possibly RAM_DIAGNOSTICS, though this didn't seem essential) for your build.
   1. You can define them in the C++ options, but then you have to rebuild all.
   2. I just added them to one of the external control files (eg, iot_define_config.xcl) that was already part of my build, as:

```
/* For heap checking: */
//-D IAR_HEAP_MONITOR
-D RAM_DIAGNOSTICS
-D RAM_TERM_PRINT
```

I had tried IAR_HEAP_MONITOR but did not find it too useful.

3. Find a place in your code that it keeps cycling through, though not too often (eg, >10s cycle) and add this output to report on the currently available heap, so you can see if it is decreasing over time:

```C
BF_DEBUG_PRINT( DBG_MSG_INFO, "After Closing: 0 heap usage = %u%% and available = %zu", Ram::Used(0), Ram::Available(0) );
BF_DEBUG_PRINT( DBG_MSG_INFO, "After Closing: 1 heap usage = %u%% and available = %zu", Ram::Used(1), Ram::Available(1) );
```

(I found that the usage of the second memory area (1) did not change in my application.)

The output looks something like this:

    After Closing: 0 heap usage = 57% and available = 62832
    After Closing: 1 heap usage = 4% and available = 5883

4. When you build, be sure to "touch" file Babelfish\Code\Lib\Ram.cpp, so it is built with the RAM_DIAGNOSTICS and RAM_TERM_PRINT defined.
5. I found the code unexpectedly Hard Faulting until I increased CONFIG_MINIMAL_STACK_SIZE in OS_Config.h:

   #if defined (uC_STM32F765_USAGE) || defined (uC_STM32F767_USAGE)
      // Or maybe use this larger size if RAM_TERM_PRINT or PX_GREEN_IOT_SUPPORT
      #define CONFIG_MINIMAL_STACK_SIZE			256U
   #else
      #define CONFIG_MINIMAL_STACK_SIZE			70U
   #endif

6. To minimize the bulk of the DebugManager messages, I disabled the timestamp and function name from the output in Debug_Manager_Config.h:

   #define BF_DBG_PRINT_TIME           DBG_OFF
   #define BF_DBG_PRINT_FUNC_NAME      DBG_OFF

7. And I increased the DebugManager queue size dramatically in Prod_Spec_Debug_Manager.cpp, and added to the heap allocation as well:

   static const uint16_t DBG_MANAGER_CIRCULAR_BUFFER_SIZE_BYTES = 8096U;

   define symbol __ICFEDIT_size_heap__   = 0x35000;



## Steps for analyzing your output

Mixed into TermIo.log output with your debug output will be the occasional heap usage reports,
and a great deal of logging output, eg:

```
 3 Ram #:0, Ptr: 2001f830, Size Alloc: 384, Percent Used: 57
 3 Ram #:0, Ptr: 2001f9b8, Size Alloc: 384, Percent Used: 57
 3 Ram #:0, Ptr: 2001fb40, Size Realloc: 512, Percent Used: 57
 3 Ram #:0, Ptr: 2001fd48
 3 Ram #:0, Ptr: 2001f830
 3 Ram #:0, Ptr: 2001f9b8
```

The first 3 lines above were logged in malloc or realloc operations, 
and the last 3 were free or delete operations.
Some of the pointers match in the lines above, where memory was allocated
and then released; others may be matched elsewhere in the output, but
any allocations without a corresponding free are possible memory leaks.

1. Open TermIO.log with an editor, and using the regular debug output, try to find an appropriate subsection to analyze for leaks.
   1. For example, if you have a regular cycle of activity that may have a leak, determine a good starting point and then the same point in the next cycle.
   2. You could analyze all the code from the start of main, but we know that there is a lot of memory allocated and never freed on purpose, so that would obscure your real memory leaks.
2. Copy that section of output to a new file. (I will use "HeapDebug.log" in this writeup; 
in my application, a 20s cycle resulted in over 2500 lines of output, 
dominated by the heap logging messages.)
3. Now you will analyze the heap messages to screen out the uninteresting allocations with matching frees, leaving just the possible leaks.
4. To do this, you will run a python script, Babelfish\Tools\Leak Detection\LeakScanner.py.
   1. If you haven't done so already, install python and PyCharm or another IDE, as described in [this confluence page](https://confluence-prod.tcc.etn.com/display/LTK/Installation+of+Python+and+PyCharm+for+PyTest+on+Windows).
   2. Run the LeakScanner script, giving it the path to your log file extract, and the name of that file.
   3. This will produce a distilled output file with your regular debug output lines plus highlighting of any possible memory leaks.
      1. This file will be name OUT_<logfilename> and located in the same folder as your logfile extract.
      2. Eg, if you named your extract HeapDebug.log, the output will found in OUT_HeapDebug.log.
   4. The leaks will be plainly noted, eg:

```
###_LEAK_####  3 Ram #:0, Ptr: 2001f6f8, Size Alloc: 384, Percent Used: 57
###_LEAK_####  3 Ram #:0, Ptr: 20016e20, Size Alloc: 16, Percent Used: 57
###_LEAK_####  3 Ram #:0, Ptr: 2001d348, Size Alloc: 8192, Percent Used: 57
###_LEAK_####  3 Ram #:0, Ptr: 20016e38, Size Alloc: 92, Percent Used: 57
```

5. Examine the output and see where the potential leaks are arising.


## Steps for Zeroing in on your leaks

Nothing in the output, unfortunately, tells you which object it is which might be leaking, so you have to use the debugger and a little extra code to find out where it is or might be.

In this example, we will try to find the source of the 384 byte leak shown above.

1. Add something like the following lines temporarily to Ram::Allocate(), inside the RAM_DIAGNOSTICS or RAM_TERM_PRINT section:

```C
        /* Catch the cases that look like our leaks, normally by size: */
        if ( size == 384 )
        {
            RAM_Term_Print( "Set a breakpoint here!" );
            // Be sure that #pragma optimize=none is set above for this breakpoint to hit correctly, just on this case.
        }
```

2. Also, as indicated, add these lines immediately before Ram::Allocate() so that compiler optimization doesn't prevent you from setting the breakpoint exactly on the RAM_Term_Print() line:

```C
#ifdef RAM_DIAGNOSTICS
    #pragma optimize=none
#endif
```

3. Set a breakpoint on the RAM_Term_Print() line, but disable it at the startup.
4. Run your code until the operations has reached your area of interest, then enable that breakpoint.
5. When it hits the breakpoint, check the Call Stack to see what is allocating this size of memory.
6. You will probably want to let the code keep running then and stop at this breakpoint again at least a few times, to make sure you have all of the instances of potential interest.

