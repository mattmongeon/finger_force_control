#include "electrode_tdc_compensator.h"
#include "data_file_reader.h"
#include <cmath>
#include <plplot/plplot.h>
#include <plplot/plstream.h>
#include <string.h>
#include <iostream>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cElectrodeTdcCompensator::cElectrodeTdcCompensator(double* pA, double* pB, double* pC,
												   double* pD)
{
	InitMembers(pA, pB, pC, pD);

	std::cout << "a[0]: " << mA[0] << ",\ta[1]: " << mA[1] << ",\ta[2]: " << mA[2] << std::endl
			  << "b[0]: " << mB[0] << ",\tb[1]: " << mB[1] << ",\tb[2]: " << mB[2] << std::endl
			  << "c[0]: " << mC[0] << ",\tc[1]: " << mC[1] << ",\tc[2]: " << mC[2] << std::endl
			  << "d[0]: " << mD[0] << ",\td[1]: " << mD[1] << ",\td[2]: " << mD[2] << std::endl
			  << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

cElectrodeTdcCompensator::cElectrodeTdcCompensator(std::ifstream& inFile)
{
	double a[5], b[5], c[5], d[5];
	inFile.read(reinterpret_cast<char*>(&a[0]), sizeof(double)*5);
	inFile.read(reinterpret_cast<char*>(&b[0]), sizeof(double)*5);
	inFile.read(reinterpret_cast<char*>(&c[0]), sizeof(double)*5);
	inFile.read(reinterpret_cast<char*>(&d[0]), sizeof(double)*5);

	InitMembers(a, b, c, d);
}

////////////////////////////////////////////////////////////////////////////////

cElectrodeTdcCompensator::~cElectrodeTdcCompensator()
{

}

////////////////////////////////////////////////////////////////////////////////

cElectrodeTdcCompensator::cElectrodeTdcCompensator(const cElectrodeTdcCompensator& copyMe)
{
	InitMembers(copyMe.mA, copyMe.mB, copyMe.mC,
				copyMe.mD);
}

////////////////////////////////////////////////////////////////////////////////

cElectrodeTdcCompensator& cElectrodeTdcCompensator::operator=(const cElectrodeTdcCompensator& rhs)
{
	InitMembers(rhs.mA, rhs.mB, rhs.mC,
				rhs.mD);

	return *this;
}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

double cElectrodeTdcCompensator::CompensateTdc(double tdc, double pdc, double electrode) const
{
	return electrode - GetUnforcedElectrodeValue(tdc, pdc);
}

////////////////////////////////////////////////////////////////////////////////

double cElectrodeTdcCompensator::GetUnforcedElectrodeValue(double tdc, double pdc) const
{
	return
		mA[0]*(1-exp((tdc+mB[0])*mC[0])) +
		mD[0];
/*
		mA[0]*pow((tdc+mB[0]), 5.0) +
		mA[1]*pow((tdc+mB[1]), 4.0) +
		mA[2]*pow((tdc+mB[2]), 3.0) +
		mA[3]*pow((tdc+mB[3]), 2.0) +
		mA[4]*pow((tdc+mB[4]), 1.0) +

		mD[0]*pow( tdc+mG[0]*exp(mE[0]*tac+mF[0])+mC[0], 5.0) +
		mD[1]*pow( tdc+mG[1]*exp(mE[1]*tac+mF[1])+mC[1], 4.0) +
		mD[2]*pow( tdc+mG[2]*exp(mE[2]*tac+mF[2])+mC[2], 3.0) +
		mD[3]*pow( tdc+mG[3]*exp(mE[3]*tac+mF[3])+mC[3], 2.0) +
		mD[4]*pow( tdc+mG[4]*exp(mE[4]*tac+mF[4])+mC[4], 1.0);
		*/
}

////////////////////////////////////////////////////////////////////////////////

void cElectrodeTdcCompensator::SaveCoefficientsToFile(std::ofstream& outFile) const
{
	outFile.write(reinterpret_cast<const char*>(&mA[0]), sizeof(double)*5);
	outFile.write(reinterpret_cast<const char*>(&mB[0]), sizeof(double)*5);
	outFile.write(reinterpret_cast<const char*>(&mC[0]), sizeof(double)*5);
	outFile.write(reinterpret_cast<const char*>(&mD[0]), sizeof(double)*5);
}

////////////////////////////////////////////////////////////////////////////////
//  Helper Functions
////////////////////////////////////////////////////////////////////////////////

void cElectrodeTdcCompensator::InitMembers( const double* pA, const double* pB, const double* pC,
											const double* pD )
{
	memcpy(&mA[0], pA, sizeof(double)*5);
	memcpy(&mB[0], pB, sizeof(double)*5);
	memcpy(&mC[0], pC, sizeof(double)*5);
	memcpy(&mD[0], pD, sizeof(double)*5);
}


