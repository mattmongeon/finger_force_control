#ifndef INCLUDED_BIOTAC_FORCE_CURVE_TARE_H
#define INCLUDED_BIOTAC_FORCE_CURVE_TARE_H


#include "electrode_comp_tare.h"
#include "../common/biotac_comm.h"


class cBioTacForceCurveTare
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//

	cBioTacForceCurveTare( double sx, double sy, double sz );

	~cBioTacForceCurveTare();
	

	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//

	double GetForce_g(const std::vector<uint16_t>& electrodes, uint16_t tdc, uint16_t pdc) const;

	void SetBaseline(const std::vector<biotac_tune_data>& values);
	
	
private:
	
	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	std::vector<cElectrodeCompensatorTare> mCompensators;
	double mElectrodeNormals[19][3];
	double mSx;
	double mSy;
	double mSz;

};


#endif  // INCLUDED_BIOTAC_FORCE_CURVE_TARE_H

