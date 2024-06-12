/*HEADER**********************************************************************
 *
 * Copyright 2010 Freescale Semiconductor, Inc.
 *
 * This software is owned or controlled by Freescale Semiconductor.
 * Use of this software is governed by the Freescale MQX RTOS License
 * distributed with this Material.
 * See the MQX_RTOS_LICENSE file distributed for more details.
 *
 * Brief License Summary:
 * This software is provided in source form for you to use free of charge,
 * but it is not open source software. You are allowed to use this software
 * but you cannot redistribute it or derivative works of it in source form.
 * The software may be used only in connection with a product containing
 * a Freescale microprocessor, microcontroller, or digital signal processor.
 * See license agreement file for full license terms including other
 * restrictions.
 *****************************************************************************
 *
 * Comments:
 *
 *   The file contains functions for internal flash read, write, erase
 *
 *   Adapted by Dave Bender for use with PXBCM
 *
 *
 * END************************************************************************/

// #include "flash_ftfe_prv.h"
// #include "flash_ftfe.h"
#define RAM_CODE_SIZE 128

struct ftfe_flash_ctx_s
{
	char flash_execute_code_ptr[RAM_CODE_SIZE];
} s_flash_ctx;

static char* ftfe_init_ram_function( void*, void*, char* );

static int ftfe_deinit_ram_function( char* );

static uint32_t ftfe_flash_command_sequence( volatile ftfe_flash_ctx*, uint8_t*, uint8_t, void*,
											 uint32_t );

static void ftfe_ram_function( volatile uint8_t*, volatile uint32_t*, unsigned cmd );

static void ftfe_ram_function_end( void );

static void kinetis_flash_invalidate_cache( uint32_t flags );

static void kinetis_flash_invalidate_cache_end( void );

static void kinetis_flash_initialize( void );

static void kinetis_flash_initialize_end( void );

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : ftfe_flash_command_sequence
 * Returned Value   : uint32_t an error code
 * Comments         :
 *    Run command in FTFE device.
 *
 * END*----------------------------------------------------------------------*/
