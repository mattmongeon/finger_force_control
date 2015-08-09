#include "electrode_tdc_compensator.h"
#include "data_file_reader.h"
#include <cmath>
#include <plplot/plplot.h>
#include <plplot/plstream.h>
#include <string.h>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cElectrodeTdcCompensator::cElectrodeTdcCompensator(double* pA, double* pB, double* pC,
												   double* pD, double* pE, double* pF,
												   double* pG)
{
	InitMembers(pA, pB, pC, pD, pE, pF, pG);
}

////////////////////////////////////////////////////////////////////////////////

cElectrodeTdcCompensator::cElectrodeTdcCompensator(std::ifstream& inFile)
{
	double a[5], b[5], c[5], d[5], e[5], f[5], g[5];
	inFile.read(reinterpret_cast<char*>(&a[0]), sizeof(double)*5);
	inFile.read(reinterpret_cast<char*>(&b[0]), sizeof(double)*5);
	inFile.read(reinterpret_cast<char*>(&c[0]), sizeof(double)*5);
	inFile.read(reinterpret_cast<char*>(&d[0]), sizeof(double)*5);
	inFile.read(reinterpret_cast<char*>(&e[0]), sizeof(double)*5);
	inFile.read(reinterpret_cast<char*>(&f[0]), sizeof(double)*5);
	inFile.read(reinterpret_cast<char*>(&g[0]), sizeof(double)*5);

	InitMembers(a, b, c, d, e, f, g);
}

////////////////////////////////////////////////////////////////////////////////

cElectrodeTdcCompensator::~cElectrodeTdcCompensator()
{

}

////////////////////////////////////////////////////////////////////////////////

cElectrodeTdcCompensator::cElectrodeTdcCompensator(const cElectrodeTdcCompensator& copyMe)
{
	InitMembers(copyMe.mA, copyMe.mB, copyMe.mC,
				copyMe.mD, copyMe.mE, copyMe.mF,
				copyMe.mG);
}

////////////////////////////////////////////////////////////////////////////////

cElectrodeTdcCompensator& cElectrodeTdcCompensator::operator=(const cElectrodeTdcCompensator& rhs)
{
	InitMembers(rhs.mA, rhs.mB, rhs.mC,
				rhs.mD, rhs.mE, rhs.mF,
				rhs.mG);

	return *this;
}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

double cElectrodeTdcCompensator::CompensateTdc(double tdc, double tac, double electrode) const
{
	return electrode - GetUnforcedElectrodeValue(tdc, tac);
}

////////////////////////////////////////////////////////////////////////////////

double cElectrodeTdcCompensator::GetUnforcedElectrodeValue(double tdc, double tac) const
{
	return
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
}

////////////////////////////////////////////////////////////////////////////////

void cElectrodeTdcCompensator::SaveCoefficientsToFile(std::ofstream& outFile) const
{
	outFile.write(reinterpret_cast<const char*>(&mA[0]), sizeof(double)*5);
	outFile.write(reinterpret_cast<const char*>(&mB[0]), sizeof(double)*5);
	outFile.write(reinterpret_cast<const char*>(&mC[0]), sizeof(double)*5);
	outFile.write(reinterpret_cast<const char*>(&mD[0]), sizeof(double)*5);
	outFile.write(reinterpret_cast<const char*>(&mE[0]), sizeof(double)*5);
	outFile.write(reinterpret_cast<const char*>(&mF[0]), sizeof(double)*5);
	outFile.write(reinterpret_cast<const char*>(&mG[0]), sizeof(double)*5);
}

////////////////////////////////////////////////////////////////////////////////
//  Helper Functions
////////////////////////////////////////////////////////////////////////////////

void cElectrodeTdcCompensator::InitMembers( const double* pA, const double* pB, const double* pC,
											const double* pD, const double* pE, const double* pF,
											const double* pG )
{
	memcpy(&mA[0], pA, sizeof(double)*5);
	memcpy(&mB[0], pB, sizeof(double)*5);
	memcpy(&mC[0], pC, sizeof(double)*5);
	memcpy(&mD[0], pD, sizeof(double)*5);
	memcpy(&mE[0], pE, sizeof(double)*5);
	memcpy(&mF[0], pF, sizeof(double)*5);
	memcpy(&mG[0], pG, sizeof(double)*5);
}


