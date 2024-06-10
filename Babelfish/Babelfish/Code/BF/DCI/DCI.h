/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details This mainly provides static methods to access the DCI
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DCI_H
#define DCI_H

#include "DCI_Defines.h"
#include "DCI_Data.h"
#include "System_Reset.h"
#include "Bit_List.h"
#include "DCI_Lock.h"

/**
 **************************************************************************************************
 *  DCI Interface Class.  This is the interface to the DCI Data.
   --------------------------------------------------------------------------------------------------
 *  @brief DCI Interface Class.  This is the interface to the DCI Data.
 */
class DCI
{
	public:
		/**
		 *  @brief Constructor
		 *  @details Performs some initialization.  Specifically related to reset handling.
		 *  @param none
		 *  @return None
		 */
		static void Init( void );

		/**
		 *  @brief Constant to be used for comparing against when an structure is not found.
		 *  @details When a data block is requested we need to return a structure pointer which
		 *  indicates that one was not found.  The following constant simply provides a nullptr.
		 */
		static DCI_DATA_BLOCK_TD const* const UNDEFINED_DCID;

		/**
		 *  @brief Constant to be used for comparing against whether an index is valid or not.
		 *  @details When a index for callback and range block needs to be validate this
		 *  constant is used
		 */
		static const uint16_t INVALID_INDEX = 0xFFFF;
		/**
		 *  @details Quick method to get back the DCI data block for an requsted DCI ID
		 *  @param[in] data_id  DCI id for which data block is requested
		 *  @return If DCI ID is valid it returns Pointer to an Data block else NULL
		 */
		static inline DCI_DATA_BLOCK_TD const* Get_Data_Block( DCI_ID_TD data_id )
		{
			DCI_DATA_BLOCK_TD const* return_block;

			if ( data_id < DCI_TOTAL_DCIDS )
			{
				return_block = &dci_data_blocks[data_id];
			}
			else
			{
				return_block = UNDEFINED_DCID;
			}

			return ( return_block );
		}

		/**
		 *  @details Quick method to get back the DCI callback block for an requsted DCI ID
		 *  @param[in] cback_index  index of DCI id for which callback block is requested
		 *  @return If DCI ID is valid it returns Pointer to an Callback block else NULL
		 */
		static inline DCI_CBACK_BLOCK_TD* Get_CBACK_Block( DCI_CBACK_INDEX_TD cback_index )
		{
			DCI_CBACK_BLOCK_TD* return_cback_block;

			if ( cback_index < TOTAL_CBACK_ARRAY_SIZE )
			{
				return_cback_block = &DCI_CBACK_ARRAY[cback_index];
			}
			else
			{
				// callback not present
				return_cback_block = nullptr;
			}

			return ( return_cback_block );
		}

		/**
		 *  @details Quick method to get back the DCI range block for an requsted DCI ID
		 *  @param[in] range_index  index of DCI id for which data block is requested
		 *  @return If DCI ID is valid it returns Pointer to an Data block else NULL
		 */
		static inline DCI_RANGE_BLOCK const* Get_Range_Block( DCI_RANGE_DATA_INDEX_TD range_index )
		{
			DCI_RANGE_BLOCK const* return_range_block;

			if ( range_index < TOTAL_RANGE_ARRAY_SIZE )
			{
				return_range_block = &DCI_RANGE_BLOCK_ARRAY[range_index];
			}
			else
			{
				// out of range block
				return_range_block = nullptr;
			}

			return ( return_range_block );
		}

		/**
		 *  @details Quick method to get back the DCI enum block for an requsted DCI ID
		 *  @param[in] enum_index  index of DCI id for which data block is requested
		 *  @return If DCI ID is valid it returns Pointer to an Data block else NULL
		 */
		static inline DCI_ENUM_BLOCK const* Get_Enum_Block( DCI_ENUM_INDEX_TD enum_index )
		{
			DCI_ENUM_BLOCK const* return_enum_block;

			if ( enum_index < TOTAL_ENUM_COUNT_ARRAY_SIZE )
			{
				return_enum_block = &DCI_ENUM_BLOCK_ARRAY[enum_index];
			}
			else
			{
				// out of enum block
				return_enum_block = nullptr;
			}

			return ( return_enum_block );
		}

