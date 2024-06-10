/**
 *****************************************************************************************
 *	@file NV_Ctrl.h
 *
 *	@brief Base class for NV memories
 *
 *	@details Defines the interface for performing operations with NV memories
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef NV_CTRL_H
#define NV_CTRL_H

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
typedef uint32_t NV_CTRL_ADDRESS_TD;
typedef uint32_t NV_CTRL_LENGTH_TD;

/**
 ****************************************************************************************
 * @brief The NV_Ctrl class contains implementations required for the access of the NV memory.
 *
 * @details It provides functionalities to read, write, erase the NV memory.
 *
 ****************************************************************************************
 */
class NV_Ctrl
{
	public:
		/**
		 * @brief Return Status for NV_Ctrl operations
		 */
		enum nv_status_t
		{
			SUCCESS,
			DATA_ERROR,
			BUSY_ERROR,
			WRITE_PROTECTED,
			INVALID_ADDRESS,
			OPERATION_NOT_SUPPORTED,
			PENDING_CALL_BACK
		};

		enum semaphore_status_t
		{
			RELEASE,
			ACQUIRE
		};

		enum nv_state_t
		{
			IDLE,
			ERASE,
			WRITE,
			READ,
			COMPLETE,
			ERROR
		};

		/**
		 * @brief Used for the call back parameter which is used as an argument in the callback function.
		 */
		typedef void* cback_param_t;

		/**
		 * @brief Used for the call back functions.
		 */
		typedef void (* cback_func_t)( cback_param_t param, nv_status_t result );

		/**
		 * @brief Structure used to define the base attributes of the NV memory.
		 */
		struct mem_attribute_struct_t
		{
			uint32_t block_count;
			uint32_t block_size;
			uint32_t erase_block_size;
		};

		/**
		 * @brief Structure used to define the base attributes of asynchronous write service.
		 */
		struct asynch_service_struct_t
		{
			cback_func_t cback;
			cback_param_t param;
			uint8_t* data;
			uint32_t address;
			uint32_t length;
		};

		struct mem_range_info_t
		{
			uint32_t start_address;
			uint32_t mirror_start_address;
			uint32_t end_address;
		};

		/**
		 *  @brief Destructor
		 *  @details It will get called when object NV_Ctrl goes out of scope or deleted.
		 *  @return None
		 */
		virtual ~NV_Ctrl( void )
		{}

		/**
		 * @brief Reads a specified number of bytes from a given address into a buffer.
		 * @param[in] data: A pointer to the buffer used to receive the data from the read operation.
		 * @param[in] address: The address of the first byte to be read.
		 * @param[in] length: The number of bytes to be read.
		 * @param[in] cback: A pointer to the function to be called when the operation is complete.
		 * @param[in] param: A void pointer to an object passed as an argument to the cback function.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Read( uint8_t* data, uint32_t address, uint32_t length,
								  cback_func_t cback, cback_param_t param )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Writes a specified number of bytes to a given address from a buffer.
		 * @param[in] data: A pointer to the buffer used to hold the data for the write operation.
		 * @param[in] address: The address of the first byte to be written.
		 * @param[in] length: The number of bytes to be written.
		 * @param[in] cback: A pointer to the function to be called when the operation is complete.
		 * @param[in] param: A void pointer to an object passed as an argument to the cback function.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Write( const uint8_t* data, uint32_t address, uint32_t length,
								   cback_func_t cback, cback_param_t param )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Erases a block of memory in the NV_Ctrl device. This may involve a range
		 * larger than specified in the address and length arguments as some devices have
		 * minimum block sizes that can be erased.
		 * @param[in] address: The address of the first byte to be erased.
		 * @param[in] length: The number of bytes to be erased.
		 * @param[in] cback: A pointer to the function to be called when the operation is complete.
		 * @param[in] param: A void pointer to an object passed as an argument to the cback function.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Erase( uint32_t address, uint32_t length,
								   cback_func_t cback, cback_param_t param )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Erases the entire NV_Ctrl device.
		 * @param[in] cback: A pointer to the function to be called when the operation is complete.
		 * @param[in] param: A void pointer to an object passed as an argument to the cback function.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Erase_All( cback_func_t cback, cback_param_t param )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Provides the status of the operation/NV_Ctrl device.
		 * @param[in] cback: A pointer to the function to be called when the operation is complete.
		 * @param[in] param: A void pointer to an object passed as an argument to the cback function.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Ready( cback_func_t cback, cback_param_t param )
		{
			return ( SUCCESS );
		}

		/**
		 * @brief Provides the attributes of the NV_Ctrl device
		 * @param[in] attrib: A pointer to the structure memory attributes are to be written to.
		 * @return status of requested operation.
		 */
		virtual nv_status_t GetAttributes( mem_attribute_struct_t* attrib )
		{
			attrib->block_count = m_mem_attributes->block_count;
			attrib->block_size = m_mem_attributes->block_count;
			attrib->erase_block_size = m_mem_attributes->erase_block_size;

			return ( SUCCESS );
		}

