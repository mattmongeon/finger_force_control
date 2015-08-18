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

	void DisplayMenu();
	
	
private:

	//--------------------------------------------------------------------------//
	//----------------------  BIOTAC OPERATION FUNCTIONS  ----------------------//
	//--------------------------------------------------------------------------//

	// Makes a single reading from the BioTac and returns the data.
	void ReadSingle() const;


	// Reads data continuously from the BioTac.
	void ReadContinuous() const;

	
	// Records a single run of force holding while recording Biotac and load cell data.
	void RecordForceHold();


	// Records a force-hold run following a force trajectory.
	void RecordForceTrajectory();


	// Tells the PIC hold the BioTac at a force value measured from the calibrated
	// BioTac.
	void RecordBioTacForceTest();


	// 2
	void TrainElectrodeCompenstors();


	// 3
	void TrainForceVectorCoefficients();


	// 7
	void TrainAllForceTerms();

	
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

