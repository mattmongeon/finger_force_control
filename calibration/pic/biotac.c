#include "biotac.h"
#include "system.h"
#include "spi.h"
#include "utils.h"
#include "uart.h"
#include "load_cell.h"
#include "torque_control.h"
#include "NU32.h"


////////////////////////////////////////////////////////////////////////////////
//  File-Local Constants
////////////////////////////////////////////////////////////////////////////////

// --- Constants --- //

#define LOOP_RATE_HZ 100
#define LOOP_TIMER_PRESCALAR 16


// --- Variables --- //

// Default to 2 seconds.
static int max_tuning_samples = 2 * LOOP_RATE_HZ;

static int force_trajectory_g[10 * LOOP_RATE_HZ];
static volatile int force_trajectory_index = 0;
static int num_force_trajectory_samples = 10 * LOOP_RATE_HZ;


// --- BioTac Sampling Commands --- //

#define BIOTAC_SAMPLE_PAC (unsigned short)(0x8001)
#define BIOTAC_SAMPLE_PDC (unsigned short)(0x8301)
#define BIOTAC_SAMPLE_TAC (unsigned short)(0x8501)
#define BIOTAC_SAMPLE_TDC (unsigned short)(0x8601)
#define BIOTAC_SAMPLE_E1  (unsigned short)(0xA201)
#define BIOTAC_SAMPLE_E2  (unsigned short)(0xA401)
#define BIOTAC_SAMPLE_E3  (unsigned short)(0xA701)
#define BIOTAC_SAMPLE_E4  (unsigned short)(0xA801)
#define BIOTAC_SAMPLE_E5  (unsigned short)(0xAB01)
#define BIOTAC_SAMPLE_E6  (unsigned short)(0xAD01)
#define BIOTAC_SAMPLE_E7  (unsigned short)(0xAE01)
#define BIOTAC_SAMPLE_E8  (unsigned short)(0xB001)
#define BIOTAC_SAMPLE_E9  (unsigned short)(0xB301)
#define BIOTAC_SAMPLE_E10 (unsigned short)(0xB501)
#define BIOTAC_SAMPLE_E11 (unsigned short)(0xB601)
#define BIOTAC_SAMPLE_E12 (unsigned short)(0xB901)
#define BIOTAC_SAMPLE_E13 (unsigned short)(0xBA01)
#define BIOTAC_SAMPLE_E14 (unsigned short)(0xBC01)
#define BIOTAC_SAMPLE_E15 (unsigned short)(0xBF01)
#define BIOTAC_SAMPLE_E16 (unsigned short)(0xC101)
#define BIOTAC_SAMPLE_E17 (unsigned short)(0xC201)
#define BIOTAC_SAMPLE_E18 (unsigned short)(0xC401)
#define BIOTAC_SAMPLE_E19 (unsigned short)(0xC701)


// --- BioTac Read Commands --- //

#define BIOTAC_READ_FLEX_VERSION             (unsigned short)(0x6110)
#define BIOTAC_READ_FLEX_VERSION_LENGTH      2

#define BIOTAC_READ_FIRMWARE_VERSION         (unsigned short)(0x6113)
#define BIOTAC_READ_FIRMWARE_VERSION_LENGTH  4

#define BIOTAC_READ_SERIAL_NUMBER            (unsigned short)(0x6115)
#define BIOTAC_READ_SERIAL_NUMBER_LENGTH     16

#define BIOTAC_READ_CPU_SPEED                (unsigned short)(0x6161)
#define BIOTAC_READ_CPU_SPEED_LENGTH         2


// This is used for sending out over the MOSI line when reading
// from the BioTac.
#define BIOTAC_DUMMY_DATA (unsigned short)(0x0100)


// --- Macros --- //

#define READ_BIOTAC_SAMPLE(cmd, var)							\
	/* Request data */											\
	__builtin_disable_interrupts();								\
	spi_comm_enable(1);											\
	spi_comm((cmd));											\
	spi_comm_enable(0);											\
	__builtin_enable_interrupts();								\
																\
	/* Let the BioTac fill in its data buffer */				\
	wait_usec(50);												\
																\
	/* Read data from BioTac */									\
	__builtin_disable_interrupts();								\
	spi_comm_enable(1);											\
	(var) = spi_comm(BIOTAC_DUMMY_DATA);						\
	spi_comm_enable(0);											\
	__builtin_enable_interrupts();								\
																\
	(var) = (((var) & 0x00FF) >> 3) | (((var) & 0xFE00) >> 4);	\
																\
	wait_usec(10);


////////////////////////////////////////////////////////////////////////////////
//  File-Local Functions
////////////////////////////////////////////////////////////////////////////////

// Reads configuration data from the BioTac as part of the unit's configuration scheme.
//
// Params:
// cmd - The data to read from the BioTac.
// numBytes - The expected number of bytes of the BioTac's response.
static void biotac_read_config(unsigned short cmd, int numBytes)
{
	spi_comm_enable(1);
	spi_comm(cmd);
	spi_comm_enable(0);
	
	/* Let the BioTac fill its data buffer */
	wait_usec(50);
	
	/* Read data from BioTac */
	int i = 0;
	for( ; i < numBytes; i+=2)
	{
		spi_comm_enable(1);
		spi_comm(BIOTAC_DUMMY_DATA);
		spi_comm_enable(0);

		wait_usec(10);
	}
}

////////////////////////////////////////////////////////////////////////////////

