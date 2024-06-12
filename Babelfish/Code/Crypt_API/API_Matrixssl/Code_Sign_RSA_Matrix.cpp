/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Code_Sign_RSA_Matrix.h"

#include "Pub_Key_RSA_Matrix.h"
#include "Sha256_Matrix.h"
#include "StdLib_MV.h"
#include "PKI_Config.h"
#include "PKI_Debug.h"
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Code_Sign_RSA_Matrix::Verify_Signature_RSA( uint8_t const* fw_location,
																	 uint32_t const fw_size,
																	 const uint8_t* rvk_pub_key_loc,
																	 const uint32_t rvk_pub_key_len,
																	 callback_t callback )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;
	PKI_Common::pki_status_t ret_status_pub_key = PKI_Common::FAILURE;

	uint8_t const* fvk_pub_key;
	uint8_t* fvk_signature;
	uint8_t* firmware_signature;

	uint16_t fvk_pub_key_len = 0;
	uint16_t fvk_signature_size = 0;
	uint16_t firmware_signature_size = 0;

	Pub_Key_Lib* rsa_pub_key = nullptr;
	Pub_Key_Lib* fw_pub_key = nullptr;
	Sig_Verify* fvk_sig_verify = nullptr;
	Sig_Verify* fw_sig_verify = nullptr;

	Sha_Lib* sha256 = new Sha256_Matrix();

	if ( sha256 != nullptr )
	{
		rsa_pub_key = new Pub_Key_RSA_Matrix( rvk_pub_key_loc, rvk_pub_key_len,
											  &ret_status_pub_key );

		/**
		 * Following is the sequence of FVK and Firmware components.
		 * Firmware/ fvk ver/ fvk pub key lenth/ Reserved word/ fvk public key/ -->
		 * fvk signature size/ fvk signature/ firmware signature size/ firmware signature
		 */
		if ( ( rsa_pub_key != nullptr ) && ( ret_status_pub_key == PKI_Common::SUCCESS ) )
		{
			ret_status_pub_key = PKI_Common::FAILURE;	///< reset the flag
			fvk_sig_verify = new Sig_Verify_RSA_Matrix( sha256, rsa_pub_key, MAX_ALLOCATE_BYTES,
														callback );

			if ( fvk_sig_verify != nullptr )
			{
				fvk_pub_key_len =
					*( reinterpret_cast<uint16_t*>( FVK_PUB_KEY_LEN_ADDR( const_cast<uint8_t*>(
																			  fw_location ),
																		  fw_size ) ) );

				if ( fvk_pub_key_len != static_cast<uint16_t>( ~0U ) )
				{
					fvk_pub_key = FVK_PUB_KEY_ADDR( fw_location, fw_size );
					fvk_signature_size =
						*( const_cast<uint8_t*>( FVK_SIGNATURE_SIZE_ADDR( fvk_pub_key,
																		  fvk_pub_key_len ) ) );
					fvk_signature_size |=
						*( const_cast<uint8_t*>
						   ( FVK_SIGNATURE_SIZE_ADDR( fvk_pub_key, fvk_pub_key_len ) ) + 1 ) << 8;

					if ( fvk_signature_size != static_cast<uint16_t>( ~0U ) )
					{
						fvk_signature =
							const_cast<uint8_t*>( FVK_SIGNATURE_ADDR( fvk_pub_key,
																	  fvk_pub_key_len ) );
						/**
						 * verify FVK  : FVK and FVK sign. first FVK is authenticated and then FVK
						 * is
						 * used to authenticate the firmware.
						 */
						if ( ( fvk_sig_verify->Verify( fvk_pub_key, fvk_pub_key_len, fvk_signature,
													   fvk_signature_size ) ) ==
							 PKI_Common::SUCCESS )
						{
							PKI_DEBUG_PRINT( DBG_MSG_INFO, "FVK Public Key and FW Pack Verification Successful" );
							firmware_signature_size =
								*( const_cast<uint8_t*>
								   ( FW_SIGNATURE_SIZE_ADDR( fvk_signature,
															 fvk_signature_size ) ) );
							firmware_signature_size |=
								*( const_cast<uint8_t*>
								   ( FW_SIGNATURE_SIZE_ADDR( fvk_signature,
															 fvk_signature_size ) ) + 1 ) << 8;

							firmware_signature = FW_SIGNATURE_ADDR( fvk_signature,
																	fvk_signature_size );

							uint32_t fw_package_size = FW_PACKAGE_SIZE( fw_size, fvk_pub_key_len,
																		fvk_signature_size );

							fw_pub_key = new Pub_Key_RSA_Matrix( fvk_pub_key, fvk_pub_key_len,
																 &ret_status_pub_key );

							if ( ( fw_pub_key != nullptr ) &&
								 ( ret_status_pub_key == PKI_Common::SUCCESS ) )
							{
								fw_sig_verify = new Sig_Verify_RSA_Matrix( sha256, fw_pub_key,
																		   MAX_ALLOCATE_BYTES,
																		   callback );

								if ( fw_sig_verify != nullptr )
								{
									/**
									 * verify firmware Pack : FW pack (FVK,FVK sign,FW) and FW pack
									 * sign.
									 */
									if ( fw_sig_verify->Verify( fw_location, fw_package_size,
																firmware_signature,
																firmware_signature_size ) ==
										 PKI_Common::SUCCESS )
									{
										ret = PKI_Common::SUCCESS;
									}
									else
									{
										PKI_DEBUG_PRINT( DBG_MSG_ERROR, "FW Pack Verification Failure" );
									}
									/*
									 *****************************************************************************************
									 * See header file for function definition.
									 *****************************************************************************************
									 */
									fw_sig_verify->~Sig_Verify();
									delete( fw_sig_verify );
								}
							}
						}
						else
						{
							PKI_DEBUG_PRINT( DBG_MSG_ERROR, "FVK Public Key Verification Failure" );
						}
					}
				}
				/*
				 *****************************************************************************************
				 * See header file for function definition.
				 *****************************************************************************************
				 */
				fvk_sig_verify->~Sig_Verify();
				delete( fvk_sig_verify );
			}
		}
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		sha256->~Sha_Lib();
		delete( sha256 );
	}
	if ( rsa_pub_key != nullptr )
	{
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		rsa_pub_key->~Pub_Key_Lib();
		delete( rsa_pub_key );
	}
	if ( fw_pub_key != nullptr )
	{
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		fw_pub_key->~Pub_Key_Lib();
		delete( fw_pub_key );
	}
	if ( ret != PKI_Common::SUCCESS )
	{
		PKI_DEBUG_PRINT( DBG_MSG_ERROR, "RSA: Signature Verification Failure" );
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Code_Sign_RSA_Matrix::Verify_Signature_RSA_NV( NV_Ctrl* nv_handle,
																		PKI_Common::nv_chip_config_t* data_nv_config,
																		const uint8_t* rvk_pub_key_loc,
																		const uint32_t rvk_pub_key_len,
																		callback_t callback )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;
	PKI_Common::pki_status_t ret_status_pub_key = PKI_Common::FAILURE;

	uint8_t const* fvk_pub_key;
	uint8_t* nv_data = nullptr;
	uint8_t* fvk_signature;

	uint32_t nv_data_start_addr;
	uint16_t fvk_pub_key_len;
	uint16_t fvk_signature_size = 0U;
	uint32_t nv_data_len = 0U;

	Pub_Key_Lib* rsa_pub_key = nullptr;
	Pub_Key_Lib* fw_pub_key = nullptr;
	Sig_Verify* fvk_sig_verify = nullptr;
	Sig_Verify* fw_sig_verify = nullptr;

	Sha_Lib* sha256 = new Sha256_Matrix();

	if ( sha256 != nullptr )
	{
		rsa_pub_key = new Pub_Key_RSA_Matrix( rvk_pub_key_loc, rvk_pub_key_len,
											  &ret_status_pub_key );

		/**
		 * Following is the sequence of FVK and Firmware components.
		 * Firmware/ fvk ver/ fvk pub key lenth/ Reserved word/ fvk public key/ -->
		 * fvk signature size/ fvk signature/ firmware signature size/ firmware signature
		 */
		if ( ( rsa_pub_key != nullptr ) && ( ret_status_pub_key == PKI_Common::SUCCESS ) )
		{
			ret_status_pub_key = PKI_Common::FAILURE;	///< reset the flag
			fvk_sig_verify = new Sig_Verify_RSA_Matrix( sha256, rsa_pub_key, MAX_ALLOCATE_BYTES,
														callback );

			if ( fvk_sig_verify != nullptr )
			{
				if ( ( NV_Ctrl::SUCCESS ==
					   nv_handle->Read( reinterpret_cast<uint8_t*>( &fvk_pub_key_len ),
										FVK_PUB_KEY_LEN_ADDR( data_nv_config->
															  start_address,
															  data_nv_config->
															  length ),
										FVK_PUB_KEY_SIZE_LEN, false ) ) &&
					 ( fvk_pub_key_len != ( uint16_t ) ~0U ) )
				{
					nv_data_start_addr = FVK_PUB_KEY_ADDR( data_nv_config->start_address,
														   data_nv_config->length );
					nv_data_len = fvk_pub_key_len + FVK_SIGN_SIZE_LENGTH;
					nv_data = new uint8_t[nv_data_len];	///< allocate memory to read fvk_pub_key and
					// fvk_signature_size

					/* Read fvk_pub_key and fvk_signature_size */
					if ( ( nv_data != nullptr ) &&
						 ( NV_Ctrl::SUCCESS ==
						   nv_handle->Read( nv_data, nv_data_start_addr, nv_data_len, false ) ) )
					{
						fvk_pub_key = new uint8_t[fvk_pub_key_len];
						if ( fvk_pub_key != nullptr )
						{
							/* copy fvk_pub_key from nv_data */
							BF_Lib::Copy_String( const_cast<uint8_t*>( fvk_pub_key ), nv_data,
												 fvk_pub_key_len );
							/* copy 1st byte of fvk_signature_size from nv_data */
							fvk_signature_size =
								*( reinterpret_cast<uint8_t*>( FVK_SIGNATURE_SIZE_ADDR( nv_data,
																						fvk_pub_key_len ) ) );
							/* Copy 2nd byte of fvk_signature_size from nv_data */
							fvk_signature_size |=
								*( reinterpret_cast<uint8_t*>( FVK_SIGNATURE_SIZE_ADDR( nv_data,
																						fvk_pub_key_len ) )
								   + 1 ) << 8;

							if ( fvk_signature_size != ( unsigned short )~0U )
							{
								nv_data_start_addr = FVK_SIGNATURE_ADDR( nv_data_start_addr,
																		 fvk_pub_key_len );
								nv_data_len = fvk_signature_size + FIRMWARE_SIGN_SIZE_LENGTH;
								delete[] nv_data;	///< free nv_data
								nv_data = new uint8_t[nv_data_len];	///< allocate memory to read
								// fvk_signature and
								// firmware_signature_size
								/* Read fvk_signature and firmware_signature_size */
								if ( ( nv_data != nullptr ) &&
									 ( NV_Ctrl::SUCCESS ==
									   nv_handle->Read( nv_data, nv_data_start_addr, nv_data_len,
														false ) ) )
								{
									fvk_signature = new uint8_t[fvk_signature_size];

									if ( fvk_signature != nullptr )
									{
										/* Copy fvk_signature from nv_data */
										BF_Lib::Copy_String( const_cast<uint8_t*>( fvk_signature ),
															 nv_data,
															 fvk_signature_size );
										/**
										 * verify FVK  : FVK and FVK sign. first FVK is
										 * authenticated and then
										 * FVK is used to
										 * authenticate the firmware.
										 */
										if ( ( fvk_sig_verify->Verify( fvk_pub_key, fvk_pub_key_len,
																	   fvk_signature,
																	   fvk_signature_size ) ) ==
											 PKI_Common::SUCCESS )
										{
											PKI_Common::nv_chip_config_t* sig_nv_config =
												new PKI_Common::nv_chip_config_t;
											if ( sig_nv_config != nullptr )
											{
												sig_nv_config->length =
													*( FW_SIGNATURE_SIZE_ADDR( nv_data,
																			   fvk_signature_size ) );

												sig_nv_config->length |=
													*( FW_SIGNATURE_SIZE_ADDR( nv_data,
																			   fvk_signature_size )
													   + 1 ) << 8;

												/* Get the address of firmware_signature */

												sig_nv_config->start_address = FW_SIGNATURE_ADDR(
													nv_data_start_addr,
													fvk_signature_size );

												data_nv_config->length = FW_PACKAGE_SIZE(
													data_nv_config->length,
													fvk_pub_key_len,
													fvk_signature_size );

												fw_pub_key = new Pub_Key_RSA_Matrix( fvk_pub_key,
																					 fvk_pub_key_len,
																					 &ret_status_pub_key );

												if ( ( fw_pub_key != nullptr ) &&
													 ( ret_status_pub_key == PKI_Common::SUCCESS ) )
												{
													fw_sig_verify = new Sig_Verify_RSA_Matrix(
														sha256, fw_pub_key,
														MAX_ALLOCATE_BYTES,
														callback );

													if ( fw_sig_verify != nullptr )
													{
														/* Verify firmware Pack : FW pack (FVK,FVK
														   sign,FW) and FW pack sign */
														if ( fw_sig_verify->Verify( nv_handle,
																					data_nv_config,
																					sig_nv_config )
															 ==
															 PKI_Common::SUCCESS )
														{
															ret = PKI_Common::SUCCESS;
														}
														else
														{
															PKI_DEBUG_PRINT( DBG_MSG_ERROR,
																			 "FW Pack Verification Failure" );
														}
														/*
														 *****************************************************************************************
														 * See header file for function definition.
														 *****************************************************************************************
														 */
														fw_sig_verify->~Sig_Verify();
														delete( fw_sig_verify );
													}
												}
												delete( sig_nv_config );
											}
										}
										else
										{
											PKI_DEBUG_PRINT( DBG_MSG_ERROR, "FVK Verification Failure" );
										}
										delete[] fvk_signature;
									}
								}
							}
							delete[] fvk_pub_key;
						}
					}
				}
				else
				{
					PKI_DEBUG_PRINT( DBG_MSG_ERROR, "FVK Public Key Memory Read Operation Failure" );
				}
				/*
				 *****************************************************************************************
				 * See header file for function definition.
				 *****************************************************************************************
				 */
				fvk_sig_verify->~Sig_Verify();
				delete( fvk_sig_verify );
			}
		}
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		sha256->~Sha_Lib();
		delete( sha256 );
	}
	if ( nv_data != nullptr )
	{
		delete[] nv_data;
	}
	if ( rsa_pub_key != nullptr )
	{
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		rsa_pub_key->~Pub_Key_Lib();
		delete( rsa_pub_key );
	}
	if ( fw_pub_key != nullptr )
	{
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		fw_pub_key->~Pub_Key_Lib();
		delete( fw_pub_key );
	}
	if ( ret != PKI_Common::SUCCESS )
	{
		PKI_DEBUG_PRINT( DBG_MSG_ERROR, "RSA_NV: Signature Verification Failure" );
	}
	return ( ret );
}
