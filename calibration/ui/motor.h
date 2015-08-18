#ifndef INCLUDED_MOTOR_H
#define INCLUDED_MOTOR_H


class cPicSerial;


// Represents the motor being used for applying force through the BioTac.
class cMotor
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//

	cMotor(const cPicSerial* picSerial);

	~cMotor();


	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//

	void DisplayMenu();
	
	
private:

	//--------------------------------------------------------------------------//
	//----------------------  MOTOR OPERATION FUNCTIONS  -----------------------//
	//--------------------------------------------------------------------------//

	// Commands the PIC to force the motor continuously hold a force as measured by
	// the load cell.
	void HoldForce();

	
	// Allows the torque controller Kp and Ki constants to be set through the UI.
	void SetTorqueControllerGains();

	
	// Requests the torque controller Kp and Ki values from the PIC and displays them.
	void GetTorqueControllerGains();

	
	// Requests a force value through the UI and then commands the PIC to hold that
	// value and return the results for analysis.
	void TestTorqueController();

	
	// Commands a certain PWM value to be used continuously on the motor.
	void SetMotorPWM();

	
	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	// The objects used for communication with the PIC.
	const cPicSerial* mpPicSerial;
};


#endif  // INCLUDED_MOTOR_H

