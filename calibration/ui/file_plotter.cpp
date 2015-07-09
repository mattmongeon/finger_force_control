#include "file_plotter.h"
#include "../common/biotac_comm.h"
#include "utils.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cFilePlotter::cFilePlotter(const std::string& file)
	: mPlottingStream( 1, 1, 255, 255, 255, "xcairo" )
{
	// --- Read Data File --- //

	std::ifstream inFile(file.c_str());
	if( !inFile.is_open() )
	{
		std::cout << "Could not open file \'" << file << "\' for plotting!" << std::endl;
		throw std::invalid_argument("Could not open file for plotting!");
	}

	std::vector<biotac_tune_data> fileData;
	biotac_tune_data data;
	while(inFile)
	{
		inFile.read(reinterpret_cast<char*>(&data), sizeof(biotac_tune_data));
		fileData.push_back(data);

	}

	inFile.close();


	// --- Initialize Plotting --- //
	
	mPlottingStream.star(2, 2);
	
	
	// --- Plot Data --- //

	ConfigureAndPlotForce(fileData, file);
	ConfigureAndPlotPressTemp(fileData, file);
	ConfigureAndPlotElectrodes1(fileData, file);
	ConfigureAndPlotElectrodes2(fileData, file);
}

////////////////////////////////////////////////////////////////////////////////

cFilePlotter::~cFilePlotter()
{

}

////////////////////////////////////////////////////////////////////////////////
//  Helper Functions
////////////////////////////////////////////////////////////////////////////////

void cFilePlotter::ConfigureAndPlotForce(const std::vector<biotac_tune_data>& data,
										 const std::string& fileName)
{
	PLFLT* pX = new PLFLT[data.size()];
	PLFLT* pLoadCell = new PLFLT[data.size()];
	PLFLT ymin = 1000000.0, ymax = -1000000.0;
	for( size_t i = 0; i < data.size(); ++i )
	{
		pX[i] = i;
		pLoadCell[i] = data[i].mLoadCell_g;

		if( pLoadCell[i] > ymax )
			ymax = pLoadCell[i];

		if( pLoadCell[i] < ymin )
			ymin = pLoadCell[i];
	}

	mPlottingStream.col0(1);
	mPlottingStream.env(0, data.size(), ymin, ymax*1.001, 0, 0);
	mPlottingStream.col0(3);
	mPlottingStream.lab("Samples", "Force", "Reference and Measured Force");
	mPlottingStream.col0(nUtils::enumPLplotColor_BLUE);
	mPlottingStream.line( data.size(), pX, pLoadCell );
	
	// NEED TO ADD LEGENDS
	
	delete[] pX;
	delete[] pLoadCell;
}

////////////////////////////////////////////////////////////////////////////////

void cFilePlotter::ConfigureAndPlotPressTemp(const std::vector<biotac_tune_data>& data,
											 const std::string& fileName)
{
	PLFLT* pX = new PLFLT[data.size()];
	PLFLT* pPDC = new PLFLT[data.size()];
	PLFLT* pPAC = new PLFLT[data.size()];
	PLFLT* pTDC = new PLFLT[data.size()];
	PLFLT* pTAC = new PLFLT[data.size()];
	PLFLT ymin = 1000000.0, ymax = -1000000.0;
	for( size_t i = 0; i < data.size(); ++i )
	{
		pX[i] = i;
		pPDC[i] = data[i].mData.pdc;
		pPAC[i] = data[i].mData.pac;
		pTDC[i] = data[i].mData.tdc;
		pTAC[i] = data[i].mData.tac;

		ymax = std::max(pPDC[i], ymax);
		ymax = std::max(pPAC[i], ymax);
		ymax = std::max(pTDC[i], ymax);
		ymax = std::max(pTAC[i], ymax);
		
		ymin = std::min(pPDC[i], ymin);
		ymin = std::min(pPAC[i], ymin);
		ymin = std::min(pTDC[i], ymin);
		ymin = std::min(pTAC[i], ymin);
	}
	
	mPlottingStream.col0(1);
	mPlottingStream.env(0, data.size(), ymin, ymax*1.001, 0, 0);
	mPlottingStream.col0(nUtils::enumPLplotColor_GREEN);
	mPlottingStream.lab("Samples", "Pressure/Temp", "Pressure and Temperature");

	mPlottingStream.col0(nUtils::enumPLplotColor_RED);
	mPlottingStream.line( data.size(), pX, pPDC );

	mPlottingStream.col0(nUtils::enumPLplotColor_GREEN);
	mPlottingStream.line( data.size(), pX, pPAC );

	mPlottingStream.col0(nUtils::enumPLplotColor_BLUE);
	mPlottingStream.line( data.size(), pX, pTDC );

	mPlottingStream.col0(nUtils::enumPLplotColor_MAGENTA);
	mPlottingStream.line( data.size(), pX, pTAC );
	
	// NEED TO ADD LEGENDS
	
	delete[] pX;
	delete[] pPDC;
}

