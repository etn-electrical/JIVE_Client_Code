/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "DCI_Monitor.h"
#include "DCI_Source_IDs.h"
#include "Timers_Lib.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Helper template class for term printing.
 *****************************************************************************************
 */
template<typename T>
class TPrint
{
	public:
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static void Print_Int( uint8_t const* data, DCI_LENGTH_TD length )
		{
			T temp_data;

			while ( length >= sizeof( T ) )
			{
				length -= static_cast<DCI_LENGTH_TD>( sizeof( T ) );

				BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &temp_data ), data, sizeof( T ) );

				Term_Print( "%d, ", temp_data );
			}
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static void Print_Uint( uint8_t const* data, DCI_LENGTH_TD length )
		{
			T temp_data;

			while ( length >= sizeof( T ) )
			{
				length -= static_cast<DCI_LENGTH_TD>( sizeof( T ) );

				BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &temp_data ), data, sizeof( T ) );

				Term_Print( "%u, ", temp_data );
			}
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static void Print_Float( uint8_t const* data, DCI_LENGTH_TD length )
		{
			T temp_data;

			while ( length >= sizeof( T ) )
			{
				length -= static_cast<DCI_LENGTH_TD>( sizeof( T ) );

				BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &temp_data ), data, sizeof( T ) );

				Term_Print( "%f, ", temp_data );
			}
		}

	private:
};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Monitor::DCI_Monitor( monitor_list_t const* dci_list, cback_param_t param, cback_func_t func ) :
	m_cback_param( param ),
	m_cback( func ),
	m_change_tracker( new Change_Tracker( &Change_Handler_Static,
										  reinterpret_cast<cback_param_t>( this ), true,
										  Change_Tracker::TRACK_ALL_ATTRIBUTES_MASK ) ),
	m_worker( new DCI_Workers() ),
	m_source_id( DCI_SOURCE_IDS_Get() )
{
	DCI_LENGTH_TD max_length = 0U;
	DCI_LENGTH_TD dci_length;

	if ( m_cback == &Change_Print_Static )
	{
		m_cback_param = reinterpret_cast<cback_param_t>( this );

		Term_Print( "DCI_Mon:, Sample Counter, Sample Time(ms), DCID, Access Type, Value(s)" );
	}

	for ( DCI_ID_TD i = 0U; i < dci_list->length; i++ )
	{
		m_change_tracker->Track_ID( dci_list->list[i] );
		m_worker->Get_Length( dci_list->list[i], &dci_length );
		if ( max_length < dci_length )
		{
			max_length = dci_length;
		}
	}

	m_temp_data = reinterpret_cast<uint8_t*>( Ram::Allocate( max_length ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Monitor::~DCI_Monitor( void )
{
	m_cback_param = reinterpret_cast<cback_param_t>( nullptr );
	m_cback = reinterpret_cast<cback_func_t>( nullptr );
	delete m_change_tracker;
	delete m_worker;
	delete m_temp_data;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Monitor::Change_Handler( DCI_ID_TD dci_id, Change_Tracker::attrib_mask_t attribute_mask )
{
	DCI_ERROR_TD return_error;
	DCI_LENGTH_TD length;
	DCI_ACCESS_ST_TD access_struct;

	Change_Tracker::attrib_t attrib = Change_Tracker::RAM_ATTRIB;

	return_error = m_worker->Get_Length( dci_id, &length );

	if ( return_error == DCI_ERR_NO_ERROR )
	{
		while ( attribute_mask != 0U )
		{
			// We have an OFI here.  We should really go through and check all the different ways
			// this attribute mask could have changed and called the callback appropriately from
			// there.
			// We should be cycling through the bitmask and calling for each attribute.  Done wins.
			if ( BF_Lib::Bit::Test( attribute_mask, Change_Tracker::RAM_ATTRIB ) )
			{
				access_struct.command = DCI_ACCESS_GET_RAM_CMD;
				attrib = Change_Tracker::RAM_ATTRIB;
				BF_Lib::Bit::Clr( attribute_mask, Change_Tracker::RAM_ATTRIB );
			}
			else if ( BF_Lib::Bit::Test( attribute_mask, Change_Tracker::NV_ATTRIB ) )
			{
				access_struct.command = DCI_ACCESS_GET_INIT_CMD;
				attrib = Change_Tracker::NV_ATTRIB;
				BF_Lib::Bit::Clr( attribute_mask, Change_Tracker::NV_ATTRIB );
			}
			else if ( BF_Lib::Bit::Test( attribute_mask, Change_Tracker::DEFAULT_ATTRIB ) )
			{
				access_struct.command = DCI_ACCESS_GET_DEFAULT_CMD;
				attrib = Change_Tracker::DEFAULT_ATTRIB;
				BF_Lib::Bit::Clr( attribute_mask, Change_Tracker::DEFAULT_ATTRIB );
			}
			else if ( BF_Lib::Bit::Test( attribute_mask, Change_Tracker::MIN_ATTRIB ) )
			{
				access_struct.command = DCI_ACCESS_GET_MIN_CMD;
				attrib = Change_Tracker::MIN_ATTRIB;
				BF_Lib::Bit::Clr( attribute_mask, Change_Tracker::MIN_ATTRIB );
			}
			else if ( BF_Lib::Bit::Test( attribute_mask, Change_Tracker::MAX_ATTRIB ) )
			{
				access_struct.command = DCI_ACCESS_GET_MAX_CMD;
				attrib = Change_Tracker::MAX_ATTRIB;
				BF_Lib::Bit::Clr( attribute_mask, Change_Tracker::MAX_ATTRIB );
			}

			access_struct.data_id = dci_id;
			access_struct.data_access.data = m_temp_data;
			access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
			access_struct.data_access.offset = ACCESS_OFFSET_NONE;
			access_struct.source_id = m_source_id;

			return_error = m_worker->Data_Access( &access_struct );

			if ( return_error == DCI_ERR_NO_ERROR )
			{
				( *m_cback )( m_cback_param, dci_id, m_temp_data, length, attrib );
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Monitor::Change_Print( DCI_ID_TD dci_id, DCI_DATA_PASS_TD const* data,
								DCI_LENGTH_TD data_length,
								Change_Tracker::attrib_t attribute ) const
{
	DCI_DATATYPE_TD datatype;
	static uint32_t sample_counter = 0U;

	Term_Print( "DCI_Mon:, %u, %u, ", sample_counter, BF_Lib::Timers::Get_Time() );
	sample_counter++;

	Term_Print( "%d, ", dci_id );

	switch ( attribute )
	{
		case Change_Tracker::RAM_ATTRIB:
			Term_Print( "Ram_Val:, " );
			break;

		case Change_Tracker::NV_ATTRIB:
			Term_Print( "NV_Val:, " );
			break;

		case Change_Tracker::DEFAULT_ATTRIB:
			Term_Print( "Default_Val:, " );
			break;

		case Change_Tracker::MIN_ATTRIB:
			Term_Print( "Min_Val:, " );
			break;

		case Change_Tracker::MAX_ATTRIB:
			Term_Print( "Max_Val:, " );
			break;

		default:
			BF_ASSERT( false );
			break;
	}

	m_worker->Get_Datatype( dci_id, &datatype );

	switch ( datatype )
	{
		case DCI_DTYPE_BOOL:
		case DCI_DTYPE_UINT8:
		case DCI_DTYPE_BYTE:
		case DCI_DTYPE_STRING8:
			TPrint<uint8_t>
			::Print_Uint( reinterpret_cast<uint8_t const*>( data ), data_length );
			break;

		case DCI_DTYPE_SINT8:
			TPrint<int8_t>::Print_Int( reinterpret_cast<uint8_t const*>( data ), data_length );
			break;

		case DCI_DTYPE_UINT16:
		case DCI_DTYPE_WORD:
			TPrint<uint16_t>
			::Print_Uint( reinterpret_cast<uint8_t const*>( data ), data_length );
			break;

		case DCI_DTYPE_SINT16:
			TPrint<int16_t>::Print_Int( reinterpret_cast<uint8_t const*>( data ), data_length );
			break;

		case DCI_DTYPE_UINT32:
		case DCI_DTYPE_DWORD:
			TPrint<uint32_t>
			::Print_Uint( reinterpret_cast<uint8_t const*>( data ), data_length );
			break;

		case DCI_DTYPE_SINT32:
			TPrint<int32_t>::Print_Int( reinterpret_cast<uint8_t const*>( data ), data_length );
			break;

		// case DCI_DTYPE_UINT64:
		// TPrint<uint64_t>::Print( reinterpret_cast<uint8_t const*>( data ), data_length );
		// break;
		//
		// case DCI_DTYPE_SINT64:
		// TPrint<int64_t>::Print( reinterpret_cast<uint8_t const*>( data ), data_length );
		// break;

		case DCI_DTYPE_FLOAT:
			TPrint<float32_t>::Print_Float( reinterpret_cast<uint8_t const*>( data ),
											data_length );
			break;

		// case DCI_DTYPE_DFLOAT:
		// TPrint<float64_t>::Print( reinterpret_cast<uint8_t const*>( data ), data_length );
		// break;
		//
		default:
			break;
	}

	Term_Print( "\n" );

}
