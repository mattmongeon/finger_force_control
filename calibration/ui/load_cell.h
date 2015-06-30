#ifndef INCLUDED_LOAD_CELL_H
#define INCLUDED_LOAD_CELL_H


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

	// Sends a request to the PIC to read from the load cell and send back the value
	// in grams.
	//
	// Return - the calibrated load cell reading in grams.
	int ReadLoadCell_grams();


	void TuneForceHolding();
	
private:

	//--------------------------------------------------------------------------//
	//-----------------------------  NESTED STRUCT  ----------------------------//
	//--------------------------------------------------------------------------//

	struct sTorqueTuneData
	{
		int mLoadCell_g;
		int mError;
		int mErrorInt;
		int mCurrent_mA;
		unsigned int mTimeStamp;
		float mLoopExeTime_ms;
	};


	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//
	
	// The object used for communication with the PIC.
	const cPicSerial* mpPicSerial;
	
};


#endif  // INCLUDED_LOAD_CELL_H

