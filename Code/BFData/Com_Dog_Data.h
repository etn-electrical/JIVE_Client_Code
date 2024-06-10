/*
*****************************************************************************************
*
*		Copyright Eaton Corporation. All Rights Reserved.
*		Creator: Mark A Verheyen
*
*		Description:
*			Contains all the parameters and data to set.
*
*
*****************************************************************************************
*/
#ifndef COM_DOG_DATA_H
	#define COM_DOG_DATA_H

#include "DCI_Owner.h"

/*
*****************************************************************************************
*		Typedefs
*****************************************************************************************
*/
enum
{
	COM_DOG_TOUT_BEH_HOLD_LAST,
	COM_DOG_TOUT_BEH_USE_INIT,
	COM_DOG_TOUT_BEH_USE_DEFAULT,
	COM_DOG_TOUT_BEH_TOTAL
};

typedef struct COM_DOG_PARAM_WATCH_TD
{
	DCI_ID_TD dcid;
	DCI_ID_TD behavior_dcid;
} COM_DOG_PARAM_WATCH_TD;


/*
*****************************************************************************************
*		Extern dog timeout struct list
*****************************************************************************************
*/
#define COM_DOG_TIMEOUT_PARAM_LIST_LEN		0

#if COM_DOG_TIMEOUT_PARAM_LIST_LEN == 0
	extern const COM_DOG_PARAM_WATCH_TD* com_dog_timeout_list;
#else
	extern const COM_DOG_PARAM_WATCH_TD com_dog_timeout_list[];
#endif


#endif