////////////////////////////////////////////////////////////////////////////////

void cFilePlotter::ConfigureAndPlotElectrodes1(const std::vector<biotac_tune_data>& data,
											   const std::string& fileName)
{
	PLFLT* pX = new PLFLT[data.size()];
	PLFLT* pE1 = new PLFLT[data.size()];
	PLFLT* pE2 = new PLFLT[data.size()];
	PLFLT* pE3 = new PLFLT[data.size()];
	PLFLT* pE4 = new PLFLT[data.size()];
	PLFLT* pE5 = new PLFLT[data.size()];
	PLFLT* pE6 = new PLFLT[data.size()];
	PLFLT* pE7 = new PLFLT[data.size()];
	PLFLT* pE8 = new PLFLT[data.size()];
	PLFLT* pE9 = new PLFLT[data.size()];
	
	PLFLT ymin = 1000000.0, ymax = -1000000.0;
	for( size_t i = 0; i < data.size(); ++i )
	{
		pX[i] = i;
		pE1[i] = data[i].mData.e1;
		pE2[i] = data[i].mData.e2;
		pE3[i] = data[i].mData.e3;
		pE4[i] = data[i].mData.e4;
		pE5[i] = data[i].mData.e5;
		pE6[i] = data[i].mData.e6;
		pE7[i] = data[i].mData.e7;
		pE8[i] = data[i].mData.e8;
		pE9[i] = data[i].mData.e9;

		ymax = std::max(pE1[i], ymax);
		ymax = std::max(pE2[i], ymax);
		ymax = std::max(pE3[i], ymax);
		ymax = std::max(pE4[i], ymax);
		ymax = std::max(pE5[i], ymax);
		ymax = std::max(pE6[i], ymax);
		ymax = std::max(pE7[i], ymax);
		ymax = std::max(pE8[i], ymax);
		ymax = std::max(pE9[i], ymax);
		
		ymin = std::min(pE1[i], ymin);
		ymin = std::min(pE2[i], ymin);
		ymin = std::min(pE3[i], ymin);
		ymin = std::min(pE4[i], ymin);
		ymin = std::min(pE5[i], ymin);
		ymin = std::min(pE6[i], ymin);
		ymin = std::min(pE7[i], ymin);
		ymin = std::min(pE8[i], ymin);
		ymin = std::min(pE9[i], ymin);
	}
	
	mPlottingStream.col0(1);
	mPlottingStream.env(0, data.size(), ymin, ymax*1.001, 0, 0);
	mPlottingStream.col0(nUtils::enumPLplotColor_GREEN);
	mPlottingStream.lab("Samples", "Electrode Measurement", "Electrodes 1-9");
	
	mPlottingStream.col0(nUtils::enumPLplotColor_RED);
	mPlottingStream.line( data.size(), pX, pE1 );

	mPlottingStream.col0(nUtils::enumPLplotColor_YELLOW);
	mPlottingStream.line( data.size(), pX, pE2 );

	mPlottingStream.col0(nUtils::enumPLplotColor_GREEN);
	mPlottingStream.line( data.size(), pX, pE3 );

	mPlottingStream.col0(nUtils::enumPLplotColor_AQUAMARINE);
	mPlottingStream.line( data.size(), pX, pE4 );

	mPlottingStream.col0(nUtils::enumPLplotColor_PINK);
	mPlottingStream.line( data.size(), pX, pE5 );

	mPlottingStream.col0(nUtils::enumPLplotColor_WHEAT);
	mPlottingStream.line( data.size(), pX, pE6 );

	mPlottingStream.col0(nUtils::enumPLplotColor_GREY);
	mPlottingStream.line( data.size(), pX, pE7 );

	mPlottingStream.col0(nUtils::enumPLplotColor_BROWN);
	mPlottingStream.line( data.size(), pX, pE8 );

	mPlottingStream.col0(nUtils::enumPLplotColor_BLUE);
	mPlottingStream.line( data.size(), pX, pE9 );
	
	// NEED TO ADD LEGENDS
	
	delete[] pX;
	delete[] pE1;
	delete[] pE2;
	delete[] pE3;
	delete[] pE4;
	delete[] pE5;
	delete[] pE6;
	delete[] pE7;
	delete[] pE8;
	delete[] pE9;
}

