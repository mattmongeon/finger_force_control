#include "biotac_force_curve_tare.h"
#include <cmath>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cBioTacForceCurveTare::cBioTacForceCurveTare( double sx, double sy, double sz )
{
	mElectrodeNormals[0][0] = 0.196;
	mElectrodeNormals[0][1] = -0.956;
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

	for( std::size_t i = 0; i < 19; ++i )
	{
		mCompensators.push_back(cElectrodeCompensatorTare());
	}
	
	mSx = sx;
	mSy = sy;
	mSz = sz;
}

////////////////////////////////////////////////////////////////////////////////

cBioTacForceCurveTare::~cBioTacForceCurveTare()
{

}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

double cBioTacForceCurveTare::GetForce_g(const std::vector<uint16_t>& electrodes, uint16_t tdc, uint16_t pdc) const
{
	double x = 0.0, y = 0.0, z = 0.0;
	for( std::size_t i = 0; i < 19; ++i )
	{
		double comp = mCompensators[i].GetCompensatedValue(electrodes[i]);
		x += mElectrodeNormals[i][0] * comp;
		y += mElectrodeNormals[i][1] * comp;
		z += mElectrodeNormals[i][2] * comp;
	}

	return sqrt(mSx*mSx*x*x + mSy*mSy*y*y + mSz*mSz*z*z);
}

////////////////////////////////////////////////////////////////////////////////

void cBioTacForceCurveTare::SetBaseline(const std::vector<biotac_tune_data>& values)
{
	std::vector<uint16_t> e1, e2, e3, e4, e5, e6, e7, e8, e9, e10,
		e11, e12, e13, e14, e15, e16, e17, e18, e19;
	for( std::size_t i = 0; i < values.size(); ++i )
	{
		e1.push_back(values[i].mData.e1);
		e2.push_back(values[i].mData.e2);
		e3.push_back(values[i].mData.e3);
		e4.push_back(values[i].mData.e4);
		e5.push_back(values[i].mData.e5);
		e6.push_back(values[i].mData.e6);
		e7.push_back(values[i].mData.e7);
		e8.push_back(values[i].mData.e8);
		e9.push_back(values[i].mData.e9);
		e10.push_back(values[i].mData.e10);
		e11.push_back(values[i].mData.e11);
		e12.push_back(values[i].mData.e12);
		e13.push_back(values[i].mData.e13);
		e14.push_back(values[i].mData.e14);
		e15.push_back(values[i].mData.e15);
		e16.push_back(values[i].mData.e16);
		e17.push_back(values[i].mData.e17);
		e18.push_back(values[i].mData.e18);
		e19.push_back(values[i].mData.e19);
	}

	mCompensators[0].SetBaseline(e1);
	mCompensators[1].SetBaseline(e2);
	mCompensators[2].SetBaseline(e3);
	mCompensators[3].SetBaseline(e4);
	mCompensators[4].SetBaseline(e5);
	mCompensators[5].SetBaseline(e6);
	mCompensators[6].SetBaseline(e7);
	mCompensators[7].SetBaseline(e8);
	mCompensators[8].SetBaseline(e9);
	mCompensators[9].SetBaseline(e10);
	mCompensators[10].SetBaseline(e11);
	mCompensators[11].SetBaseline(e12);
	mCompensators[12].SetBaseline(e13);
	mCompensators[13].SetBaseline(e14);
	mCompensators[14].SetBaseline(e15);
	mCompensators[15].SetBaseline(e16);
	mCompensators[16].SetBaseline(e17);
	mCompensators[17].SetBaseline(e18);
	mCompensators[18].SetBaseline(e19);
}