		/**
		 *  @details Quick method to get back the DCI enum block for an requsted DCI ID
		 *  @param[in] enum_index  index of DCI id for which data block is requested
		 *  @return If DCI ID is valid it returns Pointer to an Data block else NULL
		 */
		static inline DCI_ENUM_COUNT_TD Get_Enum_Count( DCI_ENUM_INDEX_TD enum_index )
		{
			DCI_ENUM_COUNT_TD return_enum_count;

			if ( enum_index < TOTAL_ENUM_COUNT_ARRAY_SIZE )
			{
				return_enum_count = DCI_ENUM_COUNT_ARRAY[enum_index];
			}
			else
			{
				// out of enum block
				return_enum_count = 0xFFFF;
			}

			return ( return_enum_count );
		}

		/**
		 *  @brief Copies data from one DCI parameter to another DCI parameter
		 *  @details  Disables interrupts while copying the data
		 *  @param[out] dest destination DCI pointer
		 *  @n @b Usage:
		 *  @param[in] dest Source DCI pointer
		 *  @n @b Usage:
		 *  @param[in]  length Length of the data to be copied
		 *  @n @b Usage:
		 *  @return None
		 *
		 */
		static void Copy_Data( DCI_DATA_PASS_TD* dest, DCI_DATA_PASS_TD const* source,
							   DCI_LENGTH_TD length );

		/**
		 *  @brief Copies data from one DCI parameter to another DCI parameter if it is changed only
		 *  @details  Disables interrupts while copying the data
		 *  @param[out] dest destination DCI pointer
		 *  @n @b Usage:
		 *  @param[in] source Source DCI pointer
		 *  @n @b Usage:
		 *  @param[in]  length Length of the data to be copied
		 *  @n @b Usage:
		 *  @return Returns true if data is changed else false
		 */
		static bool Copy_Data_Check_For_Change( DCI_DATA_PASS_TD* dest,
												DCI_DATA_PASS_TD const* source,
												DCI_LENGTH_TD length );

		/**
		 *  @details Copies Null terminated strings data
		 *  @see Copy_Data
		 */
		static void Copy_Null_String_Data( DCI_DATA_PASS_TD* dest,
										   DCI_DATA_PASS_TD const* source, DCI_LENGTH_TD* length );

		/**
		 *  @details Copies Null terminated strings data
		 *  @see Copy_Data_Check_For_Change
		 */
		static bool Copy_Null_String_Data_Check_For_Change( DCI_DATA_PASS_TD* dest,
															DCI_DATA_PASS_TD const* source,
															DCI_LENGTH_TD* length );

		/**
		 *  @details Copies 8 bit data
		 *  @see Copy_Data
		 */
		static void Copy_8BIT( DCI_DATA_PASS_TD* dest, DCI_DATA_PASS_TD const* source );

		/**
		 *  @details Copies 8 bit data
		 *  @see Copy_Data_Check_For_Change
		 */
		static bool Copy_8BIT_Check_For_Change( DCI_DATA_PASS_TD* dest,
												DCI_DATA_PASS_TD const* source );

		/**
		 *  @details Copies 16 bit data
		 *  @see Copy_Data
		 */
		static void Copy_16BIT( DCI_DATA_PASS_TD* dest, DCI_DATA_PASS_TD const* source );

		/**
		 *  @details Copies 16 bit data
		 *  @see Copy_Data_Check_For_Change
		 */
		static bool Copy_16BIT_Check_For_Change( DCI_DATA_PASS_TD* dest,
												 DCI_DATA_PASS_TD const* source );

		/**
		 *  @details Copies 32 bit data
		 *  @see Copy_Data
		 */
		static void Copy_32BIT( DCI_DATA_PASS_TD* dest, DCI_DATA_PASS_TD const* source );

		/**
		 *  @details Copies 32 bit data
		 *  @see Copy_Data_Check_For_Change
		 */
		static bool Copy_32BIT_Check_For_Change( DCI_DATA_PASS_TD* dest,
												 DCI_DATA_PASS_TD const* source );

		/**
		 *  @details Copies 64 bit data
		 *  @see Copy_Data
		 */
		static void Copy_64BIT( DCI_DATA_PASS_TD* dest, DCI_DATA_PASS_TD const* source );

		/**
		 *  @details Copies 64 bit data
		 *  @see Copy_Data_Check_For_Change
		 */
		static bool Copy_64BIT_Check_For_Change( DCI_DATA_PASS_TD* dest,
												 DCI_DATA_PASS_TD const* source );

		/**
		 *  @brief clears the current value
		 *  @details  Clears the data to DCI_CLEAR_DATA_VALUE also Disables interrupts while
		 * clearing the data
		 *  @param[out] dest pointer to DCI bocl which is supposed to be cleared
		 *  @n @b Usage:
		 *  @param[in]  length Length of the data to be cleared
		 *  @n @b Usage:
		 *  @return Returns true if data is changed else false
		 *
		 */
		static bool Clear_Data( DCI_DATA_PASS_TD* dest, DCI_LENGTH_TD length );

