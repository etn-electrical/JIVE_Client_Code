/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Code_Sign_ECC_Matrix.h"

#include "Pub_Key_ECC_Matrix.h"
#include "Sha256_Matrix.h"
#include "StdLib_MV.h"
#include "PKI_Config.h"
#include "uC_Fw_Code_Range.h"
#include "Cert_Matrix.h"
#include "PKI_Debug.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Code_Sign_ECC_Matrix::Verify_Signature_ECC(
	Cert_Store* cert_store_if, Fw_Store* fw_ctrl, bool* key_upgrade_status )
{

	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;
	PKI_Common::pki_status_t cert_ret_status = PKI_Common::FAILURE;

	/* fvk certificate and its components */
	uint8_t* fvk_cert = nullptr;
	uint16_t fvk_cert_len = 0U;

	/* ivk certificate and its length */
	uint8_t* ivk_cert = nullptr;///< pointer to ivk cert in code sign info
	uint16_t ivk_cert_len = 0U;

	/* cert objects required to get cerificate components  */
	Cert_Matrix* fvk_cert_handle = nullptr;
	Cert_Matrix* ivk_cert_handle = nullptr;

	/* flags */
	bool verify_fw_sign = false;
	bool verify_fvk_sign = false;
	bool verify_ivk_sign = false;
	bool fvk_verified = false;
	bool update_cert_store_pub_keys = false;
	bool skip_compare = false;

	/***************************************************************************************************************
	   Handle Cross signing if Cross_Sign_Status is true
	 ****************************************************************************************************************/
	if ( fw_ctrl->Get_Cross_Sign_Status() == true )
	{
		skip_compare = true;

		if ( Verify_Cross_Sign( cert_store_if, fw_ctrl ) == PKI_Common::SUCCESS )
		{
			verify_ivk_sign = true;
			PKI_DEBUG_PRINT( DBG_MSG_DEBUG,
							 "verification of trusted fw signature is successful" );
		}
	}

	/***************************************************************************************************************
	   Compare key/cert in code_sign_info and cert store. certificate of the key has (key + key
	      signature).
	   e.g. Compare FVK in code_sign_info with FVK in cert store to check if new FVK is available or
	      not
	 ****************************************************************************************************************/

	if ( skip_compare == false )
	{
		if ( Compare_Cert( fw_ctrl, cert_store_if->FVK_CERT,
						   cert_store_if ) == PKI_Common::SUCCESS )
		{
			if ( Compare_Cert( fw_ctrl, cert_store_if->IVK_CERT,
							   cert_store_if ) == PKI_Common::SUCCESS )
			{
				if ( Compare_Cert( fw_ctrl, cert_store_if->RVK_KEY,
								   cert_store_if ) == PKI_Common::SUCCESS )
				{
					verify_fw_sign = true;
				}
				else
				{
					PKI_DEBUG_PRINT( DBG_MSG_ERROR,
									 "FVK and IVK is same, RVK is not matching with existing RVK in cert store" );
				}
			}
			else
			{
				PKI_DEBUG_PRINT( DBG_MSG_ERROR, "FVK is same, IVK is not matching with existing IVK in cert store" );
			}
		}
		else if ( Compare_Cert( fw_ctrl, cert_store_if->IVK_CERT,
								cert_store_if ) == PKI_Common::SUCCESS )
		{
			if ( Compare_Cert( fw_ctrl, cert_store_if->RVK_KEY,
							   cert_store_if ) == PKI_Common::SUCCESS )
			{
				verify_fvk_sign = true;
			}
			else
			{
				PKI_DEBUG_PRINT( DBG_MSG_ERROR, "IVK is same, RVK is not matching with existing RVK in cert store" );
			}
		}
		else if ( Compare_Cert( fw_ctrl, cert_store_if->RVK_KEY,
								cert_store_if ) == PKI_Common::SUCCESS )
		{
			verify_ivk_sign = true;
		}
		else
		{
			PKI_DEBUG_PRINT( DBG_MSG_ERROR,
							 "FVK,IVK, RVK certs are not matching with existing certs in cert store and cross signing status is zero" );
		}
	}
	/***************************************************************************************************************
	    verify IVK signature
	 ****************************************************************************************************************/
	if ( verify_ivk_sign == true )
	{
		PKI_Common::pki_status_t key_parse_ret_status = PKI_Common::FAILURE;
		Pub_Key_Lib* ecc_pub_key = nullptr;

		/* ivk certificate components */
		uint8_t* ivk_cert_hash = nullptr;
		uint16_t ivk_cert_hash_len = 0U;
		uint8_t* ivk_sign = nullptr;
		uint16_t ivk_sign_len = 0U;

		/* rvk key and its length */
		uint8_t* rvk_key = nullptr;	///< pointer to rvk key in code sign info
		uint16_t rvk_key_len = 0U;

		if ( fw_ctrl->Get_IVK_Cert( &ivk_cert, &ivk_cert_len ) == PKI_Common::SUCCESS )
		{
			/* create a handle for ivk cert and parse it to get ivk components */
			ivk_cert_handle = new Cert_Matrix( ivk_cert, ivk_cert_len,
											   &cert_ret_status );

			if ( ( ivk_cert_handle != nullptr ) && ( cert_ret_status == PKI_Common::SUCCESS ) )
			{
				if ( ivk_cert_handle->Get_Cert_Hash( &ivk_cert_hash,
													 &ivk_cert_hash_len ) == PKI_Common::SUCCESS )
				{
					if ( ivk_cert_handle->Get_Signature( &ivk_sign,
														 &ivk_sign_len ) ==
						 PKI_Common::SUCCESS )
					{
						if ( fw_ctrl->Get_RVK_Key( &rvk_key,
												   &rvk_key_len ) == PKI_Common::SUCCESS )
						{
							ecc_pub_key = new Pub_Key_ECC_Matrix( const_cast<uint8_t*>( rvk_key ),
																  rvk_key_len,
																  &key_parse_ret_status );

							if ( fw_ctrl->Free_RVK_Key() != PKI_Common::SUCCESS )
							{
								PKI_DEBUG_PRINT( DBG_MSG_ERROR,
												 "Deallocation of memory while reading RVK key failed" );
							}

							if ( ( ecc_pub_key != nullptr ) &&
								 ( key_parse_ret_status == PKI_Common::SUCCESS ) )
							{
								/* verify fvk sign using ivk */
								if ( PKI_Common::SUCCESS ==
									 ecc_pub_key->Verify_Hash( ivk_sign, ivk_sign_len,
															   ivk_cert_hash,
															   ivk_cert_hash_len ) )
								{
									verify_fvk_sign = true;
								}
							}
							if ( ecc_pub_key != nullptr )
							{
								/*
								 *****************************************************************************************
								 * See header file for function definition.
								 *****************************************************************************************
								 */
								ecc_pub_key->~Pub_Key_Lib();
								delete( ecc_pub_key );
								ecc_pub_key = nullptr;
							}
						}
					}
				}
			}
			if ( ivk_cert_handle != nullptr )
			{
				/*
				 *****************************************************************************************
				 * See header file for function definition.
				 *****************************************************************************************
				 */
				ivk_cert_handle->~Cert_Matrix();
				delete( ivk_cert_handle );
				ivk_cert_handle = nullptr;
			}
		}
		if ( verify_fvk_sign == false )
		{
			PKI_DEBUG_PRINT( DBG_MSG_ERROR, "IVK verification failed" );
		}
	}

	/***************************************************************************************************************
	    verify FVK signature
	 ****************************************************************************************************************/
	if ( verify_fvk_sign == true )
	{
		/* fvk certificate and its components */
		uint8_t* fvk_cert_hash = nullptr;
		uint16_t fvk_cert_hash_len = 0U;
		uint8_t* fvk_sign = nullptr;
		uint16_t fvk_sign_len = 0U;
		uint8_t* fvk_key = nullptr;
		uint16_t fvk_key_len = 0U;
		uint8_t* fvk_cert_key = nullptr;
		uint16_t fvk_cert_key_len = 0U;

		if ( fw_ctrl->Get_FVK_Cert( &fvk_cert, &fvk_cert_len ) == PKI_Common::SUCCESS )
		{
			/* create a handle for fvk cert and parse it to get fvk components */
			fvk_cert_handle = new Cert_Matrix( fvk_cert, fvk_cert_len,
											   &cert_ret_status );

			if ( ( fvk_cert_handle != nullptr ) && ( cert_ret_status == PKI_Common::SUCCESS ) )
			{
				if ( fvk_cert_handle->Get_Cert_Hash( &fvk_cert_hash,
													 &fvk_cert_hash_len ) == PKI_Common::SUCCESS )
				{
					if ( fvk_cert_handle->Get_Signature( &fvk_sign,
														 &fvk_sign_len ) ==
						 PKI_Common::SUCCESS )
					{
						/* create a handle for ivk cert, parse ivk cert to get ivk key and its
						   length in m_cert
						 */
						if ( fw_ctrl->Get_IVK_Cert( &ivk_cert,
													&ivk_cert_len ) == PKI_Common::SUCCESS )
						{
							ivk_cert_handle =
								new Cert_Matrix( ivk_cert, ivk_cert_len,
												 &cert_ret_status );
							if ( ( ivk_cert_handle != nullptr ) &&
								 ( cert_ret_status == PKI_Common::SUCCESS ) )
							{
								/* verify fvk sign using ivk */
								if ( PKI_Common::SUCCESS ==
									 ivk_cert_handle->Verify_Hash( fvk_sign, fvk_sign_len,
																   fvk_cert_hash,
																   fvk_cert_hash_len ) )
								{
									fvk_verified = true;
									PKI_DEBUG_PRINT( DBG_MSG_INFO, "Verification of fvk sign using ivk is successful" );
									if ( fvk_cert_handle->Get_Pub_Key( &fvk_cert_key,
																	   &fvk_cert_key_len ) ==
										 PKI_Common::SUCCESS )
									{
										if ( fw_ctrl->Get_FVK_Key( &fvk_key,
																   &fvk_key_len ) ==
											 PKI_Common::SUCCESS )
										{
											if ( true ==
												 BF_Lib::Compare_Data( fvk_cert_key, fvk_key,
																	   fvk_cert_key_len ) )
											{
												verify_fw_sign = true;
												update_cert_store_pub_keys = true;
											}
											else
											{
												PKI_DEBUG_PRINT( DBG_MSG_ERROR,
																 "FVK key in Fw_Ctrl is not matching with FVK key extracted from FVK cert in Fw_Ctrl" );
											}
										}
									}
								}
							}
							if ( ivk_cert_handle != nullptr )
							{
								/*
								 *****************************************************************************************
								 * See header file for function definition.
								 *****************************************************************************************
								 */
								ivk_cert_handle->~Cert_Matrix();
								delete( ivk_cert_handle );
								ivk_cert_handle = nullptr;
							}
						}
					}
				}
			}
			if ( fvk_cert_handle != nullptr )
			{
				/*
				 *****************************************************************************************
				 * See header file for function definition.
				 *****************************************************************************************
				 */
				fvk_cert_handle->~Cert_Matrix();
				delete( fvk_cert_handle );
				fvk_cert_handle = nullptr;
			}
		}
		if ( verify_fw_sign == false )
		{
			PKI_DEBUG_PRINT( DBG_MSG_ERROR, "FVK verification failed" );
		}
	}

	/***************************************************************************************************************
	    verify firmware signature
	 ****************************************************************************************************************/
	if ( verify_fw_sign == true )
	{
		/*  read fvk certificate if needed and parse it */

		if ( fvk_verified == true )	///< check if fvk is available or not
		{
			fvk_cert_handle = new Cert_Matrix( fvk_cert, fvk_cert_len,
											   &cert_ret_status );
		}
		else///< fvk is not available, read it then parse
		{
			if ( fw_ctrl->Get_FVK_Cert( &fvk_cert, &fvk_cert_len ) == PKI_Common::SUCCESS )
			{

				fvk_cert_handle = new Cert_Matrix( fvk_cert, fvk_cert_len,
												   &cert_ret_status );
			}
		}
		if ( ( fvk_cert_handle != nullptr ) && ( cert_ret_status == PKI_Common::SUCCESS ) )
		{
			/* Verify FW signature using FVK */
			if ( Verify_Fw_Sign( fw_ctrl, fvk_cert_handle ) == PKI_Common::SUCCESS )
			{
				ret = PKI_Common::SUCCESS;
				PKI_DEBUG_PRINT( DBG_MSG_DEBUG,
								 "verification of signature of fw package using FVK successful" );

			}

		}
		if ( fvk_cert_handle != nullptr )
		{
			/*
			 *****************************************************************************************
			 * See header file for function definition.
			 *****************************************************************************************
			 */
			fvk_cert_handle->~Cert_Matrix();
			delete( fvk_cert_handle );
			fvk_cert_handle = nullptr;
		}
		if ( ret != PKI_Common::SUCCESS )
		{
			PKI_DEBUG_PRINT( DBG_MSG_ERROR, "Firmware signature verification failed" );
		}
	}

	if ( ( update_cert_store_pub_keys == true ) && ( ret == PKI_Common::SUCCESS ) )
	{
		*key_upgrade_status = true;
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Code_Sign_ECC_Matrix::Compare_Cert( Fw_Store* fw_ctrl,
															 Cert_Store::cert_store_enum_t key_cert_enum,
															 Cert_Store* cert_store_if )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;
	bool ret_val = false;
	bool free_fvk = false;
	bool free_ivk = false;
	bool free_rvk = false;
	uint8_t* key_ptr;///< pointer to key certificate in cert store which to be
					///< compared with key certificate in code sign info
	uint16_t key_len;
	uint8_t* cmp_key_ptr;///< pointer to key certificate in code sign info;
	uint16_t cmp_key_len;

	if ( key_cert_enum == cert_store_if->FVK_CERT )
	{
		if ( cert_store_if->Get_FVK_Cert( &key_ptr, &key_len ) == PKI_Common::SUCCESS )
		{
			free_fvk = true;
			if ( fw_ctrl->Get_FVK_Cert( &cmp_key_ptr, &cmp_key_len ) == PKI_Common::SUCCESS )
			{
				ret_val = true;
			}
		}
	}
	else if ( key_cert_enum == cert_store_if->IVK_CERT )
	{
		if ( cert_store_if->Get_IVK_Cert( &key_ptr, &key_len ) == PKI_Common::SUCCESS )
		{
			free_ivk = true;
			if ( fw_ctrl->Get_IVK_Cert( &cmp_key_ptr, &cmp_key_len ) == PKI_Common::SUCCESS )
			{
				ret_val = true;
			}
		}
	}
	else if ( key_cert_enum == cert_store_if->RVK_KEY )
	{
		if ( cert_store_if->Get_RVK_Key( &key_ptr, &key_len ) == PKI_Common::SUCCESS )
		{
			free_rvk = true;
			if ( fw_ctrl->Get_RVK_Key( &cmp_key_ptr, &cmp_key_len ) == PKI_Common::SUCCESS )
			{
				ret_val = true;
			}
		}
	}
	else
	{}

	if ( ret_val == true )
	{
		/* compare key cert in code_sign_info with key cert in cert store if key_length of both
		   certs are same */
		if ( key_len == cmp_key_len )
		{
			if ( true == BF_Lib::Compare_Data( cmp_key_ptr, key_ptr, cmp_key_len ) )
			{
				ret = PKI_Common::SUCCESS;
			}
		}

		if ( ret != PKI_Common::SUCCESS )
		{
			if ( free_fvk == true )
			{
				PKI_DEBUG_PRINT( DBG_MSG_INFO, " FVK Certificate Comparison Failed " );
			}
			else if ( free_ivk == true )
			{
				PKI_DEBUG_PRINT( DBG_MSG_INFO, " IVK Certificate Comparison Failed " );
			}
			else if ( free_rvk == true )
			{
				PKI_DEBUG_PRINT( DBG_MSG_INFO, " RVK Key Comparison Failed " );
			}
		}

	}

	if ( free_fvk == true )
	{
		if ( cert_store_if->Free_FVK_Cert() != PKI_Common::SUCCESS )
		{
			PKI_DEBUG_PRINT( DBG_MSG_INFO,
							 "FVK cert memory deallocation failed" );
		}
	}
	if ( free_ivk == true )
	{
		if ( cert_store_if->Free_IVK_Cert() != PKI_Common::SUCCESS )
		{
			PKI_DEBUG_PRINT( DBG_MSG_INFO,
							 "Deallocation of memory while reading IVK cert failed" );
		}
	}
	if ( free_rvk == true )
	{
		if ( cert_store_if->Free_RVK_Key() != PKI_Common::SUCCESS )
		{
			PKI_DEBUG_PRINT( DBG_MSG_INFO,
							 "RVK key memory deallocation failed" );
		}
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Code_Sign_ECC_Matrix::Verify_Fw_Sign( Fw_Store* fw_ctrl,
															   Cert_Matrix* cert_handle )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	Sig_Verify* fw_sig_verify = nullptr;

	fw_sig_verify = new Sig_Verify_ECC_Matrix( fw_ctrl, cert_handle );

	if ( fw_sig_verify != nullptr )
	{
		/* verify firmware Pack  */
		if ( fw_sig_verify->Verify() == PKI_Common::SUCCESS )
		{
			ret = PKI_Common::SUCCESS;
			PKI_DEBUG_PRINT( DBG_MSG_INFO,
							 "verification of FW pack using ECC public key successful" );
		}
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		fw_sig_verify->~Sig_Verify();
		delete( fw_sig_verify );
		fw_sig_verify = nullptr;
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Code_Sign_ECC_Matrix::Verify_Trusted_Fw_Sign( Fw_Store* fw_ctrl,
																	   Cert_Matrix* cert_handle )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	Sig_Verify* fw_sig_verify = nullptr;

	fw_sig_verify = new Sig_Verify_ECC_Matrix( fw_ctrl, cert_handle );

	if ( fw_sig_verify != nullptr )
	{
		/* verify firmware Pack  */
		if ( fw_sig_verify->Verify_Trusted_Fw_Sign() == PKI_Common::SUCCESS )
		{
			ret = PKI_Common::SUCCESS;
			PKI_DEBUG_PRINT( DBG_MSG_INFO,
							 "Trusted Fw signature verified" );
		}
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		fw_sig_verify->~Sig_Verify();
		delete( fw_sig_verify );
		fw_sig_verify = nullptr;
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Code_Sign_ECC_Matrix::Verify_Cross_Sign( Cert_Store* cert_store_if,
																  Fw_Store* fw_ctrl )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;
	PKI_Common::pki_status_t cert_ret_status = PKI_Common::FAILURE;

	/* fvk certificate and its components */
	uint8_t* fvk_cert = nullptr;
	uint16_t fvk_cert_len = 0U;
	uint8_t* fvk_cert_hash = nullptr;
	uint16_t fvk_cert_hash_len = 0U;
	uint8_t* fvk_sign = nullptr;
	uint16_t fvk_sign_len = 0U;

	/* ivk certificate */
	uint8_t* ivk_cert = nullptr;
	uint16_t ivk_cert_len = 0U;

	/* cert objects required to get cerificate components  */
	Cert_Matrix* fvk_cert_handle = nullptr;
	Cert_Matrix* ivk_cert_handle = nullptr;

	if ( fw_ctrl->Validate_Cross_Sign_CRC() == PKI_Common::SUCCESS )
	{
		if ( fw_ctrl->Get_Trusted_FVK_Cert( &fvk_cert,
											&fvk_cert_len ) == PKI_Common::SUCCESS )
		{
			/* create a handle for fvk cert and parse it to get fvk components */
			fvk_cert_handle = new Cert_Matrix( fvk_cert, fvk_cert_len,
											   &cert_ret_status );

			if ( ( fvk_cert_handle != nullptr ) && ( cert_ret_status == PKI_Common::SUCCESS ) )
			{
				if ( fvk_cert_handle->Get_Cert_Hash( &fvk_cert_hash,
													 &fvk_cert_hash_len ) ==
					 PKI_Common::SUCCESS )
				{
					if ( fvk_cert_handle->Get_Signature( &fvk_sign,
														 &fvk_sign_len ) ==
						 PKI_Common::SUCCESS )
					{
						/* Cert store should hold IVK cert using which trusted fvk is signed*/
						if ( cert_store_if->Get_IVK_Cert( &ivk_cert,
														  &ivk_cert_len ) ==
							 PKI_Common::SUCCESS )
						{
							/* create a handle for ivk cert, parse ivk cert to get ivk key and its
							   length in m_cert
							 */
							ivk_cert_handle =
								new Cert_Matrix( ivk_cert, ivk_cert_len,
												 &cert_ret_status );
							if ( ( ivk_cert_handle != nullptr ) &&
								 ( cert_ret_status == PKI_Common::SUCCESS ) )
							{
								/* verify trusted fvk sign using ivk */
								if ( PKI_Common::SUCCESS ==
									 ivk_cert_handle->Verify_Hash( fvk_sign, fvk_sign_len,
																   fvk_cert_hash,
																   fvk_cert_hash_len ) )
								{
									PKI_DEBUG_PRINT( DBG_MSG_INFO,
													 "Verification of trusted fvk sign using ivk is successful" );

									/* Verify trusted FW signature using trusted FVK */
									if ( Verify_Trusted_Fw_Sign( fw_ctrl,
																 fvk_cert_handle ) ==
										 PKI_Common::SUCCESS )
									{
										ret = PKI_Common::SUCCESS;
									}
								}
								else
								{
									PKI_DEBUG_PRINT( DBG_MSG_ERROR,
													 "Trusted FVK signature verification using IVK is failed" );
								}
							}
							if ( ivk_cert_handle != nullptr )
							{
								/*
								 *****************************************************************************************
								 * See header file for function definition.
								 *****************************************************************************************
								 */
								ivk_cert_handle->~Cert_Matrix();
								delete( ivk_cert_handle );
								ivk_cert_handle = nullptr;
							}
						}
						if ( cert_store_if->Free_IVK_Cert() != PKI_Common::SUCCESS )
						{
							PKI_DEBUG_PRINT( DBG_MSG_INFO,
											 "IVK cert memory deallocation failed" );
						}
					}
				}
			}
		}
		else
		{
			PKI_DEBUG_PRINT( DBG_MSG_ERROR, "Trusted FVK cert read failure" );
		}
		if ( fw_ctrl->Free_Trusted_FVK_Cert( fvk_cert ) != PKI_Common::SUCCESS )
		{
			PKI_DEBUG_PRINT( DBG_MSG_ERROR,
							 "FVK cert memory deallocation failed" );
		}
		if ( fvk_cert_handle != nullptr )
		{
			/*
			 *****************************************************************************************
			 * See header file for function definition.
			 *****************************************************************************************
			 */
			fvk_cert_handle->~Cert_Matrix();
			delete( fvk_cert_handle );
			fvk_cert_handle = nullptr;
		}
	}

	return ( ret );
}