#ifndef INCLUDED_ELECTRODE_COMP_TARE_H
#define INCLUDED_ELECTRODE_COMP_TARE_H


#include <vector>
#include <stdint.h>


class cElectrodeCompensatorTare
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//

	cElectrodeCompensatorTare();

	~cElectrodeCompensatorTare();


	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//

	void SetBaseline(const std::vector<uint16_t>& values);

	double GetCompensatedValue(double raw) const;


private:
	
	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	uint16_t mBaseline;
	
};


#endif  // INCLUDED_ELECTRODE_COMP_TARE_H

