#include "biotac_force_curve.h"
#include "utils.h"
#include "data_file_reader.h"
#include <plplot/plplot.h>
#include <plplot/plstream.h>
#include <cmath>
#include <fstream>
#include <map>
#include <sstream>
#include <iostream>


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

	
	std::vector<cElectrodeTdcCompensator> compensators;
	for( std::size_t i = 0; i < 19; ++i )
	{
		compensators.push_back( cElectrodeTdcCompensator(file) );
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

double cBioTacForceCurve::GetForce_g(const std::vector<uint16_t>& electrodes, uint16_t tdc, uint16_t pdc) const
{
	double x = 0.0, y = 0.0, z = 0.0;
	for( std::size_t i = 0; i < 19; ++i )
	{
		x += mElectrodeNormals[i][0] * mCompensators[i].CompensateTdc(tdc, pdc, electrodes[i]);
		y += mElectrodeNormals[i][1] * mCompensators[i].CompensateTdc(tdc, pdc, electrodes[i]);
		z += mElectrodeNormals[i][2] * mCompensators[i].CompensateTdc(tdc, pdc, electrodes[i]);
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
	
	for( std::size_t i = 0; i < mCompensators.size(); ++i )
	{
		mCompensators[i].SaveCoefficientsToFile(outFile);
	}

	outFile.close();
}

////////////////////////////////////////////////////////////////////////////////

//void cBioTacForceCurve::PlotElectrodeCurveAgainstFileData(const std::string& filePath, int electrodeNum)
	void cBioTacForceCurve::PlotElectrodeCurveAgainstFileData(const std::vector<std::string>& files, int electrodeNum)
{
	// --- Read Data From File --- //

	/*
	cDataFileReader reader(filePath);
	std::vector<biotac_tune_data> fileData = reader.GetData();
	*/
	std::vector<biotac_tune_data> fileData;
	for( std::size_t i = 0; i < files.size(); ++i )
	{
		cDataFileReader reader(files[i]);
		std::vector<biotac_tune_data> temp = reader.GetData();
		fileData.insert(fileData.end(), temp.begin(), temp.end());
	}


	// --- Organize Data --- //

	// The pair is <pdc, electrode>
	std::map< uint16_t, std::vector< std::pair<uint16_t, uint16_t> > > organizedData;
	for( std::size_t i = 0; i < fileData.size(); ++i )
	{
		switch(electrodeNum)
		{
		case 1:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e1) );
			break;
			
		case 2:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e2) );
			break;
			
		case 3:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e3) );
			break;
			
		case 4:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e4) );
			break;
			
		case 5:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e5) );
			break;
			
		case 6:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e6) );
			break;
			
		case 7:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e7) );
			break;
			
		case 8:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e8) );
			break;
			
		case 9:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e9) );
			break;
			
		case 10:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e10) );
			break;
			
		case 11:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e11) );
			break;
			
		case 12:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e12) );
			break;
			
		case 13:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e13) );
			break;
			
		case 14:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e14) );
			break;
			
		case 15:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e15) );
			break;
			
		case 16:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e16) );
			break;
			
		case 17:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e17) );
			break;
			
		case 18:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e18) );
			break;
			
		case 19:
			organizedData[fileData[i].mData.tdc].push_back( std::pair<uint16_t, uint16_t>(fileData[i].mData.pdc, fileData[i].mData.e19) );
			break;
			
		default:
			break;
		}
	}


	// --- Plot Data --- //

	PLFLT ymin = 1000000.0, ymax = -1000000.0;
	for( std::map< uint16_t, std::vector< std::pair<uint16_t, uint16_t> > >::iterator it = organizedData.begin(); it != organizedData.end(); ++it )
	{
		for( std::size_t i = 0; i < it->second.size(); ++i )
		{
			ymax = std::max<PLFLT>(ymax, it->second[i].first);
			ymax = std::max<PLFLT>(ymax, it->second[i].second);
			
			ymin = std::min<PLFLT>(ymin, it->second[i].first);
			ymin = std::min<PLFLT>(ymin, it->second[i].second);
		}
	}

	
