/**
 *****************************************************************************************
 *	@file		uC_Interrupt.h
 *
 *	@brief		Contains the uC_Interrupt Class
 *
 *	@copyright	2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_INTERRUPT_H
	#define uC_INTERRUPT_H

#include "uC_Base.h"
#include "freertos/portmacro.h"
#include "freertos/FreeRTOS.h"

/**
 ************************************************************************
 * @brief   Defines used for the interrupt configuration
 ************************************************************************
 */
#ifdef DEBUG_INT_ALLOC_DECISIONS
#define ALCHLOG( ... ) ESP_EARLY_LOGD( TAG, __VA_ARGS__ )
#else
# define ALCHLOG( ... ) do {} while ( 0 )
#endif

#if CONFIG_FREERTOS_CORETIMER_0
#define INT6RES INTDESC_RESVD
#else
#define INT6RES INTDESC_SPECIAL
#endif

#if CONFIG_FREERTOS_CORETIMER_1
#define INT15RES INTDESC_RESVD
#else
#define INT15RES INTDESC_SPECIAL
#endif

/**
 ****************************************************************************************
 * @brief Interrupt flags
 *
 ****************************************************************************************
 */
enum int_desc_flag_t
{
	INTDESC_NORMAL = 0,		///< Normal interrupt.
	INTDESC_RESVD,			///< Reserved interrupt.
	INTDESC_SPECIAL			///< For xtensa timers / software ints
};

/**
 ****************************************************************************************
 * @brief Interrupt level types
 *
 ****************************************************************************************
 */
enum int_type_t
{
	INTTP_LEVEL = 0,		///< Interrupt type is level triggered
	INTTP_EDGE,				///< Interrupt type is Edge triggered
	INTTP_NA
};

/**
 ****************************************************************************************
 * @brief Interrupt Number Definition used as dummy
 *
 ****************************************************************************************
 */
enum IRQn_Type
{
	SysTick_IRQn = -1,						/*!< 15 Cortex-M0 System Tick Interrupt                              */
	MAX_IRQTYPE = 0		///< Dummy Interript defination defined
};

/**
 ****************************************************************************************
 * @brief Interrupt configuration structure
 *
 ****************************************************************************************
 */
struct int_desc_t
{
	int32_t level;
	int_type_t type;
	int_desc_flag_t cpuflags[2];
};

/**
 ****************************************************************************************
 * @brief Interrupt shared vector configuration structure
 *
 ****************************************************************************************
 */
struct shared_vector_desc_t
{
	int32_t disabled : 1;
	int32_t source : 8;
	volatile uint32_t* statusreg;
	uint32_t statusmask;
	intr_handler_t isr;
	void* arg;
	shared_vector_desc_t* next;
};

/**
 ****************************************************************************************
 * @brief Interrupt vector configuration structure
 *
 ****************************************************************************************
 */
struct vector_desc_t
{
	int32_t flags : 16;							///< OR of VECDESC_FLAG_* defines
	uint32_t cpu : 1;							///< CPU number.
	uint32_t intno : 5;							///< Interrupt number
	int32_t source : 8;							///< Interrupt mux flags, used when not shared
	shared_vector_desc_t* shared_vec_info;		///< used when VECDESC_FL_SHARED
	vector_desc_t* next;
};

/**
 ****************************************************************************************
 * @brief Interrupt handler information structure
 *
 ****************************************************************************************
 */
struct intr_handle_data_t
{
	vector_desc_t* vector_desc;		///< Vector details.
	shared_vector_desc_t* shared_vector_desc;	///< Shared vector details.
};

/**
 ****************************************************************************************
 * @brief Interrupt Non-shared vector configuration structure
 *
 ****************************************************************************************
 */
struct non_shared_isr_arg_t
{
	intr_handler_t isr;
	void* isr_arg;
	int32_t source;
};

/**
 ***************************************************************
 * @brief	Interrupt handler table and unhandled uinterrupt routine. Duplicated
 *			From xtensa_intr.c... it's supposed to be private, but we need to look
 *			Into it in order to see if someone allocated an int32_t using
 *			xt_set_interrupt_handler
 *
 ***************************************************************
 */