// Initializes the BioTac by reading from some of its read-only registers.
static void biotac_configure()
{
	biotac_read_config(BIOTAC_READ_FLEX_VERSION, BIOTAC_READ_FLEX_VERSION_LENGTH);
	biotac_read_config(BIOTAC_READ_FIRMWARE_VERSION, BIOTAC_READ_FIRMWARE_VERSION_LENGTH);
	biotac_read_config(BIOTAC_READ_SERIAL_NUMBER, BIOTAC_READ_SERIAL_NUMBER_LENGTH);
	biotac_read_config(BIOTAC_READ_CPU_SPEED, BIOTAC_READ_CPU_SPEED_LENGTH);
}

////////////////////////////////////////////////////////////////////////////////

// Polls the BioTac for its latest readings and transmits the current timestamp,
// the BioTac data, and the latest load cell reading over UART.
static void biotac_read_and_tx()
{
	static biotac_tune_data data;
	
	data.mTimestamp = _CP0_GET_COUNT();
	read_biotac(&data.mData);
	// HACK:  SEE FUNCTION COMMENTS IN TORQUE_CONTROL.H
	data.mLoadCell_g = torque_control_get_load_cell_g();
	data.mReference_g = torque_control_get_desired_force_g();

	uart1_send_packet((unsigned char*)(&data), sizeof(biotac_tune_data));
}

////////////////////////////////////////////////////////////////////////////////
//  Interrupt Functions
////////////////////////////////////////////////////////////////////////////////

// Timer 5 interrupt which is responsible for handling real-time BioTac operations.
void __ISR(_TIMER_5_VECTOR, IPL4SOFT) biotac_reader_int()
{
	static int biotac_tune_samples = 0;
	
	switch(system_get_state())
	{
	case BIOTAC_CAL_SINGLE:
	{
		// Read a single BioTac reading and associated load cell reading
		// and transmit it.
		biotac_read_and_tx();

		++biotac_tune_samples;
		if( biotac_tune_samples >= max_tuning_samples )
		{
			// Signal end of data stream to the UI.
			biotac_tune_data data;
			memset(&data, 0x00, sizeof(biotac_tune_data));
			uart1_send_packet((unsigned char*)(&data), sizeof(biotac_tune_data));

			biotac_tune_samples = 0;
			
			system_set_state(IDLE);
		}

		break;
	}

	case BIOTAC_CAL_TRAJECTORY:
	{
		torque_control_set_desired_force( force_trajectory_g[force_trajectory_index] );
		
		// Read a single BioTac reading and associated load cell reading
		// and transmit it.
		biotac_read_and_tx();

		++force_trajectory_index;
		if( force_trajectory_index >= num_force_trajectory_samples )
		{
			// Signal end of data stream to the UI.
			biotac_tune_data data;
			memset(&data, 0x00, sizeof(biotac_tune_data));
			uart1_send_packet((unsigned char*)(&data), sizeof(biotac_tune_data));

			biotac_tune_samples = 0;
			
			system_set_state(IDLE);
		}

		break;
	}
	
	case BIOTAC_CONTINUOUS_READ:
	{
		biotac_read_and_tx();
		
		break;
	}

	default:
		biotac_tune_samples = 0;
		force_trajectory_index = 0;
		break;
	}
	
	IFS0CLR = 0x100000;
}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

void biotac_init()
{
	spi_init();
	biotac_configure();

	
	// --- Set Up Timer 3 Interrupt --- //

	PR5 = ((NU32_SYS_FREQ / LOOP_RATE_HZ) / LOOP_TIMER_PRESCALAR) - 1;
	T5CON = 0x0040;
	TMR5 = 0;

	IPC5SET = 0x11;      // priority 4, subpriority 1
	IFS0CLR = 0x100000;  // Clear the interrupt flag
	IEC0SET = 0x100000;  // Enable the interrupt

	T5CONSET = 0x8000;
}

////////////////////////////////////////////////////////////////////////////////

void read_biotac(biotac_data* pData)
{
	if( pData == 0 )
		return;

	memset(pData, 0, sizeof(biotac_data));
	
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PAC, pData->pac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_PDC, pData->pdc )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_TAC, pData->tac )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_TDC, pData->tdc )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E1,  pData->e1  )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E2,  pData->e2  )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E3,  pData->e3  )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E4,  pData->e4  )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E5,  pData->e5  )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E6,  pData->e6  )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E7,  pData->e7  )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E8,  pData->e8  )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E9,  pData->e9  )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E10, pData->e10 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E11, pData->e11 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E12, pData->e12 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E13, pData->e13 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E14, pData->e14 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E15, pData->e15 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E16, pData->e16 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E17, pData->e17 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E18, pData->e18 )
	READ_BIOTAC_SAMPLE( BIOTAC_SAMPLE_E19, pData->e19 )
}

////////////////////////////////////////////////////////////////////////////////

void biotac_set_time_length(int seconds)
{
	__builtin_disable_interrupts();

	max_tuning_samples = seconds * LOOP_RATE_HZ;

	__builtin_enable_interrupts();
}

////////////////////////////////////////////////////////////////////////////////

unsigned int biotac_receive_force_trajectory()
{
	uart1_read_packet( (unsigned char*)(&num_force_trajectory_samples), sizeof(int) );
	num_force_trajectory_samples *= LOOP_RATE_HZ;

	uart1_read_packet( (unsigned char*)(&force_trajectory_g[0]),
					   sizeof(int) * num_force_trajectory_samples );

	return num_force_trajectory_samples;
}