		/**
		 *  @brief  Quick method to lock DCI
		 *  @details Sets a lock on DCI ID so that it becomes not writable from Patron
		 *  @param[in] dci_id DCI parameter to be locked
		 *  @param[in] lock_type: Determines whether the lock is for read acces, write access or both ,By default the
		 * write
		 *  lock is enabled.
		 *  @return None
		 */
		static void Lock( DCI_ID_TD dci_id, DCI_LOCK_TYPE_TD lock_type = BF::DCI_Lock::LOCK_WRITE_MASK )
		{
			m_dci_lock->Lock( dci_id, lock_type );
		}

		/**
		 *  @brief  Quick method to unlock DCI
		 *  @details Sets a lock on DCI ID so that it becomes writable from Patron
		 *  @param[in] dci_id DCI parameter to be unlocked
		 *  @param[in] lock_type: Determines whether the lock is for read acces, write access or both ,By default the
		 * write
		 *  lock is enabled.
		 *  @return None
		 */
		static void Unlock( DCI_ID_TD dci_id, DCI_LOCK_TYPE_TD lock_type = BF::DCI_Lock::LOCK_WRITE_MASK )
		{
			m_dci_lock->Unlock( dci_id, lock_type );
		}

		/**
		 *  @brief  Quick method to get the lock status
		 *  @details Gets the status of lock on DCI ID to be written from Patron or not
		 *  @param[in] dci_id DCI parameter to get the status
		 *  @param[in] command: Determines whether the lock is for read acces or the write access,By default the write
		 * lock is enabled.
		 *  @return true is it is locked else false
		 */
		static bool Is_Locked( DCI_ID_TD dci_id, DCI_LOCK_TYPE_TD lock_type = BF::DCI_Lock::LOCK_WRITE_MASK )
		{
			return ( m_dci_lock->Locked( dci_id, lock_type ) );
		}

		/** Loads the current value into the initial space. */
		// static BOOL   Load_Current_To_Init( DCI_DATA_BLOCK_TD const* data_block );
		/** Loads the Default value into the current value. */
		// static BOOL		Load_Default_To_Current( DCI_DATA_BLOCK_TD const* data_block );
		/** Loads the default value into the initial space. */
		// static BOOL		Load_Default_To_Init( DCI_DATA_BLOCK_TD const* data_block );
		/** Loads the initial value into the current. */
		// static BOOL		Load_Init_To_Current( DCI_DATA_BLOCK_TD const* data_block );
		/**
		 *  @brief Handler for processing reset
		 *  @details Resets are of two types SYSTEM_RESET_FACTORY_RESET,
		 * SYSTEM_RESET_APP_PARAM_RESET
		 *  @param[in] handle Callback handle
		 *  @n @b Usage:
		 *  @param[in] reset_req
		 *  @n @b Usage: There are types of resets SYSTEM_RESET_FACTORY_RESET,
		 * SYSTEM_RESET_APP_PARAM_RESET
		 *  @return Always returns true
		 */
		static bool Reset_Req_Handler( BF::System_Reset::cback_param_t handle,
									   BF::System_Reset::reset_select_t reset_req );

		/**
		 *  @brief  Copies the default value to the init value
		 *  @details Loads the default value to init value
		 *  @param[in] dci_id DCI parameter to be factory resetted
		 *  @return true if it is successfully factory resetted else failed to write
		 */
		static bool Factory_Reset_DCID( DCI_ID_TD dci_id );

		/**
		 *  @brief  Will return the datatype size by using the datatype as an index.
		 *  @details A quick way of looking up the length of a datatype.
		 *  @param[in] The DCI Datatype.
		 *  @return Datatype size.
		 */
		inline static DCI_DATATYPE_SIZE_TD Get_Datatype_Size( DCI_DATATYPE_TD datatype )
		{
			return ( DATATYPE_SIZES[datatype] );
		}

		/**
		 * The value used to load into a value when cleared.
		 */
		static const uint8_t CLEAR_DATA_VALUE = 0U;

	private:
		/**
		 * Constructor, Destructor, Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		DCI( void );
		~DCI( void );
		DCI( const DCI& rhs );
		DCI & operator =( const DCI& object );

		static BF_Lib::Bit_List* m_locked_ids;
		static const DCI_DATATYPE_SIZE_TD DATATYPE_SIZES[DCI_DTYPE_MAX_TYPES];
		static BF::DCI_Lock* m_dci_lock;
};


#endif
