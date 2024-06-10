/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Interface between the DCIDs and the J1939 application.
 *
 *	@details This module will help create and read the J1939 messages from the network
 *	and update the DCID in case of read and while creating we will fetch dcid values
 *	values and create a j1939 message for a request.
 *
 *	@copyright 2017 Eaton Corporation. All Rights Reserved.
 *
 ***/

#ifndef J1939_DCI_H
	#define J1939_DCI_H

#include "Includes.h"
#include "stdlib.h"
#include "DCI_Patron.h"
#include "J1939_Msg.h"				// #include "j1939.h"

/// This structure defines the J1939 message to DCID lookup.
typedef struct
{
	uint32_t pgn;					/* PGN number of the J1939 message*/
	DCI_ID_TD dcid_count;			/* Total DCI data present in the message */
	uint16_t msg_size;				/* Total byte required for CAN message frame */
	const uint8_t* bitlength;		/* Array of bitlength for each DCI data present in the message*/
	const DCI_ID_TD* dcid_array;	/* List of all DCID present in the message */
} J1939_DCI_STRUCT;

class J1939_DCI
{
	public:
		/**
		 * @brief Defines the types of error during j1939 dci handling.
		 */
		typedef enum
		{
			NO_ERROR,
			MEMORY_ALLOCATION_FAILED,
			PGN_NOT_FOUND,
			DCID_GET_FAILED,
			DCID_SET_FAILED
		} j1939_error_codes_t;

		/**
		 *  @brief Initializes the J1939 DCI Interface.
		 *  @details
		 */
		J1939_DCI();

		/**
		 *  @brief Destructor.
		 *  @details
		 */
		~J1939_DCI();

		/**
		 *  @brief Will create the J1939 message corresponding to the pgn.
		 *  @details
		 *  @param[in] msg: j1939 msg structure to be filled.
		 */
		j1939_error_codes_t Get_J1939_Message( j1939_t* msg );

		/**
		 *  @brief Update the corresponding DCIDs from a received J1939 message
		 *  @details
		 *  @param[in] msg: Pointer to received j1939 message
		 */
		j1939_error_codes_t Set_J1939_Message( j1939_t* msg );

	private:
		/**
		 *  @brief This function will copy the src data at a specific bitposition
		 *  in the destination buffer.
		 *  @details
		 *	@param[in] src: Data to be copied.
		 *  @param[in] dest: destination buffer to copy the src data.
		 *	@param[in] bit_len: length of the source data in bits.
		 *  @param[in] bit_offset: offset bitposition in the destination buffer to copy data.
		 */
		void Mask_And_Insert( uint64_t* src, uint8_t dest[], uint8_t bit_len, uint16_t* bit_offset );

		/**
		 *  @brief This function will extract the DCID from the source data at a specific bit position
		 *  and store in the destination buffer.
		 *  @details
		 *	@param[in] src: Received J1939 message address
		 *  @param[in] dest: buffer to store the extracted DCID value
		 *	@param[in] bit_len: length of the source data in bits.
		 *  @param[in] bit_offset: offset bit position in the source buffer to copy data.
		 */
		void Mask_And_Extract( uint8_t* src, uint64_t dest[], uint8_t bit_len, uint16_t* bit_offset );

		/**
		 *  @brief This function will return the j1939 message structure from the DCI database.
		 *  @details
		 *	@param[in] pgn: pgn to be searched for.
		 *  @param[out] Return the address of the j1939 message structure.
		 */
		J1939_DCI_STRUCT* Find_J1939_Msg_Struct( uint32_t pgn );

		/**
		 *  @brief This function will update DCI variable in the DCI data base .
		 *  @details
		 *	@param[in] dcid: dcid number to set the value.
		 *  @param[in] data: data value to be checked in.
		 */
		DCI_ERROR_TD Set_DCID_Value( DCI_ID_TD dcid, uint64_t* data );

		/**
		 *  @brief This function will return the DCI value from the DCI database.
		 *  @details
		 *	@param[in] dcid: dcid number to get the value.
		 *  @param[in] data: data that will be checkout.
		 */
		DCI_ERROR_TD Get_DCID_Value( DCI_ID_TD dcid, uint64_t* data );

		/**
		 * Patron handler for getting and setting the required DCIDs.
		 */
		DCI_Patron* m_patron;

		/**
		 * Source id for patron access.
		 */
		DCI_SOURCE_ID_TD m_source_id;
};

#endif
