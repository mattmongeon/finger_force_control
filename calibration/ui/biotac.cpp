#include "biotac.h"
#include "pic_serial.h"
#include "utils.h"
#include <iostream>
#include <vector>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cBioTac::cBioTac(const cPicSerial* picSerial)
	: mpPicSerial( picSerial )
{
	
}

////////////////////////////////////////////////////////////////////////////////

cBioTac::~cBioTac()
{
	mpPicSerial = 0;
}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

void cBioTac::ReadSingle() const
{
	std::cout << "Read from BioTac:" << std::endl;
	cBioTac::sBioTacData d = Read();
	std::cout << "E1:   " << d.e1 << "\r\n";
	std::cout << "E2:   " << d.e2 << "\r\n";
	std::cout << "E3:   " << d.e3 << "\r\n";
	std::cout << "E4:   " << d.e4 << "\r\n";
	std::cout << "E5:   " << d.e5 << "\r\n";
	std::cout << "E6:   " << d.e6 << "\r\n";
	std::cout << "E7:   " << d.e7 << "\r\n";
	std::cout << "E8:   " << d.e8 << "\r\n";
	std::cout << "E9:   " << d.e9 << "\r\n";
	std::cout << "E10:  " << d.e10 << "\r\n";
	std::cout << "E11:  " << d.e11 << "\r\n";
	std::cout << "E12:  " << d.e12 << "\r\n";
	std::cout << "E13:  " << d.e13 << "\r\n";
	std::cout << "E14:  " << d.e14 << "\r\n";
	std::cout << "E15:  " << d.e15 << "\r\n";
	std::cout << "E16:  " << d.e16 << "\r\n";
	std::cout << "E17:  " << d.e17 << "\r\n";
	std::cout << "E18:  " << d.e18 << "\r\n";
	std::cout << "E19:  " << d.e19 << "\r\n";
	std::cout << "PAC:  " << d.pac << "\r\n";
	std::cout << "PDC:  " << d.pdc << "\r\n";
	std::cout << "TAC:  " << d.tac << "\r\n";
	std::cout << "TDC:  " << d.tdc << "\r\n";
	std::cout << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

void cBioTac::CalibrationSingle()
{
	mpPicSerial->WriteCommandToPic(nUtils::BIOTAC_CAL_SINGLE);

	std::cout << "Enter calibration force (g):  ";

	int force;
	std::cin >> force;

	// Ahead of time we will set up the things to be used during real-time processing.
	sBioTacTuneData rxData;
	sBioTacTuneData stopCondition;
	memset(&stopCondition, 0, sizeof(sBioTacTuneData));
	std::vector<sBioTacTuneData> tuneData;

	// Now start the process.
	mpPicSerial->WriteToPic( reinterpret_cast<unsigned char*>(&force), sizeof(int) );

	std::cout << "Waiting for tuning results..." << std::endl;

	while(true)
	{
		mpPicSerial->ReadFromPic( reinterpret_cast<unsigned char*>(&rxData), sizeof(sBioTacTuneData) );

		if( memcmp(&rxData, &stopCondition, sizeof(sBioTacTuneData)) != 0 )
			tuneData.push_back(rxData);
		else
			break;
	}


	// --- Print Results --- //

	std::cout << "Results received!" << std::endl << std::endl;

	std::cout << "Load cell\tBioTac PDC\tBioTac PAC\tBioTac TDC\tBioTac TAC\tTimestamp" << std::endl;
	for( size_t i = 0; i < tuneData.size(); ++i )
	{
		std::cout << tuneData[i].mLoadCell_g << "\t\t" << tuneData[i].mData.pdc << "\t\t" << tuneData[i].mData.pac << "\t\t"
				  << tuneData[i].mData.tdc << "\t\t" << tuneData[i].mData.tac << "\t\t" << tuneData[i].mTimestamp << std::endl;
	}

	std::cout << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
//  Helper Functions
////////////////////////////////////////////////////////////////////////////////

cBioTac::sBioTacData cBioTac::Read() const
{
	return mpPicSerial->ReadValueFromPic<sBioTacData>(nUtils::READ_BIOTAC);
}

