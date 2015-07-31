#include "function_fit_nls.h"
#include "data_file_reader.h"
#include "utils.h"
#include <fstream>
#include <map>
#include <numeric>
#include <iostream>
#include <plplot/plplot.h>
#include <plplot/plstream.h>
#include <ceres/ceres.h>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cFunctionFitNLS::cFunctionFitNLS()
{

}

////////////////////////////////////////////////////////////////////////////////

cFunctionFitNLS::~cFunctionFitNLS()
{

}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::TrainAgainstDataFiles(const std::vector<std::string>& files)
{
	sTdcElectrodeData data = ParseFiles(files);

	google::InitGoogleLogging("biotac_cal");

	// double a = -0.0001, b = -3000.0, c = 2.0, d = 3500.0;
	double a = 0, b = 0, c = 0, d = 0;
	double init_a = a, init_b = b, init_c = c, init_d = d;
	ceres::Problem problem;
	int numDataPoints = data.mE19.size();
	std::cout << "Number of data points:  " << numDataPoints << std::endl;
	for( int i = 0; i < numDataPoints; ++i )
	{
		std::cout << "Adding data point " << i << "..." << std::endl;
		problem.AddResidualBlock(
			new ceres::AutoDiffCostFunction<cExponentialResidual, 1, 1, 1, 1, 1>(
				new cExponentialResidual( data.mE19[i].first, data.mE19[i].second ) ),
			NULL,
			&a, &b, &c, &d );
	}

	std::cout << "Setting up options" << std::endl;
	ceres::Solver::Options options;
	options.max_num_iterations = 1000;
	options.linear_solver_type = ceres::DENSE_QR;
	options.minimizer_progress_to_stdout = true;

	std::cout << "Solving..." << std::endl;
	ceres::Solver::Summary summary;
	ceres::Solve(options, &problem, &summary);
	std::cout << "\r\n" << summary.FullReport() << "\r\n";
	std::cout << "Computed for E19:" << "\r\n";
	std::cout << "Initial a: " << init_a << "\t\t b: " << init_b << "\t\t c: " << init_c << "\t\t d: " << init_d << "\r\n";
	std::cout << "Final   a: " << a << "\t\t b: " << b << "\t\t c: " << c << "\t\t d: " << d << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::TestAgainstDataFiles(const std::vector<std::string>& files)
{
	sTdcElectrodeData data = ParseFiles(files);
}

////////////////////////////////////////////////////////////////////////////////
//  Helper Functions
////////////////////////////////////////////////////////////////////////////////

cFunctionFitNLS::sTdcElectrodeData cFunctionFitNLS::ParseFiles(const std::vector<std::string>& files)
{
	// --- Parse Files --- //
	
	std::map< int, std::vector<uint16_t> > e1, e2, e3, e4, e5, e6, e7, e8, e9, e10,
		                                   e11, e12, e13, e14, e15, e16, e17, e18, e19;

	for( std::size_t i = 0; i < files.size(); ++i )
	{
		cDataFileReader reader(files[i]);
		std::vector<biotac_tune_data> data = reader.GetData();
		std::size_t numDataPoints = data.size();
		for( std::size_t dataIndex = 0; dataIndex < numDataPoints; ++dataIndex )
		{
			e1[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e1);
			e2[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e2);
			e3[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e3);
			e4[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e4);
			e5[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e5);
			e6[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e6);
			e7[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e7);
			e8[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e8);
			e9[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e9);
			e10[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e10);
			e11[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e11);
			e12[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e12);
			e13[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e13);
			e14[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e14);
			e15[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e15);
			e16[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e16);
			e17[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e17);
			e18[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e18);
			e19[data[dataIndex].mData.tdc].push_back(data[dataIndex].mData.e19);
		}
	}


	// --- Generate Data --- //

	sTdcElectrodeData data;
	FillStructMember( data.mE1, e1 );
	FillStructMember( data.mE2, e2 );
	FillStructMember( data.mE3, e3 );
	FillStructMember( data.mE4, e4 );
	FillStructMember( data.mE5, e5 );
	FillStructMember( data.mE6, e6 );
	FillStructMember( data.mE7, e7 );
	FillStructMember( data.mE8, e8 );
	FillStructMember( data.mE9, e9 );
	FillStructMember( data.mE10, e10 );
	FillStructMember( data.mE11, e11 );
	FillStructMember( data.mE12, e12 );
	FillStructMember( data.mE13, e13 );
	FillStructMember( data.mE14, e14 );
	FillStructMember( data.mE15, e15 );
	FillStructMember( data.mE16, e16 );
	FillStructMember( data.mE17, e17 );
	FillStructMember( data.mE18, e18 );
	FillStructMember( data.mE19, e19 );

	return data;
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::FillStructMember( std::vector< std::pair<double, double> >& structData,
					   const std::map< int, std::vector<uint16_t> >& rawData )
{
	for( std::map< int, std::vector<uint16_t> >::const_iterator it = rawData.begin();
		 it != rawData.end();
		 ++it )
	{
		double sum = std::accumulate(it->second.begin(), it->second.end(), 0);
		sum /= static_cast<double>(it->second.size());
		std::pair<double, double> p( static_cast<double>(it->first), sum );
		structData.push_back(p);
	}
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::PlotResults(const std::vector<float>& calculated, const std::vector<float>& actual )
{
#ifdef WIN32
	std::string plotDriver = "wingcc";
#else
	std::string plotDriver = "xcairo";
#endif

	// --- Initialize --- //
	
	plstream plottingStream(1, 1, 255, 255, 255, plotDriver.c_str());
	plottingStream.init();

	PLFLT ymin = 1000000.0, ymax = -1000000.0;
	PLFLT* pX = new PLFLT[calculated.size()];
	PLFLT* pCalculated = new PLFLT[calculated.size()];
	PLFLT* pActual = new PLFLT[calculated.size()];
	for( std::size_t i = 0; i < calculated.size(); ++i )
	{
		pX[i] = i;
		pCalculated[i] = calculated[i];
		pActual[i] = actual[i];

		ymax = std::max<PLFLT>(ymax, calculated[i]);
		ymax = std::max<PLFLT>(ymax, actual[i]);

		ymin = std::min<PLFLT>(ymin, calculated[i]);
		ymin = std::min<PLFLT>(ymin, actual[i]);
	}


	// --- Generate Plot --- //

	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.env(0, calculated.size(), ymin, ymax*1.01, 0, 0);
	plottingStream.lab("Samples", "Magnitude", "Neural Network Analysis");

	plottingStream.col0(nUtils::enumPLplotColor_BLUE);
	plottingStream.line(calculated.size(), pX, pCalculated);

	plottingStream.col0(nUtils::enumPLplotColor_GREEN);
	plottingStream.line(calculated.size(), pX, pActual);


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
	text[0] = "NN Output";
	lineStyles[0] = 1;
	lineWidths[0] = 1.0;

	optArray[1] = PL_LEGEND_LINE;
	textColors[1] = nUtils::enumPLplotColor_GREEN;
	lineColors[1] = nUtils::enumPLplotColor_GREEN;
	text[1] = "Reference";
	lineStyles[1] = 1;
	lineWidths[1] = 1.0;

	PLFLT legendWidth, legendHeight;
	plottingStream.legend( &legendWidth, &legendHeight,
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
}


