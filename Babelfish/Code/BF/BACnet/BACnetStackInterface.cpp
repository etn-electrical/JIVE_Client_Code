/**
 *****************************************************************************************
 *	@file BACnetStackInterface.cpp
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "BACnetStackInterface.h"
#include "vsbhp.h"
#include "napdu.h"
#include "string.h"
#include "util.h"
#include "stdlib.h"
#include "math.h"

#include "BACnetUARTInterface.h"
#include "AVObject.h"
#include "AIObject.h"
#include "BVObject.h"
#include "BACnetDCIInterface.h"
#include "BACnet_DCI.h"
#include "Cov.h"
#include "CovChangeTracker.h"
#include "MSVObject.h"
#include "StdLib_MV.h"

extern BACnetObject avObject;
extern BACnetObject aiObject;
extern BACnetObject bvObject;
extern BACnetObject MSVObject;	// Nishchal D, 05Aug2016 - MSV Implementation
extern BACnetObject covList;
extern dword ourinstance;
CovChangeTracker* covChangeTracker;
callback_t m_msv_user_callback;
extern BACnet_DCI* BACnetDCIPtr;


static const BACNET_TARGET_INFO_ST_TD* m_bacnet_target_info;

/** @fn int32_t  fraReadProperty(dword objid, dword propid, dword aindex, frVbag *vp, dword *nextpid)
 *   @brief : read a property of an object
 *   @details :read a property of an valid object else respond with appropriate error code
 *   @param[in]  objid	the object identifier
 *   @param[in]  propid	property identifier
 *   @param[in]  aindex	the array index, or the special value noindex
 *   @param[in]  vp		points to a value bag i.e. an frVbag containing the property value
 *   @param[in]  nextpid	if propid=ALL, REQUIRED or OPTIONAL this points to the property used to get the next
 *				property in the list (if ALL, REQUIRED or OPTIONAL, then get the first)
 *   @return:0 accepted, vp filledin otherwise !0	rejected and this is the reason
 */