struct xt_handler_table_entry
{
	void* handler;		///< Interrupt handler
	void* arg;			///< Pointer
};

/*
 *****************************************************************************************
 *		Extern variable or Global variable
 *****************************************************************************************
 */
extern xt_handler_table_entry _xt_interrupt_table[XCHAL_NUM_INTERRUPTS * portNUM_PROCESSORS];

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
const static int_desc_t int_desc[32] = {
	{
		1, INTTP_LEVEL,
		{ INTDESC_RESVD, INTDESC_RESVD }
	},		///< 0
	{
		1, INTTP_LEVEL,
		{INTDESC_RESVD, INTDESC_RESVD }
	},		///< 1
	{
		1, INTTP_LEVEL,
		{INTDESC_NORMAL, INTDESC_NORMAL}
	},		///< 2
	{
		1, INTTP_LEVEL,
		{INTDESC_NORMAL, INTDESC_NORMAL}
	},		///< 3
	{
		1, INTTP_LEVEL,
		{INTDESC_RESVD, INTDESC_NORMAL}
	},		///< 4
	{
		1, INTTP_LEVEL,
		{INTDESC_RESVD, INTDESC_RESVD }
	},		///< 5
	{
		1, INTTP_NA,
		{INT6RES, INT6RES }
	},		///< 6
	{
		1, INTTP_NA,
		{INTDESC_SPECIAL, INTDESC_SPECIAL}
	},		///< 7
	{
		1, INTTP_LEVEL,
		{INTDESC_RESVD, INTDESC_RESVD }
	},		///< 8
	{
		1, INTTP_LEVEL,
		{INTDESC_NORMAL, INTDESC_NORMAL}
	},		///< 9
	{
		1, INTTP_EDGE,
		{INTDESC_NORMAL, INTDESC_NORMAL}
	},		///< 10
	{
		3, INTTP_NA,
		{INTDESC_SPECIAL, INTDESC_SPECIAL}
	},		///< 11
	{
		1, INTTP_LEVEL,
		{INTDESC_NORMAL, INTDESC_NORMAL}
	},		///< 12
	{
		1, INTTP_LEVEL,
		{INTDESC_NORMAL, INTDESC_NORMAL}
	},		///< 13
	{
		7, INTTP_LEVEL,
		{INTDESC_RESVD, INTDESC_RESVD }
	},		///< 14, NMI
	{
		3, INTTP_NA,
		{INT15RES, INT15RES }
	},	///< 15
	{
		5, INTTP_NA,
		{INTDESC_SPECIAL, INTDESC_SPECIAL}
	},		///< 16
	{
		1, INTTP_LEVEL,
		{INTDESC_NORMAL, INTDESC_NORMAL}
	},		///< 17
	{
		1, INTTP_LEVEL,
		{INTDESC_NORMAL, INTDESC_NORMAL}
	},		///< 18
	{
		2, INTTP_LEVEL,
		{INTDESC_NORMAL, INTDESC_NORMAL}
	},		///< 19
	{
		2, INTTP_LEVEL,
		{INTDESC_NORMAL, INTDESC_NORMAL}
	},	///< 20
	{
		2, INTTP_LEVEL,
		{INTDESC_NORMAL, INTDESC_NORMAL}
	},		///< 21
	{
		3, INTTP_EDGE,
		{INTDESC_RESVD, INTDESC_NORMAL}
	},		///< 22
	{
		3, INTTP_LEVEL,
		{INTDESC_NORMAL, INTDESC_NORMAL}
	},		///< 23
	{
		4, INTTP_LEVEL,
		{INTDESC_RESVD, INTDESC_NORMAL}
	},		///< 24
	{
		4, INTTP_LEVEL,
		{INTDESC_RESVD, INTDESC_RESVD }
	},		///< 25
	{
		5, INTTP_LEVEL,
		{INTDESC_RESVD, INTDESC_RESVD }
	},		///< 26
	{
		3, INTTP_LEVEL,
		{INTDESC_RESVD, INTDESC_RESVD }
	},		///< 27
	{
		4, INTTP_EDGE,
		{INTDESC_NORMAL, INTDESC_NORMAL}
	},		///< 28
	{
		3, INTTP_NA,
		{INTDESC_SPECIAL, INTDESC_SPECIAL}
	},		///< 29
	{
		4, INTTP_EDGE,
		{INTDESC_RESVD,  INTDESC_RESVD }
	},	///< 30
	{
		5, INTTP_LEVEL,
		{INTDESC_RESVD,  INTDESC_RESVD }
	},		///< 31
};

