#include "function_fit_tare.h"
#include "data_file_reader.h"
#include "utils.h"
#include <ceres/ceres.h>
#include <plplot/plplot.h>
#include <plplot/plstream.h>
#include <fstream>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cFunctionFitTare::cFunctionFitTare()
{
	mElectrodeNormalVectors[0][0] = 0.196;
	mElectrodeNormalVectors[0][1] = -0.956;
	mElectrodeNormalVectors[0][2] = -0.22;

	mElectrodeNormalVectors[1][0] = 0.0;
	mElectrodeNormalVectors[1][1] = -0.692;
	mElectrodeNormalVectors[1][2] = -0.722;

	mElectrodeNormalVectors[2][0] = 0.0;
	mElectrodeNormalVectors[2][1] = -0.692;
	mElectrodeNormalVectors[2][2] = -0.722;
	
	mElectrodeNormalVectors[3][0] = 0.0;
	mElectrodeNormalVectors[3][1] = -0.976;
	mElectrodeNormalVectors[3][2] = -0.22;

	mElectrodeNormalVectors[4][0] = 0.0;
	mElectrodeNormalVectors[4][1] = -0.692;
	mElectrodeNormalVectors[4][2] = -0.722;

	mElectrodeNormalVectors[5][0] = 0.0;
	mElectrodeNormalVectors[5][1] = -0.976;
	mElectrodeNormalVectors[5][2] = -0.22;

	mElectrodeNormalVectors[6][0] = 0.5;
	mElectrodeNormalVectors[6][1] = 0.0;
	mElectrodeNormalVectors[6][2] = -0.866;

	mElectrodeNormalVectors[7][0] = 0.5;
	mElectrodeNormalVectors[7][1] = 0.0;
	mElectrodeNormalVectors[7][2] = -0.866;

	mElectrodeNormalVectors[8][0] = 0.5;
	mElectrodeNormalVectors[8][1] = 0.0;
	mElectrodeNormalVectors[8][2] = -0.866;

	mElectrodeNormalVectors[9][0] = 0.5;
	mElectrodeNormalVectors[9][1] = 0.0;
	mElectrodeNormalVectors[9][2] = -0.866;

	mElectrodeNormalVectors[10][0] = 0.196;
	mElectrodeNormalVectors[10][1] = 0.956;
	mElectrodeNormalVectors[10][2] = -0.22;

	mElectrodeNormalVectors[11][0] = 0.0;
	mElectrodeNormalVectors[11][1] = 0.692;
	mElectrodeNormalVectors[11][2] = -0.722;

	mElectrodeNormalVectors[12][0] = 0.0;
	mElectrodeNormalVectors[12][1] = 0.692;
	mElectrodeNormalVectors[12][2] = -0.722;

	mElectrodeNormalVectors[13][0] = 0.0;
	mElectrodeNormalVectors[13][1] = 0.976;
	mElectrodeNormalVectors[13][2] = -0.22;

	mElectrodeNormalVectors[14][0] = 0.0;
	mElectrodeNormalVectors[14][1] = 0.692;
	mElectrodeNormalVectors[14][2] = -0.722;

	mElectrodeNormalVectors[15][0] = 0.0;
	mElectrodeNormalVectors[15][1] = 0.976;
	mElectrodeNormalVectors[15][2] = -0.22;

	mElectrodeNormalVectors[16][0] = 0.0;
	mElectrodeNormalVectors[16][1] = 0.0;
	mElectrodeNormalVectors[16][2] = -1.0;

	mElectrodeNormalVectors[17][0] = 0.0;
	mElectrodeNormalVectors[17][1] = 0.0;
	mElectrodeNormalVectors[17][2] = -1.0;

	mElectrodeNormalVectors[18][0] = 0.0;
	mElectrodeNormalVectors[18][1] = 0.0;
	mElectrodeNormalVectors[18][2] = -1.0;
}

////////////////////////////////////////////////////////////////////////////////

cFunctionFitTare::~cFunctionFitTare()
{

}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

