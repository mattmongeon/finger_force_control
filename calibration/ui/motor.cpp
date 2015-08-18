#include "motor.h"
#include "pic_serial.h"
#include "utils.h"
#include "../common/torque_control_comm.h"
#include "real_time_plot.h"
#include <iostream>
#include <string>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cMotor::cMotor(const cPicSerial* picSerial)
	: mpPicSerial( picSerial )
{

}

////////////////////////////////////////////////////////////////////////////////

cMotor::~cMotor()
{
	mpPicSerial = 0;
}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

void cMotor::DisplayMenu()
{
	nUtils::ClearConsole();
	
	bool keepGoing = true;
	while(keepGoing)
	{
		std::cout << "Motor Function Menu" << std::endl;
		std::cout << "-------------------" << std::endl;
		std::cout << std::endl;
		std::cout << "a: Hold force" << std::endl;
		std::cout << "b: Set torque controller gains" << std::endl;
		std::cout << "c: Get torque controller gains" << std::endl;
		std::cout << "d: Test torque controller gains" << std::endl;
		std::cout << "e: Set motor PWM duty cycle" << std::endl;
		std::cout << "q: Go back to main menu" << std::endl;
		std::cout << std::endl;

		switch(nUtils::GetMenuSelection())
		{
		case 'a':
			HoldForce();
			break;

		case 'b':
			SetTorqueControllerGains();
			break;

		case 'c':
			GetTorqueControllerGains();
			break;

		case 'd':
			TestTorqueController();
			break;

		case 'e':
			SetMotorPWM();
			break;

		case 'q':
			keepGoing = false;
			break;
			
		default:
			std::cout << "Invalid selection!  Please choose again." << std::endl << std::endl;
			break;
		}
	}

	nUtils::ClearConsole();
}

////////////////////////////////////////////////////////////////////////////////
//  Motor Operation Functions
////////////////////////////////////////////////////////////////////////////////

void cMotor::HoldForce()
{
	mpPicSerial->WriteCommandToPic(nUtils::HOLD_FORCE);

	std::cout << "Enter force to hold (g):  ";

	int force;
	std::cin >> force;

	mpPicSerial->WriteToPic( reinterpret_cast<unsigned char*>(&force), sizeof(int) );
}

////////////////////////////////////////////////////////////////////////////////

void cMotor::SetTorqueControllerGains()
{
	float k[2];
			
	std::cout << "Enter Kp:  ";
	std::cin >> k[0];

	std::cout << "Enter Ki:  ";
	std::cin >> k[1];

	mpPicSerial->WriteCommandToPic(nUtils::SET_TORQUE_CTRL_GAINS);
	mpPicSerial->WriteToPic(reinterpret_cast<unsigned char*>(&k), sizeof(k));
}

////////////////////////////////////////////////////////////////////////////////

void cMotor::GetTorqueControllerGains()
{
	mpPicSerial->WriteCommandToPic(nUtils::GET_TORQUE_CTRL_GAINS);
			
	unsigned char buffer[20];
	mpPicSerial->ReadFromPic(buffer, 8);

	float* pF = reinterpret_cast<float*>(&buffer);
	std::cout << "Kp:  " << pF[0] << std::endl;
	std::cout << "Ki:  " << pF[1] << std::endl;
	std::cout << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

void cMotor::TestTorqueController()
{
	// --- Preparations --- //

	// Ahead of time we will set up the things to be used during real-time processing so we can move fast.
	torque_tune_data rxData;
	torque_tune_data stopCondition;
	memset(&stopCondition, 0, sizeof(torque_tune_data));
	std::vector<torque_tune_data> tuneData;
	

	// --- Start The Process --- //
	
	mpPicSerial->DiscardIncomingData(0);
	mpPicSerial->WriteCommandToPic(nUtils::TUNE_TORQUE_GAINS);

	std::cout << "Enter number of seconds for this run (s):  " ;
	int seconds = 0;
	std::cin >> seconds;

	mpPicSerial->WriteValueToPic(seconds);
	
	std::cout << "Enter force to hold (g):  ";

	int force;
	std::cin >> force;

	// Do this one next.  It is really annoying to create it before sending the force, because it pops up
	// a window right in the way.
	cRealTimePlot plotter("Load Cell", "Sample", "Force (g)", "Force (g)", "", "", "", seconds * 200.0, 0.0, 10.0);

	mpPicSerial->WriteValueToPic(force);

	std::cout << "Waiting for tuning results..." << std::endl;

	while(true)
	{
		mpPicSerial->ReadFromPic( reinterpret_cast<unsigned char*>(&rxData), sizeof(torque_tune_data) );

		if( memcmp(&rxData, &stopCondition, sizeof(torque_tune_data)) != 0 )
		{
			plotter.AddDataPoint(rxData.load_cell_g);
			tuneData.push_back(rxData);
		}
		else
		{
			break;
		}
	}


	// --- Print Results --- //
	
	std::cout << "Tuning results received!" << std::endl;
	std::cout << "Load Cell\tError\t\tError Integral\t\tPWM\t\tTimestamp\t\tExe Time (ms)\t\tFrequency (Hz)" << std::endl;
	unsigned int prevTimestamp = 0;
	for( size_t i = 0; i < tuneData.size(); ++i )
	{
		std::cout << tuneData[i].load_cell_g << "\t\t"
				  << tuneData[i].error << "\t\t"
				  << tuneData[i].error_int << "\t\t"
				  << tuneData[i].pwm << "\t\t"
				  << tuneData[i].timestamp << "\t\t"
				  << tuneData[i].loop_exe_time_ms << "\t\t"
				  << 1.0 / ((tuneData[i].timestamp - prevTimestamp) * 25.0 / 1000000000.0) << "\t\t"
				  << std::endl;

		prevTimestamp = tuneData[i].timestamp;
	}


	std::cout << std::endl;


	// Just in case...
	mpPicSerial->DiscardIncomingData(0);
}

////////////////////////////////////////////////////////////////////////////////

void cMotor::SetMotorPWM()
{
	std::cout << "PWM value as integer percentage:  " << std::flush;
	int pwm = 0;
	std::cin >> pwm;
	mpPicSerial->WriteCommandToPic(nUtils::SET_PWM);
	mpPicSerial->WriteValueToPic<char>(static_cast<char>(pwm));
}