static const uint8_t uC_INT_HW_PRIORITY_0 = 0U;		///< Highest priority
static const uint8_t uC_INT_HW_PRIORITY_1 = 1U;
static const uint8_t uC_INT_HW_PRIORITY_2 = 2U;
static const uint8_t uC_INT_HW_PRIORITY_3 = 3U;
static const uint8_t uC_INT_HW_PRIORITY_4 = 4U;
static const uint8_t uC_INT_HW_PRIORITY_5 = 5U;
static const uint8_t uC_INT_HW_PRIORITY_6 = 6U;
static const uint8_t uC_INT_HW_PRIORITY_7 = 7U;
static const uint8_t uC_INT_HW_PRIORITY_8 = 8U;
static const uint8_t uC_INT_HW_PRIORITY_9 = 9U;
static const uint8_t uC_INT_HW_PRIORITY_10 = 10U;
static const uint8_t uC_INT_HW_PRIORITY_11 = 11U;
static const uint8_t uC_INT_HW_PRIORITY_12 = 12U;
static const uint8_t uC_INT_HW_PRIORITY_13 = 13U;
static const uint8_t uC_INT_HW_PRIORITY_14 = 14U;
static const uint8_t uC_INT_HW_PRIORITY_15 = 15U;	///< Lowest Priority
static const uint8_t uC_INT_HW_MAX_PRIORITY = 16U;

static const uint8_t uC_INT_HW_PRIORITY_LOWEST = ( uC_INT_HW_MAX_PRIORITY - 1U );
/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */
typedef void INT_CALLBACK_FUNC ( void );

/**
 ****************************************************************************************
 * @brief			Provides a interrupt interface. Class Declaration
 ****************************************************************************************
 */
class uC_Interrupt
{
	public:
		/**
		 * @brief							Constructor to create instance of uC_Interrupt class.
		 * @param[in] void					None
		 * @return							None
		 */
		uC_Interrupt( void ) {}

		/**
		 * @brief							Destructor to delete the uC_Interrupt instance
		 * @n								when it goes out of scope
		 * @param[in] void					None
		 * @return							None
		 */
		~uC_Interrupt( void ) {}

		/**
		 * @brief							vector set function
		 * @param[in] vector_callback		Vector Callback function pointer
		 * @param[in] vector_number			Vector number of type IRQn_Type
		 * @param[in] priority				Interrupt priority
		 * @return bool						True or False
		 */
		static bool Set_Vector( INT_CALLBACK_FUNC* vector_callback, IRQn_Type vector_number, uint16_t priority );

		/**
		 * @brief							Vector clear function
		 * @param[in] handler				Interrupt function handler
		 * @return None						None
		 */
		static void Clear_Vector( intr_handle_t handle = nullptr );

		/**
		 * @brief							Function to check whether vector is empty or not
		 * @param[in] None					None
		 * @return bool						Returns true is vector is empty if not returns false
		 */
		static bool Is_Vector_Empty( intr_handle_t handle = nullptr );

		/**
		 * @brief							Function used to to clear the interrupt or vector
		 * @param[in] arg					Parameter pointer passed to interrupt handler function
		 * @return void						Returns a vector_desc entry of an interrupt
		 */
		static void esp_intr_free_cb( void* arg );

	private:
		/**
		 * @brief							Function to enable the interrupt
		 * @param[in] vector_number			Vector number of type IRQn_Type
		 * @param[in] handler				Interrupt function handler
		 * @return bool						true if interrupt is enabled or else false will be returned
		 */
		static bool Enable_Int( IRQn_Type vector_number = MAX_IRQTYPE, intr_handle_t handle = nullptr );

