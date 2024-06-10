/*
*****************************************************************************************
*
*		Copyright 2001-2004, Eaton Corporation. All Rights Reserved.
*
*****************************************************************************************
*/
#include "Includes.h"
#include "Com_Dog_Data.h"



/*
*****************************************************************************************
*		Constants
*****************************************************************************************
*/


#if COM_DOG_TIMEOUT_PARAM_LIST_LEN == 0
	const COM_DOG_PARAM_WATCH_TD* com_dog_timeout_list = NULL;
#else
	const COM_DOG_PARAM_WATCH_TD com_dog_timeout_list[COM_DOG_TIMEOUT_PARAM_LIST_LEN] =
	{
		{
			EXAMPLE_PARAMETER_DCID,
			EXAMPLE_PARAMETER_COM_LOSS_BEHAVIOR_DCID
		}
	};
#endif

