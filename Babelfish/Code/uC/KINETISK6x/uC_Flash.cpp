/**
 *****************************************************************************************
 *	@file   uC_Flash.cpp Implementation file for Flash driver
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-2-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation
 * casts shall not be used
 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style casts
 * are required for all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */
#include "Includes.h"
#include "uC_Flash.h"
#include "StdLib_MV.h"
#include "uC_Fw_Code_Range.h"
#include "Babelfish_Assert.h"

#define RAM_CODE_SIZE 128

/* This is the pointer to the acceleration RAM buffer. */
static uint8_t* s_acc_ram_ptr = NULL;

namespace NV_Ctrls
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
// #define DO_NOT_CHANGE_MEM
#define FLASH_KEY1               ( static_cast<uint32_t>( 0x45670123U ) )
#define FLASH_KEY2               ( static_cast<uint32_t>( 0xCDEF89ABU ) )

#define CR_PSIZE_MASK               ( static_cast<uint32_t>( 0xFFFFFCFFU ) )
#define SECTOR_MASK                 ( static_cast<uint32_t>( 0xFFFFFF07U ) )

#define Get_Page_Size( address )            ( INTERNAL_FLASH_PAGE_SIZES[Get_Sector( address )]. \
											  sector_size )
#define Get_Page_Mask( address )            ( ~( Get_Page_Size( address ) - 1U ) )

#define M8( adr )  ( *( ( volatile uint8_t* ) ( adr ) ) )
#define M16( adr ) ( *( ( volatile uint16_t* ) ( adr ) ) )
#define M32( adr ) ( *( ( volatile uint32_t* ) ( adr ) ) )
// #define SCRATCH_PAGE_ADDRESS				( m_chip_cfg->end_address - Get_Page_Size(
// m_chip_cfg->end_address - 1U ) )

// #define uC_FLASH_PAGE_ALIGNMENT_MASK		~( m_chip_cfg->page_size - 1U )
// #define uC_FLASH_SCRATCH_PAGE_ADDRESS		( m_chip_cfg->end_address - ( m_chip_cfg->page_size * 2U
// ) )
// #define uC_FLASH_WORD_ALIGNMENT_MASK		~(0x01U)

// #define uC_FLASH_CR_PER_RESET                 (static_cast<uint32_t>(0x00001FFDU))
#define FLASH_ERASED_VALUE                  0xFFU
#define FLASH_BLOCK     0x40000
#define FLASH_SECTOR    0x1000

#if defined ( uC_KINETISK60DNx_USAGE )
/* FTFL commands */
#define FTFL_VERIFY_BLOCK              0x00
#define FTFL_VERIFY_SECTION            0x01
#define FTFL_PROGRAM_CHECK             0x02
#define FTFL_READ_RESOURCE             0x03
#define FTFL_PROGRAM_LONGWORD          0x06
#define FTFL_PROGRAM_PHRASE            0x07
#define FTFL_ERASE_BLOCK               0x08
#define FTFL_ERASE_SECTOR              0x09
#define FTFL_PROGRAM_SECTION           0x0B
#define FTFL_VERIFY_ALL_BLOCK          0x40
#define FTFL_READ_ONCE                 0x41
#define FTFL_PROGRAM_ONCE              0x43
#define FTFL_ERASE_ALL_BLOCK           0x44
#define FTFL_SECURITY_BY_PASS          0x45
#define FTFL_PFLASH_SWAP               0x46
#define FTFL_PROGRAM_PARTITION         0x80
#define FTFL_SET_EERAM                 0x81

/* FTFL margin read settings */
#define FTFL_MARGIN_NORMAL             0x0000
#define FTFL_USER_MARGIN_LEVEL1        0x0001
#define FTFL_USER_MARGIN_LEVEL0        0x0002
#define FTFL_FACTORY_MARGIN_LEVEL0     0x0003
#define FTFL_FACTORY_MARGIN_LEVEL1     0x0004

/* FTFL sizes */
#define FTFL_WORD_SIZE                 0x0002
#define FTFL_LONGWORD_SIZE             0x0004
#define FTFL_DPHRASE_SIZE              0x0010
#define FTFL_PROGRAM_SECTION_ALIGNMENT FTFL_DPHRASE_SIZE
#define BSP_INTERNAL_PA_RAM_BASE       0x14000000

/* FTFL error codes */
#define FTFL_OK                        0x0000
#define FTFL_ERR_SIZE                  0x0001
#define FTFL_ERR_RANGE                 0x0002
#define FTFL_ERR_ACCERR                0x0004
#define FTFL_ERR_PVIOL                 0x0008
#define FTFL_ERR_MGSTAT0               0x0010
#define FTFL_ERR_CHANGEPROT            0x0020
#define FTFL_ERR_EEESIZE               0x0040
#define FTFL_ERR_EFLASHSIZE            0x0080
#define FTFL_ERR_ADDR                  0x0100
#define FTFL_ERR_NOEEE                 0x0200
#define FTFL_ERR_EFLASHONLY            0x0400
#define FTFL_ERR_DFLASHONLY            0x0800
#define FTFL_ERR_RDCOLERR              0x1000
#define FTFL_ERR_RAMRDY                0x2000

/* Flash Swap State */
#define FTFL_SWAP_UNINIT               0x00
#define FTFL_SWAP_READY                0x01
#define FTFL_SWAP_INIT                 0x01
#define FTFL_SWAP_UPDATE               0x02
#define FTFL_SWAP_UPDATE_ERASED        0x03
#define FTFL_SWAP_COMPLETE             0x04

/* PFlash swap control codes */
#define FTFL_SWAP_SET_INDICATOR_ADDR   0x01
#define FTFL_SWAP_SET_IN_PREPARE       0x02
#define FTFL_SWAP_SET_IN_COMPLETE      0x04
#define FTFL_SWAP_REPORT_STATUS        0x08

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Ftfl_Flash_Initialize_Banks( void )
{
	uint8_t funbuffer[RAM_CODE_SIZE];

	const uint8_t* fun =
		reinterpret_cast<uint8_t*>( ( ( uint32_t )Kinetis_Flash_Initialize ) & ~0x3 );
	const uint8_t* fend =
		reinterpret_cast<uint8_t*>( ( ( uint32_t )Kinetis_Flash_Initialize_End ) & ~0x3 );

	memcpy( funbuffer, fun, fend - fun );

	void (* function)();
	function = ( void ( * )() )( funbuffer + 1 );
	function();
}

