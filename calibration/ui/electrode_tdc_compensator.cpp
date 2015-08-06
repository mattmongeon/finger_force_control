#include "electrode_tdc_compensator.h"
#include "data_file_reader.h"
#include <cmath>
#include <plplot/plplot.h>
#include <plplot/plstream.h>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cElectrodeTdcCompensator::cElectrodeTdcCompensator(double a, double b, double c, double d, double e, double f, double g)
{
	InitMembers(a, b, c, d, e, f, g);
}

////////////////////////////////////////////////////////////////////////////////

cElectrodeTdcCompensator::cElectrodeTdcCompensator(std::ifstream& inFile)
{
	double a, b, c, d, e, f, g;
	inFile.read(reinterpret_cast<char*>(&a), sizeof(double));
	inFile.read(reinterpret_cast<char*>(&b), sizeof(double));
	inFile.read(reinterpret_cast<char*>(&c), sizeof(double));
	inFile.read(reinterpret_cast<char*>(&d), sizeof(double));
	inFile.read(reinterpret_cast<char*>(&e), sizeof(double));
	inFile.read(reinterpret_cast<char*>(&f), sizeof(double));
	inFile.read(reinterpret_cast<char*>(&g), sizeof(double));

	InitMembers(a, b, c, d, e, f, g);
}

////////////////////////////////////////////////////////////////////////////////

cElectrodeTdcCompensator::~cElectrodeTdcCompensator()
{

}

////////////////////////////////////////////////////////////////////////////////

cElectrodeTdcCompensator::cElectrodeTdcCompensator(const cElectrodeTdcCompensator& copyMe)
{
	InitMembers(copyMe.mA, copyMe.mB, copyMe.mC, copyMe.mD, copyMe.mE, copyMe.mF, copyMe.mG);
}

////////////////////////////////////////////////////////////////////////////////

cElectrodeTdcCompensator& cElectrodeTdcCompensator::operator=(const cElectrodeTdcCompensator& rhs)
{
	InitMembers(rhs.mA, rhs.mB, rhs.mC, rhs.mD, rhs.mE, rhs.mF, rhs.mG);

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
	return mA*pow(tdc+mB, mC) + mD*pow(tac+mE, mF) + mG;
}

////////////////////////////////////////////////////////////////////////////////

void cElectrodeTdcCompensator::SaveCoefficientsToFile(std::ofstream& outFile) const
{
	outFile.write(reinterpret_cast<const char*>(&mA), sizeof(double));
	outFile.write(reinterpret_cast<const char*>(&mB), sizeof(double));
	outFile.write(reinterpret_cast<const char*>(&mC), sizeof(double));
	outFile.write(reinterpret_cast<const char*>(&mD), sizeof(double));
	outFile.write(reinterpret_cast<const char*>(&mE), sizeof(double));
	outFile.write(reinterpret_cast<const char*>(&mF), sizeof(double));
	outFile.write(reinterpret_cast<const char*>(&mG), sizeof(double));
}

////////////////////////////////////////////////////////////////////////////////
//  Helper Functions
////////////////////////////////////////////////////////////////////////////////

void cElectrodeTdcCompensator::InitMembers( double a, double b, double c, double d, double e, double f, double g )
{
	mA = a;
	mB = b;
	mC = c;
	mD = d;
	mE = e;
	mF = f;
	mG = g;
}