#ifdef WIN32
#define PLOT_DRIVER "wingcc"
#else
#define PLOT_DRIVER "xcairo"
#endif	
	
	// First plot the electrode and PDC values per TDC value.
	std::vector< std::pair<PLFLT*, int> > xVector, pdcVector, eVector;
	for( std::map< uint16_t, std::vector< std::pair<uint16_t, uint16_t> > >::iterator it = organizedData.begin(); it != organizedData.end(); ++it )
	{
		xVector.push_back( std::pair<PLFLT*, int>(new PLFLT[it->second.size()], it->second.size()) );
		pdcVector.push_back( std::pair<PLFLT*, int>(new PLFLT[it->second.size()], it->second.size()) );
		eVector.push_back( std::pair<PLFLT*, int>(new PLFLT[it->second.size()], it->second.size()) );

		for( std::size_t i = 0; i < it->second.size(); ++i )
		{
			xVector[xVector.size()-1].first[i] = it->first;
			pdcVector[pdcVector.size()-1].first[i] = it->second[i].first;
			eVector[eVector.size()-1].first[i] = it->second[i].second;
		}
	}

	// Now plot the data from the curve.
	cElectrodeTdcCompensator* pComp = &(mCompensators[electrodeNum-1]);
	PLFLT* pX = new PLFLT[organizedData.size()];
	PLFLT* pCompOut = new PLFLT[organizedData.size()];
	PLFLT* pAvgFileE = new PLFLT[organizedData.size()];
	int index = 0;
	for( std::map< uint16_t, std::vector< std::pair<uint16_t, uint16_t> > >::iterator it = organizedData.begin(); it != organizedData.end(); ++it )
	{
		pX[index] = it->first;
		
		pAvgFileE[index] = 0.0;
		double avgPdc = 0;
		for( std::size_t i = 0; i < it->second.size(); ++i )
		{
			avgPdc += it->second[i].first;
			pAvgFileE[index] += it->second[i].second;
		}
		avgPdc /= it->second.size();
		pAvgFileE[index] /= it->second.size();

		pCompOut[index] = pComp->GetUnforcedElectrodeValue(it->first, avgPdc);

		ymax = std::max<PLFLT>(ymax, pCompOut[index]);
		ymax = std::max<PLFLT>(ymax, pAvgFileE[index]);
			
		ymin = std::min<PLFLT>(ymin, pCompOut[index]);
		ymin = std::min<PLFLT>(ymin, pAvgFileE[index]);
		
		++index;
	}

	plstream plottingStream(1, 1, 255, 255, 255, PLOT_DRIVER);
	plottingStream.init();
	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.env( organizedData.begin()->first, organizedData.rbegin()->first, ymin*0.99, ymax*1.01, 0, 0);
	std::ostringstream title;
	title << "Compensation Curve vs File Data - Electrode " << electrodeNum;
	plottingStream.lab("TDC", "Electrode", title.str().c_str());

	for( std::size_t i = 0; i < xVector.size(); ++i )
	{
		plottingStream.col0(nUtils::enumPLplotColor_MAGENTA);
		plottingStream.poin( xVector[i].second, xVector[i].first, pdcVector[i].first, 9 );

		plottingStream.col0(nUtils::enumPLplotColor_RED);
		plottingStream.poin( xVector[i].second, xVector[i].first, eVector[i].first, 9 );

		delete[] xVector[i].first;
		delete[] pdcVector[i].first;
		delete[] eVector[i].first;
	}
	
	plottingStream.col0(nUtils::enumPLplotColor_BLUE);
	plottingStream.line( organizedData.size(), pX, pCompOut );

	plottingStream.col0(nUtils::enumPLplotColor_GREEN);
	plottingStream.line( organizedData.size(), pX, pAvgFileE );
	
	delete[] pX;
	delete[] pCompOut;
	delete[] pAvgFileE;
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
