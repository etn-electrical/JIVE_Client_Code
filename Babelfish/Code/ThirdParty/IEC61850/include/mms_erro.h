//*****************************************************************************/
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
 *  Declarations for errors.
 *
 *  This file should not require modification.
 */


#ifndef _MMS_ERRO_H
#define _MMS_ERRO_H

#ifdef __cplusplus
extern "C" {
#endif

/* ASN1 Error Processing Definitions ----------------------------
    --- These codes are set within the descriptors during
    --- processing to avoid overruns, etc.  The users really
    --- don't need to handle them, but the macro definitions
    --- below use them.
    --------------------------------------------------------------
*/

#define MMSd_noHandlerErrors                    0x00000000UL
#define MMSd_emitOverflow                       0x00000001UL
#define MMSd_fetchLengthError                   0x00000002UL
#define MMSd_data_object_invalidated            0x00000004UL
#define MMSd_data_hardware_fault                0x00000008UL
#define MMSd_data_temporarily_unavailable       0x00000010UL
#define MMSd_data_object_access_denied          0x00000020UL
#define MMSd_data_object_undefined              0x00000040UL
#define MMSd_data_invalid_address               0x00000080UL
#define MMSd_data_type_unsupported              0x00000100UL
#define MMSd_data_type_inconsistent             0x00000200UL
#define MMSd_data_object_attribute_inconsistent 0x00000400UL
#define MMSd_data_object_access_unsupported     0x00000800UL
#define MMSd_data_object_non_existent           0x00001000UL
#define MMSd_queueOverrun                       0x00002000UL
#define MMSd_null_descriptor                    0x00004000UL
#define MMSd_IOargs                             0x00008000UL
#define MMSd_neg_svc_error                      0x00010000UL
#define MMSd_data_object_value_invalid          0x00020000UL
#define MMSd_search_error                       0x00040000UL
#define MMSd_pdu_error                          0x00080000UL
#define MMSd_file_other                         0x00100000UL
#define MMSd_file_file_busy                     0x00200000UL
#define MMSd_file_name_syntax_error             0x00400000UL
#define MMSd_file_content_invalid               0x00800000UL
#define MMSd_file_file_access_denied            0x01000000UL
#define MMSd_file_file_non_existent             0x02000000UL
#define MMSd_file_duplicate_filename            0x04000000UL
#define MMSd_file_insufficient_space            0x08000000UL
#define MMSd_file_position_invalid              0x10000000UL
#define MMSd_authentication_failure             0x20000000UL
#define MMSd_authentication_mech_nr             0x40000000UL
#define MMSd_uninitiatedMessage                 0x80000000UL
#define MMSd_anyError                           0xFFFFFFFFUL
#define MMSd_GLOBAL_ERROR     \
  ( MMSd_emitOverflow | MMSd_fetchLengthError | MMSd_queueOverrun \
    | MMSd_null_descriptor | MMSd_IOargs | MMSd_neg_svc_error \
    | MMSd_search_error | MMSd_pdu_error \
    | MMSd_file_other \
    | MMSd_file_file_busy \
    | MMSd_file_name_syntax_error \
    | MMSd_file_content_invalid \
    | MMSd_file_file_access_denied \
    | MMSd_file_file_non_existent \
    | MMSd_file_duplicate_filename \
    | MMSd_file_insufficient_space \
    | MMSd_file_position_invalid \
    | MMSd_authentication_failure \
    | MMSd_authentication_mech_nr )

/* DLS Error Processing Definitions ----------------------------
    --- These codes are set by the various DLS machine functions
    --- to signal error conditions.
    --------------------------------------------------------------
*/

#define MMSd_dls_user_error                     0x00000001UL
#define MMSd_dls_in_use_error                   0x00000002UL
#define MMSd_dls_resource_error                 0x00000004UL
#define MMSd_dls_state_error                    0x00000008UL
#define MMSd_dls_invoke_id                      0x00000010UL
#define MMSd_dls_unavailable                    0x00000020UL
#define MMSd_dls_not_found                      0x00000040UL
#define MMSd_DLS_GLOBAL_ERROR     \
   ( MMSd_dls_user_error | MMSd_dls_in_use_error | MMSd_dls_resource_error \
                         | MMSd_dls_state_error | MMSd_dls_invoke_id  \
                         | MMSd_dls_not_found )

#define MMSd_NULL_RESP_ERROR     \
  ( MMSd_uninitiatedMessage )

/* Reject Reason Codes
   --- The following definitions are used to specify the
   --- code parameter to the Reject Service.  The reason
   --- codes are actually generated from the YACC grammar.
   --- (for example, cnf_request_reason1, below).
   --------------------------------------------------------------
*/

/* --- For use with reason: cnf_request_reason1 */
#define  cnf_req_other 0
#define  cnf_req_unrecognized_service 1
#define  cnf_req_unrecognized_modifier 2
#define  cnf_req_invalid_invokeID 3
#define  cnf_req_invalid_argument 4
#define  cnf_req_invalid_modifier 5
#define  cnf_req_max_serv_outstanding_exceeded 6
#define  cnf_req_max_recursion_exceeded 8
#define  cnf_req_value_out_of_range 9

/* --- For use with reason: cnf_response_reason2 */
#define  cnf_res_other 0
#define  cnf_res_unrecognized_service 1
#define  cnf_res_invalid_invokeID 2
#define  cnf_res_invalid_result 3
#define  cnf_res_max_recursion_exceeded 5
#define  cnf_res_value_out_of_range 6

/* --- For use with reason: cnf_error_reason3 */
#define  cnf_error_other 0
#define  cnf_error_unrecognized_service 1
#define  cnf_error_invalid_invokeID 2
#define  cnf_error_invalid_serviceError 3
#define  cnf_error_value_out_of_range 4

/* --- For use with reason: uncnf_reason4 */
#define  uncnf_other 0
#define  uncnf_unrecognized_service 1
#define  uncnf_invalid_argument 2
#define  uncnf_max_recursion_exceeded 3
#define  uncnf_value_out_of_range 4

/* --- For use with reason: pdu_error_reason5 */
#define  pdu_error_unknown_pdu_type 0
#define  pdu_error_invalid_pdu 1
#define  pdu_error_illegal_acse_mapping 2

/* --- For use with reason: cancel_request_reason6 */
#define  cancel_req_other 0
#define  cancel_req_invalid_invokeID 1

/* --- For use with reason: cancel_response_reason7 */
#define  cancel_res_other 0
#define  cancel_res_invalid_invokeID 1

/* --- For use with reason: cancel_error_reason8 */
#define  cancel_err_other 0
#define  cancel_err_invalid_invokeID 1
#define  cancel_err_invalid_serviceError 2
#define  cancel_err_value_out_of_range 3

/* --- For use with reason: conclude_request_reason9 */
#define  conclude_req_other 0
#define  conclude_req_invalid_argument 1

/* --- For use with reason: conclude_response_reason10 */
#define  conclude_res_other 0
#define  conclude_res_invalid_result 1

/* --- For use with reason: conclude_error_reason11 */
#define  conclude_err_other 0
#define  conclude_err_invalid_serviceError 1
#define  conclude_err_value_out_of_range 2


/* ServiceError Codes
   --- The following definitions are used to specify the
   --- codes to the MMSd_ServiceError function.  The service
   --- error codes themselves (SvcErr_???, below) are generated
   --- from the YACC grammar.
   --------------------------------------------------------------
*/

/* --- For use with SvcErr_vmd_state0 */
#define  vmd_state_other   0
#define  vmd_state_vmd_state_conflict  1
#define  vmd_state_vmd_operational_problem   2
#define  vmd_state_domain_transfer_problem   3
#define  vmd_state_state_machine_id_invalid  4

/* --- For use with SvcErr_application_reference1 */
#define  application_ref_other   0
#define  application_ref_application_unreachable   1
#define  application_ref_connection_lost  2
#define  application_ref_application_reference_invalid   3
#define  application_ref_context_unsupported 4

/* --- For use with SvcErr_definition2 */
#define  definition_other  0
#define  definition_object_undefined   1
#define  definition_invalid_address 2
#define  definition_type_unsupported   3
#define  definition_type_inconsistent  4
#define  definition_object_exists   5
#define  definition_object_attribute_inconsistent  6

/* --- For use with SvcErr_resource3 */
#define  resource_other 0
#define  resource_memory_unavailable   1
#define  resource_processor_resource_unavailable   2
#define  resource_mass_storage_unavailable   3
#define  resource_capability_unavailable  4
#define  resource_capability_unknown   5

/* --- For use with SvcErr_service4 */
#define  service_other  0
#define  service_primitives_out_of_sequence  1
#define  service_object_state_conflict 2
#define  service_pdu_size 3
#define  service_continuation_invalid  4
#define  service_object_constraint_conflict  5

/* --- For use with SvcErr_service_preempt5 */
#define  service_preempt_other   0
#define  service_preempt_timeout 1
#define  service_preempt_deadlock   2
#define  service_preempt_cancel  3

/* --- For use with SvcErr_time_resolution6 */
#define  time_resolution_other 0
#define  time_resolution_unsupportable_time_resolution 1

/* --- For use with SvcErr_access7 */
#define  access_other   0
#define  access_object_access_unsupported 1
#define  access_object_non_existent 2
#define  access_object_access_denied   3
#define  access_object_invalidated  4

/* --- For use with SvcErr_initiate8 */
#define  initiate_other 0
#define  initiate_max_outstanding_calling_insufficient   3
#define  initiate_max_outstanding_called_insufficient 4
#define  initiate_service_CBB_insufficient   5
#define  initiate_parameter_CBB_insufficient 6
#define  initiate_nesting_level_insufficient 7
#define  initiate_version_incompatible 8
#define  initiate_max_segment_insufficient 9
#define  initiate_service_ccb_insufficient 10

/* --- For use with SvcErr_conclude9 */
#define  conclude_other 0
#define  conclude_further_communication_required   1

/* --- For use with SvcErr_cancel10 */
#define  cancel_other   0
#define  cancel_invoke_id_unknown   1
#define  cancel_cancel_not_possible 2

/* --- For use with SvcErr_file11 */
#define  file_other  0
#define  file_filename_ambiguous 1
#define  file_file_busy 2
#define  file_filename_syntax_error 3
#define  file_content_type_invalid  4
#define  file_position_invalid   5
#define  file_file_access_denied 6
#define  file_file_non_existent  7
#define  file_duplicate_filename 8
#define  file_insufficient_space_in_filestore   9


/* MMSd Error Functions ----------------------------------------
   --- These are standardized routines used throughout to both
   --- detect and handle error conditions which have been
   --- marked in the flags of descriptors.
   --------------------------------------------------------------
*/

/* NVL ResultCheck Macros --------------------------------------
--- The following macros are used to finish processing of
--- each entry in a named variable list (NVL).  These must
--- be called for each entry being processed.
------------------------------------------------------------- */

#define MMSd_readResultCheck( E )   MMSd_IOerror( E, E, E )

#define MMSd_writeResultCheck( D, E )                   \
    if ( !MMSd_IOerror(castTKN( D ), castTKN( E ), castTKN( E )) )  \
        if ( !MMSd_IOerror(castTKN( E ), castTKN( E ), castTKN( E )) )\
            MMSd_emitTAG_and_LENGTH( success1, 0, castTKN( E ) )

MMSd_length MMSd_IOerror( MMSd_descriptor *T,
                          MMSd_descriptor *S,
                          MMSd_descriptor *E );

#ifdef __cplusplus
};
#endif

#endif /* _MMS_ERRO_H */