int32_t fraReadProperty( dword objid, dword propid, dword aindex, frVbag* vp, dword* nextpid )
{
	word objtype = static_cast<word>( ( objid >> 22U ) & 0x3FFU );
	const dword* lp;	// GSB
	dword pid = propid;
	dword objinst = ( objid & 0x3FFFFFU );
	int32_t i;
	float32_t valAV = 0U;
	float32_t valAI = 0U;
	uint8_t valMSV = 0U;
	int32_t retCode = 0;

	if ( objid == ( ( ( dword )DEVICE_OBJTYPE << 22U ) + ourinstance ) )	// our device object?
	{
		return ( vsbe_unknownproperty );// VSB takes care of all the DevObj properties, we have no special ones
	}
	else
	{}
	if ( !isObjectPresent( objid ) )
	{
		return ( vsbe_unknownobject );
	}
	else
	{}

	vp->narray = 0U;		// IMPORTANT!!!

	if ( objtype == ANALOG_VALUE_OBJTYPE )			// **handle AV objects Read
	{
		BACnetAV* avPtr = ( BACnetAV* )avObject.objects[( uint16_t )objinst];
nextav:
		vp->narray = 0U;							// THIS IS IMPORTANT!			***005 End
		switch ( pid )
		{
			case ALLX:
			case OPTIONALX:
			case REQUIRED:
				if ( pid == ALLX )						// all
				{
					lp = AValllist;
				}
				else if ( pid == REQUIRED )				// required
				{
					lp = AVreqlist;
				}
				else
				{
					// if(pid==OPTIONALX)			//optional
					lp = AVoptlist;
				}
				pid = *nextpid;						// search list for this one
				while ( *lp != 0xFFFFFFFFU )
				{
					if ( pid == *( lp++ ) )
					{
						*nextpid = *lp;				// get next one
						if ( *nextpid == 0xFFFFFFFFU )
						{
							break;					// we're all done
						}
						else
						{
							pid = *nextpid;
						}
						goto nextav;
					}
				}
				return ( vsbe_unknownproperty );		// we're all done

			case PRESENT_VALUE:						// present_value
				vp->pdtype = adtReal;
				retCode = getPresentValueAV( objid, valAV );
				if ( retCode != vsbe_noerror )
				{
					return ( retCode );
				}
				vp->pd.fval = valAV;
				break;

			// ---WIP---
			case PRIORITY_ARRAY:						// priority array				***005 Begin
				if ( m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[( uint16_t )objinst]
					 .accessType != IS_COMMENDABLE )
				{
					return ( vsbe_unknownproperty );
				}
				else if ( aindex == 0U )						// we want the size of the array
				{
					vp->pdtype = adtUnsigned;
					vp->pd.uval = 16U;
					break;
				}
				if ( aindex != noindex )				// we want only one value
				{
					if ( aindex > 16U )
					{
						return ( vsbe_invalidarrayindex );
					}
					if ( avPtr->priority_array.d[aindex - 1U] == nullvalue )	// ***210
					{
						vp->pdtype = adtNull;
					}
					else
					{
						vp->pdtype = adtReal;
						vp->pd.fval = avPtr->priority_array.f[aindex - 1U];	// ***210
					}
				}
				else
				{
					vp->narray = 16U;					// entire array
					vp->pdtype = adtReal;
					for ( i = 0U; i < 16U; i++ )
					{
						vp->ps.psfval[i] = avPtr->priority_array.f[i];		// ***210
					}
				}
				break;

			case RELINQUISH_DEFAULT:								// relinquish default
				if ( m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[( uint16_t )objinst]
					 .accessType != IS_COMMENDABLE )
				{
					return ( vsbe_unknownproperty );
				}
				else
				{
					vp->pdtype = adtReal;
					vp->pd.fval = avPtr->reli_default;	// ***210
				}
				break;

			// ---WIP---
			case OBJECT_IDENTIFIER:					// objectid
				vp->pdtype = adtObjectID;
				vp->pd.uval = objid;
				break;

			case OBJECT_TYPE:						// object type
				vp->pdtype = adtEnumerated;
				vp->pd.uval = ANALOG_VALUE_OBJTYPE;
				break;

			case OBJECT_NAME:						// object name
				vp->pdtype = adtCharString;
				vp->pd.stval.text = m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[objinst]
					.name.text;
				vp->pd.stval.len = m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[objinst]
					.name.len;
				break;

			case DESCRIPTION:						// object description
				vp->pdtype = adtCharString;
				vp->pd.stval.text = m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[objinst]
					.desc.text;
				vp->pd.stval.len = m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[objinst]
					.desc.len;
				break;

			case STATUS_FLAGS:						// status_flags
				vp->pdtype = adtBitString;
				vp->pd.uval = 4U;
				vp->ps.psval[0U] = 0U;					// always normal
				vp->narray = 0U;
				break;

			case EVENT_STATE:						// event_State
				vp->pdtype = adtEnumerated;
				vp->pd.uval = 0U;						// always normal
				break;

			case OUT_OF_SERVICE:					// out_of_service
				vp->pdtype = adtBoolean;
				vp->pd.uval = 0U;						// always FALSE
				break;

			case UNITSX:							// units
				vp->pdtype = adtEnumerated;
				vp->pd.uval =
					m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[objinst].units;	// no-units
				break;

			case COV_INCREMENT:	// Nishchal D, 19May2016 : Added COV_INCREMENT Read into the Analog Value object.
				vp->pdtype = adtReal;
				vp->pd.fval = avPtr->covincrement;
				break;

			default:
				return ( vsbe_unknownproperty );		// not one we know about
		}
		return ( 0 );								// good one, just return it
	}
	else if ( objtype == BINARY_VALUE_OBJTYPE )	// ******************handle read BVs**************
	{
		BACnetBV* bvPtr = ( BACnetBV* )bvObject.objects[( uint16_t )objinst];
nextbv:
		vp->narray = 0U;							// THIS IS IMPORTANT!
		switch ( pid )
		{
			case ALLX:
			case OPTIONALX:
			case REQUIRED:
				if ( pid == ALLX )						// all
				{
					lp = BValllist;
				}
				else if ( pid == REQUIRED )				// required
				{
					lp = BVreqlist;
				}
				else
				{
					// if(pid==OPTIONALX)			//optional
					lp = BVoptlist;
				}
				pid = *nextpid;						// search list for this one
				while ( *lp != 0xFFFFFFFFU )
				{
					if ( pid == *lp++ )
					{
						*nextpid = *lp;				// get next one
						if ( *nextpid == 0xFFFFFFFFU )
						{
							break;					// we're all done
						}
						pid = *nextpid;
						goto nextbv;
					}
				}
				return ( vsbe_unknownproperty );		// we're all done

			case PRESENT_VALUE:						// present_value
				vp->pdtype = adtEnumerated;
				vp->pd.uval = getPresentValueBV( objid );
				break;

			// ---WIP---
			case PRIORITY_ARRAY:						// priority array				***005 Begin
				if ( m_bacnet_target_info->class_list->BACnet_BV_to_DCID_map[( uint16_t )objinst]
					 .accessType != IS_COMMENDABLE )
				{
					return ( vsbe_unknownproperty );
				}
				else if ( aindex == 0U )						// we want the size of the array
				{
					vp->pdtype = adtUnsigned;
					vp->pd.uval = 16U;
					break;
				}
				if ( aindex != noindex )				// we want only one value
				{
					if ( aindex > 16U )
					{
						return ( vsbe_invalidarrayindex );
					}
					if ( bvPtr->priority_array[aindex - 1U] == nullvalue )	// ***210
					{
						vp->pdtype = adtNull;
					}
					else
					{
						vp->pdtype = adtEnumerated;
						vp->pd.uval = bvPtr->priority_array[aindex - 1U];	// ***210
					}
				}
				else
				{
					vp->narray = 16U;					// entire array
					vp->pdtype = adtEnumerated;
					for ( i = 0U; i < 16U; i++ )
					{
						vp->ps.psdval[i] = bvPtr->priority_array[i];		// ***210
					}
				}
				break;

			case RELINQUISH_DEFAULT:								// relinquish default
				if ( m_bacnet_target_info->class_list->BACnet_BV_to_DCID_map[( uint16_t )objinst]
					 .accessType != IS_COMMENDABLE )
				{
					return ( vsbe_unknownproperty );
				}
				else
				{										// ***405 End
					vp->pdtype = adtEnumerated;
					vp->pd.uval = bvPtr->reli_default;	// ***210
				}
				break;

			// ---WIP---
			case OBJECT_IDENTIFIER:					// objectid
				vp->pdtype = adtObjectID;
				vp->pd.uval = objid;
				break;

			case OBJECT_TYPE:						// object type
				vp->pdtype = adtEnumerated;
				vp->pd.uval = BINARY_VALUE_OBJTYPE;
				break;

			case OBJECT_NAME:						// object name
				vp->pdtype = adtCharString;
				vp->pd.stval.text = m_bacnet_target_info->class_list->BACnet_BV_to_DCID_map[objinst]
					.name.text;
				vp->pd.stval.len = m_bacnet_target_info->class_list->BACnet_BV_to_DCID_map[objinst]
					.name.len;
				break;

			case DESCRIPTION:						// object description
				vp->pdtype = adtCharString;
				vp->pd.stval.text = m_bacnet_target_info->class_list->BACnet_BV_to_DCID_map[objinst]
					.desc.text;
				vp->pd.stval.len = m_bacnet_target_info->class_list->BACnet_BV_to_DCID_map[objinst]
					.desc.len;
				break;

			case STATUS_FLAGS:						// status_flags
				vp->pdtype = adtBitString;
				vp->pd.uval = 4U;
				vp->ps.psval[0U] = 0U;					// always normal
				// vp->narray=0U;
				break;

			case EVENT_STATE:						// event_State
				vp->pdtype = adtEnumerated;
				vp->pd.uval = 0U;						// always normal
				break;

			case OUT_OF_SERVICE:					// out_of_service
				vp->pdtype = adtBoolean;
				vp->pd.uval = 0U;						// FALSE
				break;

			case ACTIVE_TEXT:
				vp->pdtype = adtCharString;
				vp->pd.stval.len = m_bacnet_target_info->class_list->BACnet_BV_to_DCID_map[objinst]
					.active_text.len;
				vp->pd.stval.text = m_bacnet_target_info->class_list->BACnet_BV_to_DCID_map[objinst]
					.active_text.text;
				if ( 0U == vp->pd.stval.len )
				{
					vp->pdtype = adtEmptyList;
				}
				vp->narray = 0U;
				break;

			case INACTIVE_TEXT:
				vp->pdtype = adtCharString;
				vp->pd.stval.len = m_bacnet_target_info->class_list->BACnet_BV_to_DCID_map[objinst]
					.inactive_text.len;
				vp->pd.stval.text = m_bacnet_target_info->class_list->BACnet_BV_to_DCID_map[objinst]
					.inactive_text.text;
				if ( 0U == vp->pd.stval.len )
				{
					vp->pdtype = adtEmptyList;
				}
				vp->narray = 0U;
				break;

			default:
				return ( vsbe_unknownproperty );		// not one we know about
		}
		return ( 0 );								// good one, just return it
	}
	else if ( objtype == ANALOG_INPUT_OBJTYPE )			// **handle AV objects Read
	{
		BACnetAI* aiPtr = ( BACnetAI* )aiObject.objects[( uint16_t )objinst];
nextai:
		vp->narray = 0U;							// THIS IS IMPORTANT!			***005 End
		switch ( pid )
		{
			case ALLX:
			case OPTIONALX:
			case REQUIRED:
				if ( pid == ALLX )						// all
				{
					lp = AIalllist;
				}
				else if ( pid == REQUIRED )				// required
				{
					lp = AIreqlist;
				}
				else
				{
					// if(pid==OPTIONALX)			//optional
					lp = AIoptlist;
				}
				pid = *nextpid;						// search list for this one
				while ( *lp != 0xFFFFFFFFUL )
				{
					if ( pid == *lp++ )
					{
						*nextpid = *lp;				// get next one
						if ( *nextpid == 0xFFFFFFFFUL )
						{
							break;					// we're all done
						}
						pid = *nextpid;
						goto nextai;
					}
				}
				return ( vsbe_unknownproperty );		// we're all done

			case PRESENT_VALUE:						// present_value
				vp->pdtype = adtReal;
				retCode = getPresentValueAI( objid, valAI );
				if ( retCode != vsbe_noerror )
				{
					return ( retCode );
				}
				vp->pd.fval = valAI;
				break;

			// ---WIP---
			// case PRIORITY_ARRAY:								//priority array				***005 Begin
			// if (m_bacnet_target_info->class_list->BACnet_AI_to_DCID_map[(uint16_t)objinst].accessType !=
			// IS_COMMENDABLE)
			// {
			// return vsbe_unknownproperty;
			// }
			// else if (aindex==0)						//we want the size of the array
			// {
			// vp->pdtype=adtUnsigned;
			// vp->pd.uval=16;
			// break;
			// }
			// if (aindex!=noindex)				//we want only one value
			// {
			// if (aindex>16)
			// return vsbe_invalidarrayindex;
			// if ( aiPtr->priority_array.d[aindex-1U]==nullvalue)	//			***210
			// vp->pdtype=adtNull;
			// else
			// {
			// vp->pdtype = adtReal;
			// vp->pd.fval =aiPtr->priority_array.f[aindex-1U];	//		***210
			// }
			// }
			// else
			// {
			// vp->narray = 16;					//entire array
			// vp->pdtype = adtReal;
			// for (i=0;i<16;i++)
			// {
			// vp->ps.psfval[i] = aiPtr->priority_array.f[i];		//	***210
			// }
			// }
			// break;
			case RELINQUISH_DEFAULT:								// relinquish default
				if ( m_bacnet_target_info->class_list->BACnet_AI_to_DCID_map[( uint16_t )objinst]
					 .accessType != IS_COMMENDABLE )
				{
					return ( vsbe_unknownproperty );
				}
				else
				{
					vp->pdtype = adtReal;
					vp->pd.fval = aiPtr->reli_default;	// ***210
				}
				break;

			// ---WIP---
			case OBJECT_IDENTIFIER:					// objectid
				vp->pdtype = adtObjectID;
				vp->pd.uval = objid;
				break;

			case OBJECT_TYPE:						// object type
				vp->pdtype = adtEnumerated;
				vp->pd.uval = ANALOG_INPUT_OBJTYPE;
				break;

			case OBJECT_NAME:						// object name
				vp->pdtype = adtCharString;
				vp->pd.stval.text = m_bacnet_target_info->class_list->BACnet_AI_to_DCID_map[objinst]
					.name.text;
				vp->pd.stval.len = m_bacnet_target_info->class_list->BACnet_AI_to_DCID_map[objinst]
					.name.len;
				break;

			case DESCRIPTION:						// object description
				vp->pdtype = adtCharString;
				vp->pd.stval.text = m_bacnet_target_info->class_list->BACnet_AI_to_DCID_map[objinst]
					.desc.text;
				vp->pd.stval.len = m_bacnet_target_info->class_list->BACnet_AI_to_DCID_map[objinst]
					.desc.len;
				break;

			case STATUS_FLAGS:						// status_flags
				vp->pdtype = adtBitString;
				vp->pd.uval = 4U;
				vp->ps.psval[0U] = 0U;					// always normal
				vp->narray = 0U;
				break;

			case EVENT_STATE:						// event_State
				vp->pdtype = adtEnumerated;
				vp->pd.uval = 0U;						// always normal
				break;

			case OUT_OF_SERVICE:					// out_of_service
				vp->pdtype = adtBoolean;
				vp->pd.uval = 0U;						// always FALSE
				break;

			case UNITSX:							// units
				vp->pdtype = adtEnumerated;
				vp->pd.uval =
					m_bacnet_target_info->class_list->BACnet_AI_to_DCID_map[objinst].units;	// no-units
				break;

			case COV_INCREMENT:	// Nishchal D, 19May2016 : Added COV_INCREMENT Read into the Analog Input object.
				vp->pdtype = adtReal;
				vp->pd.fval = aiPtr->covincrement;
				break;

			default:
				return ( vsbe_unknownproperty );		// not one we know about
		}
		return ( 0 );								// good one, just return it
	}
	// Nishchal D,4th Aug: Added MSV.
	else if ( objtype == MULTI_STATE_VALUE_OBJTYPE )			// **handle MSV objects Read
	{
		BACnetMSV* msvPtr = ( BACnetMSV* )MSVObject.objects[( uint16_t )objinst];
nextMSV:
		vp->narray = 0U;							// THIS IS IMPORTANT!			***005 End
		uint16_t length = 0;
		int32_t abort_Code = 0;
		switch ( pid )
		{
			case ALLX:
			case OPTIONALX:
			case REQUIRED:
				if ( pid == ALLX )						// all
				{
					lp = MSValllist;
				}
				else if ( pid == REQUIRED )				// required
				{
					lp = MSVreqlist;
				}
				else
				{
					// if(pid==OPTIONALX)			//optional
					lp = MSVoptlist;
				}
				pid = *nextpid;						// search list for this one
				while ( *lp != 0xFFFFFFFFUL )
				{
					if ( pid == *lp++ )
					{
						*nextpid = *lp;				// get next one
						if ( *nextpid == 0xFFFFFFFFUL )
						{
							break;					// we're all done
						}
						pid = *nextpid;
						goto nextMSV;
					}
				}
				return ( vsbe_unknownproperty );		// we're all done

			case PRESENT_VALUE:						// present_value
				vp->pdtype = adtUnsigned;	// Fixed DH1-186
				retCode = getPresentValueMSV( objid, valMSV );
				if ( retCode != vsbe_noerror )
				{
					return ( retCode );
				}
				vp->pd.uval = valMSV;	// Fixed DH1-186
				break;

			case PRIORITY_ARRAY:						// priority array				***005 Begin
				if ( m_bacnet_target_info->class_list->BACnet_MSV_to_DCID_map[( uint16_t )objinst]
					 .accessType != IS_COMMENDABLE )
				{
					return ( vsbe_unknownproperty );
				}
				else if ( aindex == 0U )						// we want the size of the array
				{
					vp->pdtype = adtUnsigned;
					vp->pd.uval = 16U;
					break;
				}
				if ( aindex != noindex )				// we want only one value
				{
					if ( aindex > 16U )
					{
						return ( vsbe_invalidarrayindex );
					}
					if ( msvPtr->priority_array.d[aindex - 1U] == nullvalue )	// ***210
					{
						vp->pdtype = adtNull;
					}
					else
					{
						vp->pdtype = adtUnsigned;
						vp->pd.fval = msvPtr->priority_array.f[aindex - 1U];	// ***210
					}
				}
				else
				{
					vp->narray = 16U;					// entire array
					vp->pdtype = adtUnsigned;
					for ( i = 0U; i < 16U; i++ )
					{
						vp->ps.psfval[i] = msvPtr->priority_array.f[i];		// ***210
					}
				}
				break;

			case RELINQUISH_DEFAULT:								// relinquish default
				if ( m_bacnet_target_info->class_list->BACnet_MSV_to_DCID_map[( uint16_t )objinst]
					 .accessType != IS_COMMENDABLE )
				{
					return ( vsbe_unknownproperty );
				}
				else
				{
					vp->pdtype = adtUnsigned;
					vp->pd.fval = msvPtr->reli_default;	// ***210
				}
				break;

			case OBJECT_IDENTIFIER:					// objectid
				vp->pdtype = adtObjectID;
				vp->pd.uval = objid;
				break;

			case OBJECT_TYPE:						// object type
				vp->pdtype = adtEnumerated;
				vp->pd.uval = MULTI_STATE_VALUE_OBJTYPE;
				break;

			case OBJECT_NAME:						// object name
				vp->pdtype = adtCharString;
				vp->pd.stval.text =
					m_bacnet_target_info->class_list->BACnet_MSV_to_DCID_map[objinst].name.text;
				vp->pd.stval.len = m_bacnet_target_info->class_list->BACnet_MSV_to_DCID_map[objinst]
					.name.len;
				break;

			case DESCRIPTION:						// object description
				vp->pdtype = adtCharString;
				vp->pd.stval.text =
					m_bacnet_target_info->class_list->BACnet_MSV_to_DCID_map[objinst].desc.text;
				vp->pd.stval.len = m_bacnet_target_info->class_list->BACnet_MSV_to_DCID_map[objinst]
					.desc.len;
				break;

			case STATUS_FLAGS:						// status_flags
				vp->pdtype = adtBitString;
				vp->pd.uval = 4U;
				vp->ps.psval[0U] = 0U;					// always normal
				vp->narray = 0U;
				break;

			case EVENT_STATE:						// event_State
				vp->pdtype = adtEnumerated;
				vp->pd.uval = 0U;						// always normal
				break;

			case OUT_OF_SERVICE:					// out_of_service
				vp->pdtype = adtBoolean;
				vp->pd.uval = 0U;						// always FALSE
				break;

			case NUMBER_OF_STATES:	// Nishchal: Added property NUMBER_OF_STATES (number of possible states)
				vp->pdtype = adtUnsigned;
				vp->pd.uval = MSV_Object_State_Count[objinst];
				break;

			case STATE_TEXT:	// Fixed State_Text implementatin DH1-185
				if ( aindex == 0U )
				{
					vp->pdtype = adtUnsigned;
					vp->pd.uval = MSV_Object_State_Count[objinst];
				}
				else if ( aindex <= MSV_Object_State_Count[objinst] )
				{
					vp->pdtype = adtCharString;

					buildfrString( &vp->pd.stval, vp->ps.psval, false,
								   m_bacnet_target_info->class_list->BACnet_MSV_to_DCID_map[objinst]
								   .MSV_len_to_Obj[aindex - 1].msv.len,
								   ( byte )sizeof( vp->ps.psval ) );
					byteset( vp->ps.psval, 0x00, ( byte )sizeof( vp->ps.psval ) );
					BF_Lib::Copy_String( ( vp->ps.psval ),
										 reinterpret_cast<uint8_t*>( m_bacnet_target_info->class_list->
																	 BACnet_MSV_to_DCID_map[objinst].MSV_len_to_Obj[
																		 aindex - 1].msv.text ),
										 ( maxobjectlist * 4 ) );
				}
				else if ( aindex == noindex )
				{
					abort_Code = m_msv_user_callback( objinst );
					if ( abort_Code > abortreason )
					{
						return ( abort_Code );
					}

					vp->pdtype = adtCharString;
					vp->narray = MSV_Object_State_Count[objinst];

					length = 0;
					for ( i = 0; i < MSV_Object_State_Count[objinst]; i++ )			// get the length
																					// of each
																					// string
					{
						length += m_bacnet_target_info->class_list->BACnet_MSV_to_DCID_map[objinst]
							.MSV_len_to_Obj[i].msv.len;
						if ( i != ( MSV_Object_State_Count[objinst] - 1 ) )			// add space for
																					// the comma!
						{
							length++;
						}
					}
					buildfrString( &vp->pd.stval, ( octet* )vp->ps.psval, true, length,
								   ( byte )sizeof( vp->ps.psval ) );
					byteset( vp->ps.psval, 0x00, ( byte )sizeof( vp->ps.psval ) );
					for ( i = 0; i < MSV_Object_State_Count[objinst]; i++ )			// build a csv
																					// string of all
																					// string vals
					{
						strcat( ( char* )vp->ps.psval,
								m_bacnet_target_info->class_list->BACnet_MSV_to_DCID_map[objinst].MSV_len_to_Obj[
									i].msv.text );
						if ( i != ( MSV_Object_State_Count[objinst] - 1 ) )
						{
							strcat( ( char* )vp->ps.psval, "," );
						}
					}
				}
				else if ( aindex > MSV_Object_State_Count[objinst] )
				{
					return ( vsbe_invalidarrayindex );			// fre_invalidarrayindex;
				}
				break;

			default:
				return ( vsbe_unknownproperty );		// not one we know about
		}
		return ( 0 );								// good one, just return it
	}
	else
	{
		return ( vsbe_unknownobject );				// bad one
	}
}