		/**
		 * @brief							Function to disable the interrupt
		 * @param[in] vector_number			Vector number of type IRQn_Type
		 * @param[in] handler				Interrupt function handler
		 * @return bool						true if interrupt disabled or else false will be returned
		 */
		static bool Disable_Int( IRQn_Type vector_number = MAX_IRQTYPE, intr_handle_t handle = nullptr );

		/**
		 * @brief							Function to clear the interrupt
		 * @param[in] vector_number			Vector number of type IRQn_Type
		 * @param[in] handler				Interrupt function handler
		 * @param[in] arg					Parameter pointer passed to interrupt handler function
		 * @param[in] flags					Parameter pointer passed to interrupt handler function
		 * @param[in] cpu					Current CPU number
		 * @param[in] intr					Interrupt number
		 * @return							none
		 */
		static void Clear_Int( IRQn_Type vector_number = MAX_IRQTYPE, intr_handler_t handler = nullptr,
							   int32_t intr = 0, void* arg = nullptr, uint32_t cpu = 0, int32_t flags = 0 );

		/**
		 * @brief							Function to insert a vector
		 * @param[in] to_insert				Vector configurtaion used for inseration
		 * @return							none
		 */
		static void insert_vector_desc( vector_desc_t* to_insert );

		/**
		 * @brief							Common shared isr handler. Chain-call all ISRs.
		 * @param[in] arg					Parameter pointer passed to interrupt handler function
		 * @return							none
		 */
		static void shared_intr_isr( void* arg );

		/**
		 * @brief							Locate a free interrupt compatible with the flags given.
		 * @param[in] flags					Parameter pointer passed to interrupt handler function
		 * @param[in] cpu					Current CPU number
		 * @param[in] force					force checking a certain interrupt (can be -1, or 0-31)
		 * @param[in] source				Source of Interrupt
		 * @return int32_t					Interrupt value
		 */
		static int32_t get_available_int( int32_t flags, uint32_t cpu, int32_t force, int32_t source );

		/**
		 * @brief							Function to check whether the vector is usable or not
		 * @param[in] flags					Parameter pointer passed to interrupt handler function
		 * @param[in] cpu					Current CPU number
		 * @param[in] force					force checking a certain interrup (can be -1, or 0-31)
		 * @return bool						Return true is the vector is usable or else false will be returned.
		 */
		static bool is_vect_desc_usable( vector_desc_t* vd, int32_t flags, uint32_t cpu, int32_t force );

		/**
		 * @brief							Function to check if handler for interrupt is not the default unhandled
		 * @n								interrupt handler
		 * @param[in] intr					Interrupt number
		 * @param[in] cpu					Current CPU number
		 * @return bool						Returns true if handler for interrupt is not the default unhandled interrupt
		 * handler
		 */
		static bool int_has_handler( int32_t intr, uint32_t cpu );

		/**
		 * @brief							Function to return a vector_desc entry for an source
		 * @param[in] source				Interrupt source
		 * @param[in] cpu					the cpu parameter is used to tell GPIO_INT and GPIO_NMI from different CPUs
		 * @return vector_desc_t			Returns a vector_desc of a source
		 */
		static vector_desc_t* find_desc_for_source( int32_t source, uint32_t cpu );

		/**
		 * @brief							Function to return a vector_desc entry for an intno/cpu.
		 * @n								Either returns a preexisting one or allocates a new one and inserts
		 * @n								It into the list. Returns NULL on malloc fail.
		 * @param[in] intno					Interrupt number
		 * @param[in] cpu					Current CPU number
		 * @return vector_desc_t			Returns a vector_desc entry of a interrupt
		 */
		static vector_desc_t* get_desc_for_int( int32_t intno, uint32_t cpu );

		/**
		 * @brief							Function used to return a vector_desc entry for an intno/cpu, or NULL if
		 * @n								none exists.
		 * @param[in] intno					Interrupt number
		 * @param[in] cpu					Current CPU number
		 * @return vector_desc_t			Returns a vector_desc entry of an interrupt
		 */
		static vector_desc_t* find_desc_for_int( int32_t intno, uint32_t cpu );

};

#endif

