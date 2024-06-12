/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 2001-2015 */
/*****************************************************************************/
/*                                                                           */
/* This file is the property of:                                             */
/*                                                                           */
/*                       Triangle MicroWorks, Inc.                           */
/*                      Raleigh, North Carolina USA                          */
/*                      <www.TriangleMicroWorks.com>                         */
/*                          (919) 870-6615                                   */
/*                    <support@trianglemicroworks.com>                       */
/*                                                                           */
/* This Source Code and the associated Documentation contain proprietary     */
/* information of Triangle MicroWorks, Inc. and may not be copied or         */
/* distributed in any form without the written permission of Triangle        */
/* MicroWorks, Inc.  Copies of the source code may be made only for backup   */
/* purposes.                                                                 */
/*                                                                           */
/* Your License agreement may limit the installation of this source code to  */
/* specific products.  Before installing this source code on a new           */
/* application, check your license agreement to ensure it allows use on the  */
/* product in question.  Contact Triangle MicroWorks for information about   */
/* extending the number of products that may use this source code library or */
/* obtaining the newest revision.                                            */
/*                                                                           */
/*****************************************************************************/

/*
 *
 *  This file contains
 *  Declarations for Session and Presentation ASN.1 Tags
 *
 *  This file should not require modification.
 */

#ifndef _PRTAGS_H
#define _PRTAGS_H


#define	BITSTRING	0x03
#define	CLOSE	0x0a
#define	END_OF_CONTENTS	0x00
#define	EXTERNAL	0x28
#define	extra29	0x3d
#define	GeneralizedTime24	0x18
#define	GraphicString25	0x19
#define	IA5String22	0x16
#define	INTEGER	0x02
#define	ISO646String22	0x16
#define	Null	0x05
#define	NumericString18	0x12
#define	OBJECT_DESCRIPTOR	0x07
#define	OBJECT_IDENTIFIER	0x06
#define	OCTETSTRING	0x04
#define	Pres_arbitrary2	0x82
#define	Pres_aru0	0xa0
#define	Pres_called_selector2	0x82
#define	Pres_calling_selector1	0x81
#define	Pres_context_definition4	0xa4
#define	Pres_context_definition_res5	0xa5
#define	Pres_context_identifier_list0	0xa0
#define	Pres_context_name6	0xa6
#define	Pres_context_result7	0x87
#define	Pres_def_abstract_syntax0	0x80
#define	Pres_def_transfer_syntax1	0x81
#define	Pres_dummy_application2	0x62
#define	Pres_event_identifier1	0x80
#define	Pres_fully_encoded_data1	0x61
#define	Pres_info2	0xa2
#define	Pres_mode_selector0	0xa0
#define	Pres_mode_tag0	0x80
#define	Pres_octet_aligned1	0x81
#define	Pres_protocol_version0	0x80
#define	Pres_provider_reason0	0x80
#define	Pres_reason10	0x8a
#define	Pres_reason2	0x82
#define	Pres_requirements8	0x88
#define	Pres_responding_selector3	0x83
#define	Pres_result0	0x80
#define	Pres_session_requirements9	0x89
#define	Pres_simply_encoded_data0	0x60
#define	Pres_single_ASN1_type0	0xa0
#define	Pres_transfer_syntax_name1	0x81
#define	PrintableString19	0x13
#define	sequenceof16	0x30
#define	Sess_AA_PDU_26	0x7a
#define	Sess_AB_PDU_25	0x79
#define	Sess_AC_PDU_14	0x6e
#define	Sess_Activity_identifier41	0x29
#define	Sess_Additional_reference_info12	0x6c
#define	Sess_Called_session_selector52	0x34
#define	Sess_Called_SS_user_reference9	0x69
#define	Sess_Calling_session_selector51	0x33
#define	Sess_Calling_SS_user_reference10	0x6a
#define	Sess_CDO_PDU_15	0x6f
#define	Sess_CN_PDU_13	0x6d
#define	Sess_Common_reference11	0x6b
#define	Sess_Connect_Accept_item5	0x65
#define	Sess_Connection_identifier1	0x61
#define	Sess_Data_overflow60	0x3c
#define	Sess_DN_PDU_10	0x6a
#define	Sess_DT_PDU_1	0x61
#define	Sess_Enclosure_item25	0x79
#define	Sess_Extended_user_data194	0xa2
#define	Sess_FN_PDU_9	0x69
#define	Sess_GT_PDU_1	0x61
#define	Sess_Initial_serial_number23	0x77
#define	Sess_Linking_information33	0x21
#define	Sess_New_activity_identifier41	0x29
#define	Sess_OA_PDU_16	0x70
#define	Sess_Old_activity_identifier41	0x29
#define	Sess_PR_PDU_7	0x67
#define	Sess_Prepare_type24	0x78
#define	Sess_Protocol_options19	0x73
#define	Sess_PT_PDU_2	0x62
#define	Sess_Reason_code50	0x32
#define	Sess_Reflect_parameter49	0x31
#define	Sess_Resync_type27	0x7b
#define	Sess_RF_PDU_12	0x6c
#define	Sess_Serial_number42	0x2a
#define	Sess_Session_user_requirements20	0x74
#define	Sess_Sync_type_item15	0x6f
#define	Sess_Token_item16	0x70
#define	Sess_Token_setting_item26	0x7a
#define	Sess_Transport_disconnect17	0x71
#define	Sess_TSDU_maximum_size21	0x75
#define	Sess_User_data193	0xa1
#define	Sess_User_data_MIA46	0x2e
#define	Sess_Version_number22	0x76
#define	setof17	0x31
#define	TeletexString20	0x14
#define	UTCTime23	0x17
#define	VideotexString21	0x15
#define	VisibleString26	0x1a
#endif      /*  _PRTAGS_H */
