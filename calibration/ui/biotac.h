#ifndef INCLUDED_BIOTAC_H
#define INCLUDED_BIOTAC_H


#include "../common/biotac_comm.h"


class cPicSerial;


// This class represents the BioTac and handles communication with it.
class cBioTac
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//

	cBioTac(const cPicSerial* picSerial);

	~cBioTac();


	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//

	// Makes a single reading from the BioTac and returns the data.
	void ReadSingle() const;


	// Reads data continuously from the BioTac.
	void ReadContinuous() const;

	
	// Records a single run of calibrating the BioTac.
	void RecordCalibrationRun();
	
	
private:

	//--------------------------------------------------------------------------//
	//---------------------------  HELPER FUNCTIONS  ---------------------------//
	//--------------------------------------------------------------------------//

	// Reads from the BioTac and returns the data.
	//
	// Return - the data struct containing all the data from a single reading.
	biotac_data Read() const;
	

	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	// The object used for communication with the PIC.
	const cPicSerial* mpPicSerial;

};


#endif  // INCLUDED_BIOTAC_H

