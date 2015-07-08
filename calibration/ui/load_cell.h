#ifndef INCLUDED_LOAD_CELL_H
#define INCLUDED_LOAD_CELL_H


#include "../common/torque_control_comm.h"


class cPicSerial;


class cLoadCell
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//
	
	cLoadCell(const cPicSerial* picSerial);

	~cLoadCell();


	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//

	// Runs through the calibration routine for the load cell.  It will ask the user
	// for weight values and store a load cell value that has been averaged over
	// several readings.  When finished, it will print the weights and associated
	// values to generate a linear fit.
	void RunCalibrationRoutine();

	
	// Reads the calibrated value of the load cell in grams.
	void ReadSingle();


	// Continuously reads streaming data from the load cell.
	void ReadContinuous();
	

	// Requests a force value through the UI and then commands the PIC to hold that
	// value and return the results for analysis.
	void TuneForceHolding();

	
private:

	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//
	
	// The object used for communication with the PIC.
	const cPicSerial* mpPicSerial;
	
};


#endif  // INCLUDED_LOAD_CELL_H

