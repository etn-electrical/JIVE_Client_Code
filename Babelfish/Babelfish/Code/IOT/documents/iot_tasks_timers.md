# Tasks and Timers in PXGreen IOT Code

Since the PXGreen code is based on FreeRTOS, there are no separate threads of execution; all operation flows from tasks within the FreeRTOS framework.

The tasks described in the following sections are involved in the startup and operation of the IoT code.

## Initialization Task

The IoT code begins in PROD_SPEC_IOT_Init(), which is started as part of the PROD_SPEC_ETH_Init() task.

If IoT is enabled in the build,

1. PROD_SPEC_IOT_Init() instantiates IOT_Net::IOT_Net().
2. IOT_Net() creates a timer to run IOT_Net::Connection_Check_Timer_Task().
3. If IoT is enabled in the DCI configuration, IOT_Net instantiates and initializes IOT_Pub_Sub::IOT_Pub_Sub().
4. IOT_Pub_Sub in turn instantiates IOT_Group_Handler::IOT_Group_Handler() once for each Cadence Group.
5. Each instance of IOT_Group_Handler creates a timer to run IOT_Group_Handler::Cadence_Check_Timer_Task() whose time period is set by cadence rate
6. The function Initialize_Cadence() reads the cadence rate set by user, validates it and sets the time period of Cadence_Check_Timer_Task(). In this case, if cadence rate is -1(stop publishing) then IOT_Group_Handler creates a timer but doesn't start it
7. Another function Reinitialize_Cadence() also reads the cadence rate set by user, validates it and sets the time period of Cadence_Check_Timer_Task() at runtime(dynamically). When new cadence rate is set by user, it stops the Cadence_Check_Timer_Task() and then restarts it with new cadence rate immediately. In this case, if cadence rate is -1(stop publishing) then the timer is simply stopped
8. If cadence rate is 0(immediate publishing) then time period of the timer is set to 100ms as IOT_Task() sleep time is 100ms which is configured by IOT_MESSAGE_SLEEP_MS. That's the minimum resolution we can support.
9. If the group has Realtimes or StoreMe *( aka StoreMe on change)*, IOT_Group_Handler instantiates Change_Tracker_Plus::Change_Tracker_Plus() to track updates in its channels.

Note that all of this has happened within what is actually the Ethernet startup task. Note too that the code has not yet attempted to connect to Azure or publish messages.
This is intentional design, to keep what happens within the Ethernet startup task brief and to not hold up the other areas initiated by the Ethernet task.

There is an alternate flow if IoT is **not** enabled in the DCI configuration at step 3. In that case, the IOT_Net::Connection_Check_Timer_Task() will check when it is periodically run to see if IoT has now changed to **enabled** in the DCI configuration, and then run the rest of steps 3-6 if so.

## IoT Timers

In the FreeRTOS system, all the timers are executed under one task.
Consequently, it is important to keep the timer task execution brief and not block or sleep.

With that in mind, the IoT timer tasks named above have been designed to do their work and set up work for the IOT_Task() and messageThread() to carry out.

## The IoT SDK's messageThread

After initialization, almost all of the activity in the IoT code takes place under the control of the IOT_Task() and its worker function, messageThread().
The  IOT_Task is created as a FreeRTOS OS task upon the first attempt to connect to Azure.

The IOT_Task() runs in a loop that does just two things:

1) Call the messageThread worker function.
2) Sleep for the time configured by IOT_MESSAGE_SLEEP_MS (by default, 100 milliseconds, set in IOT_Config.h).

The messageThread() worker function in turn does just two things:

1) Call the registered (so-called) timerCallback() function.
2) Call IoTHubClient_LL_DoWork(), which enables the Azure SDK code to do any sending, receiving, or house-keeping that it as to manage.

In the PXGreen IoT code, there is no configured delay at step 1 of the messageThread(), since we want to invoke the timerCallback every time through the messageThread loop. The function that is actually called is IOT_Net::Timer_Task(). Note that this isn't a true "Timer Task" in the FreeRTOS sense of the term; it is called that in the IoT SDK code for historic reasons, however.

Note that all of the IoT SDK's callback functions are actually invoked by Azure code running under the lower level Azure DoWork() functions.

For the "sleep" in step 2 of the IOT_Task(), PXGreen uses a FreeRTOS semaphore in its implementation of platform_message_thread_sleep(). This provides the advantage that if the product code needs to awaken the messageThread() sooner, it can call platform_wake_message_thread() to signal that semaphore and cause the IOT_Task() to immediately go to step 1.
In normal operation, the wait on that semaphore just times out after IOT_MESSAGE_SLEEP_MS.

### Design of the IOT_Task and Use of the "ThreadAPI" functions

The design for the IOT_Task is (and compare to the History in the next section) to

* Create the IOT_Task as an OS Task to support the IoT activities once upon the first connection attempt
* This is done in ThreadAPI_Create() the first time.
* The connection code will pass the new connection structure to the IOT_Task each time it establishes a new connection, via the call to ThreadAPI_Create().
* The Eaton IoT SDK has a separate messageThread() function for PXGreen, apart from the thread-based one for PXRed.
* The IOT_Task just calls the messageThread() as a run-to-completion worker function, over and over
* The pacing wait function is located in the IOT_Task code, instead of down in the messageThread code.
* The ThreadAPI_Create() API, if called for something other than the messageThread, invokes a Stop_Here() exception.

If we ever want to support Blob Upload in PXGreen, we’ll have to revisit a lot of this design,
but that is an expensive choice, as then we would have to support both HTTPS and MQTT transports for IoT.

### History of the Use of the "ThreadAPI" functions

The initial implementation was for PXRed, which can use the pthread library to fulfill these functions
(ThreadAPI_Create(), ThreadAPI_Join(), and ThreadAPI_Exit()).
Their names suggest that thread-oriented origin.

If you search for uses of ThreadAPI_Create(), you find the following, adding some notes about the actual usage:

* Creating the task or thread for the messageThread, at the beginning of each Azure connection
* For the connection status callback in PXRed, not used in PXGreen
* For the transport_worker_thread() in iothubtransport.c, which is apparently not linked in to the PXGreen application (eg, cannot set a breakpoint anywhere in that file).
* For the ScheduleWork_Thread() in iothub_client_core.c, which is not even compiled into the PXGreen application (instead, we use iothub_client_core_ll.c – “low level”)
* For startHttpWorkerThread() for IoT Blob Upload support, which is not supported in PXGreen.

If you search for the ending functions, ThreadAPI_Join() or ThreadAPI_Exit(), you find them only in the same areas.

We began by following the Azure advice for supporting the Azure SDK (“implement these threading APIs”); we implemented ThreadAPI_Create() to create an OS Task each time it was called, and had ThreadAPI_Join() and ThreadAPI_Exit() delete that task; this happened once per connection.
But we

* Finally realized the reality that we only need to call ThreadAPI_Create() once in the PXGreen code, for our own Eaton IoT SDK messageThread task i.e we create IOT OS task only once and never delete it
* Experienced unreliability which _seemed to be_ stemming from frequently creating and deleting the FreeRTOS OS Task.
* Found some online forum issues that pointed to problems in the creating and deleting of FreeRTOS OS Tasks.