/** @fn int32_t    fraWriteProperty(dword objid, dword propid, dword aindex, frVbag *vp)
 *   @brief : write a property of an object
 *   @details :write a property of an valid object else respond with appropriate error code
 *   @param[in]  objid	the object identifier
 *   @param[in]  propid	property identifier
 *   @param[in]  aindex	the array index, or the special value noindex
 *   @param[in]  vp		points to a value bag i.e. an frVbag containing the property value
 *   @return:0 accepted, vp written otherwise !0	rejected and this is the reason
 */
int32_t fraWriteProperty( dword objid, dword propid, dword aindex, frVbag* vp )
{
	int32_t retCode = 0;
	dword objType = ( objid >> 22U );
	dword objInst = ( objid & 0x3FFFFFU );

	int32_t n;
	int32_t i;
	float32_t presentValueAV;
	uint8_t presentValueBV;

	if ( !isObjectPresent( objid ) )
	{
		return ( vsbe_unknownobject );
	}
#if 0
	if ( aindex != noindex )// check if there is index given
	{
		// yes there is a index provided, but is that property is an array?
		if ( isPropertyAnArray( propid ) == FALSE )
		{
			return ( vsbe_propertyIsNotAnArray );
		}
	}
#endif
	if ( objType == ANALOG_VALUE_OBJTYPE )			// handle AV objects
	{
		BACnetAV* avPtr = ( BACnetAV* )avObject.objects[( uint16_t )objInst];
		switch ( propid )
		{
			case COV_INCREMENT:	// Nishchal D, 18May2016 : Added COV_INCREMENT Write into the Analog Value object.
				if ( vp->pdtype != adtReal )
				{
					return ( vsbe_invaliddatatype );
				}
				avPtr->covincrement = vp->pd.fval;
				return ( retCode );

			case PRESENT_VALUE:
				if ( objType == ANALOG_VALUE_OBJTYPE )
				{
					if ( m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[( uint16_t )objInst]
						 .accessType != IS_COMMENDABLE )
					{
						// Implementation without priority_array and relinguish_default
						if ( vp->pdtype != adtReal )
						{
							return ( vsbe_invaliddatatype );
						}
						// if(vp->pd.fval > 0xFFFF) return  vsbe_valueoutofrange;
						presentValueAV = vp->pd.fval;
						retCode = setPresentValueAV( objid, presentValueAV );
						return ( retCode );	// check this later
					}
					// **********************************************
					if ( ( vp->priority > 16U ) || ( vp->priority == 6U ) )
					{
						return ( vsbe_inconsistentparameters );	// this will eventually be changed by an addendum
					}
					else if ( vp->priority == 0U )
					{
						vp->priority = 16U;							// use lowest priority
					}
					if ( vp->pdtype == adtReal )
					{	// in our example let's limit this to 0.0 to 100.0
						// if (vp->pd.fval<0.0||vp->pd.fval>100.0)
						// return vsbe_valueoutofrange;
						avPtr->priority_array.f[vp->priority - 1U] = vp->pd.fval;
					}
					else if ( vp->pdtype == adtNull )
					{
						avPtr->priority_array.d[vp->priority - 1U] = nullvalue;
					}
					else
					{
						return ( vsbe_invaliddatatype );
					}
					// Note: At this point, you must search the priority array to determine if the present_value
					// needs to be updated because:
					// 1. This priority is higher than (1=highest, 16=lowest) one in effect
					// 2. This was the highest and is now a NULL and needs to be replaced with the next
					// highest or the relinquish_default
					// checkpriorityarray :
					n = -1;											// indicate not found
					for ( i = 0; i < 16; i++ )
					{
						if ( avPtr->priority_array.d[i] != nullvalue )
						{
							n = i;
							break;
						}
					}
					if ( n == -1 )
					{
						presentValueAV = avPtr->reli_default;
						retCode = setPresentValueAV( objid, presentValueAV );
					}
					else
					{
						presentValueAV = avPtr->priority_array.f[n];
						retCode = setPresentValueAV( objid, presentValueAV );
					}
				}
				return ( retCode );

				break;

			case RELINQUISH_DEFAULT:					// relinquish_default			***005 Begin
			case OUT_OF_SERVICE:
			case UNITSX:
			case OBJECT_NAME:
			case DESCRIPTION:
			case OBJECT_IDENTIFIER:
			case OBJECT_TYPE:
			case EVENT_STATE:
			case STATUS_FLAGS:
			case PRIORITY_ARRAY:
				return ( vsbe_writeaccessdenied );

			default:
				return ( vsbe_unknownproperty );
		}
	}
	else if ( objType == BINARY_VALUE_OBJTYPE )	////////////////////////////////handle write BV Object
												/// properties////////////////////////
	{
		BACnetBV* bvPtr = ( BACnetBV* )bvObject.objects[( uint16_t )objInst];
		switch ( propid )
		{
			case PRESENT_VALUE:
				if ( m_bacnet_target_info->class_list->BACnet_BV_to_DCID_map[( uint16_t )objInst]
					 .accessType != IS_COMMENDABLE )// if present value is not commendable
				{
					if ( vp->pdtype != adtEnumerated )
					{
						return ( vsbe_invaliddatatype );
					}
					if ( !( ( vp->pd.uval == 0U ) || ( vp->pd.uval == 1U ) ) )
					{
						return ( vsbe_valueoutofrange );
					}
					presentValueBV = vp->pd.uval;
					retCode = setPresentValueBV( objid, presentValueBV );
				}
				else
				{
					if ( ( vp->priority > 16U ) || ( vp->priority == 6U ) )
					{
						return ( vsbe_inconsistentparameters );	// vsbe_writeaccessdenied; 	//this will eventually be
																// changed by an addendum
					}
					else if ( vp->priority == 0U )
					{
						vp->priority = 16U;							// use lowest priority
					}
					if ( vp->pdtype == adtEnumerated )
					{	// in our example let's limit this to 0.0 to 100.0
						if ( vp->pd.uval > 1U )
						{
							return ( vsbe_valueoutofrange );
						}
						bvPtr->priority_array[vp->priority - 1U] = vp->pd.uval;
					}
					else if ( vp->pdtype == adtNull )
					{
						bvPtr->priority_array[vp->priority - 1U] = nullvalue;
					}
					else
					{
						return ( vsbe_invaliddatatype );
					}
					// Note: At this point, you must search the priority array to determine if the present_value
					// needs to be updated because:
					// 1. This priority is higher than (1=highest, 16=lowest) one in effect
					// 2. This was the highest and is now a NULL and needs to be replaced with the next
					// highest or the relinquish_default
					// checkpriorityarraybv :
					n = -1;											// indicate not found
					for ( i = 0; i < 16; i++ )
					{
						if ( bvPtr->priority_array[i] != nullvalue )
						{
							n = i;
							break;
						}
					}
					if ( n == -1 )
					{
						presentValueBV = bvPtr->reli_default;
						retCode = setPresentValueBV( objid, presentValueBV );
					}
					else
					{
						if ( 255U == bvPtr->priority_array[n] )
						{
							presentValueBV = 0U;
						}
						else
						{
							presentValueBV = bvPtr->priority_array[n];
						}
						retCode = setPresentValueBV( objid, presentValueBV );
					}
				}
				return ( retCode );

				break;

			case RELINQUISH_DEFAULT:
			case OUT_OF_SERVICE:
			case OBJECT_NAME:
			case DESCRIPTION:
			case OBJECT_IDENTIFIER:
			case OBJECT_TYPE:
			case EVENT_STATE:
			case STATUS_FLAGS:
			case ACTIVE_TEXT:
			case INACTIVE_TEXT:
			case PRIORITY_ARRAY:
				return ( vsbe_writeaccessdenied );

			default:
				return ( vsbe_unknownproperty );
		}
	}
	else if ( objType == ANALOG_INPUT_OBJTYPE )			// handle AI objects
	{
		BACnetAI* aiPtr = ( BACnetAI* )aiObject.objects[( uint16_t )objInst];
		switch ( propid )
		{
			case PRESENT_VALUE:
			case RELINQUISH_DEFAULT:
			case OUT_OF_SERVICE:
			case UNITSX:
			case OBJECT_NAME:
			case DESCRIPTION:
			case OBJECT_IDENTIFIER:
			case OBJECT_TYPE:
			case EVENT_STATE:
			case STATUS_FLAGS:
			case PRIORITY_ARRAY:
				return ( vsbe_writeaccessdenied );

			case COV_INCREMENT:	// Nishchal D, 18May2016 : Added COV_INCREMENT Write into the Analog Input object.
				if ( vp->pdtype != adtReal )
				{
					return ( vsbe_invaliddatatype );
				}
				aiPtr->covincrement = vp->pd.fval;
				return ( 0 );

			default:
				return ( vsbe_unknownproperty );
		}
	}
	// Proper return writeaccessdenied added for MSV write requests to supported properties.
	else if ( objType == MULTI_STATE_VALUE_OBJTYPE )
	{
		// BACnetMSV *msvPtr = (BACnetMSV*)MSVObject.objects[(uint16_t)objInst];
		switch ( propid )
		{
			case PRESENT_VALUE:
			case RELINQUISH_DEFAULT:
			case OUT_OF_SERVICE:
			case OBJECT_NAME:
			case DESCRIPTION:
			case OBJECT_IDENTIFIER:
			case OBJECT_TYPE:
			case EVENT_STATE:
			case STATUS_FLAGS:
			case STATE_TEXT:
			case NUMBER_OF_STATES:
			case PRIORITY_ARRAY:
				return ( vsbe_writeaccessdenied );

			default:
				return ( vsbe_unknownproperty );
		}
	}
	else
	{
		return ( vsbe_unknownobject );
	}
}

