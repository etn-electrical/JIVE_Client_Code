/**
 *****************************************************************************************
 *	@brief This file includes the interfaces for DCI Owners to access DCI Data base
 *
 *	@details
 *	@n @b DCI Terminologies
 *	@n @b 1. RAM Value - DCI Parameter value from RAM
 *	@n @b 2. Init Value- DCI parameter value will be stored in NV memory
 *	@n @b 3. Default Value - DCI parameter will retain this value after factory_reset
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef DCI_OWNER_H
#define DCI_OWNER_H

#include "DCI_Defines.h"
#include "DCI_Data.h"
#include "DCI.h"
#include "OS_Queue.h"
#include "Change_Tracker.h"

#ifdef DEBUG
// #define DEBUG_DCI_DATA		///This enables a generic data pointer for debug purposes.
#endif

/*
 * @brief Constants
 */

/**
 * @brief Defines the OS stack size assigned to the cleanup callback function.
 */
#define DCI_OWNER_CHANGE_CALLBACK_OS_STACK_SIZE         250

/**
 * @brief The number of items that can be listed in the change queue.
 */
#define DCI_OWNER_CHANGE_ID_QUEUE_SIZE                  25

/**
 * @brief This timeout is mostly meaningless.  It signifies a long time.  No significance.
 */
#define DCI_OWNER_CHANGE_QUEUE_TIMEOUT                  100000

/**
 * @brief This denotes that no memory is allocated.
 */
#define DCI_OWNER_ALLOC_NO_MEM                          ( reinterpret_cast<uint8_t*>( static_cast< \
																						  uint8_t> \
																					  ( ~(  \
																							static_cast<uint8_t>( 0U ) ) ) ) )

/**
 ****************************************************************************************
 * @brief This is a DCI_Owner class
 * @details DCI Owner allocates and provides access to individual data items.
 * The DCI Owner also provides the necessary access functions to manipulate the data.
 * All owner data access should happen through this class.
 *
 * @n @b Usage: It provides public methods to
 * @n @b 1. Create DCI Owners, DCI Owners are essentially for the DCI parameters owned by the
 * application
 * @n @b 2. Writes in (Check_In()) the Ram values, Init values, default values and offsets
 * @n @b 3. Reads out (Check_Out()) the Ram values, Init values, default values
 ****************************************************************************************
 */
class DCI_Owner
{
	public:
		/**
		 *  @brief Constructor
		 *  @details This will create the space associated with the data parameter.
		 *  It will also load that new data with the initial value if present.
		 *  @param[in] data_id DCI parameter id
		 *  @n @b Usage: Please pass valid DCI ID else this function will throw an exception
		 *  @return None
		 */
		DCI_Owner( DCI_ID_TD data_id );

		/**
		 *  @brief Constructor
		 *  @details This gives the ability to create an owner that is attached a passed in ptr.
		 *  This is intended for data values that are constructed at request time.
		 *  his requires that the module using this data must avoid tearing by
		 *  disabling and enabling interrupts on every usage.  Plus a gratuitous Check_In
		 *  should be done to trigger the change tracker.
		 *  @param[in] data_id DCI parameter id
		 *  @n @b Usage: Use valid DCI ID else this function will throw an exception
		 *  @param[in] data_ptr Pointer to the data structure to fill in at runtime
		 *  @return None
		 *
		 */
		DCI_Owner( DCI_ID_TD data_id, DCI_DATA_PASS_TD* data_ptr );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object DCI_Owner goes out of scope or deleted.
		 *  @return None
		 */
		~DCI_Owner( void );

