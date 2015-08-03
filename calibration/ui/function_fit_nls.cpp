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
	std::cout << std::endl << std::endl;

	std::cout << "E2:" << std::endl;
	FitToElectrodeData(data.mE2, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl << std::endl;

	std::cout << "E3:" << std::endl;
	FitToElectrodeData(data.mE3, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl << std::endl;

	std::cout << "E4:" << std::endl;
	FitToElectrodeData(data.mE4, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl << std::endl;

	std::cout << "E5:" << std::endl;
	FitToElectrodeData(data.mE5, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl << std::endl;

	std::cout << "E6:" << std::endl;
	FitToElectrodeData(data.mE6, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl << std::endl;

	std::cout << "E7:" << std::endl;
	FitToElectrodeData(data.mE7, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl << std::endl;

	std::cout << "E8:" << std::endl;
	FitToElectrodeData(data.mE8, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl << std::endl;

	std::cout << "E9:" << std::endl;
	FitToElectrodeData(data.mE9, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl << std::endl;

	std::cout << "E10:" << std::endl;
	FitToElectrodeData(data.mE10, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl << std::endl;

	std::cout << "E11:" << std::endl;
	FitToElectrodeData(data.mE11, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl << std::endl;

	std::cout << "E12:" << std::endl;
	FitToElectrodeData(data.mE12, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl << std::endl;

	std::cout << "E13:" << std::endl;
	FitToElectrodeData(data.mE13, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl << std::endl;

	std::cout << "E14:" << std::endl;
	FitToElectrodeData(data.mE14, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl << std::endl;

	std::cout << "E15:" << std::endl;
	FitToElectrodeData(data.mE15, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl << std::endl;

	std::cout << "E16:" << std::endl;
	FitToElectrodeData(data.mE16, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl << std::endl;

	std::cout << "E17:" << std::endl;
	FitToElectrodeData(data.mE17, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl << std::endl;

	std::cout << "E18:" << std::endl;
	FitToElectrodeData(data.mE18, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl << std::endl;

	std::cout << "E19:" << std::endl;
	FitToElectrodeData(data.mE19, a, b, c, d );
	retVal.push_back(cElectrodeTdcCompensator(a, b, c, d));
	std::cout << std::endl << std::endl;

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::TestAgainstDataFiles(const std::vector<std::string>& files,
										   const std::vector<cElectrodeTdcCompensator>& compensators)
{
	std::vector<double> errors(19);
	
	for( std::size_t i = 0; i < files.size(); ++i )
	{
		cDataFileReader reader(files[i]);
		for( std::size_t errorIndex = 0; errorIndex < errors.size(); ++errorIndex )
			errors[errorIndex] = 0;
		
		std::vector<biotac_tune_data> data = reader.GetData();
		for( std::size_t dataIndex = 0; dataIndex < reader.GetNumDataPoints(); ++dataIndex )
		{
			errors[0] += compensators[0].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e1);
			errors[1] += compensators[1].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e2);
			errors[2] += compensators[2].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e3);
			errors[3] += compensators[3].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e4);
			errors[4] += compensators[4].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e5);
			errors[5] += compensators[5].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e6);
			errors[6] += compensators[6].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e7);
			errors[7] += compensators[7].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e8);
			errors[8] += compensators[8].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e9);
			errors[9] += compensators[9].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e10);
			errors[10] += compensators[10].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e11);
			errors[11] += compensators[11].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e12);
			errors[12] += compensators[12].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e13);
			errors[13] += compensators[13].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e14);
			errors[14] += compensators[14].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e15);
			errors[15] += compensators[15].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e16);
			errors[16] += compensators[16].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e17);
			errors[17] += compensators[17].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e18);
			errors[18] += compensators[18].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.e19);
		}

		std::cout << "Average errors for " << files[i] << std::endl;
		for( std::size_t errorIndex = 0; errorIndex < errors.size(); ++errorIndex )
		{
			errors[errorIndex] /= static_cast<double>(reader.GetNumDataPoints());
			std::cout << "E" << errorIndex << ": " << errors[errorIndex] << ", ";
		}

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
	for( int i = 0; i < numDataPoints; ++i )
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
	options.minimizer_progress_to_stdout = false;

	std::cout << "Solving... " << std::endl;
	ceres::Solver::Summary summary;
	ceres::Solve(options, &problem, &summary);
	std::cout << "Final a: " << a << "  b: " << b << "  c: " << c << "  d: " << d << std::endl;
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