/** @fn dword  fraGetNumberOfObjects()
 *   @brief : get total number of objects
 *   @param[]  None
 *   @return: Total number of Objects
 */

dword fraGetNumberOfObjects()
{
	return ( TOTAL_AV + TOTAL_BV + TOTAL_AI + TOTAL_MSV + 1U );	// 1 for Device Object
}

/** @fn dword fraGetObjectAt(dword idx)
 *   @brief :Find out the objectid in our "database" at a specific index
 *   @param[in] idx					the index of the object to return 1..(1+TOTAL_AV+numBVs)
 *   @return:d Object ID
 */

dword fraGetObjectAt( dword idx )
{
	dword d = ( ( dword )DEVICE_OBJTYPE << 22U ) + ourinstance;	// make a Device objectid

	if ( --idx == 0U )
	{
		return ( d );						// 1 			our device object
	}
	else
	{
		--idx;
	}
	if ( idx < ( TOTAL_AV ) )							// 2..1+TOTAL_AV	AV1..TOTAL_AV
	{
		d = ( ( dword )ANALOG_VALUE_OBJTYPE << 22U ) + idx;
	}
	else if ( idx < ( TOTAL_AV + TOTAL_AI ) )
	{
		d = ( ( dword )ANALOG_INPUT_OBJTYPE << 22U ) + ( idx - ( TOTAL_AV ) );
	}
	// Prateek: Fixed DH1-48 to return correct object list.
	else if ( idx < ( TOTAL_AV + TOTAL_AI + TOTAL_BV ) )
	{
		d = ( ( dword )BINARY_VALUE_OBJTYPE << 22U ) + ( idx - ( TOTAL_AV + TOTAL_AI ) );
	}
	else
	{
		d = ( ( dword )MULTI_STATE_VALUE_OBJTYPE << 22U )
			+ ( idx - ( TOTAL_AV + TOTAL_AI + TOTAL_BV ) );
	}
	return ( d );
}