cBioTacForceCurveTare cFunctionFitTare::TrainAgainstDataFiles(const std::vector<std::string>& files)
{
	// --- Parse Files --- //

	double s1 = 1.01237, s2 = 0.407605, s3 = 0.00012177;
	ceres::Problem problem;
	
	// We will parse the files to set up all of our training data, and then we will start
	// the training session.
	for( std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it )
	{
		cDataFileReader reader(*it);
		std::vector<biotac_tune_data> data = reader.GetData();

		// First we will use the very beginning of the file to tare the compensators.
		std::vector<uint16_t> e1, e2, e3, e4, e5, e6, e7, e8, e9, e10,
			e11, e12, e13, e14, e15, e16, e17, e18, e19;
		for( std::size_t i = 0; (i < 200) && (data[i].mReference_g < 30.0); ++i )
		{
			e1.push_back(data[i].mData.e1);
			e2.push_back(data[i].mData.e2);
			e3.push_back(data[i].mData.e3);
			e4.push_back(data[i].mData.e4);
			e5.push_back(data[i].mData.e5);
			e6.push_back(data[i].mData.e6);
			e7.push_back(data[i].mData.e7);
			e8.push_back(data[i].mData.e8);
			e9.push_back(data[i].mData.e9);
			e10.push_back(data[i].mData.e10);
			e11.push_back(data[i].mData.e11);
			e12.push_back(data[i].mData.e12);
			e13.push_back(data[i].mData.e13);
			e14.push_back(data[i].mData.e14);
			e15.push_back(data[i].mData.e15);
			e16.push_back(data[i].mData.e16);
			e17.push_back(data[i].mData.e17);
			e18.push_back(data[i].mData.e18);
			e19.push_back(data[i].mData.e19);
		}

		std::vector<cElectrodeCompensatorTare> compensators;
		for( std::size_t i = 0; i < 19; ++i )
		{
			compensators.push_back(cElectrodeCompensatorTare());
		}
		
		compensators[0].SetBaseline(e1);
		compensators[1].SetBaseline(e2);
		compensators[2].SetBaseline(e3);
		compensators[3].SetBaseline(e4);
		compensators[4].SetBaseline(e5);
		compensators[5].SetBaseline(e6);
		compensators[6].SetBaseline(e7);
		compensators[7].SetBaseline(e8);
		compensators[8].SetBaseline(e9);
		compensators[9].SetBaseline(e10);
		compensators[10].SetBaseline(e11);
		compensators[11].SetBaseline(e12);
		compensators[12].SetBaseline(e13);
		compensators[13].SetBaseline(e14);
		compensators[14].SetBaseline(e15);
		compensators[15].SetBaseline(e16);
		compensators[16].SetBaseline(e17);
		compensators[17].SetBaseline(e18);
		compensators[18].SetBaseline(e19);
		

		// Now we can start training the force vector.
		
		for( std::size_t i = 300; i < data.size(); ++i )
		{
			std::vector<uint16_t> electrodes;
			electrodes.push_back(data[i].mData.e1);
			electrodes.push_back(data[i].mData.e2);
			electrodes.push_back(data[i].mData.e3);
			electrodes.push_back(data[i].mData.e4);
			electrodes.push_back(data[i].mData.e5);
			electrodes.push_back(data[i].mData.e6);
			electrodes.push_back(data[i].mData.e7);
			electrodes.push_back(data[i].mData.e8);
			electrodes.push_back(data[i].mData.e9);
			electrodes.push_back(data[i].mData.e10);
			electrodes.push_back(data[i].mData.e11);
			electrodes.push_back(data[i].mData.e12);
			electrodes.push_back(data[i].mData.e13);
			electrodes.push_back(data[i].mData.e14);
			electrodes.push_back(data[i].mData.e15);
			electrodes.push_back(data[i].mData.e16);
			electrodes.push_back(data[i].mData.e17);
			electrodes.push_back(data[i].mData.e18);
			electrodes.push_back(data[i].mData.e19);
			
			problem.AddResidualBlock(
				new ceres::AutoDiffCostFunction<cTrainingResidual, 1, 1, 1, 1>(
					new cTrainingResidual(electrodes,
										  data[i].mData.tdc,
										  data[i].mData.pdc,
										  data[i].mLoadCell_g,
										  compensators)),
				NULL,
				&s1, &s2, &s3 );
		}
	}
	

	// --- Start Training --- //

 	ceres::Solver::Options options;
	options.max_num_iterations = 1000;
 	options.linear_solver_type = ceres::DENSE_QR;
 	options.minimizer_progress_to_stdout = true;
	options.num_threads = 8;
 	options.function_tolerance = 1.0e-4;
	options.initial_trust_region_radius = 1e10;

	std::cout << "Solving... " << std::endl;
	ceres::Solver::Summary summary;
	ceres::Solve(options, &problem, &summary);
	std::cout << summary.FullReport() << std::endl;
	std::cout << "Final s1: " << s1 << "  s2: " << s2 << "  s3: " << s3 << std::endl;

	return cBioTacForceCurveTare(s1, s2, s3);
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitTare::TestAgainstDataFiles(const std::vector<std::string>& files, cBioTacForceCurveTare& curve)
{
	std::vector<float> calculated, actual;

	int fileNum = 1;
	for( std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it )
	{
		// --- Prepare The File --- //
		
		calculated.clear();
		actual.clear();

		cDataFileReader reader(*it);
		std::vector<biotac_tune_data> data = reader.GetData();


		// --- Set Electrode Baselines --- //

		std::vector<biotac_tune_data> baselineData;
		for( std::size_t i = 0; i < 200; ++i )
			baselineData.push_back(data[i]);

		curve.SetBaseline(baselineData);


		// --- Execute Force Curve --- //

		std::ofstream outFile("./sine.csv");
		for( std::size_t dataIndex = 0; dataIndex < reader.GetNumDataPoints(); ++dataIndex )
		{
			std::vector<uint16_t> electrodes;
			electrodes.push_back(data[dataIndex].mData.e1);
			electrodes.push_back(data[dataIndex].mData.e2);
			electrodes.push_back(data[dataIndex].mData.e3);
			electrodes.push_back(data[dataIndex].mData.e4);
			electrodes.push_back(data[dataIndex].mData.e5);
			electrodes.push_back(data[dataIndex].mData.e6);
			electrodes.push_back(data[dataIndex].mData.e7);
			electrodes.push_back(data[dataIndex].mData.e8);
			electrodes.push_back(data[dataIndex].mData.e9);
			electrodes.push_back(data[dataIndex].mData.e10);
			electrodes.push_back(data[dataIndex].mData.e11);
			electrodes.push_back(data[dataIndex].mData.e12);
			electrodes.push_back(data[dataIndex].mData.e13);
			electrodes.push_back(data[dataIndex].mData.e14);
			electrodes.push_back(data[dataIndex].mData.e15);
			electrodes.push_back(data[dataIndex].mData.e16);
			electrodes.push_back(data[dataIndex].mData.e17);
			electrodes.push_back(data[dataIndex].mData.e18);
			electrodes.push_back(data[dataIndex].mData.e19);

			// Use all of the calibration data and terms to generate a force value from the
			// BioTac data and save it for plotting.
			calculated.push_back( curve.GetForce_g(electrodes, data[dataIndex].mData.tdc, data[dataIndex].mData.pdc) );
			actual.push_back( data[dataIndex].mLoadCell_g );

			outFile << calculated[calculated.size()-1] << ", "
					<< actual[actual.size()-1] << ", "
					<< data[dataIndex].mReference_g
					<< std::endl;
		}
		outFile.close();

		std::cout << "Plotting file \'" << *it << "\' - " << fileNum << std::endl;
		++fileNum;
		PlotResults(calculated, actual);
	}
}

////////////////////////////////////////////////////////////////////////////////
//  Helper Functions
////////////////////////////////////////////////////////////////////////////////

void cFunctionFitTare::PlotResults(const std::vector<float>& calculated, const std::vector<float>& actual )
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
	plottingStream.lab("Samples", "Magnitude", "Function Fit Analysis");

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
	text[0] = "Curve Output";
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