#else
/* FTFE commands */
#define FTFE_VERIFY_BLOCK              0x00
#define FTFE_VERIFY_SECTION            0x01
#define FTFE_PROGRAM_CHECK             0x02
#define FTFE_READ_RESOURCE             0x03
#define FTFE_PROGRAM_LONGWORD          0x06
#define FTFE_PROGRAM_PHRASE            0x07
#define FTFE_ERASE_BLOCK               0x08
#define FTFE_ERASE_SECTOR              0x09
#define FTFE_PROGRAM_SECTION           0x0B
#define FTFE_VERIFY_ALL_BLOCK          0x40
#define FTFE_READ_ONCE                 0x41
#define FTFE_PROGRAM_ONCE              0x43
#define FTFE_ERASE_ALL_BLOCK           0x44
#define FTFE_SECURITY_BY_PASS          0x45
#define FTFE_PFLASH_SWAP               0x46
#define FTFE_PROGRAM_PARTITION         0x80
#define FTFE_SET_EERAM                 0x81

/* FTFE margin read settings */
#define FTFE_MARGIN_NORMAL             0x0000
#define FTFE_USER_MARGIN_LEVEL1        0x0001
#define FTFE_USER_MARGIN_LEVEL0        0x0002
#define FTFE_FACTORY_MARGIN_LEVEL0     0x0003
#define FTFE_FACTORY_MARGIN_LEVEL1     0x0004

/* FTFE sizes */
#define FTFE_WORD_SIZE                 0x0002
#define FTFE_LONGWORD_SIZE             0x0004
#define FTFE_DPHRASE_SIZE              0x0010
#define FTFE_PROGRAM_SECTION_ALIGNMENT FTFE_DPHRASE_SIZE
#define BSP_INTERNAL_PA_RAM_BASE       0x14000000

/* FTFE error codes */
#define FTFE_OK                        0x0000
#define FTFE_ERR_SIZE                  0x0001
#define FTFE_ERR_RANGE                 0x0002
#define FTFE_ERR_ACCERR                0x0004
#define FTFE_ERR_PVIOL                 0x0008
#define FTFE_ERR_MGSTAT0               0x0010
#define FTFE_ERR_CHANGEPROT            0x0020
#define FTFE_ERR_EEESIZE               0x0040
#define FTFE_ERR_EFLASHSIZE            0x0080
#define FTFE_ERR_ADDR                  0x0100
#define FTFE_ERR_NOEEE                 0x0200
#define FTFE_ERR_EFLASHONLY            0x0400
#define FTFE_ERR_DFLASHONLY            0x0800
#define FTFE_ERR_RDCOLERR              0x1000
#define FTFE_ERR_RAMRDY                0x2000

/* Flash Swap State */
#define FTFE_SWAP_UNINIT               0x00
#define FTFE_SWAP_READY                0x01
#define FTFE_SWAP_INIT                 0x01
#define FTFE_SWAP_UPDATE               0x02
#define FTFE_SWAP_UPDATE_ERASED        0x03
#define FTFE_SWAP_COMPLETE             0x04

/* PFlash swap control codes */
#define FTFE_SWAP_SET_INDICATOR_ADDR   0x01
#define FTFE_SWAP_SET_IN_PREPARE       0x02
#define FTFE_SWAP_SET_IN_COMPLETE      0x04
#define FTFE_SWAP_REPORT_STATUS        0x08

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Ftfe_Flash_Initialize_Banks( void )
{
	uint8_t funbuffer[RAM_CODE_SIZE];

	const uint8_t* fun =
		reinterpret_cast<uint8_t*>( ( ( uint32_t )Kinetis_Flash_Initialize ) & ~0x3 );
	const uint8_t* fend =
		reinterpret_cast<uint8_t*>( ( ( uint32_t )Kinetis_Flash_Initialize_End ) & ~0x3 );

	memcpy( funbuffer, fun, fend - fun );

	void (* function)();
	function = ( void ( * )() )( funbuffer + 1 );
	function();
}

