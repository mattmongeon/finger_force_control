#ifndef INCLUDED_BIOTAC_FORCE_CURVE_H
#define INCLUDED_BIOTAC_FORCE_CURVE_H


#include "electrode_tdc_compensator.h"
#include <vector>
#include <stdint.h>
#include <string>


class cBioTacForceCurve
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//

	// Takes in the tdc/electrode compensators and the S terms for the force vector
	// equation and fills its own data members.
	//
	// Params:
	// compensators - The vector of all 19 TDC/electrode compensators.
	// sx - The Sx term of the force vector.
	// sy - The Sy term of the force vector.
	// sz - The Sz term of the force vector.
	cBioTacForceCurve( const std::vector<cElectrodeTdcCompensator>& compensators,
					   double sx, double sy, double sz );

	// Takes in the path to the file containing the saved coefficients.
	//
	// filePath - The path to the file containing the saved data.
	cBioTacForceCurve( const std::string& filePath );

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
	double GetForce_g(const std::vector<uint16_t>& electrodes, uint16_t tdc, uint16_t pdc) const;

	// Writes the coefficients of the compensators and as well as the S terms for
	// this object to file.
	//
	// Params:
	// file - The path to the file to be written.
	void SaveFitTermsToFile(const std::string& file);

	// Reads in the data contained in the file specified by the parameter string
	// and plots the TDC-electrode compensation curve based on the file's TDC
	// for comparison.
	//
	// Params:
	// filePath - The path to the file to be parsed.
	// electrodeNum - The number of the electrode to compare.
	// void PlotElectrodeCurveAgainstFileData(const std::string& filePath, int electrodeNum);
	void PlotElectrodeCurveAgainstFileData(const std::vector<std::string>& files, int electrodeNum);
	
	
private:

	//--------------------------------------------------------------------------//
	//---------------------------  HELPER FUNCTIONS ----------------------------//
	//--------------------------------------------------------------------------//

	// Initializes the data members of this class with the given parameters.
	//
	// Params:
	// compensators - The vector of all 19 TDC/electrode compensators.
	// sx - The Sx term of the force vector.
	// sy - The Sy term of the force vector.
	// sz - The Sz term of the force vector.
	void InitMembers( const std::vector<cElectrodeTdcCompensator>& compensators,
					  double sx, double sy, double sz );
	
	
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