		/**
		 *  @brief
		 *  @details Writes the value of ram_data to the DCI RAM data value
		 *  @param[in] ram_data Data to be copied to DCI parameter's RAM value
		 *  @n @b Usage: Use valid pointer to the data to be copied
		 *  @return Always returns True
		 */
		bool Check_In( DCI_DATA_PASS_TD const* ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for NULL terminated strings.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_In_Null_String( char_t const* ram_data ) const;

		/**
		 *  @brief
		 *  @details Checks out (reads) the RAM value
		 *  @param[out] ram_data In this parameter RAM value of DCI ID will be fetched back
		 *  @return Always returns true
		 */
		bool Check_Out( DCI_DATA_PASS_TD* ram_data ) const;

		/**
		 *  @brief
		 *  @details Allows an owner to check out one element at a time in an array
		 *  This function allows owner to access one element at a time in an array.
		 *  Index is the native data type index. i.e.: given an array of 16bit
		 *  variables to access the 3word, an index of 3 is passed in.
		 *  @param[out] ram_data In this parameter value present at given index will be fetched back
		 *  @n @b Usage: Always give valid index
		 *  @param[in] index Index is the native data type index.
		 *  @n @b Usage: For example given an array of 16bit variables to access the 3word, an index
		 * of 3 is passed in
		 *  @return Always returns true, but if invalid index is given then throws an exception
		 */
		bool Check_Out_Index( DCI_DATA_PASS_TD* ram_data, DCI_LENGTH_TD index ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function allows owner to access one element at a time in an
		 * array.
		 *                       Index is the native data type index. i.e.: given an array of 16bit
		 *                       variables to access the 3word, an index of 3 is passed in.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @param [in]  index    Offset in the data block
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_In_Index( DCI_DATA_PASS_TD const* ram_data, DCI_LENGTH_TD index ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for NULL terminated strings.
		 *                       Ram data points at the destination and length initially defines the
		 *                       max length that can be checked out. Then the length of the string
		 * is
		 *                       returned. If length is left to point to null then the total string
		 *                       will be copied back with no length passed back.
		 *                       variables to access the 3word, an index of 3 is passed in.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @param [in]  length   NULL
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_Out_Null_String( DCI_DATA_PASS_TD* ram_data,
									DCI_LENGTH_TD* length =
									reinterpret_cast<DCI_LENGTH_TD*>( nullptr ) ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for boolean type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_In( bool ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for uint8_t type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_In( uint8_t ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for uint16_t type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_In( uint16_t ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for uint32_t type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_In( uint32_t ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for uint64_t type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_In( uint64_t ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for int8_t type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_In( int8_t ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for int16_t type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_In( int16_t ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for int32_t type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_In( int32_t ram_data ) const;

		/**

		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for int64_t type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_In( int64_t ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for single precision float type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_In( float32_t ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for double precision float type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_In( float64_t ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for reference to boolean type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_Out( bool& ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for reference to uint8_t type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_Out( uint8_t& ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for reference to uint16_t type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_Out( uint16_t& ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for reference to uint32_t type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_Out( uint32_t& ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for reference to uint64_t type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_Out( uint64_t& ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for reference to int8_t type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_Out( int8_t& ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for reference to int16_t type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_Out( int16_t& ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for reference to int32_t type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_Out( int32_t& ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for reference to int64_t type data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_Out( int64_t& ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for reference to single precision float
		 * data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_Out( float32_t& ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       This function is intended for reference to double precision float
		 * data.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_Out( float64_t& ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       Loads init space with ram value.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In Init operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_In_Init( DCI_DATA_PASS_TD const* ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       Loads ram value with init.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check Out Init operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_Out_Init( DCI_DATA_PASS_TD* ram_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       Loads ram value into default. This only works with a writable
		 * default.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check In Default operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_In_Default( DCI_DATA_PASS_TD const* default_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       Loads default value into ram value.
		 * @param [in]  ram_data Pointer to data in RAM
		 * @return[out] success  Returns the status of Check Out Default operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_Out_Default( DCI_DATA_PASS_TD* default_data ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       Loads the ram value from offset, upto length into the parameter
		 * @param [in]  ram_data Pointer to data in RAM
		 * @param [in]  offset   NO_OFFSET
		 * @param [in]  length   SET_ALL
		 * @return[out] success  Returns the status of Check In Offset operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_In_Offset( DCI_DATA_PASS_TD const* ram_data,
							  DCI_LENGTH_TD offset = ACCESS_OFFSET_NONE,
							  DCI_LENGTH_TD length = ACCESS_LENGTH_SET_ALL ) const;

		/**
		 * @brief                The following function provide access to the RAM data.
		 *                       Loads the parameter value from offset, upto length into the ram
		 * value
		 * @param [in]  ram_data Pointer to data in RAM
		 * @param [in]  offset   NO_OFFSET
		 * @param [in]  length   SET_ALL
		 * @return[out] success  Returns the status of Check Out Default operation
		 * @n                    true = Success
		 * @n                    false = Failure
		 * @n
		 */
		bool Check_Out_Offset( DCI_DATA_PASS_TD* ram_data,
							   DCI_LENGTH_TD offset = ACCESS_OFFSET_NONE,
							   DCI_LENGTH_TD length = ACCESS_LENGTH_GET_ALL ) const;

		/**
		 * @brief                This function will store the init value of the owner beginning at
		 * the
		 *                       offset and continuing for length bytes
		 * @param [in] ram_data  The data to be checked in of length length
		 * @param [in] offset    The byte offset into the block to begin checking in data
		 * @param [in] length    The byte length of the data to check in
		 * @return[out] success  true for success, false for failure
		 */
		bool Check_In_Init_Offset( DCI_DATA_PASS_TD const* ram_data,
								   DCI_LENGTH_TD offset, DCI_LENGTH_TD length ) const;

		/**
		 * @brief                This function will read the init value of the owner beginning at
		 * the
		 *                       offset and continuing for length bytes
		 * @param [in] ram_data  The data to be checked out of length length
		 * @param [in] offset    The byte offset into the block to begin checking in data
		 * @param [in] length    The byte length of the data to check in
		 * @return[out] success  true for success, false for failure
		 */
		bool Check_Out_Init_Offset( DCI_DATA_PASS_TD* ram_data,
									DCI_LENGTH_TD offset, DCI_LENGTH_TD length ) const;

		/**
		 * @brief                   This function will store the default value of the owner
		 * beginning at
		 *                          the offset and continuing for length bytes
		 * @param [in] default_data The data to be checked in of length length
		 * @param [in] offset       The byte offset into the block to begin checking in data
		 * @param [in] length       The byte length of the data to check in
		 * @return[out] success     true for success, false for failure
		 */
		bool Check_In_Default_Offset( DCI_DATA_PASS_TD const* default_data,
									  DCI_LENGTH_TD offset, DCI_LENGTH_TD length ) const;

		/**
		 * @brief                   This function will read the default value of the owner beginning
		 * at
		 *                          the offset and continuing for length bytes
		 * @param [in] default_data The data to be checked in of length length
		 * @param [in] offset       The byte offset into the block to begin checking in data
		 * @param [in] length       The byte length of the data to check in
		 * @return[out] success     true for success, false for failure
		 */
		bool Check_Out_Default_Offset( DCI_DATA_PASS_TD* default_data,
									   DCI_LENGTH_TD offset, DCI_LENGTH_TD length ) const;

		/**
		 * @brief               Loads the range attribute with the passed values.
		 *                      Only works if Range is writable.
		 * @param [in] min      The data to be checked in for minimum value
		 * @return[out] success true for success, false for failure

		 */
		bool Check_In_Min( DCI_DATA_PASS_TD const* min ) const;

		/**
		 * @brief               Loads the range attribute with the passed values.
		 *                      Only works if Range is writable.
		 * @param [in] max      The data to be checked in for maximum value
		 * @return[out] success true for success, false for failure
		 */
		bool Check_In_Max( DCI_DATA_PASS_TD const* max ) const;

		/**
		 * @brief               Loads the range attribute with the passed values.
		 *                      Only works if Range is writable.
		 * @param [in] min      The data to be checked out for minimum value
		 * @return[out] success true for success, false for failure
		 */
		bool Check_Out_Min( DCI_DATA_PASS_TD* min ) const;

		/**
		 * @brief               Loads the range attribute with the passed values.
		 *                      Only works if Range is writable.
		 * @param [in] max      The data to be checked out for maximum value
		 * @return[out] success true for success, false for failure
		 */
		bool Check_Out_Max( DCI_DATA_PASS_TD* max ) const;

		/**
		 * @brief				Retrieves the number of enumeration values
		 *                      in the present list.
		 * @param element_count The returned number of elements in the enum
		 * list.
		 * @return True if the enum list exists.  False if it does not.
		 */
		bool Check_Out_Enum_Element_Count( DCI_LENGTH_TD* element_count ) const;

		/**
		 * @brief				Retrieves the enumeration values from the current
		 *                      enum list.  This method provides several options.
		 *                      Since this option will rarely be used the full
		 *                      list can be extracted or individual elements.
		 *                      Remember that the element offset is a value index
		 *                      and not a byte index.
		 * @param enum_dest The destination pointer for the enum value(s).
		 * @param element_offset The index or element offset of the enum.
		 * @param element_count The number of elements that you would like to retrieve.
		 * @return True if you requested a valid element.  False if you requested an
		 * offset/count which is not available.
		 */
		bool Check_Out_Enum( DCI_ENUM_LIST_TD* enum_dest, DCI_LENGTH_TD element_offset,
							 DCI_LENGTH_TD element_count ) const;

		/**
		 * @brief               Will set the alternate enum value array.
		 *                      This will then be utilized by the range check
		 *                      function and also can be requested through the
		 *                      patron interface.  It works by passing in an array
		 *                      of values.  They do not need to be sequential.
		 *                      An element count must also be passed in.
		 *                      This is the number of values NOT the BYTE count.
		 *                      This error out if ENUM is not checked as RW in the
		 *                      workbook.
		 * @param [in] enum_list A pointer to an array of alternate enum values.
		 * @param [in] element_count The number of values in the
		 *                      list (values not bytes).
		 * @return[out] success  true for success, false for failure
		 */
		bool Check_In_Enum( DCI_ENUM_LIST_TD const* enum_list,
							DCI_LENGTH_TD element_count ) const;

		/**
		 * @brief               Provides a method to change the length.
		 *                      Only works if length is writable.
		 * @param [in] length   The pointer to the value to be checked in for length
		 * @return[out] success true for success, false for failure
		 */
		bool Check_In_Length( DCI_LENGTH_TD const* length ) const;

		/**
		 * @brief               Provides a method to change the length.
		 *                      Only works if length is writable.
		 * @param [in] length   The value to be checked in for length
		 * @return[out] success true for success, false for failure
		 */
		bool Check_In_Length( DCI_LENGTH_TD length ) const;

		/**
		 * @brief               Provides a method to extract the length.
		 *                      Only works if length is writable.
		 * @param [in] length   Pointer to the variable where length to be extracted
		 * @return[out] success true for success, false for failure
		 */
		bool Check_Out_Length( DCI_LENGTH_TD* length ) const;

		/**
		 * @brief              Provides a quick method to get the length of data from DCI Owner
		 * @param [in]         None
		 * @return[out] length Returns the length of DCI Owner data block
		 */
		DCI_LENGTH_TD Get_Length( void ) const;

		/**
		 * @brief                Provides a quick method to get the datatype of DCI_Owner
		 * @param [in]           None
		 * @return[out] Datatype Returns the datatype of the DCI Owner
		 */
		DCI_DATATYPE_TD Get_Datatype( void ) const;

		/**
		 * @brief                  Provides a method to attached a callback to handle the parameter
		 * on
		 *                         patron access.
		 * @param [in] cback_func  Pointer to the function that need to be called.
		 * @param [in] cback_param Parameters for Callback function
		 * @param [in] cback_mask  A bitfield of commands which will cause the callback.
		 * @return[out] success    true for success, false for failure
		 */
		bool Attach_Callback( DCI_CBACK_FUNC_TD* cback_func,
							  DCI_CBACK_PARAM_TD cback_param, DCI_CB_MSK_TD cback_mask ) const;

		/**
		 * @brief               Provides a method to move data from current to init.
		 * @param [in]          None
		 * @return[out] success true for success, false for failure
		 */
		bool Load_Current_To_Init( void ) const;

		/**
		 * @brief               Provides a method to move data from default to current.
		 *                      This will trigger the change tracker.
		 * @param [in]          None
		 * @return[out] success true for success, false for failure

		 */
		bool Load_Default_To_Current( void ) const;

		/**
		 * @brief               Provides a method to move data from default to init.
		 *                      This will trigger the change tracker.
		 * @param [in]          None
		 * @return[out] success true for success, false for failure
		 */
		bool Load_Default_To_Init( void ) const;

		/**
		 * @brief               Provides a method to move data from init to current.
		 *                      This will trigger the change tracker.
		 * @param [in]          None
		 * @return[out] success true for success, false for failure
		 */
		bool Load_Init_To_Current( void ) const;

		/**
		 * @brief               Provides a method to clear current value.
		 *                      This will trigger the change tracker.
		 * @param [in]          None
		 * @return[out] success true for success, false for failure
		 */
		bool Clear_Current_Val( void ) const;

		/**
		 * @brief       Provides a method to retrieve the owner attributes.
		 * @param [in]  None
		 * @return[out] Returns Owner attribute information
		 */
		DCI_OWNER_ATTRIB_TD Get_Owner_Attrib( void )
		{
			return ( m_data_block->owner_attrib_info );
		}

		/**
		 * @brief       Provides a method to retrieve Patron attributes
		 * @param [in]  None
		 * @return[out] Returns Patron attribute information
		 */
		DCI_PATRON_ATTRIB_TD Get_Patron_Attrib( void )
		{
			return ( m_data_block->patron_attrib_info );
		}

		/**
		 * @brief               Provides a method to checkin
		 *                      This is a gratuitis check in. It provides an external method of
		 *                      triggering a change indication. This is most useful when the module
		 *                      is in charge of the data the module decides to change the data.
		 * @param [in]          None
		 * @return[out] success true for success, false for failure
		 */
		bool Check_In( void ) const;

		/**
		 * @brief               Provides a method to checkin init value
		 * @param [in]          None
		 * @return[out] success true for success, false for failure
		 */
		bool Check_In_Init( void ) const;

		/**
		 * @brief       Provides a method to Lock
		 *              These locking mechanisms provide a method of disallowing patrons from
		 *              performing a write operation on a normally read/write value.
		 *              This allows the owner to lock out other access when it is not ready.
		 *  @param[in] lock_type: Determines whether the lock is for read access, write accessor both ,By default the
		 *  write lock is enabled.
		 * @return
		 */
		void Lock( DCI_LOCK_TYPE_TD lock_type = BF::DCI_Lock::LOCK_WRITE_MASK ) const;

		/**
		 * @brief      Provides a method to Lock
		 *              These locking mechanisms provide a method of disallowing patrons from
		 *              performing a write operation on a normally read/write value.
		 *              This allows the owner to lock out other access when it is not ready..
		 * @param[in] lock_type : Determines whether the lock is for read acces, write access or both ,By default the
		 * write lock is enabled.
		 * @return
		 */
		void Unlock( DCI_LOCK_TYPE_TD lock_type = BF::DCI_Lock::LOCK_WRITE_MASK ) const;

		/**
		 * @brief       Provides a method to check if value is locked.
		 *              This allows the patrons to know if a value is locked.
		 * @param[in] lock_type : Determines whether the lock is for read acces or the write access,By default the write
		 *  lock is enabled.
		 * @return
		 */
		bool Is_Locked( DCI_LOCK_TYPE_TD lock_type = BF::DCI_Lock::LOCK_WRITE_MASK ) const;

		/**
		 *  @brief Determine whether an owner has been created for this DCID.
		 *  @details This allows someone to identify whether the owner was created for this
		 *  DCID somewhere else.  Typically to error out without erroring out inside of DCI_Owner.
		 *  @return True if an owner has already been created for this DCID.
		 */
		static bool Is_Created( DCI_ID_TD data_id );

	private:
		/**
		 * @brief      Provides a method to enable cleanup tasks
		 * @param [in]
		 * @return
		 */
		static void Enable_Cleanup_Tasks( void );

		/**
		 * @brief                Provides a method to initialize the Owner.
		 *                       This function facilitates multiple constructors.
		 * @param [in] param     Callback Parameter
		 * @param [in] dci_id    DCI Parameter ID
		 * @param [in] attribute Attributes of parameter
		 * @return
		 */
		static void Post_Access_Func( Change_Tracker::cback_param_t param,
									  DCI_ID_TD dci_id,
									  Change_Tracker::attrib_mask_t attribute_mask );

		/**
		 * @brief Pointer to Change Tracker
		 */
		static Change_Tracker* m_change_tracker;

		/**
		 * @brief               Provides a method to initialize the Owner.
		 *                      This function facilitates multiple constructors.
		 * @param [in] data_id  Parameter id
		 * @param [in] data_ptr Pointer to value in ram
		 * @return
		 */
		void Init_Owner( DCI_ID_TD data_id, DCI_DATA_PASS_TD* data_ptr );

		/**
		 * @brief Pointer to the data block
		 */
		DCI_DATA_BLOCK_TD const* m_data_block;

#ifdef DEBUG_DCI_DATA
#pragma pack(1)
		BF_Lib::SUPER_DATA_TYPE* m_data;
#pragma pack()
#endif
};

#ifdef DOXYGEN_EXAMPLE_BLOCK
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DOXYGEN_COMMENT_BLOCK_START

\ example DCI_Owner An example code for DCI Owner
\ section DCI_Owner
The sample code explains use of different APIs of DCI_Owner Class.
\ code
void DCIInit( void )
{
	// ************************************************************************************
	new DCI_Owner( DCI_PWR_BRD_HARDWARE_ID_VAL_DCID );

	// ************************************************************************************

	DCI_Owner* m_prod_data_owner;
	DCI_ID_TD prod_data_id;
	uint8_t m_dummy_owner_data;

	m_prod_data_owner = new DCI_Owner( prod_data_id, &m_dummy_owner_data );

	// ************************************************************************************

	DCI_ID_TD address_id;
	DCI_Owner* m_address_owner;
	uint8_t new_address;
	uint8_t temp_state;

	m_address_owner = new DCI_Owner( address_id );

	m_address_owner->Check_Out( new_address );

	temp_state = 3;
	m_address_owner->Check_In( temp_state );

	// ************************************************************************************

	DCI_Owner* test_Owner_Check_In_Init = new DCI_Owner( DCI_TEST_ATTRI_0100000000_000000_DCID );

	uint8_t actualVal;
	uint8_t testVal[1] = { 1 };

	test_Owner_Check_In_Init->Check_Out( &actualVal );
	test_Owner_Check_In_Init->Check_In_Init( testVal );
	test_Owner_Check_In_Init->Check_Out_Init( &actualVal );

	// ************************************************************************************

	BF_Lib::bool_t retVal;

	DCI_Owner* test_Lk = new DCI_Owner( DCI_TEST_LOCK_DCID );

	test_Lk->Lock();

	if ( TRUE == test_Lk->Is_Locked() )
	{
		retVal = PASS;
	}
	else
	{
		retVal = FALSE;
	}

	// ************************************************************************************

	BF_Lib::bool_t retVal;

	DCI_Owner* test_Lk = new DCI_Owner( DCI_TEST_LOCK_DCID );

	test_Lk->Unlock();

	if ( FALSE == test_Lk->Is_Locked() )
	{
		retVal = PASS;
	}
	else
	{
		retVal = FALSE;
	}

	// ************************************************************************************

	BF_Lib::bool_t retVal;

	DCI_Owner* test_get_owner_attr = new DCI_Owner( DCI_TEST_GET_OWNER_ATTRIBUTE_DCID );

	DCI_OWNER_ATTRIB_TD retrivedAttributes;
	DCI_OWNER_ATTRIB_TD actualDCIDAttributes = 0x01FE;	// 0b0000000111111110

	retrivedAttributes = test_get_owner_attr->Get_Owner_Attrib();

	if ( retrivedAttributes == actualDCIDAttributes )
	{
		retVal = PASS;
	}
	else
	{
		retVal = FALSE;
	}

	// ************************************************************************************

	DCI_ID_TD prod_list_id = 10;

	DCI_Owner* m_prod_list_owner = new DCI_Owner( prod_list_id );

	DCI_LENGTH_TD index = 0;
	PROFI_ID_TD prod_list_mod;

	m_prod_list_owner->Check_Out_Index( &prod_list_mod, index );
	m_prod_list_owner->Check_In_Index( &prod_list_mod, index );

	// ************************************************************************************

	DCI_ID_TD count_crc_id = 20;
	uint16_t m_num_crc_errors = 0;

	DCI_Owner* m_count_of_crc_failures = new DCI_Owner( count_crc_id );

	m_num_crc_errors++;

	if ( m_count_of_crc_failures != nullptr )
	{
		m_count_of_crc_failures->Check_In( m_num_crc_errors );
		m_count_of_crc_failures->Load_Current_To_Init();
	}

	// ************************************************************************************

	DCI_ID_TD prod_list_id = 10;
	DCI_ID_TD prod_list[10];

	DCI_Owner* m_prod_list_owner = new DCI_Owner( prod_list_id );

	length = 2;

	if ( m_prod_list_owner->Check_In_Length( &length ) )
	{
		id_ptr = &prod_list[0];
		m_prod_list_owner->Check_In( id_ptr );
	}

	// ************************************************************************************

	DCI_ID_TD dci_id = 25;
	DCI_LENGTH_TD temp_asm_len = 0;

	owner = new DCI_Owner( dci_id, DCI_OWNER_ALLOC_NO_MEM );

	temp_asm_len++;

	owner->Check_In_Length( temp_asm_len );

	// ************************************************************************************

	#define DCI_MULTICAST_ENABLE_DCID            111
	#define DCI_BROADCAST_ENABLE_DCID            112

	UINT32 broadcast_settings = ETH_BroadcastFramesReception_Enable;
	UINT32 multicast_settings = ETH_MulticastFramesFilter_None;

	DCI_Owner* broadcast_owner = new DCI_Owner( DCI_MULTICAST_ENABLE_DCID );

	DCI_Owner* multicast_owner = new DCI_Owner( DCI_BROADCAST_ENABLE_DCID );

	broadcast_owner->Check_Out_Init( ( UINT8* ) &broadcast_settings );
	multicast_owner->Check_Out_Init( ( UINT8* ) &multicast_settings );

	// ************************************************************************************

	#define DCI_PREVIOUS_IP_SETTINGS_IN_USE_STATUS_DCID            30

	previous_IP_settings_in_use_owner =
		new DCI_Owner( DCI_PREVIOUS_IP_SETTINGS_IN_USE_STATUS_DCID );

	previous_ip_settings = FALSE;

	previous_IP_settings_in_use_owner->Check_In( previous_ip_settings );
	previous_IP_settings_in_use_owner->Check_In_Init( ( uint8_t* )&previous_ip_settings );

	// ************************************************************************************

	static void Prod_Data_Callback_Static( void )
	{
		// Do Nothing;
	}

	m_prod_data_owner->Attach_Callback( Prod_Data_Callback_Static,
										reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
										static_cast<DCI_CB_MASK_TD>
										( DCI_ACCESS_GET_RAM_CMD_MASK ) );
}

\ endcode
DOXYGEN_EXAMPLE_BLOCK_END
#endif	// END_DOXYGEN_EXAMPLE_BLOCK

#endif