#endif
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Kinetis_Flash_Initialize( void )
{
	/* Configure the flash.
	 * Banks 0-1 contains instructions and read only variables.
	 * Banks 2-3 contains the web page firmware and NO instructions.
	 * -Disable Instruction cache and instruction prefetches.
	 * */
#ifdef uC_KINETISK66x_USAGE
	FMC->PFB23CR &= ~( FMC_PFB23CR_B1ICE_MASK | FMC_PFB23CR_B1IPE_MASK
					   | FMC_PFB23CR_B1DCE_MASK | FMC_PFB23CR_B1DPE_MASK | FMC_PFB23CR_RFU_MASK );
#endif
#ifdef uC_KINETISK60x_USAGE
	FMC->PFB23CR &= ~( FMC_PFB23CR_B23ICE_MASK | FMC_PFB23CR_B23IPE_MASK
					   | FMC_PFB23CR_B23DCE_MASK | FMC_PFB23CR_B23DPE_MASK |
					   FMC_PFB23CR_B23SEBE_MASK );
#endif
#ifdef uC_KINETISK60DNx_USAGE
	FMC->PFB1CR &= ~( FMC_PFB1CR_B1ICE_MASK | FMC_PFB1CR_B1IPE_MASK
					  | FMC_PFB1CR_B1DCE_MASK | FMC_PFB1CR_B1DPE_MASK |
					  FMC_PFB1CR_B1SEBE_MASK );
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Kinetis_Flash_Initialize_End( void )
{}

#if defined ( uC_KINETISK60DNx_USAGE )
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Flash::Ftfl_Flash_Command_Sequence(	/*volatile ftfl_flash_ctx* dev_spec_ptr,*/
	uint8_t* command_array, uint8_t count,
	void* affected_addr, uint32_t affected_size )
{
	uint8_t fstat;
	uint32_t result;

	void (* RunInRAM)( volatile uint8_t*, volatile uint32_t*, uint8_t ) = Ftfl_Ram_Function;

	/* set the default return as FTFL_OK */
	result = 0;

	/* check CCIF bit of the flash status register */
	while ( 0 == ( FTFL->FSTAT & FTFL_FSTAT_CCIF_MASK ) )
	{ }

	/* clear RDCOLERR & ACCERR & FPVIOL error flags in flash status register */
	if ( FTFL->FSTAT & FTFL_FSTAT_RDCOLERR_MASK )
	{
		FTFL->FSTAT |= FTFL_FSTAT_RDCOLERR_MASK;
	}
	if ( FTFL->FSTAT & FTFL_FSTAT_ACCERR_MASK )
	{
		FTFL->FSTAT |= FTFL_FSTAT_ACCERR_MASK;
	}
	if ( FTFL->FSTAT & FTFL_FSTAT_FPVIOL_MASK )
	{
		FTFL->FSTAT |= FTFL_FSTAT_FPVIOL_MASK;
	}

	switch ( count )
	{
		case 12:
			FTFL->FCCOBB = command_array[--count];

		case 11:
			FTFL->FCCOBA = command_array[--count];

		case 10:
			FTFL->FCCOB9 = command_array[--count];

		case 9:
			FTFL->FCCOB8 = command_array[--count];

		case 8:
			FTFL->FCCOB7 = command_array[--count];

		case 7:
			FTFL->FCCOB6 = command_array[--count];

		case 6:
			FTFL->FCCOB5 = command_array[--count];

		case 5:
			FTFL->FCCOB4 = command_array[--count];

		case 4:
			FTFL->FCCOB3 = command_array[--count];

		case 3:
			FTFL->FCCOB2 = command_array[--count];

		case 2:
			FTFL->FCCOB1 = command_array[--count];

		case 1:
			FTFL->FCCOB0 = command_array[--count];

		default:
			break;
	}

	Disable_Interrupts();

	/* run command and wait for it to finish (must execute from RAM) */
	RunInRAM( &FTFL->FSTAT, &FMC->PFB0CR, command_array[0] );

	fstat = FTFL->FSTAT;
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

	Enable_Interrupts();

	/* checking access error */
	if ( 0 != ( fstat & FTFL_FSTAT_ACCERR_MASK ) )
	{
		/* return an error code FTFL_ERR_ACCERR */
		result = 1;
	}
	/* checking protection error */
	else if ( 0 != ( fstat & FTFL_FSTAT_FPVIOL_MASK ) )
	{
		/* return an error code FTFL_ERR_PVIOL */
		result = 2;
	}
	/* checking MGSTAT0 non-correctable error */
	else if ( 0 != ( fstat & FTFL_FSTAT_MGSTAT0_MASK ) )
	{
		/* return an error code FTFL_ERR_MGSTAT0 */
		result = 3;
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Flash::Ftfl_Flash_Prog_Check( const uint8_t* from_ptr,
										  uint32_t expected )
{
	uint32_t result;
	uint32_t write_addr;
	uint8_t command_array[12];

	write_addr = reinterpret_cast<uint32_t>( from_ptr );

	/* If already all 1's then the sector is erased.
	 * Use the normal margin read level. */
	command_array[0] = FTFL_PROGRAM_CHECK;
	command_array[1] = static_cast<uint8_t>( write_addr >> 16 );
	command_array[2] = static_cast<uint8_t>( ( write_addr >> 8 ) & 0xFF );
	command_array[3] = static_cast<uint8_t>( write_addr & 0xFF );
	command_array[4] = 1;
	command_array[5] = 0;
	command_array[6] = 0;
	command_array[7] = 0;

	command_array[8] = ( expected >> 24 ) & 0xFF;
	command_array[9] = ( expected >> 16 ) & 0xFF;
	command_array[10] = ( expected >> 8 ) & 0xFF;
	command_array[11] = expected & 0xFF;

	result = Ftfl_Flash_Command_Sequence( command_array, 12,
										  reinterpret_cast<void*>( write_addr ), 4 );

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Flash::Ftfl_Flash_Erase_Sector( uint8_t* from_ptr,
											uint32_t size )
{
	uint32_t result;
	uint32_t write_addr;
	uint8_t command_array[7];
	bool_t exit_flag = false;

	write_addr = reinterpret_cast<uint32_t>( from_ptr );

	/* If already all 1's then the sector is erased.
	 * Use the normal margin read level. */
	command_array[0] = FTFL_VERIFY_SECTION;
	command_array[1] = static_cast<uint8_t>( write_addr >> 16 );
	command_array[2] = static_cast<uint8_t>( ( write_addr >> 8 ) & 0xFF );
	command_array[3] = static_cast<uint8_t>( write_addr & 0xFF );
	command_array[4] = static_cast<uint8_t>( ( size / 16 ) >> 8 );
	command_array[5] = static_cast<uint8_t>( ( size / 16 ) & 0xFF );
	command_array[6] = 0;
	result = Ftfl_Flash_Command_Sequence( command_array, 7,
										  reinterpret_cast<void*>( write_addr ), size );
	if ( 0 == result )
	{
		exit_flag = true;
	}

	if ( exit_flag == false )
	{
		/* preparing passing parameter to erase a flash block */
		command_array[0] = FTFL_ERASE_SECTOR;
		command_array[1] = static_cast<uint8_t>( write_addr >> 16 );
		command_array[2] = static_cast<uint8_t>( ( write_addr >> 8 ) & 0xFF );
		command_array[3] = static_cast<uint8_t>( write_addr & 0xFF );

		/* call flash command sequence function to execute the command */
		result =
			Ftfl_Flash_Command_Sequence( command_array, 4, reinterpret_cast<void*>( write_addr ),
										 size );
	}
	else
	{
		result = 0;
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Flash::Ftfl_Flash_Write_Phrase( const uint8_t* from_ptr,
											uint8_t* to_ptr, unsigned size )
{
	uint8_t result = 0U;
	uint32_t i;
	uint32_t write_addr = 0;
	uint8_t command_array[4 + FTFL_PHRASE_SIZE];
	uint8_t temp_data[FTFL_PHRASE_SIZE];
	uint8_t byte_data_counter = 0;
	uint32_t offset = ( reinterpret_cast<uint32_t>( to_ptr ) ) & 0x00000007;
	bool_t exit_flag = false;

	/* write to address mod 8 correction */
	if ( offset )
	{
		/* align pointer to writable address */
		to_ptr -= offset;

		/* jump over old data */
		byte_data_counter = offset;
	}

	write_addr = unsigned( to_ptr );

	/* heading space should be 0xFF */
	for ( i = 0; i < offset; i++ )
	{
		temp_data[i] = 0xFF;
	}

	while ( size )
	{
		/* move data to temporary char array */
		while ( ( byte_data_counter < FTFL_PHRASE_SIZE ) && size )
		{
			temp_data[byte_data_counter++] = *from_ptr++;
			size--;
		}

		/* remaining space should be 0xFF */
		while ( byte_data_counter < FTFL_PHRASE_SIZE )
		{
			temp_data[byte_data_counter++] = 0xFF;
		}

		/* prepare parameters to program the flash block */
		command_array[0] = FTFL_PROGRAM_PHRASE;
		command_array[1] = static_cast<uint8_t>( write_addr >> 16 );
		command_array[2] = static_cast<uint8_t>( ( write_addr >> 8 ) & 0xFF );
		command_array[3] = static_cast<uint8_t>( write_addr & 0xFF );

		command_array[4] = temp_data[3];
		command_array[5] = temp_data[2];
		command_array[6] = temp_data[1];
		command_array[7] = temp_data[0];

		command_array[8] = temp_data[7];
		command_array[9] = temp_data[6];
		command_array[10] = temp_data[5];
		command_array[11] = temp_data[4];

		/* call flash command sequence function to execute the command */
		result = Ftfl_Flash_Command_Sequence( command_array, 4 + FTFL_PHRASE_SIZE,
											  reinterpret_cast<void*>( write_addr ),
											  FTFL_PHRASE_SIZE );
		if ( result )
		{
			exit_flag = true;
		}

		if ( exit_flag == false )
		{
			/* update destination address for next iteration */
			write_addr += FTFL_PHRASE_SIZE;
			/* init variables for next loop */
			byte_data_counter = 0;
		}
		else
		{
			break;
		}
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_Flash::Ftfl_Flash_Ram_Ready( void )
{
	if ( !( FTFL->FCNFG & FTFL_FCNFG_RAMRDY_MASK ) )
	{
		return ( NULL );
	}

	return ( reinterpret_cast<uint8_t*>( BSP_INTERNAL_PA_RAM_BASE ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t uC_Flash::Ftfl_Flash_Ram_Flush( uint8_t* to_ptr,
										uint8_t* end, uint16_t len )
{
	uint16_t len1 = 0;
	int32_t result = 0;

	len1 = len;
	bool_t exit_flag = false;

	if ( ( unsigned long )to_ptr & ( FTFL_PROGRAM_SECTION_ALIGNMENT - 1 ) )
	{
		exit_flag = true;
	}

	if ( exit_flag == false )
	{
		const uint8_t* ram_end = Ftfl_Flash_Ram_End();
		if ( end > ram_end )
		{
			exit_flag = true;

		}

		if ( exit_flag == false )
		{
			/* Fill in remaining space with 0xFF */
			while ( end != ram_end )
			{
				*end = 0xFF;
				++end;
			}

			uint32_t write_addr = reinterpret_cast<uint32_t>( to_ptr );
			uint8_t command_array[6];

			/* preparing passing parameter to program section */
			command_array[0] = FTFL_PROGRAM_SECTION;
			command_array[1] = static_cast<uint8_t>( ( write_addr >> 16 ) & 0xFF );
			command_array[2] = static_cast<uint8_t>( ( write_addr >> 8 ) & 0xFF );
			command_array[3] = static_cast<uint8_t>( write_addr & 0xFF );
			/* Number of 16 byte entries to write. */
			len = len >> 4;
			len = len >> 8;
			command_array[4] = len;
			len1 = len1 >> 4;
			command_array[5] = len1;

			result = Ftfl_Flash_Command_Sequence( command_array, 6,
												  to_ptr, ( PROG_RAM ) );
		}
		else
		{
			result = -2;
		}
	}
	else
	{
		result = -1;
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const uint8_t* uC_Flash::Ftfl_Flash_Ram_End( void )
{
	return ( reinterpret_cast<const uint8_t*>( BSP_INTERNAL_PA_RAM_BASE + ( PROG_RAM ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
__ramfunc void uC_Flash::Ftfl_Ram_Function( volatile uint8_t* ftfe_fstat_ptr,
											volatile uint32_t* fmc_ptr,
											uint8_t cmd )
{
	/* start flash write */
	*ftfe_fstat_ptr |= FTFL_FSTAT_CCIF_MASK;

	/* wait until execution complete */
	while ( 0 == ( *ftfe_fstat_ptr & FTFL_FSTAT_CCIF_MASK ) )
	{ }

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Fill_Ram_Buffer( const uint8_t* data_ram, NV_CTRL_LENGTH_TD length )
{
	s_acc_ram_ptr = Ftfl_Flash_Ram_Ready();
	uint8_t* address_ptr;

	address_ptr = s_acc_ram_ptr;

	while ( length )
	{
		*address_ptr = *data_ram;
		address_ptr++;
		data_ram++;
		length--;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Flash::uC_Flash( uC_FLASH_CHIP_CONFIG_TD const* chip_config, FTFL_Type* flash_ctrl,
					bool_t synchronus_erase, cback_func_t cback_func, cback_param_t param ) :
	NV_Ctrl(),
	m_chip_cfg( reinterpret_cast<uC_FLASH_CHIP_CONFIG_TD const*>( chip_config ) ),
	m_flash_ctrl_ptr( reinterpret_cast<FTFL_Type*>( flash_ctrl ) ),
	m_erase_val_u8( 0U ),
	m_synchronous( synchronus_erase ),
	m_call_back_ptr( cback_func ),
	m_cback_param( param ),
	m_flash_busy( false )
{
	// Ftfl_Flash_Initialize_Banks();
	// ftfe_flash_ctx* ctx = ftfe_flash_init();
	/// * Initialize the flash configuration register*/
	// m_flash_ctrl->FCNFG&= ~FTFL_FCNFG_EEERDY_MASK;
	// m_flash_ctrl->FCNFG|= FTFL_FCNFG_RAMRDY_MASK;
	// m_flash_ctrl->FCNFG|= FTFL_FCNFG_PFLSH_MASK;
	// m_flash_ctrl->FCNFG&= FTFL_FCNFG_ERSSUSP_MASK;
	// m_flash_ctrl->FCNFG|= FTFL_FCNFG_CCIE_MASK;
	// m_flash_ctrl->FCNFG|= FTFL_FCNFG_RDCOLLIE_MASK;
	// m_flash_ctrl->SR |= ( FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR );
	//
	// Unlock_Flash();
	if ( false == m_synchronous )
	{
		BF_ASSERT( NULL != cback_func );

		new CR_Tasker( &Flash_Async_Task_Static, reinterpret_cast<CR_TASKER_PARAM>( this ),
					   CR_TASKER_IDLE_PRIORITY, "Flash_Routine" );
	}
}

#else
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Flash::Ftfe_Flash_Command_Sequence(	/*volatile ftfe_flash_ctx* dev_spec_ptr,*/
	uint8_t* command_array, uint8_t count,
	void* affected_addr, uint32_t affected_size )
{
	uint8_t fstat;
	uint32_t result;

	void (* RunInRAM)( volatile uint8_t*, volatile uint32_t*, uint8_t ) = Ftfe_Ram_Function;

	/* set the default return as FTFE_OK */
	result = FTFE_OK;

	/* check CCIF bit of the flash status register */
	while ( 0 == ( FTFE->FSTAT & FTFE_FSTAT_CCIF_MASK ) )
	{ }

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

	Disable_Interrupts();

#if defined ( uC_KINETISK66x_USAGE ) || defined ( uC_KINETISK60x_USAGE )
	/* run command and wait for it to finish (must execute from RAM) */
	RunInRAM( &FTFE->FSTAT, &FMC->PFB01CR, command_array[0] );
#else
	/* run command and wait for it to finish (must execute from RAM) */
	RunInRAM( &FTFE->FSTAT, &FMC->PFB0CR, command_array[0] );
#endif
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

	Enable_Interrupts();

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

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Flash::Ftfe_Flash_Prog_Check( const uint8_t* from_ptr,
										  uint32_t expected )
{
	uint32_t result;
	uint32_t write_addr;
	uint8_t command_array[12];

	write_addr = reinterpret_cast<uint32_t>( from_ptr );

	/* If already all 1's then the sector is erased.
	 * Use the normal margin read level. */
	command_array[0] = FTFE_PROGRAM_CHECK;
	command_array[1] = static_cast<uint8_t>( write_addr >> 16 );
	command_array[2] = static_cast<uint8_t>( ( write_addr >> 8 ) & 0xFF );
	command_array[3] = static_cast<uint8_t>( write_addr & 0xFF );
	command_array[4] = 1;
	command_array[5] = 0;
	command_array[6] = 0;
	command_array[7] = 0;

	command_array[8] = ( expected >> 24 ) & 0xFF;
	command_array[9] = ( expected >> 16 ) & 0xFF;
	command_array[10] = ( expected >> 8 ) & 0xFF;
	command_array[11] = expected & 0xFF;

	result = Ftfe_Flash_Command_Sequence( command_array, 12,
										  reinterpret_cast<void*>( write_addr ), 4 );

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Flash::Ftfe_Flash_Erase_Sector( uint8_t* from_ptr,
											uint32_t size )
{
	uint32_t result;
	uint32_t write_addr;
	uint8_t command_array[7];
	bool_t exit_flag = false;

	write_addr = reinterpret_cast<uint32_t>( from_ptr );

	/* If already all 1's then the sector is erased.
	 * Use the normal margin read level. */
	command_array[0] = FTFE_VERIFY_SECTION;
	command_array[1] = static_cast<uint8_t>( write_addr >> 16 );
	command_array[2] = static_cast<uint8_t>( ( write_addr >> 8 ) & 0xFF );
	command_array[3] = static_cast<uint8_t>( write_addr & 0xFF );
	command_array[4] = static_cast<uint8_t>( ( size / 16 ) >> 8 );
	command_array[5] = static_cast<uint8_t>( ( size / 16 ) & 0xFF );
	command_array[6] = 0;
	result = Ftfe_Flash_Command_Sequence( command_array, 7,
										  reinterpret_cast<void*>( write_addr ), size );
	if ( 0 == result )
	{
		exit_flag = true;
	}

	if ( exit_flag == false )
	{
		/* preparing passing parameter to erase a flash block */
		command_array[0] = FTFE_ERASE_SECTOR;
		command_array[1] = static_cast<uint8_t>( write_addr >> 16 );
		command_array[2] = static_cast<uint8_t>( ( write_addr >> 8 ) & 0xFF );
		command_array[3] = static_cast<uint8_t>( write_addr & 0xFF );

		/* call flash command sequence function to execute the command */
		result =
			Ftfe_Flash_Command_Sequence( command_array, 4, reinterpret_cast<void*>( write_addr ),
										 size );
	}
	else
	{
		result = 0;
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Flash::Ftfe_Flash_Write_Phrase( const uint8_t* from_ptr,
											uint8_t* to_ptr, unsigned size )
{
	uint32_t result = 0U;
	uint32_t i;
	uint32_t write_addr = 0U;
	uint8_t command_array[4 + FTFE_PHRASE_SIZE];
	uint8_t temp_data[FTFE_PHRASE_SIZE];
	uint32_t byte_data_counter = 0U;
	uint32_t offset = ( reinterpret_cast<uint32_t>( to_ptr ) ) & 0x00000007;
	bool_t exit_flag = false;

	/* write to address mod 8 correction */
	if ( offset )
	{
		/* align pointer to writable address */
		to_ptr -= offset;

		/* jump over old data */
		byte_data_counter = offset;
	}

	write_addr = unsigned( to_ptr );

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
		command_array[1] = static_cast<uint8_t>( write_addr >> 16 );
		command_array[2] = static_cast<uint8_t>( ( write_addr >> 8 ) & 0xFF );
		command_array[3] = static_cast<uint8_t>( write_addr & 0xFF );

		command_array[4] = temp_data[3];
		command_array[5] = temp_data[2];
		command_array[6] = temp_data[1];
		command_array[7] = temp_data[0];

		command_array[8] = temp_data[7];
		command_array[9] = temp_data[6];
		command_array[10] = temp_data[5];
		command_array[11] = temp_data[4];

		/* call flash command sequence function to execute the command */
		result = Ftfe_Flash_Command_Sequence( command_array, 4 + FTFE_PHRASE_SIZE,
											  reinterpret_cast<void*>( write_addr ),
											  FTFE_PHRASE_SIZE );
		if ( result )
		{
			exit_flag = true;
		}

		if ( exit_flag == false )
		{
			/* update destination address for next iteration */
			write_addr += FTFE_PHRASE_SIZE;
			/* init variables for next loop */
			byte_data_counter = 0;
		}
		else
		{
			break;
		}
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_Flash::Ftfe_Flash_Ram_Ready( void )
{
	if ( !( FTFE->FCNFG & FTFE_FCNFG_RAMRDY_MASK ) )
	{
		return ( NULL );
	}

	return ( reinterpret_cast<uint8_t*>( BSP_INTERNAL_PA_RAM_BASE ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t uC_Flash::Ftfe_Flash_Ram_Flush( uint8_t* to_ptr,
										uint8_t* end, uint16_t len )
{
	uint16_t len1 = 0;
	int32_t result = 0;

	len1 = len;
	bool_t exit_flag = false;

	if ( ( unsigned long )to_ptr & ( FTFE_PROGRAM_SECTION_ALIGNMENT - 1 ) )
	{
		exit_flag = true;
	}

	if ( exit_flag == false )
	{
		const uint8_t* ram_end = Ftfe_Flash_Ram_End();
		if ( end > ram_end )
		{
			exit_flag = true;

		}

		if ( exit_flag == false )
		{
			/* Fill in remaining space with 0xFF */
			while ( end != ram_end )
			{
				*end = 0xFF;
				++end;
			}

			uint32_t write_addr = reinterpret_cast<uint32_t>( to_ptr );
			uint8_t command_array[6];

			/* preparing passing parameter to program section */
			command_array[0] = FTFE_PROGRAM_SECTION;
			command_array[1] = static_cast<uint8_t>( ( write_addr >> 16 ) & 0xFF );
			command_array[2] = static_cast<uint8_t>( ( write_addr >> 8 ) & 0xFF );
			command_array[3] = static_cast<uint8_t>( write_addr & 0xFF );
			/* Number of 16 byte entries to write. */
			len = len >> 4;
			len = len >> 8;
			command_array[4] = len;
			len1 = len1 >> 4;
			command_array[5] = len1;

			result = Ftfe_Flash_Command_Sequence( command_array, 6,
												  to_ptr, ( PROG_RAM ) );
		}
		else
		{
			result = -2;
		}
	}
	else
	{
		result = -1;
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const uint8_t* uC_Flash::Ftfe_Flash_Ram_End( void )
{
	return ( reinterpret_cast<const uint8_t*>( BSP_INTERNAL_PA_RAM_BASE + ( PROG_RAM ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
__ramfunc void uC_Flash::Ftfe_Ram_Function( volatile uint8_t* ftfe_fstat_ptr,
											volatile uint32_t* fmc_ptr,
											uint8_t cmd )
{
	/* start flash write */
	*ftfe_fstat_ptr |= FTFE_FSTAT_CCIF_MASK;

	/* wait until execution complete */
	while ( 0 == ( *ftfe_fstat_ptr & FTFE_FSTAT_CCIF_MASK ) )
	{ }

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Fill_Ram_Buffer( const uint8_t* data_ram, NV_CTRL_LENGTH_TD length )
{
	s_acc_ram_ptr = Ftfe_Flash_Ram_Ready();
	uint8_t* address_ptr;

	address_ptr = s_acc_ram_ptr;

	while ( length )
	{
		*address_ptr = *data_ram;
		address_ptr++;
		data_ram++;
		length--;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Flash::uC_Flash( uC_FLASH_CHIP_CONFIG_TD const* chip_config, FTFE_Type* flash_ctrl,
					bool_t synchronus_erase, cback_func_t cback_func, cback_param_t param ) :
	NV_Ctrl(),
	m_chip_cfg( reinterpret_cast<uC_FLASH_CHIP_CONFIG_TD const*>( chip_config ) ),
	m_flash_ctrl_ptr( reinterpret_cast<FTFE_Type*>( flash_ctrl ) ),
	m_erase_val_u8( 0U ),
	m_synchronous( synchronus_erase ),
	m_call_back_ptr( cback_func ),
	m_cback_param( param ),
	m_flash_busy( false )
{
	// Ftfe_Flash_Initialize_Banks();
	// ftfe_flash_ctx* ctx = ftfe_flash_init();
	/// * Initialize the flash configuration register*/
	// m_flash_ctrl->FCNFG&= ~FTFE_FCNFG_EEERDY_MASK;
	// m_flash_ctrl->FCNFG|= FTFE_FCNFG_RAMRDY_MASK;
	// m_flash_ctrl->FCNFG|= FTFE_FCNFG_PFLSH_MASK;
	// m_flash_ctrl->FCNFG&= FTFE_FCNFG_ERSSUSP_MASK;
	// m_flash_ctrl->FCNFG|= FTFE_FCNFG_CCIE_MASK;
	// m_flash_ctrl->FCNFG|= FTFE_FCNFG_RDCOLLIE_MASK;
	// m_flash_ctrl->SR |= ( FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR );
	//
	// Unlock_Flash();
	if ( false == m_synchronous )
	{
		BF_ASSERT( NULL != cback_func );

		new CR_Tasker( &Flash_Async_Task_Static, reinterpret_cast<CR_TASKER_PARAM>( this ),
					   CR_TASKER_IDLE_PRIORITY, "Flash_Routine" );
	}
}

#endif
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
									 bool use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;
	bool good_data;

	if ( Check_Range( address, length ) && ( length != 0U ) )
	{
		good_data = Read_Now( data, address, length );

		if ( good_data == true )
		{
			return_status = NV_Ctrl::SUCCESS;
		}
		else
		{
			return_status = NV_Ctrl::DATA_ERROR;
		}
	}
	else
	{
		return_status = NV_Ctrl::INVALID_ADDRESS;
	}
	if ( use_protection )
	{}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Flash::Get_Sector( uint32_t address ) const
{
	uint32_t page_index = 0U;
	uint32_t test_address;

	test_address = INTERNAL_FLASH_START_ADDRESS + INTERNAL_FLASH_PAGE_SIZES[page_index].sector_size;
	while ( ( address >= test_address ) &&
			( INTERNAL_FLASH_PAGE_SIZES[page_index].sector_size != 0U ) )
	{
		page_index++;
		test_address += INTERNAL_FLASH_PAGE_SIZES[page_index].sector_size;
	}

	return ( INTERNAL_FLASH_PAGE_SIZES[page_index].sector_no );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Write( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
									  bool use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;
	bool good_data;
	NV_CTRL_LENGTH_TD rem_length = 0;
	int32_t count = 0;

	if ( length > FTFE_FLASH_RAM_BUFFER )
	{
		count = ( length / FTFE_FLASH_RAM_BUFFER ) + 1;
	}
	else
	{
		count = ( length / FTFE_FLASH_RAM_BUFFER );
	}
	if ( Check_Range( address, length ) )
	{
		do
		{
			rem_length = ( length > FTFE_FLASH_RAM_BUFFER )? FTFE_FLASH_RAM_BUFFER:length;
			good_data = Write_Now( data, address, rem_length );

			if ( good_data == true )
			{
				good_data = Check_Data( data, address, rem_length );
			}
			length -= PROG_RAM;
			address += PROG_RAM;
			data += PROG_RAM;
			count--;
		}while ( count > 0 );
		if ( good_data == true )
		{
			return_status = NV_Ctrl::SUCCESS;
		}
		else
		{
			return_status = NV_Ctrl::DATA_ERROR;
		}
	}
	else
	{
		return_status = NV_Ctrl::INVALID_ADDRESS;
	}

	if ( use_protection )
	{}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Flash::Write_Now( uint8_t* data, uint32_t dest_address, NV_CTRL_LENGTH_TD length )
{
	bool_t return_val = false;
	bool_t status = false;
	uint32_t address = dest_address;
	bool_t erase_status = false;
	uint32_t len_modulo = 0U;

	erase_status = Erase_Page( address );
#if defined ( uC_KINETISK60DNx_USAGE )
	s_acc_ram_ptr = Ftfl_Flash_Ram_Ready();
#else
	s_acc_ram_ptr = Ftfe_Flash_Ram_Ready();
#endif
	Fill_Ram_Buffer( const_cast<uint8_t*>( data ),
					 length );
	len_modulo = length % BYTE_ALIGNMENT;
	// need to make length 16 byte aligned.
	if ( len_modulo )
	{
		length = length + ( BYTE_ALIGNMENT - len_modulo );
	}

	s_acc_ram_ptr = s_acc_ram_ptr + length;

	if ( erase_status == true )
	{
#if defined ( uC_KINETISK60DNx_USAGE )
		status = Ftfl_Flash_Ram_Flush( reinterpret_cast<uint8_t*>( dest_address ),
									   s_acc_ram_ptr, uint16_t( length ) );
#else
		status = Ftfe_Flash_Ram_Flush( reinterpret_cast<uint8_t*>( dest_address ),
									   s_acc_ram_ptr, uint16_t( length ) );
#endif
		if ( !status )
		{
			return_val = true;
		}
		else
		{
			return_val = false;
		}
	}
	else
	{
		return_val = false;
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
											  NV_CTRL_LENGTH_TD length )
{
	*checksum_dest = 0U;
	if ( address == length )
	{}
	// return ( NV_Ctrl::SUCCESS );
	return ( NV_Ctrl::DATA_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Erase( uint32_t address, NV_CTRL_LENGTH_TD length,
									  uint8_t erase_data, bool protected_data )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;
	bool_t erase_status = false;

	m_erase_val_u8 = erase_data;
	// while (length >= 0)
	{
		erase_status = Erase_Page( address );
		if ( true == erase_status )
		{
			return_status = NV_Ctrl::SUCCESS;
		}
		// length -= PROG_RAM;
		// address += PROG_RAM;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Flash::Erase_Page( uint32_t address )
{
	bool status;
	uint8_t* x = reinterpret_cast<uint8_t*>( address );

#if defined ( uC_KINETISK60DNx_USAGE )
	if ( Ftfl_Flash_Erase_Sector( x, ( PROG_RAM ) ) )
#else
	if ( Ftfe_Flash_Erase_Sector( x, ( PROG_RAM ) ) )
#endif
	{

		status = false;
	}
	else
	{
		status = true;
	}
	return ( status );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Flash::uc_flash_status_enum_t uC_Flash::Wait_For_Complete( void ) const
{
	uc_flash_status_enum_t status;

	Push_TGINT();
	/* Wait for a Flash operation to complete or a TIMEOUT to occur */
	do
	{
		status = Get_Status();
		uC_Watchdog::Force_Feed_Dog();
	} while ( status == UC_FLASH_BUSY );

	Pop_TGINT();
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Flash::uc_flash_status_enum_t uC_Flash::Get_Status( void ) const
{
	uc_flash_status_enum_t status = UC_FLASH_COMPLETE;

	/* CCIF flag should be 1 for flash command write sequence*/
#if defined ( uC_KINETISK60DNx_USAGE )
	if ( ( m_flash_ctrl_ptr->FSTAT & FTFL_FSTAT_CCIF_MASK ) != FTFL_FSTAT_CCIF_MASK )
#else
	if ( ( m_flash_ctrl_ptr->FSTAT & FTFE_FSTAT_CCIF_MASK ) != FTFE_FSTAT_CCIF_MASK )
#endif
	{
		status = UC_FLASH_BUSY;
	}
	else
	{
		/* Check for ACCERRR and FPVIOL bit in FTAT to be zero*/
		volatile uint32_t SR;
		SR = m_flash_ctrl_ptr->FSTAT;
		if ( SR & FLASH_ERR )
		{
			status = UC_FLASH_ERROR;
		}
		else
		{
			// if ( 1/*( m_flash_ctrl->SR & FLASH_SR_WRPERR ) != 0U */)
			// {
			// status = uC_FLASH_ERROR_WRP;
			// }
			// else
			// {
			// status = uC_FLASH_COMPLETE;
			// }
			status = UC_FLASH_COMPLETE;
		}
	}
	// m_flash_ctrl->FSTAT|= FLASH_ERR;
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Erase_All( void )
{
	return ( Erase( m_chip_cfg->start_address,
					( m_chip_cfg->end_address - m_chip_cfg->start_address ),
					uC_FLASH_DEFAULT_ERASE_VAL, false ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Flash::Read_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length ) const
{
	BF_Lib::Copy_String( data, reinterpret_cast<const uint8_t*>( address ), length );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Flash::Check_Data( uint8_t* check_data, uint32_t address, uint32_t length ) const
{
	bool good_data = true;
	uint8_t* src_data;

	src_data = ( uint8_t* )address;

	while ( ( length > 0U ) && ( good_data == true ) )
	{
		length--;
		if ( *check_data != *src_data )
		{
			good_data = false;
		}
		if ( check_data != &m_erase_val_u8 )
		{
			check_data++;
		}
		src_data++;
	}

	return ( good_data );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Flash::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const
{
	bool range_good = false;
	NV_CTRL_ADDRESS_TD end_address;

	end_address = m_chip_cfg->end_address;

	if ( ( address >= m_chip_cfg->start_address ) && ( ( address + length ) <= ( end_address + 1U ) ) )
	{
		range_good = true;
	}

	return ( range_good );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Unlock_Flash( void )
{
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule: 2-13-3
	 * @n PCLint: Note 1960: Violates MISRA C++ 2008 Required Rule 2-13-3, Unsigned octal and
	 * hexadecimal literals require a 'U' suffix
	 * @n Justification: The ST supplied includes where this #define resides shall not be changed.
	 */
	/*lint -e{1960} */
	// if (1 /*( m_flash_ctrl->CR & FLASH_CR_LOCK ) != 0U */)
	// {
	////		m_flash_ctrl->KEYR = FLASH_KEY1;
	////		m_flash_ctrl->KEYR = FLASH_KEY2;
	// }
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Lock_Flash( void )
{
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule: 2-13-3
	 * @n PCLint: Note 1960: Violates MISRA C++ 2008 Required Rule 2-13-3, Unsigned octal and
	 * hexadecimal literals require a 'U' suffix
	 * @n Justification: The ST supplied includes where this #define resides shall not be changed.
	 */
	/*lint -e{1960} */
	// m_flash_ctrl->CR |= FLASH_CR_LOCK;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#define MICROSECONDS_ADJUSTMENT         1000U
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Flash::Write_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address,
							   NV_CTRL_LENGTH_TD length, bool include_erase_time ) const
{
	uint32_t write_time;
	uint32_t byte_write_time;
	uint32_t page_index;
	uint32_t test_address;
	uint32_t erased_values;	// To count the number of values that are actually erased and don't need

	// programming.

	write_time = 0U;
	page_index = 0U;
	test_address = INTERNAL_FLASH_START_ADDRESS + INTERNAL_FLASH_PAGE_SIZES[page_index].sector_size;
	while ( INTERNAL_FLASH_PAGE_SIZES[page_index].sector_size != 0U )
	{
		if ( ( address <= test_address ) && ( ( address + length ) > test_address ) )
		{
			write_time += ( SECTOR_ERASE_TIME_BASELINE_MS *
							( INTERNAL_FLASH_PAGE_SIZES[page_index].sector_size /
							  INTERNAL_FLASH_PAGE_SIZES[0].sector_size ) );
		}
		page_index++;
		test_address += INTERNAL_FLASH_PAGE_SIZES[page_index].sector_size;
	}

	// Check to see if we are calculating the erase time or a write time.
	if ( data != NULL )
	{
		erased_values = 0U;
		for ( uint32_t i = 0U; i < length; i++ )
		{
			if ( data[i] == FLASH_ERASED_VALUE )
			{
				erased_values++;
			}
		}

		byte_write_time = BYTE_WRITE_TIME_US * ( length - erased_values );
		if ( byte_write_time < MICROSECONDS_ADJUSTMENT )
		{
			byte_write_time = MICROSECONDS_ADJUSTMENT;
		}
		write_time += Round_Div( byte_write_time, MICROSECONDS_ADJUSTMENT );// Because the byte write
																			// time is in
																			// microseconds we have
																			// to adjust it down.
	}
	else
	{
		// because we will erase the page we are on as well as the next page if we get there.
		write_time += ( SECTOR_ERASE_TIME_BASELINE_MS
						* ( Get_Page_Size( address ) / INTERNAL_FLASH_PAGE_SIZES[0].sector_size ) );
	}

	return ( write_time );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Flash_Async_Task_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{

	( ( uC_Flash* )param )->Flash_Async_Task();

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Flash_Async_Task( void )
{
	nv_status_t result;

	if ( true == m_flash_busy )
	{
		uc_flash_status_enum_t status = Get_Status();
		if ( status != UC_FLASH_BUSY )
		{
			// m_flash_ctrl->CR &= ( ~FLASH_CR_SER );
			// m_flash_ctrl->CR &= SECTOR_MASK;
			m_flash_busy = false;
			// Check for Error
			// if ( 1 /*FLASH->SR & FLASH_PGERR*/ )
			// {
			////Reset Error Flags
			////FLASH->SR |= FLASH_PGERR;
			// }
			Lock_Flash();

			if ( status == UC_FLASH_COMPLETE )
			{
				result = SUCCESS;
			}
			else
			{
				result = DATA_ERROR;
			}
			m_call_back_ptr( m_cback_param, result );
		}
	}
}

}
/*lint +e1924 */
