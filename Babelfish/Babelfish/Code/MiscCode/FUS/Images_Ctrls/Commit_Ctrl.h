/**
 *****************************************************************************************
 *  @file

 *	@brief commit control abstract class
 *
 *	@details This class will provide the commit operation from one memory to another.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef COMMIT_CTRL_H
#define COMMIT_CTRL_H

#include "FUS_Defines.h"
#include "NV_Ctrl.h"
#include "Image.h"

#ifndef DEFAULT_WAIT_TIME_PERCENT
#define DEFAULT_WAIT_TIME_PERCENT   10U
#endif
#ifndef DEFAULT_FUS_WAIT_TIME_MS
#define DEFAULT_FUS_WAIT_TIME_MS    2000U		/* Default wait time for FUS operation */
#endif
#ifndef VALIDATE_WAIT_TIME_MS
#define VALIDATE_WAIT_TIME_MS       2000U		/* Wait time for Validate operation */
#endif
#ifndef BUSY_ERROR_WAIT_TIME_MS
#define BUSY_ERROR_WAIT_TIME_MS     2000U		/* Wait time for status Busy Operation */
#endif

namespace BF_FUS
{
/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class Commit_Ctrl
{
	public:

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		Commit_Ctrl() {}

		virtual ~Commit_Ctrl( void ) {}

		/*
		 *****************************************************************************************
		 * @ Handle the Commit operation of the image
		 * @ param[in] process_time : Operation execution time
		 * @ return fus_op_status_t : operation status of the function.
		 *****************************************************************************************
		 */
		virtual void Image_Handler( fw_state_op_t* commit ) = 0;

		/*
		 *****************************************************************************************
		 * @ Call back function for the NV Ctrls operation status
		 * @param[in] status: NV Ctrl status
		 * @return : None
		 *****************************************************************************************
		 */
		virtual void Commit_Status_Cb( NV_Ctrl::nv_status_t status ) = 0;

		/*
		 *****************************************************************************************
		 * @ STatic Call back function for the NV Ctrls operation status
		 * @param[in] param: A void pointer to an object passed as an argument to the cback function.
		 * @param[in] status: NV Ctrl status
		 * @return : None
		 *****************************************************************************************
		 */
		static void Commit_Status_Cb_Static( NV_Ctrl::cback_param_t param, NV_Ctrl::nv_status_t status )
		{
			reinterpret_cast<Commit_Ctrl*>( param )->Commit_Status_Cb( status );
		}

		/*
		 *****************************************************************************************
		 * Function to add the buffer time in the calulated wait time.
		 * @param[in] time : input time in milliseconds for buffer time calculation
		 * @return : wait time in millisecond
		 *****************************************************************************************
		 */

		static uint32_t Add_Buffer_Time( uint32_t time )
		{
			time = static_cast<uint32_t>( ( ( time * DEFAULT_WAIT_TIME_PERCENT ) / 100 ) + 1 );
			return ( time );
		}

		/*
		 *****************************************************************************************
		 * @ Reset the commit controls flags and variables values
		 * @ return : None
		 *****************************************************************************************
		 */
		virtual void Reset( void ) {}

};

}


#endif	// COMMIT_CTRL
