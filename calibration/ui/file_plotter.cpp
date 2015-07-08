#include "file_plotter.h"
#include "../common/biotac_comm.h"
#include <fstream>
#include <vector>
#include <iostream>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cFilePlotter::cFilePlotter(const std::string& file)
	: mPlotStream( 1, 1, 255, 255, 255, "xcairo" )
{
	// --- Read Data File --- //

	std::ifstream inFile(file.c_str());

	std::vector<biotac_tune_data> fileData;
	biotac_tune_data data;
	while(!inFile.eof())
	{
		inFile.read(reinterpret_cast<char*>(&data), sizeof(biotac_tune_data));
		fileData.push_back(data);
	}

	inFile.close();


	// --- Prepare Data To Plot --- //

	PLFLT* pY = new PLFLT[fileData.size()];
	PLFLT* pX = new PLFLT[fileData.size()];
	PLFLT ymin = 1000000.0, ymax = -1000.0;
	for( size_t i = 0; i < fileData.size(); ++i )
	{
		pX[i] = i;
		pY[i] = fileData[i].mData.pdc;

		if( pY[i] > ymax )
			ymax = pY[i];

		if( pY[i] < ymin )
			ymin = pY[i];

		std::cout << i << "\t" << pX[i] << "\t" << pY[i] << std::endl;
	}


	// --- Plot It --- //

	mPlotStream.init();

	mPlotStream.env(0, fileData.size(), ymin, ymax*1.01, 0, 0);
	mPlotStream.lab("Samples", "Data", file.c_str());
	mPlotStream.col0(3);
	mPlotStream.line( fileData.size(), pX, pY );
	
	
	delete[] pX;
	delete[] pY;
}

////////////////////////////////////////////////////////////////////////////////

cFilePlotter::~cFilePlotter()
{

}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