		/**
		 * @brief Synchronously reads a specified number of bytes from a given address into a buffer.
		 * @param[in] data: A pointer to the buffer used to receive the data from the read operation.
		 * @param[in] address: The address of the first byte to be read.
		 * @param[in] length: The number of bytes to be read.
		 * @param[in] use_protection: A flag to indicate fault detection/correction is used for this operation.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Read( uint8_t* data, NV_CTRL_ADDRESS_TD address,
								  NV_CTRL_LENGTH_TD length, bool use_protection = true )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		virtual nv_status_t Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
										   NV_CTRL_LENGTH_TD length )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Synchronously writes a specified number of bytes to a given address from a buffer.
		 * @param[in] data: A pointer to the buffer used to hold the data for the write operation.
		 * @param[in] address: The address of the first byte to be written.
		 * @param[in] length: The number of bytes to be written.
		 * @param[in] use_protection: A flag to indicate fault detection/correction is used for this operation.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Write( uint8_t* data, uint32_t address,
								   NV_CTRL_LENGTH_TD length, bool use_protection = true )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Synchronously erases a block of memory in the NV_Ctrl device. This may involve a range
		 * larger than specified in the address and length arguments as some devices have
		 * minimum block sizes that can be erased.
		 * @param[in] address: The address of the first byte to be erased.
		 * @param[in] length: The number of bytes to be erased.
		 * @param[in] use_protection: A flag to indicate fault detection/correction is used for this operation.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Erase( NV_CTRL_ADDRESS_TD address,
								   NV_CTRL_LENGTH_TD length, uint8_t erase_data = 0, bool protected_data = true )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Synchronously erases the entire NV_Ctrl device.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Erase_All( void )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Returns the status of device connection.
		 * @return status: Status of connection.
		 */
		virtual bool Device_connected()
		{
			return ( true );
		}

		/**
		 * @brief Attaches the call back
		 */
		virtual void Attach_Callback( cback_func_t func_callback, cback_param_t func_param )
		{}

		/**
		 * @brief Returns the memory range information
		 * @param[in] mem_range_info: range information will get updated here
		 */
		virtual void Get_Memory_Info( mem_range_info_t* mem_range_info )
		{}

		/**
		 * @brief Returns wait time to perform current operation
		 * @return wait_time: time in milli sec
		 */
		virtual uint32_t Get_Wait_Time( void )
		{
			return ( 0 );
		}

		/**
		 * @brief Returns wait time to perform read operation
		 * @param[in] data: data to be read into
		 * @param[in] address: Start address
		 * @param[in] length: length of data
		 * @return wait_time: time in milli sec
		 */
		virtual uint32_t Read_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address,
									NV_CTRL_LENGTH_TD length ) const
		{
			return ( 0 );
		}

		/**
		 * @brief Returns wait time to perform write operation
		 * @param[in] data: data to be written
		 * @param[in] address: Start address
		 * @param[in] length: length of data
		 * @param[in] include_erase_time: Flag to include erase time or not

		 * @return wait_time: time in milli sec
		 */
		virtual uint32_t Write_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address,
									 NV_CTRL_LENGTH_TD length, bool include_erase_time = true ) const
		{
			return ( 0 );
		}

		/**
		 * @brief Returns wait time to perform erase operation
		 * @param[in] address: Start address
		 * @param[in] length: length of data
		 * @return wait_time: time in milli sec
		 */
		virtual uint32_t Erase_Time( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length )
		{
			return ( 0 );
		}

	protected:
		/**
		 * @brief A protected constructor to ensure the class is only instantiated via specialized
		 * classes for specific devices.
		 * @param[in] mem_attributes: A pointer to the structure holding the attributes for the instantiated
		 * NV_Ctrl object. The structure this pointer references must persist for the life of the object.
		 */
		NV_Ctrl( const mem_attribute_struct_t* mem_attributes = nullptr )
		{
			m_mem_attributes = mem_attributes;
		}

		/**
		 * @brief A pure, virtual call back handler to support asynchronous execution of operations.
		 * This must be implemented in any derived class.
		 */
		virtual void Continue( void ) = 0;

		/**
		 * @brief A static class call back redirector to pass control to the appropriate
		 * object-specific call back handler.
		 * @param[in] param: pointer to NV_Ctrl object call back is intended for.
		 */
		static void Continue_Call_Back( void* param )
		{
			static_cast<NV_Ctrl*>( param )->Continue();
		}

		/**
		 * @brief						Private member variable for a mem attribute structure.
		 *@n details					Memory Attributes.
		 */
		const mem_attribute_struct_t* m_mem_attributes;
};

#endif
