#include "file_plotter.h"
#include "../common/biotac_comm.h"
#include "utils.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>


#ifdef WIN32
#define PLOT_DRIVER "wingcc"
#else
#define PLOT_DRIVER "xcairo"
#endif

#define BIOTAC_FREQUENCY 100.0


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cFilePlotter::cFilePlotter(const std::string& file)
	: mPlottingStream( 1, 1, 255, 255, 255, PLOT_DRIVER )
{
	// --- Read Data File --- //

	std::ifstream inFile(file.c_str());
	if( !inFile.is_open() )
	{
		std::cout << "Could not open file \'" << file << "\' for plotting!" << std::endl;
		throw std::invalid_argument("Could not open file for plotting!");
	}

	inFile.seekg(0, std::ios::end);
	std::streampos fileSize = inFile.tellg();
	inFile.seekg(0, std::ios::beg);
	
	std::vector<biotac_tune_data> fileData(fileSize/sizeof(biotac_tune_data));
	inFile.read( reinterpret_cast<char*>(&(fileData[0])), fileSize);
	
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
	PLFLT* pReference = new PLFLT[data.size()];


	// --- Prepare Data --- //
	
	PLFLT ymin = 1000000.0, ymax = -1000000.0;
	for( size_t i = 0; i < data.size(); ++i )
	{
		pX[i] = i / BIOTAC_FREQUENCY;
		pLoadCell[i] = data[i].mLoadCell_g;
		pReference[i] = data[i].mReference_g;

		ymax = std::max(pLoadCell[i], ymax);
		ymax = std::max(pReference[i], ymax);

		ymin = std::min(pLoadCell[i], ymin);
		ymin = std::min(pReference[i], ymin);
	}

	// Just in case.
	if( ymin == ymax )
	{
		ymin -= 0.01;
		ymax += 0.01;
	}


	// --- Generate Plot --- //
	
	mPlottingStream.col0(nUtils::enumPLplotColor_RED);
	mPlottingStream.env(0, data.size() / BIOTAC_FREQUENCY, ymin*0.99, ymax*1.01, 0, 0);
	mPlottingStream.lab("Time (s)", "Force (g)", "Reference and Measured Force");

	mPlottingStream.col0(nUtils::enumPLplotColor_BLUE);
	mPlottingStream.line( data.size(), pX, pLoadCell );
	
	mPlottingStream.col0(nUtils::enumPLplotColor_GREEN);
	mPlottingStream.line( data.size(), pX, pReference );


	// --- Create Legend --- //

	PLINT optArray[2];
	PLINT textColors[2];
	PLINT lineColors[2];
	PLINT lineStyles[2];
	PLFLT lineWidths[2];
	const char* text[2];

	optArray[0] = PL_LEGEND_LINE;
	textColors[0] = nUtils::enumPLplotColor_BLUE;
	lineColors[0] = nUtils::enumPLplotColor_BLUE;
	text[0] = "Load Cell (g)";
	lineStyles[0] = 1;
	lineWidths[0] = 1.0;

	optArray[1] = PL_LEGEND_LINE;
	textColors[1] = nUtils::enumPLplotColor_GREEN;
	text[1] = "Reference (g)";
	lineColors[1] = nUtils::enumPLplotColor_GREEN;
	lineStyles[1] = 1;
	lineWidths[1] = 1.0;

	PLFLT legendWidth, legendHeight;
	mPlottingStream.legend( &legendWidth, &legendHeight,
							PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX,
							PL_POSITION_RIGHT | PL_POSITION_BOTTOM,
							0.0, 0.0, 0.01,
							15, 1, 1, 0, 0,
							2, optArray,
							0.5, 0.5, 1.0, 0.0,
							textColors, (const char* const *) text,
							NULL, NULL, NULL, NULL,
							lineColors, lineStyles, lineWidths,
							NULL, NULL, NULL, NULL );
	
	delete[] pX;
	delete[] pLoadCell;
	delete[] pReference;
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
		pX[i] = i / BIOTAC_FREQUENCY;
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

	// Just in case.
	if( ymin == ymax )
	{
		ymin -= 0.01;
		ymax += 0.01;
	}


	// --- Generate Plot --- //
	
	mPlottingStream.col0(nUtils::enumPLplotColor_RED);
	mPlottingStream.env(0, data.size() / BIOTAC_FREQUENCY, ymin*0.99, ymax*1.01, 0, 0);
	mPlottingStream.lab("Time (s)", "Pressure/Temp", "Pressure and Temperature");

	mPlottingStream.col0(nUtils::enumPLplotColor_GREEN);
	mPlottingStream.line( data.size(), pX, pPDC );

	mPlottingStream.col0(nUtils::enumPLplotColor_BLUE);
	mPlottingStream.line( data.size(), pX, pPAC );

	mPlottingStream.col0(nUtils::enumPLplotColor_AQUAMARINE);
	mPlottingStream.line( data.size(), pX, pTDC );

	mPlottingStream.col0(nUtils::enumPLplotColor_MAGENTA);
	mPlottingStream.line( data.size(), pX, pTAC );
	

	// --- Create Legend --- //

	PLINT optArray[4];
	PLINT textColors[4];
	PLINT lineColors[4];
	PLINT lineStyles[4];
	PLFLT lineWidths[4];
	const char* text[4];

	optArray[0] = PL_LEGEND_LINE;
	textColors[0] = nUtils::enumPLplotColor_GREEN;
	lineColors[0] = nUtils::enumPLplotColor_GREEN;
	text[0] = "PDC";
	lineStyles[0] = 1;
	lineWidths[0] = 1.0;

	optArray[1] = PL_LEGEND_LINE;
	textColors[1] = nUtils::enumPLplotColor_BLUE;
	text[1] = "PAC";
	lineColors[1] = nUtils::enumPLplotColor_BLUE;
	lineStyles[1] = 1;
	lineWidths[1] = 1.0;

	optArray[2] = PL_LEGEND_LINE;
	textColors[2] = nUtils::enumPLplotColor_AQUAMARINE;
	lineColors[2] = nUtils::enumPLplotColor_AQUAMARINE;
	text[2] = "TDC";
	lineStyles[2] = 1;
	lineWidths[2] = 1.0;

	optArray[3] = PL_LEGEND_LINE;
	textColors[3] = nUtils::enumPLplotColor_MAGENTA;
	text[3] = "TAC";
	lineColors[3] = nUtils::enumPLplotColor_MAGENTA;
	lineStyles[3] = 1;
	lineWidths[3] = 1.0;

	PLFLT legendWidth, legendHeight;
	mPlottingStream.legend( &legendWidth, &legendHeight,
							PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX,
							PL_POSITION_RIGHT | PL_POSITION_BOTTOM,
							0.0, 0.0, 0.01,
							15, 1, 1, 0, 0,
							4, optArray,
							0.5, 0.5, 1.0, 0.0,
							textColors, (const char* const *) text,
							NULL, NULL, NULL, NULL,
							lineColors, lineStyles, lineWidths,
							NULL, NULL, NULL, NULL );
	
	
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
		pX[i] = i / BIOTAC_FREQUENCY;
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
	
	// Just in case.
	if( ymin == ymax )
	{
		ymin -= 0.01;
		ymax += 0.01;
	}


	// --- Generate Plot --- //
	
	mPlottingStream.col0(nUtils::enumPLplotColor_RED);
	mPlottingStream.env(0, data.size() / BIOTAC_FREQUENCY, ymin*0.99, ymax*1.01, 0, 0);
	mPlottingStream.lab("Time (s)", "Electrode Measurement", "Electrodes 1-9");
	
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
	
	
	// --- Create Legend --- //

	PLINT optArray[9];
	PLINT textColors[9];
	PLINT lineColors[9];
	PLINT lineStyles[9];
	PLFLT lineWidths[9];
	const char* text[9];

	optArray[0] = PL_LEGEND_LINE;
	textColors[0] = nUtils::enumPLplotColor_RED;
	lineColors[0] = nUtils::enumPLplotColor_RED;
	text[0] = "E1";
	lineStyles[0] = 1;
	lineWidths[0] = 1.0;

	optArray[1] = PL_LEGEND_LINE;
	textColors[1] = nUtils::enumPLplotColor_YELLOW;
	text[1] = "E2";
	lineColors[1] = nUtils::enumPLplotColor_YELLOW;
	lineStyles[1] = 1;
	lineWidths[1] = 1.0;

	optArray[2] = PL_LEGEND_LINE;
	textColors[2] = nUtils::enumPLplotColor_GREEN;
	lineColors[2] = nUtils::enumPLplotColor_GREEN;
	text[2] = "E3";
	lineStyles[2] = 1;
	lineWidths[2] = 1.0;

	optArray[3] = PL_LEGEND_LINE;
	textColors[3] = nUtils::enumPLplotColor_AQUAMARINE;
	text[3] = "E4";
	lineColors[3] = nUtils::enumPLplotColor_AQUAMARINE;
	lineStyles[3] = 1;
	lineWidths[3] = 1.0;

	optArray[4] = PL_LEGEND_LINE;
	textColors[4] = nUtils::enumPLplotColor_PINK;
	lineColors[4] = nUtils::enumPLplotColor_PINK;
	text[4] = "E5";
	lineStyles[4] = 1;
	lineWidths[4] = 1.0;

	optArray[5] = PL_LEGEND_LINE;
	textColors[5] = nUtils::enumPLplotColor_WHEAT;
	text[5] = "E6";
	lineColors[5] = nUtils::enumPLplotColor_WHEAT;
	lineStyles[5] = 1;
	lineWidths[5] = 1.0;

	optArray[6] = PL_LEGEND_LINE;
	textColors[6] = nUtils::enumPLplotColor_GREY;
	lineColors[6] = nUtils::enumPLplotColor_GREY;
	text[6] = "E7";
	lineStyles[6] = 1;
	lineWidths[6] = 1.0;

	optArray[7] = PL_LEGEND_LINE;
	textColors[7] = nUtils::enumPLplotColor_BROWN;
	text[7] = "E8";
	lineColors[7] = nUtils::enumPLplotColor_BROWN;
	lineStyles[7] = 1;
	lineWidths[7] = 1.0;

	optArray[8] = PL_LEGEND_LINE;
	textColors[8] = nUtils::enumPLplotColor_BLUE;
	text[8] = "E9";
	lineColors[8] = nUtils::enumPLplotColor_BLUE;
	lineStyles[8] = 1;
	lineWidths[8] = 1.0;

	PLFLT legendWidth, legendHeight;
	mPlottingStream.legend( &legendWidth, &legendHeight,
							PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX,
							PL_POSITION_RIGHT | PL_POSITION_TOP,
							0.0, 0.0, 0.01,
							15, 1, 1, 0, 0,
							9, optArray,
							0.5, 0.5, 1.0, 0.0,
							textColors, (const char* const *) text,
							NULL, NULL, NULL, NULL,
							lineColors, lineStyles, lineWidths,
							NULL, NULL, NULL, NULL );
		
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
		pX[i] = i / BIOTAC_FREQUENCY;
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

	// Just in case.
	if( ymin == ymax )
	{
		ymin -= 0.01;
		ymax += 0.01;
	}


	// --- Generate Plot --- //
	
	mPlottingStream.col0(nUtils::enumPLplotColor_RED);
	mPlottingStream.env(0, data.size() / BIOTAC_FREQUENCY, ymin*0.99, ymax*1.01, 0, 0);
	mPlottingStream.lab("Time (s)", "Electrode Measurement", "Electrodes 11-19");
	
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
	

	// --- Create Legend --- //

	PLINT optArray[9];
	PLINT textColors[9];
	PLINT lineColors[9];
	PLINT lineStyles[9];
	PLFLT lineWidths[9];
	const char* text[9];

	optArray[0] = PL_LEGEND_LINE;
	textColors[0] = nUtils::enumPLplotColor_RED;
	lineColors[0] = nUtils::enumPLplotColor_RED;
	text[0] = "E10";
	lineStyles[0] = 1;
	lineWidths[0] = 1.0;

	optArray[1] = PL_LEGEND_LINE;
	textColors[1] = nUtils::enumPLplotColor_YELLOW;
	text[1] = "E11";
	lineColors[1] = nUtils::enumPLplotColor_YELLOW;
	lineStyles[1] = 1;
	lineWidths[1] = 1.0;

	optArray[2] = PL_LEGEND_LINE;
	textColors[2] = nUtils::enumPLplotColor_GREEN;
	lineColors[2] = nUtils::enumPLplotColor_GREEN;
	text[2] = "E12";
	lineStyles[2] = 1;
	lineWidths[2] = 1.0;

	optArray[3] = PL_LEGEND_LINE;
	textColors[3] = nUtils::enumPLplotColor_AQUAMARINE;
	text[3] = "E13";
	lineColors[3] = nUtils::enumPLplotColor_AQUAMARINE;
	lineStyles[3] = 1;
	lineWidths[3] = 1.0;

	optArray[4] = PL_LEGEND_LINE;
	textColors[4] = nUtils::enumPLplotColor_PINK;
	lineColors[4] = nUtils::enumPLplotColor_PINK;
	text[4] = "E14";
	lineStyles[4] = 1;
	lineWidths[4] = 1.0;

	optArray[5] = PL_LEGEND_LINE;
	textColors[5] = nUtils::enumPLplotColor_WHEAT;
	text[5] = "E15";
	lineColors[5] = nUtils::enumPLplotColor_WHEAT;
	lineStyles[5] = 1;
	lineWidths[5] = 1.0;

	optArray[6] = PL_LEGEND_LINE;
	textColors[6] = nUtils::enumPLplotColor_GREY;
	lineColors[6] = nUtils::enumPLplotColor_GREY;
	text[6] = "E16";
	lineStyles[6] = 1;
	lineWidths[6] = 1.0;

	optArray[7] = PL_LEGEND_LINE;
	textColors[7] = nUtils::enumPLplotColor_BROWN;
	text[7] = "E17";
	lineColors[7] = nUtils::enumPLplotColor_BROWN;
	lineStyles[7] = 1;
	lineWidths[7] = 1.0;

	optArray[8] = PL_LEGEND_LINE;
	textColors[8] = nUtils::enumPLplotColor_BLUE;
	text[8] = "E18";
	lineColors[8] = nUtils::enumPLplotColor_BLUE;
	lineStyles[8] = 1;
	lineWidths[8] = 1.0;

	optArray[9] = PL_LEGEND_LINE;
	textColors[9] = nUtils::enumPLplotColor_BLUE_VIOLET;
	text[9] = "E19";
	lineColors[9] = nUtils::enumPLplotColor_BLUE_VIOLET;
	lineStyles[9] = 1;
	lineWidths[9] = 1.0;

	PLFLT legendWidth, legendHeight;
	mPlottingStream.legend( &legendWidth, &legendHeight,
							PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX,
							PL_POSITION_RIGHT | PL_POSITION_TOP,
							0.0, 0.0, 0.01,
							15, 1, 1, 0, 0,
							10, optArray,
							0.5, 0.5, 1.0, 0.0,
							textColors, (const char* const *) text,
							NULL, NULL, NULL, NULL,
							lineColors, lineStyles, lineWidths,
							NULL, NULL, NULL, NULL );
		
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
