/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This is used for the temporary data storage in stack.
 *
 *	@details This module contains functionality to allocate memory from the
 *	scratch ram.
 *
 *  @n @b Usage: Temporary data storage.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef SCRATCH_RAM_H
#define SCRATCH_RAM_H

namespace BF_Lib
{
/**
 ****************************************************************************************
 * @brief This is the Scratch_Ram class and generally Scratch Ram is used for
 * faster data access.
 * @details It provides an interface to allocate memory from the temporary stack.
 * @n @b Usage:
 * @n @b 1. Temporary memory allocation for faster data access.
 * @n @b 2. It provides public methods to: to initialize and allocate memory from the
 * scratch stack space.
 ****************************************************************************************
 */
class Scratch_Ram	// : public Ram_Handler
{
	public:

		/**
		 *  @brief
		 *  @details Static function to initialize the scratch stack space.
		 *  @param[in] size: The heap size.
		 *  @param[in] alignment_boundry_size:
		 *  return None
		 */
		static void Init( size_t size, uint8_t alignment_boundry_size =
						  PROCESSOR_REGISTER_BASE_SIZE );

		/**
		 *  @brief Constructor.
		 *  @details Does the data members initialization and call to allocate in the
		 *  scratch Ram memory.
		 *  @param[in] size: The total data size to be allocated the scratch Ram memory.
		 *  @param[in] fill: The status bit whether to fill memory or not. True implies fill in.
		 *  @param[in] fill_val: Value to be filled in memory, initially filled zero.
		 *  return None
		 */
		Scratch_Ram( size_t size = 0, bool fill = true, uint8_t fill_val = 0U );

		/**
		 *  @brief Destructor for the Scratch_Ram Module.
		 *  @details This will get called when object of Scratch_Ram goes out of scope.
		 */
		~Scratch_Ram();

		/**
		 *  @brief Used to allocate data in the temporary stack.
		 *  @n @b Usage:
		 *  @n @b 1. Do not call this function if you have already passed in a size to the constructor.
		 *  It will error out.
		 *  @n @b 2. Allocate will reserve some space on the temporary stack.
		 *  @param[in] size: The total data size to be allocated the scratch Ram memory.
		 *  @param[in] fill: The status bit whether to fill memory or not. True implies fill in.
		 *  @param[in] fill_val: Value to be filled in memory, initially filled zero.
		 *  @return void pointer to the allocated memory.
		 */
		void* Allocate( size_t size, bool fill = true, uint8_t fill_val = 0U );

		/// The following if used should be used as a stack variable in a function.
		/// It relies on the destructor working after the function exit.
		/// If you use pass a size of 0 you will get a null pointer for m_data.  Therefore
		/// if there is a potential for a 0 sized construction verify that the m_data pointer
		/// is not null before use.
		uint8_t* m_data;
		size_t m_size;

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Scratch_Ram( const Scratch_Ram& object );
		Scratch_Ram & operator =( const Scratch_Ram& object );

		static uint8_t Used( void );

		static uint8_t Calculate_Percent_Used( void );

		static uint8_t* m_heap_start;
		static uint8_t* m_heap_head;
		static uint8_t* m_high_water_mark;
		static size_t m_heap_size;

		static uint8_t m_alignment_boundry_size;
		static uint8_t m_percent_used;

};

}

#endif	/*SCRATCH_RAM_H*/
