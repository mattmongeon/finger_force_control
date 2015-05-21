#ifndef INCLUDED_CALIBRATION_H
#define INCLUDED_CALIBRATION_H


class cPicSerial;


class cLoadCell
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//
	
	cLoadCell(const cPicSerial* picComm);

	~cLoadCell();


	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//

	// Runs through the calibration routine for the load cell.  It will ask the user
	// for weight values and store a load cell value that has been averaged over
	// several readings.  When finished, it will print the weights and associated
	// values to generate a linear fit.
	void RunCalibrationRoutine();

	// Sends a request to the PIC to read from the load cell and send back the value
	// in grams.
	//
	// Return - the calibrated load cell reading in grams.
	int ReadLoadCell_grams();

	
private:

	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//
	
	// The object used for communication with the PIC.
	const cPicSerial* mpPicSerial;
	
};


#endif  // INCLUDED_CALIBRATION_H

