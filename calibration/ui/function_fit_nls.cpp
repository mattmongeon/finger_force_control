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


bool cFunctionFitNLS::mLoggingInitialized = false;


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cFunctionFitNLS::cFunctionFitNLS()
{
	if( !mLoggingInitialized )
		google::InitGoogleLogging("biotac_cal");
}

////////////////////////////////////////////////////////////////////////////////

cFunctionFitNLS::~cFunctionFitNLS()
{

}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

std::vector<cElectrodeTdcCompensator> cFunctionFitNLS::TrainAgainstDataFiles(const std::vector<std::string>& files)
{
	sTdcElectrodeData data = ParseFiles(files);
	std::vector<cElectrodeTdcCompensator> retVal;
	double a, b, c, d;
	
	std::cout << "E1:" << std::endl;
	FitToElectrodeData(data.mE1, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	std::cout << "E2:" << std::endl;
	FitToElectrodeData(data.mE2, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	std::cout << "E3:" << std::endl;
	FitToElectrodeData(data.mE3, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	std::cout << "E4:" << std::endl;
	FitToElectrodeData(data.mE4, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	std::cout << "E5:" << std::endl;
	FitToElectrodeData(data.mE5, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	std::cout << "E6:" << std::endl;
	FitToElectrodeData(data.mE6, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	std::cout << "E7:" << std::endl;
	FitToElectrodeData(data.mE7, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	std::cout << "E8:" << std::endl;
	FitToElectrodeData(data.mE8, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	std::cout << "E9:" << std::endl;
	FitToElectrodeData(data.mE9, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	std::cout << "E10:" << std::endl;
	FitToElectrodeData(data.mE10, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	std::cout << "E11:" << std::endl;
	FitToElectrodeData(data.mE11, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	std::cout << "E12:" << std::endl;
	FitToElectrodeData(data.mE12, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	std::cout << "E13:" << std::endl;
	FitToElectrodeData(data.mE13, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	std::cout << "E14:" << std::endl;
	FitToElectrodeData(data.mE14, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	std::cout << "E15:" << std::endl;
	FitToElectrodeData(data.mE15, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	std::cout << "E16:" << std::endl;
	FitToElectrodeData(data.mE16, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	std::cout << "E17:" << std::endl;
	FitToElectrodeData(data.mE17, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	std::cout << "E18:" << std::endl;
	FitToElectrodeData(data.mE18, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	std::cout << "E19:" << std::endl;
	FitToElectrodeData(data.mE19, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl;

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::TestAgainstDataFiles(const std::vector<std::string>& files,
										   const std::vector<cElectrodeTdcCompensator>& compensators)
{
	std::vector<double> errors[19];
	for( std::size_t i = 0; i < 19; ++i )
		errors[i] = std::vector<double>();
	
	for( std::size_t i = 0; i < files.size(); ++i )
	{
		cDataFileReader reader(files[i]);
		for( std::size_t j = 0; j < 19; ++j )
			errors[j] = std::vector<double>();


		// --- Test Againts All Data Points --- //
		
		std::vector<biotac_tune_data> data = reader.GetData();
		for( std::size_t dataIndex = 10; dataIndex < reader.GetNumDataPoints(); ++dataIndex )
		{
			errors[0].push_back(compensators[0].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e1));
			errors[1].push_back(compensators[1].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e2));
			errors[2].push_back(compensators[2].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e3));
			errors[3].push_back(compensators[3].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e4));
			errors[4].push_back(compensators[4].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e5));
			errors[5].push_back(compensators[5].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e6));
			errors[6].push_back(compensators[6].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e7));
			errors[7].push_back(compensators[7].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e8));
			errors[8].push_back(compensators[8].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e9));
			errors[9].push_back(compensators[9].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e10));
			errors[10].push_back(compensators[10].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e11));
			errors[11].push_back(compensators[11].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e12));
			errors[12].push_back(compensators[12].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e13));
			errors[13].push_back(compensators[13].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e14));
			errors[14].push_back(compensators[14].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e15));
			errors[15].push_back(compensators[15].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e16));
			errors[16].push_back(compensators[16].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e17));
			errors[17].push_back(compensators[17].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e18));
			errors[18].push_back(compensators[18].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e19));
		}


		// --- Plot Errors --- //

		PlotTestingErrors(errors);


		std::cout << std::endl << std::endl;
	}
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

void cFunctionFitNLS::FitToElectrodeData( const std::vector< std::pair<double, double> >& data,
										  double& a, double& b, double& c, double& d )
{
	a = b = c = d = 0;
	ceres::Problem problem;
	int numDataPoints = data.size();
	for( int i = 10; i < numDataPoints; ++i )
	{
		problem.AddResidualBlock(
			new ceres::AutoDiffCostFunction<cExponentialResidual, 1, 1, 1, 1, 1>(
				new cExponentialResidual( data[i].first, data[i].second ) ),
			NULL,
			&a, &b, &c, &d );
	}

	ceres::Solver::Options options;
	options.max_num_iterations = 1000;
	options.linear_solver_type = ceres::DENSE_QR;
	options.minimizer_progress_to_stdout = true;

	std::cout << "Solving... " << std::endl;
	ceres::Solver::Summary summary;
	ceres::Solve(options, &problem, &summary);
	std::cout << "Final a: " << a << "  b: " << b << "  c: " << c << "  d: " << d << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::PlotTestingErrors(const std::vector<double>* errors )
{
#ifdef WIN32
	std::string plotDriver = "wingcc";
#else
	std::string plotDriver = "xcairo";
#endif

	// --- Initialize --- //
	
	plstream plottingStream(1, 1, 255, 255, 255, plotDriver.c_str());
	plottingStream.star(2, 2);

	PlotE1ThroughE5(errors, plottingStream);
	PlotE6ThroughE10(&(errors[5]), plottingStream);
	PlotE11ThroughE15(&(errors[10]), plottingStream);
	PlotE16ThroughE19(&(errors[15]), plottingStream);
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::PlotE1ThroughE5(const std::vector<double>* errors, plstream& plottingStream)
{
	PLFLT* pX = new PLFLT[errors[0].size()];
	PLFLT* pE1 = new PLFLT[errors[0].size()];
	PLFLT* pE2 = new PLFLT[errors[1].size()];
	PLFLT* pE3 = new PLFLT[errors[2].size()];
	PLFLT* pE4 = new PLFLT[errors[3].size()];
	PLFLT* pE5 = new PLFLT[errors[4].size()];

	PLFLT ymin = 1000000.0, ymax = -1000000.0;
	for( std::size_t i = 0; i < errors[0].size(); ++i )
	{
		pX[i] = i / 100.0;
		pE1[i] = errors[0][i];
		pE2[i] = errors[1][i];
		pE3[i] = errors[2][i];
		pE4[i] = errors[3][i];
		pE5[i] = errors[4][i];

		ymax = std::max(pE1[i], ymax);
		ymax = std::max(pE2[i], ymax);
		ymax = std::max(pE3[i], ymax);
		ymax = std::max(pE4[i], ymax);
		ymax = std::max(pE5[i], ymax);

		ymin = std::min(pE1[i], ymin);
		ymin = std::min(pE2[i], ymin);
		ymin = std::min(pE3[i], ymin);
		ymin = std::min(pE4[i], ymin);
		ymin = std::min(pE5[i], ymin);
	}


	// --- Generate Plot --- //

	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.env(0, errors[0].size() / 100.0, ymin*0.99, ymax*1.01, 0, 0);
	plottingStream.lab("Time (s)", "Electrode Measurement", "Electrodes 1-5");

	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.line(errors[0].size(), pX, pE1);

	plottingStream.col0(nUtils::enumPLplotColor_YELLOW);
	plottingStream.line(errors[0].size(), pX, pE2);

	plottingStream.col0(nUtils::enumPLplotColor_GREEN);
	plottingStream.line(errors[0].size(), pX, pE3);

	plottingStream.col0(nUtils::enumPLplotColor_AQUAMARINE);
	plottingStream.line(errors[0].size(), pX, pE4);

	plottingStream.col0(nUtils::enumPLplotColor_PINK);
	plottingStream.line(errors[0].size(), pX, pE5);


	// --- Create Legend --- //

	PLINT optArray[5];
	PLINT textColors[5];
	PLINT lineColors[5];
	PLINT lineStyles[5];
	PLFLT lineWidths[5];
	const char* text[5];

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

	PLFLT legendWidth, legendHeight;
	plottingStream.legend( &legendWidth, &legendHeight,
						   PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX,
						   PL_POSITION_RIGHT | PL_POSITION_TOP,
						   0.0, 0.0, 0.01,
						   15, 1, 1, 0, 0,
						   5, optArray,
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
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::PlotE6ThroughE10(const std::vector<double>* errors, plstream& plottingStream)
{
	PLFLT* pX = new PLFLT[errors[0].size()];
	PLFLT* pE6 = new PLFLT[errors[0].size()];
	PLFLT* pE7 = new PLFLT[errors[1].size()];
	PLFLT* pE8 = new PLFLT[errors[2].size()];
	PLFLT* pE9 = new PLFLT[errors[3].size()];
	PLFLT* pE10 = new PLFLT[errors[4].size()];

	PLFLT ymin = 1000000.0, ymax = -1000000.0;
	for( std::size_t i = 0; i < errors[0].size(); ++i )
	{
		pX[i] = i / 100.0;
		pE6[i] = errors[0][i];
		pE7[i] = errors[1][i];
		pE8[i] = errors[2][i];
		pE9[i] = errors[3][i];
		pE10[i] = errors[4][i];

		ymax = std::max(pE6[i], ymax);
		ymax = std::max(pE7[i], ymax);
		ymax = std::max(pE8[i], ymax);
		ymax = std::max(pE9[i], ymax);
		ymax = std::max(pE10[i], ymax);

		ymin = std::min(pE6[i], ymin);
		ymin = std::min(pE7[i], ymin);
		ymin = std::min(pE8[i], ymin);
		ymin = std::min(pE9[i], ymin);
		ymin = std::min(pE10[i], ymin);
	}


	// --- Generate Plot --- //

	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.env(0, errors[0].size() / 100.0, ymin*0.99, ymax*1.01, 0, 0);
	plottingStream.lab("Time (s)", "Electrode Measurement", "Electrodes 6-10");

	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.line(errors[0].size(), pX, pE6);

	plottingStream.col0(nUtils::enumPLplotColor_YELLOW);
	plottingStream.line(errors[0].size(), pX, pE7);

	plottingStream.col0(nUtils::enumPLplotColor_GREEN);
	plottingStream.line(errors[0].size(), pX, pE8);

	plottingStream.col0(nUtils::enumPLplotColor_AQUAMARINE);
	plottingStream.line(errors[0].size(), pX, pE9);

	plottingStream.col0(nUtils::enumPLplotColor_PINK);
	plottingStream.line(errors[0].size(), pX, pE10);


	// --- Create Legend --- //

	PLINT optArray[5];
	PLINT textColors[5];
	PLINT lineColors[5];
	PLINT lineStyles[5];
	PLFLT lineWidths[5];
	const char* text[5];

	optArray[0] = PL_LEGEND_LINE;
	textColors[0] = nUtils::enumPLplotColor_RED;
	lineColors[0] = nUtils::enumPLplotColor_RED;
	text[0] = "E6";
	lineStyles[0] = 1;
	lineWidths[0] = 1.0;

	optArray[1] = PL_LEGEND_LINE;
	textColors[1] = nUtils::enumPLplotColor_YELLOW;
	text[1] = "E7";
	lineColors[1] = nUtils::enumPLplotColor_YELLOW;
	lineStyles[1] = 1;
	lineWidths[1] = 1.0;

	optArray[2] = PL_LEGEND_LINE;
	textColors[2] = nUtils::enumPLplotColor_GREEN;
	lineColors[2] = nUtils::enumPLplotColor_GREEN;
	text[2] = "E8";
	lineStyles[2] = 1;
	lineWidths[2] = 1.0;

	optArray[3] = PL_LEGEND_LINE;
	textColors[3] = nUtils::enumPLplotColor_AQUAMARINE;
	text[3] = "E9";
	lineColors[3] = nUtils::enumPLplotColor_AQUAMARINE;
	lineStyles[3] = 1;
	lineWidths[3] = 1.0;

	optArray[4] = PL_LEGEND_LINE;
	textColors[4] = nUtils::enumPLplotColor_PINK;
	lineColors[4] = nUtils::enumPLplotColor_PINK;
	text[4] = "E10";
	lineStyles[4] = 1;
	lineWidths[4] = 1.0;

	PLFLT legendWidth, legendHeight;
	plottingStream.legend( &legendWidth, &legendHeight,
						   PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX,
						   PL_POSITION_RIGHT | PL_POSITION_TOP,
						   0.0, 0.0, 0.01,
						   15, 1, 1, 0, 0,
						   5, optArray,
						   0.5, 0.5, 1.0, 0.0,
						   textColors, (const char* const *) text,
						   NULL, NULL, NULL, NULL,
						   lineColors, lineStyles, lineWidths,
						   NULL, NULL, NULL, NULL );
	
	delete[] pX;
	delete[] pE6;
	delete[] pE7;
	delete[] pE8;
	delete[] pE9;
	delete[] pE10;
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::PlotE11ThroughE15(const std::vector<double>* errors, plstream& plottingStream)
{
	PLFLT* pX = new PLFLT[errors[0].size()];
	PLFLT* pE11 = new PLFLT[errors[0].size()];
	PLFLT* pE12 = new PLFLT[errors[1].size()];
	PLFLT* pE13 = new PLFLT[errors[2].size()];
	PLFLT* pE14 = new PLFLT[errors[3].size()];
	PLFLT* pE15 = new PLFLT[errors[4].size()];

	PLFLT ymin = 1000000.0, ymax = -1000000.0;
	for( std::size_t i = 0; i < errors[0].size(); ++i )
	{
		pX[i] = i / 100.0;
		pE11[i] = errors[0][i];
		pE12[i] = errors[1][i];
		pE13[i] = errors[2][i];
		pE14[i] = errors[3][i];
		pE15[i] = errors[4][i];

		ymax = std::max(pE11[i], ymax);
		ymax = std::max(pE12[i], ymax);
		ymax = std::max(pE13[i], ymax);
		ymax = std::max(pE14[i], ymax);
		ymax = std::max(pE15[i], ymax);

		ymin = std::min(pE11[i], ymin);
		ymin = std::min(pE12[i], ymin);
		ymin = std::min(pE13[i], ymin);
		ymin = std::min(pE14[i], ymin);
		ymin = std::min(pE15[i], ymin);
	}


	// --- Generate Plot --- //

	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.env(0, errors[0].size() / 100.0, ymin*0.99, ymax*1.01, 0, 0);
	plottingStream.lab("Time (s)", "Electrode Measurement", "Electrodes 11-15");

	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.line(errors[0].size(), pX, pE11);

	plottingStream.col0(nUtils::enumPLplotColor_YELLOW);
	plottingStream.line(errors[0].size(), pX, pE12);

	plottingStream.col0(nUtils::enumPLplotColor_GREEN);
	plottingStream.line(errors[0].size(), pX, pE13);

	plottingStream.col0(nUtils::enumPLplotColor_AQUAMARINE);
	plottingStream.line(errors[0].size(), pX, pE14);

	plottingStream.col0(nUtils::enumPLplotColor_PINK);
	plottingStream.line(errors[0].size(), pX, pE15);


	// --- Create Legend --- //

	PLINT optArray[5];
	PLINT textColors[5];
	PLINT lineColors[5];
	PLINT lineStyles[5];
	PLFLT lineWidths[5];
	const char* text[5];

	optArray[0] = PL_LEGEND_LINE;
	textColors[0] = nUtils::enumPLplotColor_RED;
	lineColors[0] = nUtils::enumPLplotColor_RED;
	text[0] = "E11";
	lineStyles[0] = 1;
	lineWidths[0] = 1.0;

	optArray[1] = PL_LEGEND_LINE;
	textColors[1] = nUtils::enumPLplotColor_YELLOW;
	text[1] = "E12";
	lineColors[1] = nUtils::enumPLplotColor_YELLOW;
	lineStyles[1] = 1;
	lineWidths[1] = 1.0;

	optArray[2] = PL_LEGEND_LINE;
	textColors[2] = nUtils::enumPLplotColor_GREEN;
	lineColors[2] = nUtils::enumPLplotColor_GREEN;
	text[2] = "E13";
	lineStyles[2] = 1;
	lineWidths[2] = 1.0;

	optArray[3] = PL_LEGEND_LINE;
	textColors[3] = nUtils::enumPLplotColor_AQUAMARINE;
	text[3] = "E14";
	lineColors[3] = nUtils::enumPLplotColor_AQUAMARINE;
	lineStyles[3] = 1;
	lineWidths[3] = 1.0;

	optArray[4] = PL_LEGEND_LINE;
	textColors[4] = nUtils::enumPLplotColor_PINK;
	lineColors[4] = nUtils::enumPLplotColor_PINK;
	text[4] = "E115";
	lineStyles[4] = 1;
	lineWidths[4] = 1.0;

	PLFLT legendWidth, legendHeight;
	plottingStream.legend( &legendWidth, &legendHeight,
						   PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX,
						   PL_POSITION_RIGHT | PL_POSITION_TOP,
						   0.0, 0.0, 0.01,
						   15, 1, 1, 0, 0,
						   5, optArray,
						   0.5, 0.5, 1.0, 0.0,
						   textColors, (const char* const *) text,
						   NULL, NULL, NULL, NULL,
						   lineColors, lineStyles, lineWidths,
						   NULL, NULL, NULL, NULL );
	
	delete[] pX;
	delete[] pE11;
	delete[] pE12;
	delete[] pE13;
	delete[] pE14;
	delete[] pE15;
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::PlotE16ThroughE19(const std::vector<double>* errors, plstream& plottingStream)
{
	PLFLT* pX = new PLFLT[errors[0].size()];
	PLFLT* pE16 = new PLFLT[errors[0].size()];
	PLFLT* pE17 = new PLFLT[errors[1].size()];
	PLFLT* pE18 = new PLFLT[errors[2].size()];
	PLFLT* pE19 = new PLFLT[errors[3].size()];

	PLFLT ymin = 1000000.0, ymax = -1000000.0;
	for( std::size_t i = 0; i < errors[0].size(); ++i )
	{
		pX[i] = i / 100.0;
		pE16[i] = errors[0][i];
		pE17[i] = errors[1][i];
		pE18[i] = errors[2][i];
		pE19[i] = errors[3][i];

		ymax = std::max(pE16[i], ymax);
		ymax = std::max(pE17[i], ymax);
		ymax = std::max(pE18[i], ymax);
		ymax = std::max(pE19[i], ymax);

		ymin = std::min(pE16[i], ymin);
		ymin = std::min(pE17[i], ymin);
		ymin = std::min(pE18[i], ymin);
		ymin = std::min(pE19[i], ymin);
	}


	// --- Generate Plot --- //

	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.env(0, errors[0].size() / 100.0, ymin*0.99, ymax*1.01, 0, 0);
	plottingStream.lab("Time (s)", "Electrode Measurement", "Electrodes 16-19");

	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.line(errors[0].size(), pX, pE16);

	plottingStream.col0(nUtils::enumPLplotColor_YELLOW);
	plottingStream.line(errors[0].size(), pX, pE17);

	plottingStream.col0(nUtils::enumPLplotColor_GREEN);
	plottingStream.line(errors[0].size(), pX, pE18);

	plottingStream.col0(nUtils::enumPLplotColor_AQUAMARINE);
	plottingStream.line(errors[0].size(), pX, pE19);


	// --- Create Legend --- //

	PLINT optArray[4];
	PLINT textColors[4];
	PLINT lineColors[4];
	PLINT lineStyles[4];
	PLFLT lineWidths[4];
	const char* text[4];

	optArray[0] = PL_LEGEND_LINE;
	textColors[0] = nUtils::enumPLplotColor_RED;
	lineColors[0] = nUtils::enumPLplotColor_RED;
	text[0] = "E16";
	lineStyles[0] = 1;
	lineWidths[0] = 1.0;

	optArray[1] = PL_LEGEND_LINE;
	textColors[1] = nUtils::enumPLplotColor_YELLOW;
	text[1] = "E17";
	lineColors[1] = nUtils::enumPLplotColor_YELLOW;
	lineStyles[1] = 1;
	lineWidths[1] = 1.0;

	optArray[2] = PL_LEGEND_LINE;
	textColors[2] = nUtils::enumPLplotColor_GREEN;
	lineColors[2] = nUtils::enumPLplotColor_GREEN;
	text[2] = "E18";
	lineStyles[2] = 1;
	lineWidths[2] = 1.0;

	optArray[3] = PL_LEGEND_LINE;
	textColors[3] = nUtils::enumPLplotColor_AQUAMARINE;
	text[3] = "E19";
	lineColors[3] = nUtils::enumPLplotColor_AQUAMARINE;
	lineStyles[3] = 1;
	lineWidths[3] = 1.0;

	PLFLT legendWidth, legendHeight;
	plottingStream.legend( &legendWidth, &legendHeight,
						   PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX,
						   PL_POSITION_RIGHT | PL_POSITION_TOP,
						   0.0, 0.0, 0.01,
						   15, 1, 1, 0, 0,
						   4, optArray,
						   0.5, 0.5, 1.0, 0.0,
						   textColors, (const char* const *) text,
						   NULL, NULL, NULL, NULL,
						   lineColors, lineStyles, lineWidths,
						   NULL, NULL, NULL, NULL );
	
	delete[] pX;
	delete[] pE16;
	delete[] pE17;
	delete[] pE18;
	delete[] pE19;
}

