#ifndef INCLUDED_ELECTRODE_TDC_COMPENSATOR_H
#define INCLUDED_ELECTRODE_TDC_COMPENSATOR_H


// This class handles removing temperature effects on the electrodes.
class cElectrodeTdcCompensator
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//
	
	cElectrodeTdcCompensator(double a, double b, double c, double d);

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
	double CompensateTdc(double tdc, double electrode) const;

	// Takes in the current raw BioTac temperature sensor value and returns a
	// value that corresponds to the electrode normalizing value, which is also
	// the estimated electrode value when at unforced.
	//
	// Params:
	// tdc - The current temperature value raw from the BioTac.
	//
	// Return - The electrode normalizing value.
	double GetUnforcedElectrodeValue(double tdc) const;

	// Takes references as parameters and fills them with the coefficient values.
	//
	// Params:
	// a - [OUT] - The reference to receive the a coefficient.
	// b - [OUT] - The reference to receive the b coefficient.
	// c - [OUT] - The reference to receive the c coefficient.
	// d - [OUT] - The reference to receive the d coefficient.
	void GetCoefficients(double& a, double& b, double& c, double& d) const;

	
private:

	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	// The values that are part of the equation that maps TDC to electrode values.
	double mA;
	double mB;
	double mC;
	double mD;
};


#endif  // INCLUDED_ELECTRODE_TDC_COMPENSATOR_H