/** @fn     void   fraWhoHas(word snet, bool_t byname, dword objid, frString *oname)
 *   @brief : Process WhoHas request
 *   @details : process whohas request and transmit the I-have response if Object is found
 *   @param[in] byname	TRUE if oname should match, else objid should match
 *   @param[in] objid	the object identifier to match if byname==FALSE
 *   @param[in] oname	the object name if byname==TRUE
 *   @return:None
 */

void fraWhoHas( word snet, bool_t byname, dword objid, frString* oname )
{
	dword oid = objid & 0x3FFFFF;
	uint32_t i;
	word objtype = ( objid >> 22U ) & 0x3FFU;

	if ( byname == FALSE )							// search by objid
	{
		if ( ( objtype == ANALOG_VALUE_OBJTYPE ) && ( oid < TOTAL_AV ) )
		{
			// frTransmitIHave(snet,objid, &AVnames[oid]);
			frString s_oid = m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[oid].name;	// GSB on 23Mar16
			frTransmitIHave( snet, objid, &s_oid );	// GSB on 13Nov14
		}
		else if ( ( objtype == BINARY_VALUE_OBJTYPE ) && ( oid < TOTAL_BV ) )
		{
			// frTransmitIHave(snet, objid, &BVnames[oid]);
			frString s_oid = m_bacnet_target_info->class_list->BACnet_BV_to_DCID_map[oid].name;	// GSB on 23Mar16
			frTransmitIHave( snet, objid, &s_oid );	// GSB on 13Nov14
		}
		else if ( ( objtype == ANALOG_INPUT_OBJTYPE ) && ( oid < TOTAL_AI ) )
		{
			frString s_oid = m_bacnet_target_info->class_list->BACnet_AI_to_DCID_map[oid].name;
			frTransmitIHave( snet, objid, &s_oid );
		}
		// Nishchal D: Added who has for MSV
		else if ( ( objtype == MULTI_STATE_VALUE_OBJTYPE ) && ( oid < TOTAL_MSV ) )
		{
			frString s_oid = m_bacnet_target_info->class_list->BACnet_MSV_to_DCID_map[oid].name;
			frTransmitIHave( snet, objid, &s_oid );
		}
		return;									// nothing matching
	}
	else										// otherwise search by name
	{
		for ( i = 0U; i < ( TOTAL_AV ); i++ )
		{	// if names match
			if ( ( oname->len == m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[i].name.len ) &&
				 ( _strnicmp( oname->text,
							  m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[i].name.text,
							  oname->len ) == 0 ) )
			{
				frTransmitIHave( snet, ( ANALOG_VALUE_OBJTYPE << 22U ) | i, oname );
				return;
			}
		}
		for ( i = 0U; i < ( TOTAL_BV ); i++ )
		{
			if ( ( ( oname->len == m_bacnet_target_info->class_list->BACnet_BV_to_DCID_map[i].name.len )
				   &&	// if names match
				   ( _strnicmp( oname->text,
								m_bacnet_target_info->class_list->BACnet_BV_to_DCID_map[i].name.text,
								oname->len ) == 0 ) ) )
			{
				frTransmitIHave( snet, ( BINARY_VALUE_OBJTYPE << 22U ) | i, oname );
				return;
			}
		}
		for ( i = 0U; i < ( TOTAL_AI ); i++ )
		{
			if ( ( oname->len == m_bacnet_target_info->class_list->BACnet_AI_to_DCID_map[i].name.len )
				 &&		// if names match
				 ( _strnicmp( oname->text,
							  m_bacnet_target_info->class_list->BACnet_AI_to_DCID_map[i].name.text,
							  oname->len ) == 0 ) )
			{
				frTransmitIHave( snet, ( ANALOG_INPUT_OBJTYPE << 22U ) | i, oname );
				return;
			}
		}
		// Nishchal D: Added who has for MSV
		for ( i = 0U; i < ( TOTAL_MSV ); i++ )
		{
			if ( ( oname->len ==
				   m_bacnet_target_info->class_list->BACnet_MSV_to_DCID_map[i].name.len ) &&	// if names match
				 ( _strnicmp( oname->text,
							  m_bacnet_target_info->class_list->BACnet_MSV_to_DCID_map[i].name.text,
							  oname->len ) == 0 ) )
			{
				frTransmitIHave( snet, ( MULTI_STATE_VALUE_OBJTYPE << 22U ) | i, oname );
				return;
			}
		}
	}
}

/** @fn int32_t createBACnetObjects()
 *   @brief Creates the BACnet objects
 *   @details Allocate dynamic memory and returns  TRUE if the object is created otherwise returns FALSE
 *   @param[in] None
 *   @return TRUE or FALSE
 *   @retval TRUE If the requested object is created
 *   @retval FALSE If the requested object is not created
 */

int32_t createBACnetObjects( const BACNET_TARGET_INFO_ST_TD* bacnet_target_info,
							 callback_t msv_user_callback )
{
	m_msv_user_callback = msv_user_callback;
	int32_t retVal = 0;

	m_bacnet_target_info = bacnet_target_info;
	retVal = createAvObjects( m_bacnet_target_info, TOTAL_AV );
	retVal = createBvObjects( m_bacnet_target_info, TOTAL_BV );
	retVal = createAIObjects( m_bacnet_target_info, TOTAL_AI );
	retVal = createMSVObjects( m_bacnet_target_info, TOTAL_MSV );
	return ( retVal );
}

/** @fn int32_t createBACnetCOVList()
 *   @brief Creates the BACnet COV object instance list
 *   @details Allocate dynamic memory and returns  TRUE if the object is created otherwise returns FALSE
 *   @param[in] None
 *   @return TRUE or FALSE
 *   @retval TRUE If the requested object is created
 *   @retval FALSE If the requested object is not created
 */

int32_t createBACnetCOVList()
{
	int32_t retVal = 0;

	retVal = createCOVList( MAXCOVSUBS );
	return ( retVal );
}

/** @fn bool_t isObjectPresent(uint32_t objid)
 *   @brief give the status of object presense
 *   @details Returns the TRUE if the object is present otherwise returns FALSE
 *   @param[in] objid object identifier
 *   @return TRUE or FALSE
 *   @retval TRUE If the requested object is present
 *   @retval FALSE If the requested object is not present
 */

