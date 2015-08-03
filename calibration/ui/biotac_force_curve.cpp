#include "biotac_force_curve.h"
#include <cmath>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cBioTacForceCurve::cBioTacForceCurve( const std::vector<cElectrodeTdcCompensator>& compensators,
									  double sx, double sy, double sz )
	: mCompensators(compensators),
	  mSx(sx),
	  mSy(sy),
	  mSz(sz)
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

