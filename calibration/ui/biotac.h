#ifndef INCLUDED_BIOTAC_H
#define INCLUDED_BIOTAC_H


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
	//-----------------------------  NESTED STRUCT  ----------------------------//
	//--------------------------------------------------------------------------//

	// The struct containing all of the BioTac data from a single read.
	struct sBioTacData
	{
		unsigned short e1;
		unsigned short e2;
		unsigned short e3;
		unsigned short e4;
		unsigned short e5;
		unsigned short e6;
		unsigned short e7;
		unsigned short e8;
		unsigned short e9;
		unsigned short e10;
		unsigned short e11;
		unsigned short e12;
		unsigned short e13;
		unsigned short e14;
		unsigned short e15;
		unsigned short e16;
		unsigned short e17;
		unsigned short e18;
		unsigned short e19;

		unsigned short pac;
		unsigned short pdc;

		unsigned short tac;
		unsigned short tdc;
	};

	struct sBioTacTuneData
	{
		unsigned long mTimestamp;
		sBioTacData mData;
		int mLoadCell_g;
	};


	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//

	// Makes a single reading from the BioTac and returns the data.
	void ReadSingle() const;


	// Reads data continuously from the BioTac.
	void ReadContinuous() const;

	
	// Performs a single calibration run with the BioTac
	void CalibrationSingle();
	
	
private:

	//--------------------------------------------------------------------------//
	//---------------------------  HELPER FUNCTIONS  ---------------------------//
	//--------------------------------------------------------------------------//

	// Reads from the BioTac and returns the data.
	//
	// Return - the data struct containing all the data from a single reading.
	sBioTacData Read() const;
	

	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	// The object used for communication with the PIC.
	const cPicSerial* mpPicSerial;

};


#endif  // INCLUDED_BIOTAC_H