bool_t isObjectPresent( uint32_t objid )
{
	bool_t ret = FALSE;
	uint32_t objType;
	uint32_t instance;

	objType = ( objid >> SHIFT_GET_OBJECT_ID );
	instance = objid & OBJ_INSTANCE_MASK;
	switch ( objType )
	{
		case ANALOG_VALUE_OBJTYPE:
			if ( instance < avObject.numObjects )
			{
				ret = TRUE;
			}
			break;

		case BINARY_VALUE_OBJTYPE:
			if ( instance < bvObject.numObjects )
			{
				ret = TRUE;
			}
			break;

		case ANALOG_INPUT_OBJTYPE:
			if ( instance < aiObject.numObjects )
			{
				ret = TRUE;
			}
			break;

		case MULTI_STATE_VALUE_OBJTYPE:
			if ( instance < MSVObject.numObjects )
			{
				ret = TRUE;
			}
			break;
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t fraCanDoAlarms( void )
{
	return ( FALSE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t fraCanDoCOV( void )
{
	return ( TRUE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t fraCanDoFiles( void )
{
	return ( FALSE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t fraCanDoPvtTransfer( bool_t cnf )		// ***261 Begin
{
	return ( FALSE );
}												// ***261 End

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t fraCanDoReadRange( void )			// ***400 Begin
{
	return ( FALSE );
}												// ***400 End

// bool_t  fraCanDoRWPM(void)				//	***418 Begin
// {
// return TRUE;
// }

bool_t fraCanDoRPM( void )
{
	return ( TRUE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t fraCanDoWPM( void )
{
	return ( TRUE );
}												// ***418 End

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t fraCanDoStructuredView( void )		// ***600 Begin
{
	return ( FALSE );
}												// ***600 End

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t fraCanDoTimeSync( void )				// ***261 Begin
{
	return ( FALSE );
}												// ***261 End

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t fraCanDoUTCTimeSync( void )			// ***404 Begin
{
	return ( FALSE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t fraCanDoAlarmSummary( void )
{
	return ( FALSE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t fraCanDoAlarmAck( void )
{
	return ( FALSE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t fraCanDoEventInfo( void )
{
	return ( FALSE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t fraCanDoReinit( void )
{
	return ( TRUE );
}												// ***404 End

/** @fn fraResponse(frVbag *vb)
 *   @brief : Response Handler for Confirmed requests
 *   @details : This function is Response Handler for Confirmed requests
 *   @param[in]:vb      points to an frVbag that indicates the response to the request
 *   @return:
 */

void fraResponse( frVbag* vb )
{
	return;											// we manage completions in TimerEvent
}

/** @fn fraReadRange(dword objid, dword propid, dword aindex, frVbag *vp, frRange *range, byte *rflags)
 *   @brief : readRange a property of an object.
 *   @details : This function is to readRange a property of an object
 *   @param[in]:objid   the object identifier
 *   @param[in]:propid  property identifier
 *   @param[in]:aindex  the array index, or the special value noindex
 *   @param[in]:vp      points to a value bag. i.e. an frVbag to receive the property value
 *   @param[in]:range   pointer to an frRange struct
 *   @param[in]:rflags  pointer of the result flags
 *   @return:0 accepted, vp filled-in. !0 rejected and this is the reason
 */

int32_t fraReadRange( dword objid, dword propid, dword aindex, frVbag* vp, frRange* range,
					  byte* rflags )
{
	return ( vsbe_unknownobject );
}																		// ***400 End

/** @fn Track_COV_registration()
 *   @brief : To Track COV registrations for expire time.
 *   @details : This function is called cyclically at 1 sec to un-subscribe instance if
 *              cov time remaining is zero.
 *   @param[in]:
 *   @return:
 */

void Track_COV_registration( void )
{
	uint32_t i;
	uint8_t bitNum;

	for ( i = 0U; i < covList.numObjects; i++ )
	{
		COV* covPtr = ( COV* )covList.objects[i];
		if ( ( covPtr->covtimeremaining != 0U ) && ( covPtr->covlifetime != 0U ) )
		{
			covPtr->covtimeremaining--;
			if ( covPtr->covtimeremaining == 0U )
			{
				switch ( covPtr->objType )
				{
					case ANALOG_VALUE_OBJTYPE:
						covChangeTracker->UnTrack_ID(
							getDCIDForAVObject( covPtr->objid & OBJ_INSTANCE_MASK ) );
						break;

					case ANALOG_INPUT_OBJTYPE:
						covChangeTracker->UnTrack_ID(
							getDCIDForAIObject( covPtr->objid & OBJ_INSTANCE_MASK ) );
						break;

					case BINARY_VALUE_OBJTYPE:
						covChangeTracker->UnTrack_ID(
							getDCIDForBVObject( ( covPtr->objid & OBJ_INSTANCE_MASK ), bitNum ) );
						break;

					case MULTI_STATE_VALUE_OBJTYPE:
						covChangeTracker->UnTrack_ID(
							getDCIDForMSVObject( covPtr->objid & OBJ_INSTANCE_MASK ) );
						break;

					default:
						break;
				}
				covPtr->covsubscribe = 0U;
				covPtr->covlifetime = 0U;
				covPtr->objid = 0U;
				covPtr->covtimeremaining = 0U;
			}
		}
	}
}

/** @fn DoSolicitedCOV(DCI_ID_TD dci_id)
 *   @brief : To transmit COV notification when value of subcribed instance changes.
 *   @details : This function transmits the COV notification when value of subscribed instance changes
 *              depending on COV_increment values for AV and AI objects. Any change in value for BV is
 *              transmitted.
 *   @param[in]: dci_id
 *   @return:
 */

void DoSolicitedCOV( DCI_ID_TD dci_id )
{
	frSubscribeCOV scov =
	{ 0 };						// ***210
	uint32_t i;
	float32_t temp;
	uint8_t bitnum;
	BACnetAV* avPtr;
	BACnetAI* aiPtr;
	BACnetBV* bvPtr;
	BACnetMSV* msvPtr;
	uint8_t instance;

	for ( i = 0U; i < covList.numObjects; i++ )
	{
		bool_t transmit = FALSE;
		COV* covPtr = ( COV* )covList.objects[i];
		instance = covPtr->objid & OBJ_INSTANCE_MASK;

		if ( covPtr->covsubscribe && ( ( covPtr->covlifetime && covPtr->covtimeremaining ) ||
									   ( ( covPtr->covlifetime == 0U ) && ( covPtr->covtimeremaining == 0U ) ) ) )
		{
			switch ( covPtr->objType )
			{
				default:
					break;

				case ANALOG_VALUE_OBJTYPE:
					if ( getDCIDForAVObject( instance ) == dci_id )
					{
						avPtr = ( BACnetAV* )avObject.objects[( uint16_t ) ( covPtr->objid
																			 & OBJ_INSTANCE_MASK )];
						getPresentValueAV( avPtr->objid, avPtr->present_value );
						temp = fabs( avPtr->present_value - covPtr->old_present_value.udwData );
						if ( temp >= avPtr->covincrement )
						{
							transmit = TRUE;
							scov.propval[0U].value.pd.fval = avPtr->present_value;	// the value
							scov.propval[0U].value.pdtype = adtReal;
						}
						covPtr->old_present_value.udwData = avPtr->present_value;
					}
					break;

				case ANALOG_INPUT_OBJTYPE:
					if ( getDCIDForAIObject( instance ) == dci_id )
					{
						aiPtr = ( BACnetAI* )aiObject.objects[( uint16_t ) ( covPtr->objid
																			 & OBJ_INSTANCE_MASK )];
						getPresentValueAI( aiPtr->objid, aiPtr->present_value );
						temp = fabs( aiPtr->present_value - covPtr->old_present_value.udwData );
						if ( temp >= aiPtr->covincrement )
						{
							transmit = TRUE;
							scov.propval[0U].value.pd.fval = aiPtr->present_value;	// the value
							covPtr->old_present_value.udwData = aiPtr->present_value;
							scov.propval[0U].value.pdtype = adtReal;
						}
					}
					break;

				case BINARY_VALUE_OBJTYPE:
					if ( getDCIDForBVObject( instance, bitnum ) == dci_id )
					{
						bvPtr = ( BACnetBV* )bvObject.objects[( uint16_t ) ( covPtr->objid
																			 & OBJ_INSTANCE_MASK )];
						bvPtr->present_value = getPresentValueBV( bvPtr->objid );
						if ( bvPtr->present_value != covPtr->old_present_value.ubData[0U] )
						{
							transmit = TRUE;
							scov.propval[0U].value.pd.uval = bvPtr->present_value;	// Fixed DH1-191
							covPtr->old_present_value.ubData[0U] = bvPtr->present_value;
							scov.propval[0U].value.pdtype = adtEnumerated;	// Fixed DH1-191
						}
					}
					break;

				case MULTI_STATE_VALUE_OBJTYPE:
					if ( getDCIDForMSVObject( instance ) == dci_id )
					{
						msvPtr = ( BACnetMSV* )MSVObject.objects[( uint16_t ) ( covPtr->objid
																				& OBJ_INSTANCE_MASK )];
						getPresentValueMSV( msvPtr->objid, msvPtr->present_value );
						if ( msvPtr->present_value != covPtr->old_present_value.ubData[0U] )
						{
							transmit = TRUE;
							scov.propval[0U].value.pd.uval = msvPtr->present_value;	// Fixed DH1-186
							covPtr->old_present_value.ubData[0U] = msvPtr->present_value;
							scov.propval[0U].value.pdtype = adtUnsigned;// Fixed DH1-186
						}
					}
					break;
			}
			scov.deviceid = ourinstance;
			scov.src = covPtr->src;
			scov.processid = covPtr->covprocessid;
			scov.objid = covPtr->objid;
			scov.time = covPtr->covtimeremaining;
			// scov.confirmed = ( covPtr->covsubscribe == confirmedCOVNotification ); //confirmedCOVNotification = 1
			if ( covPtr->covsubscribe == confirmedCOVNotification )	// confirmedCOVNotification = 1
			{
				scov.confirmed = TRUE;
			}
			else
			{
				scov.confirmed = FALSE;
			}
			scov.propval[0U].propid = PRESENT_VALUE;				// present value
			scov.propval[0U].arrayindex = noindex;					// the array index
			scov.propval[1U].propid = STATUS_FLAGS;					// status
			scov.propval[1U].arrayindex = noindex;					// the array index
			scov.propval[1U].value.pdtype = adtBitString;
			scov.propval[1U].value.len = 4U;						// bitstring length  ***602
			scov.propval[1U].value.pd.psval[0U] = 0U;				// the value
			scov.numberpvs = 2U;
			if ( transmit )
			{
				frcCOVNotify( &scov, 0U );
			}
		}
	}
}

/** @fn findInstanceIfAlreadySubscribed(dword objid, frSource src, uint8_t &subscribedPos)
 *   @brief : To find if the instance is already subscribed.
 *   @details : This function finds if the instance is already subscribed.
 *   @param[in]: Object identifier
 *   @param[in]: Source
 *   @param[in]: Subscribed position.
 *   @return:0 if not subscribed. 1 if subscribed.
 */

bool_t findIfInstanceAlreadySubscribed( dword objid, frSource src, dword processid,
										uint8_t& subscribedPos )
{
	uint32_t i;

	for ( i = 0U; i < covList.numObjects; i++ )
	{
		COV* covPtr = ( COV* )covList.objects[i];
		if ( covPtr->covsubscribe && ( covPtr->objid == objid ) &&
			 ( covPtr->covprocessid == processid ) )
		{
			if ( strcmp( ( reinterpret_cast<const char*>( covPtr->src.sadr ) ),
						 ( reinterpret_cast<const char*>( src.sadr ) ) ) == 0 )
			{
				break;
			}
		}
	}
	subscribedPos = i;
	if ( i >= covList.numObjects )
	{
		return ( FALSE );
	}
	else
	{
		return ( TRUE );
	}
}

/** @fn findAPlaceForInstanceToSubscribe(dword objid)
 *   @brief : To find place in the active cov list for subscription
 *   @details : This function return the index where new instance can be subscribed for COV functionality.
 *   @param[in]: Object identifier
 *   @return:index which can be allocated in COV list.
 */

uint32_t findAPlaceForInstanceToSubscribe( dword objid )
{
	uint32_t i;

	for ( i = 0U; i < covList.numObjects; i++ )
	{
		COV* covPtr = ( COV* )covList.objects[i];
		if ( covPtr->covsubscribe == 0U )
		{
			break;
		}
	}
	return ( i );
}

/** @fn fraSubscribeCOV(_frSubscribeCOV)
 *   @brief : To Count COV subscriptions
 *   @details : This routine is called to subscribe COV functionality.S
 *   @param[in]: cp pointer to a frSubscribeCOV struct
 *   @return:0=success, else error
 */

int32_t fraSubscribeCOV( _frSubscribeCOV* cp )
{
	uint32_t objType;
	BACnetAV* avPtr = nullptr;
	BACnetAI* aiPtr = nullptr;
	BACnetBV* bvPtr = nullptr;
	BACnetMSV* msvPtr = nullptr;
	float32_t valAV = 0.0F;
	float32_t valAI = 0.0F;
	uint8_t valBV;
	uint8_t valMSV = 0U;
	dword instance;
	uint32_t i;
	uint8_t subscribedPos;
	uint8_t bitNum;

	objType = ( cp->objid >> SHIFT_GET_OBJECT_ID );
	instance = cp->objid & OBJ_INSTANCE_MASK;
	int32_t error = 0;

	covChangeTracker = CovChangeTrackerPt();

	switch ( objType )
	{
		case ANALOG_VALUE_OBJTYPE:
			if ( instance < TOTAL_AV )
			{
				avPtr = ( BACnetAV* )avObject.objects[( uint16_t ) ( instance )];
			}
			else
			{
				error = vsbe_unknownobject;
				return ( error );
			}
			break;

		case ANALOG_INPUT_OBJTYPE:
			if ( instance < TOTAL_AI )
			{
				aiPtr = ( BACnetAI* )aiObject.objects[( uint16_t ) ( instance )];
			}
			else
			{
				error = vsbe_unknownobject;
				return ( error );
			}
			break;

		case BINARY_VALUE_OBJTYPE:
			if ( instance < TOTAL_BV )
			{
				bvPtr = ( BACnetBV* )bvObject.objects[( uint16_t ) ( instance )];
			}
			else
			{
				error = vsbe_unknownobject;
				return ( error );
			}
			break;

		case MULTI_STATE_VALUE_OBJTYPE:
			if ( instance < TOTAL_MSV )
			{
				msvPtr = ( BACnetMSV* )MSVObject.objects[( uint16_t ) ( instance )];
			}
			else
			{
				error = vsbe_unknownobject;
				return ( error );
			}
			break;

		default:
			error = vsbe_servicerequestdenied;	// As we do not support COV other than AI, AV, BV and MV.
			return ( error );

			break;
	}

	// Check for available subscribe list
	if ( !findIfInstanceAlreadySubscribed( cp->objid, cp->src, cp->processid, subscribedPos ) )
	{
		i = findAPlaceForInstanceToSubscribe( cp->objid );
	}
	else
	{
		i = subscribedPos;
	}
	// If all 10 subscribe list is completed
	if ( i >= covList.numObjects )
	{
		// Cancellations that are issued for which no matching COV context can be found shall succeed as if a context
		// had
		// existed, returning 'Result(+)'.
		if ( cp->cancel == TRUE )
		{
			return ( error );
		}
		else
		{
			error = vsbe_nospacetoaddlistelement;	// Fixed DH1-204
			return ( error );
		}
	}
	else
	{
		COV* covPtr = ( COV* )covList.objects[i];
		if ( cp->cancel == TRUE )	// Unsubscribe COV
		{
			switch ( covPtr->objType )
			{
				case ANALOG_VALUE_OBJTYPE:
					covChangeTracker->UnTrack_ID(
						getDCIDForAVObject( covPtr->objid & OBJ_INSTANCE_MASK ) );
					break;

				case ANALOG_INPUT_OBJTYPE:
					covChangeTracker->UnTrack_ID(
						getDCIDForAIObject( covPtr->objid & OBJ_INSTANCE_MASK ) );
					break;

				case BINARY_VALUE_OBJTYPE:
					covChangeTracker->UnTrack_ID(
						getDCIDForBVObject( ( covPtr->objid & OBJ_INSTANCE_MASK ), bitNum ) );
					break;

				case MULTI_STATE_VALUE_OBJTYPE:
					covChangeTracker->UnTrack_ID(
						getDCIDForMSVObject( ( covPtr->objid & OBJ_INSTANCE_MASK ) ) );
					break;

				default:
					break;
			}
			covPtr->covsubscribe = 0U;
			covPtr->covlifetime = 0U;
			covPtr->covtimeremaining = 0U;
			cp->numberpvs = 0U;											// don't send a notification
			covPtr->objid = 0U;
		}
		else	// Subscribe COV
		{
			covPtr->covsubscribe =
				( cp->confirmed ) ? confirmedCOVNotification : unconfirmedCOVNotification;	// confirmedCOVNotification
																							// = 1
			covPtr->covprocessid = cp->processid;
			covPtr->covlifetime = cp->time;
			covPtr->covtimeremaining = covPtr->covlifetime;
			covPtr->src = cp->src;
			covPtr->objid = cp->objid;
			covPtr->objType = objType;
			cp->numberpvs = 2U;										// setup the initial notification
			cp->propval[0U].propid = PRESENT_VALUE;
			cp->propval[0U].arrayindex = noindex;
			switch ( covPtr->objType )
			{
				case ANALOG_VALUE_OBJTYPE:
					avPtr->objid = covPtr->objid;
					cp->propval[0U].value.pdtype = adtReal;
					getPresentValueAV( avPtr->objid, valAV );
					covPtr->old_present_value.udwData = avPtr->present_value = valAV;
					cp->propval[0U].value.pd.fval = avPtr->present_value;
					covChangeTracker->Track_ID( getDCIDForAVObject( covPtr->objid & OBJ_INSTANCE_MASK ) );
					break;

				case ANALOG_INPUT_OBJTYPE:
					aiPtr->objid = covPtr->objid;
					cp->propval[0U].value.pdtype = adtReal;
					getPresentValueAI( aiPtr->objid, valAI );
					covPtr->old_present_value.udwData = aiPtr->present_value = valAI;
					cp->propval[0U].value.pd.fval = aiPtr->present_value;
					covChangeTracker->Track_ID( getDCIDForAIObject( covPtr->objid & OBJ_INSTANCE_MASK ) );
					break;

				case BINARY_VALUE_OBJTYPE:
					bvPtr->objid = covPtr->objid;
					cp->propval[0U].value.pdtype = adtEnumerated;	// Fixed DH1-191
					valBV = getPresentValueBV( bvPtr->objid );
					covPtr->old_present_value.ubData[0U] = bvPtr->present_value = valBV;
					cp->propval[0U].value.pd.uval = bvPtr->present_value;	// Fixed DH1-191
					covChangeTracker->Track_ID( getDCIDForBVObject( ( covPtr->objid & OBJ_INSTANCE_MASK ), bitNum ) );
					break;

				case MULTI_STATE_VALUE_OBJTYPE:
					msvPtr->objid = covPtr->objid;
					cp->propval[0U].value.pdtype = adtUnsigned;	// Fixed DH1-186
					getPresentValueMSV( msvPtr->objid, valMSV );
					covPtr->old_present_value.ubData[0U] = msvPtr->present_value = valMSV;
					cp->propval[0U].value.pd.uval = msvPtr->present_value;	// Fixed DH1-186
					covChangeTracker->Track_ID( getDCIDForMSVObject( covPtr->objid & OBJ_INSTANCE_MASK ) );
					break;

				default:
					break;
			}

			cp->propval[1U].propid = STATUS_FLAGS;	// status flags,   need to check all these in [1U]
			cp->propval[1U].arrayindex = noindex;						// the array index
			cp->propval[1U].value.pdtype = adtBitString;				// datatype
			cp->propval[1U].value.len = 4U;								// bitstring length
			cp->propval[1U].value.pd.psval[0U] = 0U;					// the value
		}
	}
	return ( error );
}

/** @fn fraGetNumberOfCOVSubscriptions()
 *   @brief : To Count COV subscriptions
 *   @details : This routine is used to get the count of currently active cov subscriptions
 *   @param[in]:
 *   @return:number of cov subscriptions
 */

uint32_t fraGetNumberOfCOVSubscriptions()
{
	uint32_t i;
	uint32_t numOfCOVSubscribe = 0U;

	for ( i = 0U; i < covList.numObjects; i++ )
	{
		COV* covPtr = ( COV* )covList.objects[i];
		if ( covPtr->covsubscribe && ( ( covPtr->covlifetime && covPtr->covtimeremaining ) ||
									   ( ( covPtr->covlifetime == 0U ) && ( covPtr->covtimeremaining == 0U ) ) ) )
		{
			numOfCOVSubscribe++;
		}
	}
	return ( numOfCOVSubscribe );
}

/** @fn fraGetCOVSubscriptions(_frVbag)
 *   @brief : To read COV subscriptions
 *   @details : This function is used to read current COV subscriptions.
 *   @param[in] _frVbag: vp pointer to return data. vp points to the vbag to filled in
 *   @return:vsbe_noerror success. error otherwise
 */

int32_t fraGetCOVSubscriptions( _frVbag* vp )
{
	byte idx = 0U;
	uint32_t i = 0U;
	BACnetAV* avPtr = nullptr;
	BACnetAI* aiPtr = nullptr;

	for ( i = 0U; i < covList.numObjects; i++ )
	{
		COV* covPtr = ( COV* )covList.objects[i];
		switch ( covPtr->objType )
		{

			case ANALOG_VALUE_OBJTYPE:
				avPtr = ( BACnetAV* )avObject.objects[( uint16_t ) ( covPtr->objid & OBJ_INSTANCE_MASK )];
				break;

			case ANALOG_INPUT_OBJTYPE:
				aiPtr = ( BACnetAI* )aiObject.objects[( uint16_t ) ( covPtr->objid & OBJ_INSTANCE_MASK )];
				break;

			case BINARY_VALUE_OBJTYPE:
			case MULTI_STATE_VALUE_OBJTYPE:
			default:
				break;
		}

		if ( covPtr->covsubscribe && ( ( covPtr->covlifetime && covPtr->covtimeremaining ) ||
									   ( ( covPtr->covlifetime == 0U ) && ( covPtr->covtimeremaining == 0U ) ) ) )
		{
			vp->ps.cs[idx].recip.type = 1U;
			vp->ps.cs[idx].timeremaining = covPtr->covtimeremaining;
			memcpy( vp->ps.cs[idx].recip.recipient.addr.address, covPtr->src.sadr,
					covPtr->src.slen );
			vp->ps.cs[idx].recip.recipient.addr.noctets = covPtr->src.slen;
			vp->ps.cs[idx].recip.recipient.addr.network = covPtr->src.snet.w;
			vp->ps.cs[idx].procid = covPtr->covprocessid;
			vp->ps.cs[idx].propref.objid = covPtr->objid;
			vp->ps.cs[idx].propref.propid = PRESENT_VALUE;
			vp->ps.cs[idx].propref.arrayindex = 0xFFFFFFFFUL;
			vp->ps.cs[idx].hasincrement = 1U;
			switch ( covPtr->objType )
			{
				case ANALOG_VALUE_OBJTYPE:
					vp->ps.cs[idx].increment = avPtr->covincrement;
					break;

				case ANALOG_INPUT_OBJTYPE:
					vp->ps.cs[idx].increment = aiPtr->covincrement;
					break;

				case BINARY_VALUE_OBJTYPE:
					vp->ps.cs[idx].increment = 0U;
					break;

				case MULTI_STATE_VALUE_OBJTYPE:
					vp->ps.cs[idx].increment = 0U;
					break;

				default:
					break;
			}
			vp->ps.cs[idx].issue = ( covPtr->covsubscribe == 1U );	// As confirmedCOVNotification =1
			idx++;
		}
	}
	return ( vsbe_noerror );
}

/** @fn fraCanDoCOVNotifications(bool_t)
 *   @brief : To check if DH1 can receive notifications
 *   @details : This function is to notify if application can receive COV notification.
 *              POWER XL DH1 don't support to receive COV notifications.
 *   @param[in] :cnf
 *   @return:0 if COV notifications accepted and 1 if COV notifications are not accepted.
 */

bool_t fraCanDoCOVNotifications( bool_t cnf )
{
	if ( cnf )
	{
		return ( TRUE );
	}
	else
	{
		return ( FALSE );
	}
}

/** @fn fraCOV(frCOV*)
 *   @brief :create  COV list of object instances
 *   @details : This function is to notify application if COV is received.
 *              POWERXL DH1 don't support it.
 *   @param[in] frCOV*: COV message pointer
 *   @return:0 if not supported.
 */

int32_t fraCOV( frCOV* )
{
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void fraGetAlarms( uint8_t, uint16_t*, void*, bool_t* )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t fraAcknowledgeAlarm( _frAckAlarm* )
{
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
dword fraGetNumberOfStructuredViewObjects()
{
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
dword fraGetStructuredViewObjectAt( dword )
{
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t fraAtomicWriteFile( uint32_t, uint32_t*, uint8_t, uint8_t* )
{
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t fraAtomicReadFile( dword, dword, word*, byte*, bool_t* )
{
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void fraUnsubscribedCOV( _frCOV* )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void fraPrivateTransfer( _frSource*, uint16_t, uint8_t*, uint16_t )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void fraLog( char_t* )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t fraAtomicWriteFile( dword fid, dword* start, word count, byte* bp )
{
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t fraCanDoCOVProperty( void )
{
	return ( FALSE );
}

// ***1223
bool_t fraCanDoOCD( void )
{
	return ( FALSE );
}

// ***1205
bool_t fraCanDoRestartNotify( void )
{
	return ( FALSE );
}

// ***1200
void fraConfirmedPrivateTransfer( frSource*, word, byte*, word, frVbag* )
{}

// ***1205
int32_t fraCreateObject( dword, byte, frCOprop* )
{
	return ( 0 );
}

// ***1205
int32_t fraDeleteObject( dword )
{
	return ( 0 );
}

// ***1205
byte fraGetNumberofRestartRecipients( void )
{
	return ( 0U );
}

// ***1200
byte fraGetPropertyDatatype( dword, byte )
{
	return ( 0U );
}

// ***1205
int32_t fraGetRestartNotifyRecipients( frVbag* )
{
	return ( 0 );
}

// ***1200
bool_t fraGetLastRestartTime( frTimeDate* )
{
	return ( FALSE );
}

// ***1200
void fraIHave( dword, dword, frString* )
{}

// ***1202
int32_t fraSubscribeCOVProperty( frSubscribeCOVProperty* )
{
	return ( 0 );
}

// ***1223
