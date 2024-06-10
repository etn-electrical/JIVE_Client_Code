/* sys.c */

#include "PTP_Slave_If_C_Connector.h"
#include "../PTP_Debug_C_Connector.h"

void displayStats( const PtpClock*ptpClock )
{
	const char*s;
	unsigned char*uuid;
	char sign;

	uuid = ( unsigned char* ) ptpClock->parentDS.parentPortIdentity.clockIdentity;

	/* Master clock UUID */
	printf( "%02X%02X:%02X%02X:%02X%02X:%02X%02X\n",
			uuid[0], uuid[1],
			uuid[2], uuid[3],
			uuid[4], uuid[5],
			uuid[6], uuid[7] );

	switch ( ptpClock->portDS.portState )
	{
		case PTP_INITIALIZING:
			s = "init";
			break;

		case PTP_FAULTY:
			s = "faulty";
			break;

		case PTP_LISTENING:
			s = "listening";
			break;

		case PTP_PASSIVE:
			s = "passive";
			break;

		case PTP_UNCALIBRATED:
			s = "uncalibrated";
			break;

		case PTP_SLAVE:
			s = "slave";
			break;

		case PTP_PRE_MASTER:
			s = "pre master";
			break;

		case PTP_MASTER:
			s = "master";
			break;

		case PTP_DISABLED:
			s = "disabled";
			break;

		default:
			s = "?";
			break;
	}

	/* State of the PTP */
	printf( "state: %s\n", s );

	/* One way delay */
	switch ( ptpClock->portDS.delayMechanism )
	{
		case E2E:
			printf( "path delay: %d nsec\n", ptpClock->currentDS.meanPathDelay.nanoseconds );
			break;

		case P2P:
			printf( "path delay: %d nsec\n", ptpClock->portDS.peerMeanPathDelay.nanoseconds );
			break;

		default:
			printf( "path delay: unknown\n" );
			/* none */
			break;
	}

	/* Offset from master */
	if ( ptpClock->currentDS.offsetFromMaster.seconds )
	{
		printf( "offset: %d sec\n", ptpClock->currentDS.offsetFromMaster.seconds );
	}
	else
	{
		printf( "offset: %d nsec\n", ptpClock->currentDS.offsetFromMaster.nanoseconds );
	}

	/* Observed drift from master */
	sign = ' ';
	if ( ptpClock->observedDrift > 0 )
	{
		sign = '+';
	}
	if ( ptpClock->observedDrift < 0 )
	{
		sign = '-';
	}

	printf( "drift: %c%d.%03d ppm\n", sign, abs( ptpClock->observedDrift / 1000 ),
			abs( ptpClock->observedDrift % 1000 ) );
}

void getTime( TimeInternal*time )
{
    Ptp_Interface_Get_Time( &(time->seconds), &(time->nanoseconds) );
}

void setTime( const TimeInternal*time )
{
    Ptp_Interface_Set_Time( time->seconds, time->nanoseconds );
    
	/* ETH_PTPTime_SetTime(&ts); commented by K. To replace with uC_PTP declarations */
	DBG( "resetting system clock to %d sec %d nsec\n", time->seconds, time->nanoseconds );
}

void updateTime( const TimeInternal*time )
{
	DBGV( "updateTime: %d sec %d nsec\n", time->seconds, time->nanoseconds );
    Ptp_Interface_Update_Offset( time->seconds, time->nanoseconds );

	/*ETH_PTPTime_UpdateOffset(&timeoffset);commented by K. To replace with uC_PTP declarations */
	DBGV( "updateTime: updated\n" );
}

uint32_t getRand( uint32_t randMax )
{
	return ( rand() % randMax );
}

bool adjFreq( int32_t adj )
{
	DBGV( "adjFreq %d\n", adj );

	if ( adj > ADJ_FREQ_MAX )
	{
		adj = ADJ_FREQ_MAX;
	}
	else if ( adj < -ADJ_FREQ_MAX )
	{
		adj = -ADJ_FREQ_MAX;
	}

	/* Fine update method */
    Ptp_Interface_Adj_Frq( adj );
	//ETH_PTPTime_AdjFreq(adj);commented by K. To replace with uC_PTP declarations

	return ( TRUE );
}

void sys_timer_new( const PtpClock*ptpClock )
{
    Ptp_Interface_Init_Timers( ptpClock );
}

void sys_timer_stop( int32_t index )
{
    Ptp_Interface_Stop_Timer( index );
}

void sys_timer_start( int32_t index, uint32_t interval_ms )
{
    Ptp_Interface_Start_Timer( index, interval_ms );
}

void ptpd_alert( void )
{
	/* Send a message to the alert queue to wake up the PTP thread. */
	if ( sys_mbox_trypost( Ptp_Interface_Alert_Queue(), NULL ) != ERR_OK )
	{
      if ( !( ERR_MEM ) )
      {
        PTP_BF_Assert( __LINE__, __func__  );
      }
	}
	else
	{}
}
