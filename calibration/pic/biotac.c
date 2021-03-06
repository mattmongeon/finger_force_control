#include "biotac.h"
#include "system.h"
#include "spi.h"
#include "utils.h"
#include "uart.h"
#include "load_cell.h"
#include "torque_control.h"
#include "NU32.h"
#include "LCD.h"
#include <math.h>


////////////////////////////////////////////////////////////////////////////////
//  File-Local Constants
////////////////////////////////////////////////////////////////////////////////

// --- Constants --- //

#define LOOP_RATE_HZ 1000
#define LOOP_TIMER_PRESCALAR 16

#define LOW_SPEED_RATE_HZ 100


// --- Variables --- //

// Default to 2 seconds.
static int max_tuning_samples = 2 * LOOP_RATE_HZ;

<<<<<<< HEAD
static uint16_t force_trajectory_g[10 * LOOP_RATE_HZ];
static volatile int force_trajectory_index = 0;
static int num_force_trajectory_samples = 10 * LOOP_RATE_HZ;

static biotac_characterize cal_traj_buffer[LOOP_RATE_HZ * 10];
static uint16_t cal_traj_buffer_index = 0;

static uint16_t biotac_force_g = 0;

static uint16_t desired_testing_force_g = 0;

static const int wait_secs_before_force = 3;

static int loop_counter = 0;
=======
static int force_trajectory_g[10 * LOOP_RATE_HZ];
static volatile int force_trajectory_index = 0;
static int num_force_trajectory_samples = 10 * LOOP_RATE_HZ;

static electrode_compensator compensators[19];
static float electrode_normals[19][3];
static float force_terms[3];

static int biotac_force_g = 0;

static int desired_testing_force_g = 0;

static const int wait_secs_before_force = 2;
>>>>>>> 729c484522a8cd7ad99a2cfba976b662d0b0c046


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


