/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This module is responsible for refreshing the NV Memory on a database upgrade.
 *
 *	@details This is used to update parameters which have been added or changed in a
 *	firmware upgrade.  This is typically used once the product is released to the field.
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef NV_REFRESH_LIST_H
   #define NV_REFRESH_LIST_H

#include "NV_Refresh.h"

extern const NV_Refresh::data_update_package_t ver_update_package;

#endif