////////////////////////////////////////////////////////////////////////////////

void cFilePlotter::ConfigureAndPlotElectrodes2(const std::vector<biotac_tune_data>& data,
											   const std::string& fileName)
{
	PLFLT* pX = new PLFLT[data.size()];
	PLFLT* pE10 = new PLFLT[data.size()];
	PLFLT* pE11 = new PLFLT[data.size()];
	PLFLT* pE12 = new PLFLT[data.size()];
	PLFLT* pE13 = new PLFLT[data.size()];
	PLFLT* pE14 = new PLFLT[data.size()];
	PLFLT* pE15 = new PLFLT[data.size()];
	PLFLT* pE16 = new PLFLT[data.size()];
	PLFLT* pE17 = new PLFLT[data.size()];
	PLFLT* pE18 = new PLFLT[data.size()];
	PLFLT* pE19 = new PLFLT[data.size()];
	
	PLFLT ymin = 1000000.0, ymax = -1000000.0;
	for( size_t i = 0; i < data.size(); ++i )
	{
		pX[i] = i;
		pE10[i] = data[i].mData.e10;
		pE11[i] = data[i].mData.e11;
		pE12[i] = data[i].mData.e12;
		pE13[i] = data[i].mData.e13;
		pE14[i] = data[i].mData.e14;
		pE15[i] = data[i].mData.e15;
		pE16[i] = data[i].mData.e16;
		pE17[i] = data[i].mData.e17;
		pE18[i] = data[i].mData.e18;
		pE19[i] = data[i].mData.e19;

		ymax = std::max(pE10[i], ymax);
		ymax = std::max(pE11[i], ymax);
		ymax = std::max(pE12[i], ymax);
		ymax = std::max(pE13[i], ymax);
		ymax = std::max(pE14[i], ymax);
		ymax = std::max(pE15[i], ymax);
		ymax = std::max(pE16[i], ymax);
		ymax = std::max(pE17[i], ymax);
		ymax = std::max(pE18[i], ymax);
		ymax = std::max(pE19[i], ymax);
		
		ymin = std::min(pE10[i], ymin);
		ymin = std::min(pE11[i], ymin);
		ymin = std::min(pE12[i], ymin);
		ymin = std::min(pE13[i], ymin);
		ymin = std::min(pE14[i], ymin);
		ymin = std::min(pE15[i], ymin);
		ymin = std::min(pE16[i], ymin);
		ymin = std::min(pE17[i], ymin);
		ymin = std::min(pE18[i], ymin);
		ymin = std::min(pE19[i], ymin);
	}
	
	mPlottingStream.col0(1);
	mPlottingStream.env(0, data.size(), ymin, ymax*1.001, 0, 0);
	mPlottingStream.col0(nUtils::enumPLplotColor_GREEN);
	mPlottingStream.lab("Samples", "Electrode Measurement", "Electrodes 11-19");
	
	mPlottingStream.col0(nUtils::enumPLplotColor_RED);
	mPlottingStream.line( data.size(), pX, pE10 );

	mPlottingStream.col0(nUtils::enumPLplotColor_YELLOW);
	mPlottingStream.line( data.size(), pX, pE11 );

	mPlottingStream.col0(nUtils::enumPLplotColor_GREEN);
	mPlottingStream.line( data.size(), pX, pE12 );

	mPlottingStream.col0(nUtils::enumPLplotColor_AQUAMARINE);
	mPlottingStream.line( data.size(), pX, pE13 );

	mPlottingStream.col0(nUtils::enumPLplotColor_PINK);
	mPlottingStream.line( data.size(), pX, pE14 );

	mPlottingStream.col0(nUtils::enumPLplotColor_WHEAT);
	mPlottingStream.line( data.size(), pX, pE15 );

	mPlottingStream.col0(nUtils::enumPLplotColor_GREY);
	mPlottingStream.line( data.size(), pX, pE16 );

	mPlottingStream.col0(nUtils::enumPLplotColor_BROWN);
	mPlottingStream.line( data.size(), pX, pE17 );

	mPlottingStream.col0(nUtils::enumPLplotColor_BLUE);
	mPlottingStream.line( data.size(), pX, pE18 );

	mPlottingStream.col0(nUtils::enumPLplotColor_BLUE_VIOLET);
	mPlottingStream.line( data.size(), pX, pE19 );
	
	// NEED TO ADD LEGENDS
	
	delete[] pX;
	delete[] pE10;
	delete[] pE11;
	delete[] pE12;
	delete[] pE13;
	delete[] pE14;
	delete[] pE15;
	delete[] pE16;
	delete[] pE17;
	delete[] pE18;
	delete[] pE19;
}
