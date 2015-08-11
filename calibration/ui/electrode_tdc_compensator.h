#ifndef INCLUDED_ELECTRODE_TDC_COMPENSATOR_H
#define INCLUDED_ELECTRODE_TDC_COMPENSATOR_H


#include <string>
#include <fstream>


// This class handles removing temperature effects on the electrodes.
class cElectrodeTdcCompensator
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//
	
	cElectrodeTdcCompensator(double* pA, double* pB, double* pC,
							 double* pD, double* pE, double* pF);

	cElectrodeTdcCompensator(std::ifstream& inFile);

	~cElectrodeTdcCompensator();

	cElectrodeTdcCompensator(const cElectrodeTdcCompensator& copyMe);

	cElectrodeTdcCompensator& operator=(const cElectrodeTdcCompensator& rhs);

	
	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//

	// Takes in the current raw BioTac temperature sensor value and a raw BioTac
	// electrode value and returns a "normalized" electrode value that will be
	// close to 0 when no forces are applied to the BioTac.
	//
	// Params:
	// tdc - The current temperature value raw from the BioTac.
	// electrode - The current electrode value raw from the BioTac.
	//
	// Return - The electrode value after compensating for temperature.
	double CompensateTdc(double tdc, double tac, double electrode) const;

	// Takes in the current raw BioTac temperature sensor value and returns a
	// value that corresponds to the electrode normalizing value, which is also
	// the estimated electrode value when at unforced.
	//
	// Params:
	// tdc - The current temperature value raw from the BioTac.
	//
	// Return - The electrode normalizing value.
	double GetUnforcedElectrodeValue(double tdc, double tac) const;
	
	void SaveCoefficientsToFile(std::ofstream& outFile) const;

	
private:

	//--------------------------------------------------------------------------//
	//---------------------------  HELPER FUNCTIONS ----------------------------//
	//--------------------------------------------------------------------------//

	void InitMembers( const double* pA, const double* pB, const double* pC,
					  const double* pD, const double* pE, const double* pF );

	
	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	// The values that are part of the equation that maps TDC to electrode values.
	double mA[5];
	double mB[5];
	double mC[5];
	double mD[5];
	double mE[5];
	double mF[5];
	
};


#endif  // INCLUDED_ELECTRODE_TDC_COMPENSATOR_H