#define COMP_ELECTRODE(comp,e,tdc)				\
	((float)(e)) - (((comp).a)*(1-exp((tdc+((comp).b))*((comp).c))) + ((comp).d));


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
<<<<<<< HEAD
void __ISR(_TIMER_5_VECTOR, IPL4SOFT) biotac_timer_int()
=======
void __ISR(_TIMER_5_VECTOR, IPL4SOFT) biotac_reader_int()
>>>>>>> 729c484522a8cd7ad99a2cfba976b662d0b0c046
{
	static int biotac_tune_samples = 0;
	
	switch(system_get_state())
	{
	case BIOTAC_CAL_SINGLE:
	{
<<<<<<< HEAD
		++loop_counter;
		loop_counter %= (LOOP_RATE_HZ  / LOW_SPEED_RATE_HZ);
		if( loop_counter != 0 )
			break;
		
		if( biotac_tune_samples >= wait_secs_before_force * LOW_SPEED_RATE_HZ)
=======
		if( biotac_tune_samples >= wait_secs_before_force * LOOP_RATE_HZ )
>>>>>>> 729c484522a8cd7ad99a2cfba976b662d0b0c046
			torque_control_set_desired_force(desired_testing_force_g);
		else
			torque_control_set_desired_force(0);
		
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

<<<<<<< HEAD
	case BIOTAC_CAL_TRAJECTORY:
	{
		if( biotac_tune_samples < (LOOP_RATE_HZ * wait_secs_before_force) )
		{
			torque_control_set_desired_force(0);
		}
		else if( biotac_tune_samples < (LOOP_RATE_HZ * 6) )
		{
			torque_control_set_desired_force( force_trajectory_g[0] );
		}
		else
		{
			cal_traj_buffer[cal_traj_buffer_index].mLoadCell_g = torque_control_get_load_cell_g();
			cal_traj_buffer[cal_traj_buffer_index].mReference_g = force_trajectory_g[force_trajectory_index];
			++cal_traj_buffer_index;

			torque_control_set_desired_force( force_trajectory_g[force_trajectory_index] );
			++force_trajectory_index;
		}

		++biotac_tune_samples;
		
		// Read a single BioTac reading and associated load cell reading
		// and transmit it.
		// biotac_read_and_tx();

		if( force_trajectory_index >= num_force_trajectory_samples )
		{
			uart1_send_packet((unsigned char*)(&cal_traj_buffer[0]), sizeof(cal_traj_buffer));
			
			// Signal end of data stream to the UI.
			biotac_characterize data;
			memset(&data, 0x00, sizeof(biotac_characterize));
			uart1_send_packet((unsigned char*)(&data), sizeof(biotac_characterize));
=======
	case BIOTAC_TRACK:
	{
		static biotac_tune_data data;

		if( biotac_tune_samples >= LOOP_RATE_HZ )
			torque_control_set_desired_force(desired_testing_force_g);
		else
			torque_control_set_desired_force(0);
		
		unsigned int start = _CP0_GET_COUNT();
		// Read from the BioTac
		read_biotac(&(data.mData));
		
		// Run through the compensators
		float e1 = COMP_ELECTRODE(compensators[0], data.mData.e1, data.mData.tdc)
		float e2 = COMP_ELECTRODE(compensators[1], data.mData.e2, data.mData.tdc)
		float e3 = COMP_ELECTRODE(compensators[2], data.mData.e3, data.mData.tdc)
		float e4 = COMP_ELECTRODE(compensators[3], data.mData.e4, data.mData.tdc)
		float e5 = COMP_ELECTRODE(compensators[4], data.mData.e5, data.mData.tdc)
		float e6 = COMP_ELECTRODE(compensators[5], data.mData.e6, data.mData.tdc)
		float e7 = COMP_ELECTRODE(compensators[6], data.mData.e7, data.mData.tdc)
		float e8 = COMP_ELECTRODE(compensators[7], data.mData.e8, data.mData.tdc)
		float e9 = COMP_ELECTRODE(compensators[8], data.mData.e9, data.mData.tdc)
		float e10 = COMP_ELECTRODE(compensators[9], data.mData.e10, data.mData.tdc)
		float e11 = COMP_ELECTRODE(compensators[10], data.mData.e11, data.mData.tdc)
		float e12 = COMP_ELECTRODE(compensators[11], data.mData.e12, data.mData.tdc)
		float e13 = COMP_ELECTRODE(compensators[12], data.mData.e13, data.mData.tdc)
		float e14 = COMP_ELECTRODE(compensators[13], data.mData.e14, data.mData.tdc)
		float e15 = COMP_ELECTRODE(compensators[14], data.mData.e15, data.mData.tdc)
		float e16 = COMP_ELECTRODE(compensators[15], data.mData.e16, data.mData.tdc)
		float e17 = COMP_ELECTRODE(compensators[16], data.mData.e17, data.mData.tdc)
		float e18 = COMP_ELECTRODE(compensators[17], data.mData.e18, data.mData.tdc)
		float e19 = COMP_ELECTRODE(compensators[18], data.mData.e19, data.mData.tdc)
		
		// Use the compensated electrodes in the force equation
		float x = e1 * electrode_normals[0][0] +
			e2 * electrode_normals[1][0] + 
			e3 * electrode_normals[2][0] + 
			e4 * electrode_normals[3][0] + 
			e5 * electrode_normals[4][0] + 
			e6 * electrode_normals[5][0] + 
			e7 * electrode_normals[6][0] + 
			e8 * electrode_normals[7][0] + 
			e9 * electrode_normals[8][0] + 
			e10 * electrode_normals[9][0] + 
			e11 * electrode_normals[10][0] + 
			e12 * electrode_normals[11][0] + 
			e13 * electrode_normals[12][0] + 
			e14 * electrode_normals[13][0] + 
			e15 * electrode_normals[14][0] + 
			e16 * electrode_normals[15][0] + 
			e17 * electrode_normals[16][0] + 
			e18 * electrode_normals[17][0] + 
			e19 * electrode_normals[18][0];
		x *= force_terms[0];

		float y = e1 * electrode_normals[0][1] +
			e2 * electrode_normals[1][1] + 
			e3 * electrode_normals[2][1] + 
			e4 * electrode_normals[3][1] + 
			e5 * electrode_normals[4][1] + 
			e6 * electrode_normals[5][1] + 
			e7 * electrode_normals[6][1] + 
			e8 * electrode_normals[7][1] + 
			e9 * electrode_normals[8][1] + 
			e10 * electrode_normals[9][1] + 
			e11 * electrode_normals[10][1] + 
			e12 * electrode_normals[11][1] + 
			e13 * electrode_normals[12][1] + 
			e14 * electrode_normals[13][1] + 
			e15 * electrode_normals[14][1] + 
			e16 * electrode_normals[15][1] + 
			e17 * electrode_normals[16][1] + 
			e18 * electrode_normals[17][1] + 
			e19 * electrode_normals[18][1];
		y *= force_terms[1];

		float z = e1 * electrode_normals[0][2] +
			e2 * electrode_normals[1][2] + 
			e3 * electrode_normals[2][2] + 
			e4 * electrode_normals[3][2] + 
			e5 * electrode_normals[4][2] + 
			e6 * electrode_normals[5][2] + 
			e7 * electrode_normals[6][2] + 
			e8 * electrode_normals[7][2] + 
			e9 * electrode_normals[8][2] + 
			e10 * electrode_normals[9][2] + 
			e11 * electrode_normals[10][2] + 
			e12 * electrode_normals[11][2] + 
			e13 * electrode_normals[12][2] + 
			e14 * electrode_normals[13][2] + 
			e15 * electrode_normals[14][2] + 
			e16 * electrode_normals[15][2] + 
			e17 * electrode_normals[16][2] + 
			e18 * electrode_normals[17][2] + 
			e19 * electrode_normals[18][2];
		z *= force_terms[2];
		
		// Set saved force value equal to magnitude of force vector
		biotac_force_g = sqrt(x*x + y*y + z*z);

		// Fill in and transmit data
		data.mTimestamp = _CP0_GET_COUNT();
		// HACK:  SEE FUNCTION COMMENTS IN TORQUE_CONTROL.H
		data.mLoadCell_g = torque_control_get_load_cell_g();
		data.mReference_g = torque_control_get_desired_force_g();

		uart1_send_packet((unsigned char*)(&data), sizeof(biotac_tune_data));

		uart1_send_packet((unsigned char*)(&biotac_force_g), sizeof(biotac_force_g));
		
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
>>>>>>> 729c484522a8cd7ad99a2cfba976b662d0b0c046

			biotac_tune_samples = 0;
			
			system_set_state(IDLE);
		}

		break;
	}
	
	case BIOTAC_CONTINUOUS_READ:
	{
<<<<<<< HEAD
		++loop_counter;
		loop_counter %= (LOOP_RATE_HZ  / LOW_SPEED_RATE_HZ);
		if( loop_counter != 0 )
			break;
		
=======
>>>>>>> 729c484522a8cd7ad99a2cfba976b662d0b0c046
		biotac_read_and_tx();
		
		break;
	}

	default:
<<<<<<< HEAD
		cal_traj_buffer_index = 0;
=======
>>>>>>> 729c484522a8cd7ad99a2cfba976b662d0b0c046
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
	// --- Initialize BioTac Communication --- //
	
	spi_init();
	biotac_configure();

<<<<<<< HEAD
=======

	// --- Initialize Compensators --- //

	compensators[0].a = 398.615;
	compensators[0].b = -1232.17;
	compensators[0].c = -0.00152345;
	compensators[0].d = 2818.41;

	compensators[1].a = 526.55;
	compensators[1].b = -764.712;
	compensators[1].c = -0.0023338;
	compensators[1].d = 3000.48;

	compensators[2].a = 529.63;
	compensators[2].b = -766.768;
	compensators[2].c = -0.00268468;
	compensators[2].d = 3001.55;

	compensators[3].a = 670.453;
	compensators[3].b = -752.5;
	compensators[3].c = -0.00281257;
	compensators[3].d = 2729.95;

	compensators[4].a = 533.689;
	compensators[4].b = -791.039;
	compensators[4].c = -0.00269191;
	compensators[4].d = 2995.86;

	compensators[5].a = 691.525;
	compensators[5].b = -767.93;
	compensators[5].c = -0.00251413;
	compensators[5].d = 2733.18;

	compensators[6].a = 705.889;
	compensators[6].b = -818.915;
	compensators[6].c = -0.00142316;
	compensators[6].d = 2586.5;
	
	compensators[7].a = 676.765;
	compensators[7].b = -775.942;
	compensators[7].c = -0.00164395;
	compensators[7].d = 2679.84;

	compensators[8].a = 686.158;
	compensators[8].b = -748.538;
	compensators[8].c = -0.0016712;
	compensators[8].d = 2679.12;

	compensators[9].a = 627.525;
	compensators[9].b = -762.805;
	compensators[9].c = -0.001872;
	compensators[9].d = 2788.79;

	compensators[10].a = 476.722;
	compensators[10].b = -1091.05;
	compensators[10].c = -0.00152282;
	compensators[10].d = 2681.74;

	compensators[11].a = 629.794;
	compensators[11].b = -677.452;
	compensators[11].c = -0.00237826;
	compensators[11].d = 2867.54;

	compensators[12].a = 633.253;
	compensators[12].b = -694.829;
	compensators[12].c = -0.00271722;
	compensators[12].d = 2864.8;

	compensators[13].a = 468.709;
	compensators[13].b = -871.527;
	compensators[13].c = -0.00292024;
	compensators[13].d = 2864.48;

	compensators[14].a = 627.331;
	compensators[14].b = -730.641;
	compensators[14].c = -0.00278862;
	compensators[14].d = 2853.57;

	compensators[15].a = 493.346;
	compensators[15].b = -883.858;
	compensators[15].c = -0.00269728;
	compensators[15].d = 2865.7;

	compensators[16].a = 664.641;
	compensators[16].b = -682.099;
	compensators[16].c = -0.00218528;
	compensators[16].d = 2810.61;

	compensators[17].a = 695.038;
	compensators[17].b = -675.955;
	compensators[17].c = -0.00257467;
	compensators[17].d = 2825.06;

	compensators[18].a = 669.269;
	compensators[18].b = -702.581;
	compensators[18].c = -0.0025263;
	compensators[18].d = 2807.38;


	electrode_normals[0][0] = 0.196;
	electrode_normals[0][1] = -0.953;
	electrode_normals[0][2] = -0.22;

	electrode_normals[1][0] = 0.0;
	electrode_normals[1][1] = -0.692;
	electrode_normals[1][2] = -0.722;

	electrode_normals[2][0] = 0.0;
	electrode_normals[2][1] = -0.692;
	electrode_normals[2][2] = -0.722;
	
	electrode_normals[3][0] = 0.0;
	electrode_normals[3][1] = -0.976;
	electrode_normals[3][2] = -0.22;

	electrode_normals[4][0] = 0.0;
	electrode_normals[4][1] = -0.692;
	electrode_normals[4][2] = -0.722;

	electrode_normals[5][0] = 0.0;
	electrode_normals[5][1] = -0.976;
	electrode_normals[5][2] = -0.22;

	electrode_normals[6][0] = 0.5;
	electrode_normals[6][1] = 0.0;
	electrode_normals[6][2] = -0.866;

	electrode_normals[7][0] = 0.5;
	electrode_normals[7][1] = 0.0;
	electrode_normals[7][2] = -0.866;

	electrode_normals[8][0] = 0.5;
	electrode_normals[8][1] = 0.0;
	electrode_normals[8][2] = -0.866;

	electrode_normals[9][0] = 0.5;
	electrode_normals[9][1] = 0.0;
	electrode_normals[9][2] = -0.866;

	electrode_normals[10][0] = 0.196;
	electrode_normals[10][1] = 0.956;
	electrode_normals[10][2] = -0.22;

	electrode_normals[11][0] = 0.0;
	electrode_normals[11][1] = 0.692;
	electrode_normals[11][2] = -0.722;

	electrode_normals[12][0] = 0.0;
	electrode_normals[12][1] = 0.692;
	electrode_normals[12][2] = -0.722;

	electrode_normals[13][0] = 0.0;
	electrode_normals[13][1] = 0.976;
	electrode_normals[13][2] = -0.22;

	electrode_normals[14][0] = 0.0;
	electrode_normals[14][1] = 0.692;
	electrode_normals[14][2] = -0.722;

	electrode_normals[15][0] = 0.0;
	electrode_normals[15][1] = 0.976;
	electrode_normals[15][2] = -0.22;

	electrode_normals[16][0] = 0.0;
	electrode_normals[16][1] = 0.0;
	electrode_normals[16][2] = -1.0;

	electrode_normals[17][0] = 0.0;
	electrode_normals[17][1] = 0.0;
	electrode_normals[17][2] = -1.0;

	electrode_normals[18][0] = 0.0;
	electrode_normals[18][1] = 0.0;
	electrode_normals[18][2] = -1.0;

	force_terms[0] = 0.982401;
	force_terms[1] = 0.401165;
	force_terms[2] = -0.0226676;
	
>>>>>>> 729c484522a8cd7ad99a2cfba976b662d0b0c046
	
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

	// Add one second which will simply be a time of zero force before applying
	// and holding the force value.
	max_tuning_samples = (seconds + wait_secs_before_force) * LOOP_RATE_HZ;

	__builtin_enable_interrupts();
}

////////////////////////////////////////////////////////////////////////////////

unsigned int biotac_receive_force_trajectory()
{
<<<<<<< HEAD
	uart1_read_packet( (unsigned char*)(&num_force_trajectory_samples), sizeof(uint16_t) );
	num_force_trajectory_samples *= LOOP_RATE_HZ;  // The GUI sends us time in seconds...
	
	uart1_read_packet( (unsigned char*)(&force_trajectory_g[0]),
					   sizeof(uint16_t) * num_force_trajectory_samples );
=======
	uart1_read_packet( (unsigned char*)(&num_force_trajectory_samples), sizeof(int) );
	num_force_trajectory_samples *= LOOP_RATE_HZ;

	uart1_read_packet( (unsigned char*)(&force_trajectory_g[0]),
					   sizeof(int) * num_force_trajectory_samples );
>>>>>>> 729c484522a8cd7ad99a2cfba976b662d0b0c046

	return num_force_trajectory_samples;
}

////////////////////////////////////////////////////////////////////////////////

int biotac_get_force_g()
{
	return biotac_force_g;
}

////////////////////////////////////////////////////////////////////////////////

void biotac_set_desired_testing_force_g(int force_g)
{
	__builtin_disable_interrupts();

	desired_testing_force_g = force_g;

	__builtin_enable_interrupts();
}
