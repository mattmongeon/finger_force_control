#ifndef INCLUDED_BIOTAC_FORCE_CURVE_H
#define INCLUDED_BIOTAC_FORCE_CURVE_H


#include "electrode_tdc_compensator.h"
#include <vector>
#include <stdint.h>


class cBioTacForceCurve
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//

	cBioTacForceCurve( const std::vector<cElectrodeTdcCompensator>& compensators,
					   double sx, double sy, double sz );

	~cBioTacForceCurve();


	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//

	// Takes in electrode values and the associated TDC value and returns the
	// corresponding force in grams.
	//
	// Params:
	// electrodes - A vector containing all 19 of the electrode values.
	// tdc - The associated TDC measurement.
	//
	// Return - The associated force in grams.
	double GetForce_g(const std::vector<uint16_t>& electrodes, uint16_t tdc) const;
	
	
private:

	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	std::vector<cElectrodeTdcCompensator> mCompensators;
	double mElectrodeNormals[19][3];
	double mSx;
	double mSy;
	double mSz;
};


#endif  // INCLUDED_BIOTAC_FORCE_CURVE_H