static uint32_t ftfe_flash_command_sequence(
	/* [IN] Flash specific structure */
	volatile ftfe_flash_ctx* dev_spec_ptr,

	/* [IN] Command byte array */
	uint8_t* command_array,

	/* [IN] Number of values in the array */
	uint8_t count,

	/* [IN] The address which will be affected by command */
	void* affected_addr,

	/* [IN] The address which will be affected by command */
	uint32_t affected_size )
{
	uint8_t fstat;
	uint32_t result;

	void (* RunInRAM)( volatile uint8_t*, volatile uint32_t*, unsigned );
	void (* RunInvalidateInRAM)( uint32_t );

	/* get pointer to RunInRAM function
	 * Add 1 to comply with THUMB */
	RunInRAM = ( void ( * )( volatile uint8_t*, volatile uint32_t*, unsigned ) )( dev_spec_ptr
																				  ->flash_execute_code_ptr + 1 );

	/* set the default return as FTFE_OK */
	result = 0;

	/* check CCIF bit of the flash status register */
	while ( 0 == ( FTFE->FSTAT & FTFE_FSTAT_CCIF_MASK ) )
	{}

	/* clear RDCOLERR & ACCERR & FPVIOL error flags in flash status register */
	if ( FTFE->FSTAT & FTFE_FSTAT_RDCOLERR_MASK )
	{
		FTFE->FSTAT |= FTFE_FSTAT_RDCOLERR_MASK;
	}
	if ( FTFE->FSTAT & FTFE_FSTAT_ACCERR_MASK )
	{
		FTFE->FSTAT |= FTFE_FSTAT_ACCERR_MASK;
	}
	if ( FTFE->FSTAT & FTFE_FSTAT_FPVIOL_MASK )
	{
		FTFE->FSTAT |= FTFE_FSTAT_FPVIOL_MASK;
	}

	switch ( count )
	{
		case 12:
			FTFE->FCCOBB = command_array[--count];

		case 11:
			FTFE->FCCOBA = command_array[--count];

		case 10:
			FTFE->FCCOB9 = command_array[--count];

		case 9:
			FTFE->FCCOB8 = command_array[--count];

		case 8:
			FTFE->FCCOB7 = command_array[--count];

		case 7:
			FTFE->FCCOB6 = command_array[--count];

		case 6:
			FTFE->FCCOB5 = command_array[--count];

		case 5:
			FTFE->FCCOB4 = command_array[--count];

		case 4:
			FTFE->FCCOB3 = command_array[--count];

		case 3:
			FTFE->FCCOB2 = command_array[--count];

		case 2:
			FTFE->FCCOB1 = command_array[--count];

		case 1:
			FTFE->FCCOB0 = command_array[--count];

		default:
			break;
	}

	__disable_interrupt();

	/* run command and wait for it to finish (must execute from RAM) */
	RunInRAM( &FTFE->FSTAT, &FMC->PFB01CR, command_array[0] );

	fstat = FTFE->FSTAT;
	/*
	   invalidate data cache of 'affected_addr' address and 'affected_size' size
	   because reading flash through code-bus may show incorrect data
	 */
#if defined( _DCACHE_INVALIDATE_MLINES ) || defined( _ICACHE_INVALIDATE_MLINES )
	if ( affected_size )
	{
#if defined( _DCACHE_INVALIDATE_MLINES )
		_DCACHE_INVALIDATE_MLINES( affected_addr, affected_size );
#endif
#if defined( _ICACHE_INVALIDATE_MLINES )
		_ICACHE_INVALIDATE_MLINES( affected_addr, affected_size );
#endif
	}
#endif

	__enable_interrupt();

	/* checking access error */
	if ( 0 != ( fstat & FTFE_FSTAT_ACCERR_MASK ) )
	{
		/* return an error code FTFE_ERR_ACCERR */
		result = 1;
	}
	/* checking protection error */
	else if ( 0 != ( fstat & FTFE_FSTAT_FPVIOL_MASK ) )
	{
		/* return an error code FTFE_ERR_PVIOL */
		result = 2;
	}
	/* checking MGSTAT0 non-correctable error */
	else if ( 0 != ( fstat & FTFE_FSTAT_MGSTAT0_MASK ) )
	{
		/* return an error code FTFE_ERR_MGSTAT0 */
		result = 3;
	}

	return ( result );
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : ftfe_flash_init
 * Returned Value   :
 * Comments         :
 *   Initialize flash specific information.
 *
 * END*----------------------------------------------------------------------*/
ftfe_flash_ctx* ftfe_flash_init( void )
{
	/* save pointer to function in ram */
	if ( !ftfe_init_ram_function( ( void* )ftfe_ram_function,
								  ( void* )ftfe_ram_function_end,
								  s_flash_ctx.flash_execute_code_ptr ) )
	{
		return ( NULL );
	}

	return ( &s_flash_ctx );
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : ftfe_flash_deinit
 * Returned Value   : none
 * Comments         :
 *   Release flash specific information.
 *
 * END*----------------------------------------------------------------------*/
void ftfe_flash_deinit(
	/* [IN] File pointer */
	ftfe_flash_ctx* dev_ptr )
{}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : ftfe_init_ram_function
 * Returned Value   : pointer to allocated RAM function
 * Comments         :
 *   Allocate and copy flash RAM function.
 *
 * END*----------------------------------------------------------------------*/
static char* ftfe_init_ram_function(
	void* function_start,
	void* function_end,
	char* ram_code_ptr )
{
	char* fstart;
	char* fend;

	/* remove thumb2 flag from the address and align to word size */
	fstart = ( char* ) ( ( unsigned )function_start & ~3 );
	fend = ( char* ) ( ( unsigned )function_end & ~3 );

	if ( fend < fstart )
	{
		return ( NULL );
	}

	if ( fend - fstart > RAM_CODE_SIZE )
	{
		return ( NULL );
	}

	/* copy code to RAM buffer */
	memcpy( ram_code_ptr, fstart, fend - fstart );

	return ( ram_code_ptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
unsigned ftfe_flash_prog_check(
	/* [IN] File pointer */
	ftfe_flash_ctx* dev_ptr,

	/* [IN] Erased sector address */
	const char* from_ptr,

	/* Expected value to match */
	uint32_t expected )
{
	unsigned result;
	unsigned write_addr;
	uint8_t command_array[12];

	write_addr = ( unsigned )from_ptr;

	/* If already all 1's then the sector is erased.
	 * Use the normal margin read level. */
	command_array[0] = FTFE_PROGRAM_CHECK;
	command_array[1] = ( uint8_t )( write_addr >> 16 );
	command_array[2] = ( uint8_t )( ( write_addr >> 8 ) & 0xFF );
	command_array[3] = ( uint8_t )( write_addr & 0xFF );
	command_array[4] = 1;
	command_array[5] = 0;
	command_array[6] = 0;
	command_array[7] = 0;

	command_array[8] = ( expected >> 24 ) & 0xFF;
	command_array[9] = ( expected >> 16 ) & 0xFF;
	command_array[10] = ( expected >> 8 ) & 0xFF;
	command_array[11] = expected & 0xFF;

	result = ftfe_flash_command_sequence( dev_ptr, command_array, 12,
										  ( void* )write_addr, 4 );

	return ( result );
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : ftfe_flash_erase_sector
 * Returned Value   : 1 if successful, 0 otherwise
 * Comments         :
 *   Erase a flash memory block.
 *
 * END*----------------------------------------------------------------------*/
unsigned ftfe_flash_erase_sector(
	/* [IN] File pointer */
	ftfe_flash_ctx* dev_ptr,

	/* [IN] Erased sector address */
	char* from_ptr,

	/* [IN] Erased sector size */
	unsigned size )
{
	unsigned result;
	unsigned write_addr;
	uint8_t command_array[7];

	write_addr = ( unsigned )from_ptr;

	/* If already all 1's then the sector is erased.
	 * Use the normal margin read level. */
	command_array[0] = FTFE_VERIFY_SECTION;
	command_array[1] = ( uint8_t )( write_addr >> 16 );
	command_array[2] = ( uint8_t )( ( write_addr >> 8 ) & 0xFF );
	command_array[3] = ( uint8_t )( write_addr & 0xFF );
	command_array[4] = ( uint8_t )( ( size / 16 ) >> 8 );
	command_array[5] = ( uint8_t )( ( size / 16 ) & 0xFF );
	command_array[6] = 0;
	result = ftfe_flash_command_sequence( dev_ptr, command_array, 7,
										  ( void* )write_addr, size );
	if ( 0 == result )
	{
		return ( 0 );
	}

	/* preparing passing parameter to erase a flash block */
	command_array[0] = FTFE_ERASE_SECTOR;
	command_array[1] = ( uint8_t )( write_addr >> 16 );
	command_array[2] = ( uint8_t )( ( write_addr >> 8 ) & 0xFF );
	command_array[3] = ( uint8_t )( write_addr & 0xFF );

	/* call flash command sequence function to execute the command */
	return ( ftfe_flash_command_sequence( dev_ptr, command_array, 4, ( void* )write_addr, size ) );
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : ftfe_flash_write_phrase
 * Returned Value   : 1 if successful, 0 otherwise
 * Comments         :
 *   Performs a write into flash memory.
 *
 * END*----------------------------------------------------------------------*/
unsigned ftfe_flash_write_phrase(
	/* [IN] File pointer */
	ftfe_flash_ctx* dev_ptr,

	/* [IN] Source address */
	const char* from_ptr,

	/* [IN] Destination address */
	char* to_ptr,

	/* [IN] Number of bytes to write */
	unsigned size )
{
	unsigned result;
	unsigned i;
	unsigned write_addr = 0;
	uint8_t command_array[4 + FTFE_PHRASE_SIZE];
	char temp_data[FTFE_PHRASE_SIZE];
	unsigned char byte_data_counter = 0;
	uint32_t offset = ( ( uint32_t )to_ptr ) & 0x00000007;

	/* write to address mod 8 correction */
	if ( offset )
	{
		/* align pointer to writable address */
		to_ptr -= offset;

		/* jump over old data */
		byte_data_counter = offset;
	}

	write_addr = ( unsigned )to_ptr;

	/* heading space should be 0xFF */
	for ( i = 0; i < offset; i++ )
	{
		temp_data[i] = 0xFF;
	}

	while ( size )
	{
		/* move data to temporary char array */
		while ( ( byte_data_counter < FTFE_PHRASE_SIZE ) && size )
		{
			temp_data[byte_data_counter++] = *from_ptr++;
			size--;
		}

		/* remaining space should be 0xFF */
		while ( byte_data_counter < FTFE_PHRASE_SIZE )
		{
			temp_data[byte_data_counter++] = 0xFF;
		}

		/* prepare parameters to program the flash block */
		command_array[0] = FTFE_PROGRAM_PHRASE;
		command_array[1] = ( uint8_t )( write_addr >> 16 );
		command_array[2] = ( uint8_t )( ( write_addr >> 8 ) & 0xFF );
		command_array[3] = ( uint8_t )( write_addr & 0xFF );

		command_array[4] = temp_data[3];
		command_array[5] = temp_data[2];
		command_array[6] = temp_data[1];
		command_array[7] = temp_data[0];

		command_array[8] = temp_data[7];
		command_array[9] = temp_data[6];
		command_array[10] = temp_data[5];
		command_array[11] = temp_data[4];

		/* call flash command sequence function to execute the command */
		result = ftfe_flash_command_sequence( dev_ptr, command_array, 4 + FTFE_PHRASE_SIZE,
											  ( void* )write_addr, FTFE_PHRASE_SIZE );
		if ( result )
		{
			return ( result );
		}

		/* update destination address for next iteration */
		write_addr += FTFE_PHRASE_SIZE;
		/* init variables for next loop */
		byte_data_counter = 0;
	}

	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* ftfe_flash_ram_ready( ftfe_flash_ctx* dev_ptr )
{
	if ( !( FTFE->FCNFG & FTFE_FCNFG_RAMRDY_MASK ) )
	{
		return ( NULL );
	}

	return ( ( uint8_t* )BSP_INTERNAL_PA_RAM_BASE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int ftfe_flash_ram_flush( ftfe_flash_ctx* dev_ptr, uint8_t* to_ptr,
						  uint8_t* end )
{
	if ( ( unsigned long )to_ptr & ( FTFE_PROGRAM_SECTION_ALIGNMENT - 1 ) )
	{
		return ( -1 );
	}

	const uint8_t* ram_end = ftfe_flash_ram_end( dev_ptr );

	if ( end > ram_end )
	{
		return ( -2 );
	}

	/* Fill in remaining space with 0xFF */
	while ( end != ram_end )
	{
		*end = 0xFF;
		++end;
	}

	unsigned write_addr = ( unsigned )to_ptr;
	uint8_t command_array[6];

	/* preparing passing parameter to program section */
	command_array[0] = FTFE_PROGRAM_SECTION;
	command_array[1] = ( uint8_t )( ( write_addr >> 16 ) & 0xFF );
	command_array[2] = ( uint8_t )( ( write_addr >> 8 ) & 0xFF );
	command_array[3] = ( uint8_t )( write_addr & 0xFF );
	/* Number of 16 byte entries to write. */
	command_array[4] = ( 4096 >> 4 ) >> 8;
	command_array[5] = 0;

	return ( ftfe_flash_command_sequence( dev_ptr, command_array, 6,
										  to_ptr, 4 * 1024 ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const uint8_t* ftfe_flash_ram_end( ftfe_flash_ctx* dev_ptr )
{
	return ( ( const uint8_t* )BSP_INTERNAL_PA_RAM_BASE + 4 * 1024 );
}

uint32_t s_test_read = 0;
/********************************************************************
*
*  Code required to run in SRAM to perform flash commands.
*  All else can be run in flash.
*  Parameter is an address of flash status register.
*
********************************************************************/
static void ftfe_ram_function(
	/* [IN] Flash info structure */
	volatile uint8_t* ftfe_fstat_ptr,
	volatile uint32_t* fmc_ptr,
	unsigned cmd )
{
	/* start flash write */
	*ftfe_fstat_ptr |= FTFE_FSTAT_CCIF_MASK;

	/* wait until execution complete */
	while ( 0 == ( *ftfe_fstat_ptr & FTFE_FSTAT_CCIF_MASK ) )
	{}

	/* get flash status register value */
	unsigned countdown = 512;

	while ( countdown && ( *ftfe_fstat_ptr & FTFE_FSTAT_CCIF_MASK ) )
	{
		--countdown;
	}

	while ( 0 == ( *ftfe_fstat_ptr & FTFE_FSTAT_CCIF_MASK ) )
	{}

	if ( ( cmd == FTFE_ERASE_SECTOR ) ||
		 ( cmd == FTFE_PROGRAM_PHRASE ) ||
		 ( cmd == FTFE_PROGRAM_SECTION ) )
	{
		*fmc_ptr |= FMC_PFB01CR_S_B_INV_MASK | FMC_PFB01CR_CINV_WAY_MASK;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static void ftfe_ram_function_end( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static void kinetis_flash_invalidate_cache(
	/* [IN] What exactly to invalidate */
	uint32_t flags )
{
	/*
	   uint32_t PFB0CR_restore = FMC->PFB01CR;
	   FMC->PFB01CR = PFB0CR_restore | FMC_PFB01CR_S_B_INV_MASK | FMC_PFB01CR_CINV_WAY(15);
	 */
	FMC->PFB01CR |= FMC_PFB01CR_S_B_INV_MASK | FMC_PFB01CR_CINV_WAY_MASK;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static void kinetis_flash_invalidate_cache_end( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ftfe_flash_initialize_banks( void )
{
	uint8_t funbuffer[RAM_CODE_SIZE];

	const char* fun = ( const char* ) ( ( ( unsigned )kinetis_flash_initialize ) & ~0x3 );
	const char* fend = ( const char* ) ( ( ( unsigned )kinetis_flash_initialize_end ) & ~0x3 );

	memcpy( funbuffer, fun, fend - fun );

	void (* function)();
	function = ( void ( * )() )( funbuffer + 1 );
	function();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static void kinetis_flash_initialize( void )
{
	/* Configure the flash.
	 * Banks 0-1 contains instructions and read only variables.
	 * Banks 2-3 contains the web page firmware and NO instructions.
	 * -Disable Instruction cache and instruction prefetches.
	 * */
	FMC->PFB23CR &= ~( FMC_PFB23CR_B23ICE_MASK | FMC_PFB23CR_B23IPE_MASK
					   | FMC_PFB23CR_B23DCE_MASK | FMC_PFB23CR_B23DPE_MASK | FMC_PFB23CR_B23SEBE_MASK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static void kinetis_flash_initialize_end( void )
{}
