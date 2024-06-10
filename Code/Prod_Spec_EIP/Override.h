/**
 *****************************************************************************************
 * @file       Override.h
 * @details    This file is used to override variable declaration as per need.
 * @copyright  2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef OVERRIDE_H
#define OVERRIDE_H

#include "eipinc.h"
/*
 * routerObjects is redefined here, which is first declared in the eiproutr.h file.
 * This is added to resolve compilation error caused due to routerObjects declared as
 * static in eiproutr.c and made it extern in eiproutr.h
 */
#define routerObjects routerObjects_static

#endif	/* OVERRIDE_H */
