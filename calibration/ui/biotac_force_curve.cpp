#include "biotac_force_curve.h"
#include <cmath>
#include <fstream>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cBioTacForceCurve::cBioTacForceCurve( const std::vector<cElectrodeTdcCompensator>& compensators,
									  double sx, double sy, double sz )
{
	InitMembers(compensators, sx, sy, sz);
}

////////////////////////////////////////////////////////////////////////////////

cBioTacForceCurve::cBioTacForceCurve( const std::string& filePath )
{
	std::ifstream file(filePath.c_str(), std::ios::binary | std::ios::in);

	double sx, sy, sz;
	file.read(reinterpret_cast<char*>(&sx), sizeof(double));
	file.read(reinterpret_cast<char*>(&sy), sizeof(double));
	file.read(reinterpret_cast<char*>(&sz), sizeof(double));

	
	double a, b, c, d;
	std::vector<cElectrodeTdcCompensator> compensators;
	for( std::size_t i = 0; i < 19; ++i )
	{
		file.read(reinterpret_cast<char*>(&a), sizeof(double));
		file.read(reinterpret_cast<char*>(&b), sizeof(double));
		file.read(reinterpret_cast<char*>(&c), sizeof(double));
		file.read(reinterpret_cast<char*>(&d), sizeof(double));

		compensators.push_back( cElectrodeTdcCompensator(a, b, c, d) );
	}


	InitMembers(compensators, sx, sy, sz);
}

////////////////////////////////////////////////////////////////////////////////

cBioTacForceCurve::~cBioTacForceCurve()
{

}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

double cBioTacForceCurve::GetForce_g(const std::vector<uint16_t>& electrodes, uint16_t tdc) const
{
	double x = 0.0, y = 0.0, z = 0.0;
	for( std::size_t i = 0; i < 19; ++i )
	{
		x += mElectrodeNormals[i][0] * mCompensators[i].CompensateTdc(tdc, electrodes[i]);
		y += mElectrodeNormals[i][1] * mCompensators[i].CompensateTdc(tdc, electrodes[i]);
		z += mElectrodeNormals[i][2] * mCompensators[i].CompensateTdc(tdc, electrodes[i]);
	}

	return sqrt(mSx*mSx*x*x + mSy*mSy*y*y + mSz*mSz*z*z);
}

////////////////////////////////////////////////////////////////////////////////

void cBioTacForceCurve::SaveFitTermsToFile(const std::string& file)
{
	std::ofstream outFile(file.c_str(), std::ios::binary | std::ios::out);

	
	outFile.write(reinterpret_cast<const char*>(&mSx), sizeof(double));
	outFile.write(reinterpret_cast<const char*>(&mSy), sizeof(double));
	outFile.write(reinterpret_cast<const char*>(&mSz), sizeof(double));

	
	double a, b, c, d;
	for( std::size_t i = 0; i < mCompensators.size(); ++i )
	{
		mCompensators[i].GetCoefficients(a, b, c, d);
		outFile.write(reinterpret_cast<const char*>(&a), sizeof(double));
		outFile.write(reinterpret_cast<const char*>(&b), sizeof(double));
		outFile.write(reinterpret_cast<const char*>(&c), sizeof(double));
		outFile.write(reinterpret_cast<const char*>(&d), sizeof(double));
	}

	outFile.close();
}

////////////////////////////////////////////////////////////////////////////////
//  Helper Functions
////////////////////////////////////////////////////////////////////////////////

void cBioTacForceCurve::InitMembers( const std::vector<cElectrodeTdcCompensator>& compensators,
									 double sx, double sy, double sz )
{
	mElectrodeNormals[0][0] = 0.196;
	mElectrodeNormals[0][1] = -0.953;
	mElectrodeNormals[0][2] = -0.22;

	mElectrodeNormals[1][0] = 0.0;
	mElectrodeNormals[1][1] = -0.692;
	mElectrodeNormals[1][2] = -0.722;

	mElectrodeNormals[2][0] = 0.0;
	mElectrodeNormals[2][1] = -0.692;
	mElectrodeNormals[2][2] = -0.722;
	
	mElectrodeNormals[3][0] = 0.0;
	mElectrodeNormals[3][1] = -0.976;
	mElectrodeNormals[3][2] = -0.22;

	mElectrodeNormals[4][0] = 0.0;
	mElectrodeNormals[4][1] = -0.692;
	mElectrodeNormals[4][2] = -0.722;

	mElectrodeNormals[5][0] = 0.0;
	mElectrodeNormals[5][1] = -0.976;
	mElectrodeNormals[5][2] = -0.22;

	mElectrodeNormals[6][0] = 0.5;
	mElectrodeNormals[6][1] = 0.0;
	mElectrodeNormals[6][2] = -0.866;

	mElectrodeNormals[7][0] = 0.5;
	mElectrodeNormals[7][1] = 0.0;
	mElectrodeNormals[7][2] = -0.866;

	mElectrodeNormals[8][0] = 0.5;
	mElectrodeNormals[8][1] = 0.0;
	mElectrodeNormals[8][2] = -0.866;

	mElectrodeNormals[9][0] = 0.5;
	mElectrodeNormals[9][1] = 0.0;
	mElectrodeNormals[9][2] = -0.866;

	mElectrodeNormals[10][0] = 0.196;
	mElectrodeNormals[10][1] = 0.956;
	mElectrodeNormals[10][2] = -0.22;

	mElectrodeNormals[11][0] = 0.0;
	mElectrodeNormals[11][1] = 0.692;
	mElectrodeNormals[11][2] = -0.722;

	mElectrodeNormals[12][0] = 0.0;
	mElectrodeNormals[12][1] = 0.692;
	mElectrodeNormals[12][2] = -0.722;

	mElectrodeNormals[13][0] = 0.0;
	mElectrodeNormals[13][1] = 0.976;
	mElectrodeNormals[13][2] = -0.22;

	mElectrodeNormals[14][0] = 0.0;
	mElectrodeNormals[14][1] = 0.692;
	mElectrodeNormals[14][2] = -0.722;

	mElectrodeNormals[15][0] = 0.0;
	mElectrodeNormals[15][1] = 0.976;
	mElectrodeNormals[15][2] = -0.22;

	mElectrodeNormals[16][0] = 0.0;
	mElectrodeNormals[16][1] = 0.0;
	mElectrodeNormals[16][2] = -1.0;

	mElectrodeNormals[17][0] = 0.0;
	mElectrodeNormals[17][1] = 0.0;
	mElectrodeNormals[17][2] = -1.0;

	mElectrodeNormals[18][0] = 0.0;
	mElectrodeNormals[18][1] = 0.0;
	mElectrodeNormals[18][2] = -1.0;

	mCompensators = compensators;
	mSx = sx;
	mSy = sy;
	mSz = sz;
}
