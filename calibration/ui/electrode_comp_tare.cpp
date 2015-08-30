#include "electrode_comp_tare.h"


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cElectrodeCompensatorTare::cElectrodeCompensatorTare()
{
	mBaseline = 0;
}

////////////////////////////////////////////////////////////////////////////////

cElectrodeCompensatorTare::~cElectrodeCompensatorTare()
{

}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

void cElectrodeCompensatorTare::SetBaseline(const std::vector<uint16_t>& values)
{
	double total = 0;
	for( std::size_t i = 0; i < values.size(); ++i )
	{
		total += values[i];
	}

	mBaseline = total / values.size();
}

////////////////////////////////////////////////////////////////////////////////

double cElectrodeCompensatorTare::GetCompensatedValue(double raw) const
{
	return raw - mBaseline;
}
