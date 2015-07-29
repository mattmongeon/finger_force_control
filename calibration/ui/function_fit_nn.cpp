#include "function_fit_nn.h"
#include "data_file_reader.h"
#include "file_utils.h"
#include "utils.h"
#include "../common/biotac_comm.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <plplot/plplot.h>
#include <plplot/plstream.h>


#define MAX_BIOTAC_LOAD_G 5098.581064889641  // 50 N


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cFunctionFitNN::cFunctionFitNN()
	: mNumLayers(4),
	  mNumInputs(20),
	  mNumHiddenNeurons(40),
	  mNumOutputs(1)
{
	mpANN = fann_create_standard(mNumLayers, mNumInputs, mNumHiddenNeurons, mNumHiddenNeurons, mNumOutputs);

	fann_set_activation_function_hidden(mpANN, FANN_SIGMOID);
	fann_set_activation_function_output(mpANN, FANN_SIGMOID);
}

////////////////////////////////////////////////////////////////////////////////

cFunctionFitNN::~cFunctionFitNN()
{
	fann_destroy(mpANN);
	mpANN = 0;
}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNN::LoadNeuralNetwork(const std::string& fileName)
{
	fann_destroy(mpANN);
	std::string path("./nn/");
	path += fileName;
	mpANN = fann_create_from_file(path.c_str());
	
	if( !mpANN )
	{
		std::string msg = "Error loading neural network from \'";
		msg += path;
		msg += "\'!";
		
		throw std::invalid_argument(msg);
	}
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNN::TrainAgainstDataFiles(const std::vector<std::string>& files)
{
	nUtils::ClearConsole();
	std::cout << "Training on " << files.size() << " total file(s)." << std::endl;

	float desiredError = 0.000001;
	unsigned int maxEpochs = 10000;
	unsigned int epochsBetweenReports = 100;
	float bitFailLimit = 0.004f;

	std::ostringstream summary;
	summary << "19 Electrodes + TDC"
			<< ", layers: "<< mNumLayers 
			<< ", limit: " << bitFailLimit
			<< ", hidden: " << mNumHiddenNeurons;
	std::cout << summary.str() << std::endl;
	
	fann_train_data* pTrainData = fann_read_train_from_file( ConvertDataFileToNNFile(files).c_str() );
	// fann_set_training_algorithm(mpANN, FANN_TRAIN_INCREMENTAL);
	// fann_set_activation_steepness_hidden(mpANN, 1);
	// fann_set_activation_steepness_hidden(mpANN, 1);

	fann_set_train_stop_function(mpANN, FANN_STOPFUNC_MSE);
	fann_set_bit_fail_limit(mpANN, bitFailLimit);
		
	fann_set_training_algorithm(mpANN, FANN_TRAIN_RPROP);

	fann_init_weights(mpANN, pTrainData);

	fann_train_on_data(mpANN, pTrainData, maxEpochs, epochsBetweenReports, desiredError);

	std::vector<float> errors;
	for(unsigned int j = 0; j < fann_length_train_data(pTrainData); j++)
	{
		fann_type* calc_out = fann_run(mpANN, pTrainData->input[j]);
		// std::cout << "Point " << j << " -> "
		// 		  << calc_out[0] * MAX_BIOTAC_LOAD_G << ", should be " << pTrainData->output[j][0] * MAX_BIOTAC_LOAD_G
		// 		  << ", difference=" << fann_abs(calc_out[0] - pTrainData->output[j][0]) * MAX_BIOTAC_LOAD_G << "\r\n";

		errors.push_back(fann_abs(calc_out[0] - pTrainData->output[j][0]));
	}

	std::sort(errors.begin(), errors.end());
	float sum = std::accumulate(errors.begin(), errors.end(), 0.0f);
	std::cout << "Total training points: " << fann_length_train_data(pTrainData) << "\r\n"
			  << "Total error: " << sum * MAX_BIOTAC_LOAD_G << "\r\n"
			  << "Average error: " << (sum / fann_length_train_data(pTrainData)) * MAX_BIOTAC_LOAD_G << "\r\n"
			  << "Max error: " << errors[errors.size() - 1] * MAX_BIOTAC_LOAD_G << "\r\n"
			  << "Min error: " << errors[0] * MAX_BIOTAC_LOAD_G << "\r\n"
			  << "Mode: " << errors[errors.size() / 2] * MAX_BIOTAC_LOAD_G << "\r\n";
		
	std::cout << std::flush;

	fann_destroy_train(pTrainData);

	std::cout << std::endl;
	std::cout << summary.str() << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNN::TestAgainstDataFiles(const std::vector<std::string>& files)
{
	nUtils::ClearConsole();
	std::cout << "Testing against " << files.size() << " total file(s)." << std::endl;

	fann_train_data* pTestData = fann_read_train_from_file( ConvertDataFileToNNFile(files).c_str() );

	unsigned int dataLength = fann_length_train_data(pTestData);
	std::vector<float> errors;
	std::vector<float> calculated;
	std::vector<float> actualData;
	for( unsigned int j = 0; j < dataLength; ++j )
	{
		fann_reset_MSE(mpANN);

		fann_type* calc_out = fann_test(mpANN, pTestData->input[j], pTestData->output[j]);

		std::cout << "Point " << j << " -> "
				  << calc_out[0] * MAX_BIOTAC_LOAD_G << ", should be " << pTestData->output[j][0] * MAX_BIOTAC_LOAD_G
				  << ", difference=" << fann_abs(calc_out[0] - pTestData->output[j][0]) * MAX_BIOTAC_LOAD_G << "\r\n";

		errors.push_back(fann_abs(calc_out[0] - pTestData->output[j][0]));
		calculated.push_back(calc_out[0] * MAX_BIOTAC_LOAD_G);
		actualData.push_back(pTestData->output[j][0] * MAX_BIOTAC_LOAD_G);
	}

	
	// --- Print Out Statistics --- //

	std::sort(errors.begin(), errors.end());
	float sum = std::accumulate(errors.begin(), errors.end(), 0.0f);
	std::cout << "Total training points: " << fann_length_train_data(pTestData) << "\r\n"
			  << "Total error: " << sum * MAX_BIOTAC_LOAD_G << "\r\n"
			  << "Average error: " << (sum / fann_length_train_data(pTestData)) * MAX_BIOTAC_LOAD_G << "\r\n"
			  << "Max error: " << errors[errors.size() - 1] * MAX_BIOTAC_LOAD_G << "\r\n"
			  << "Min error: " << errors[0] * MAX_BIOTAC_LOAD_G << "\r\n"
			  << "Mode: " << errors[errors.size() / 2] * MAX_BIOTAC_LOAD_G << "\r\n";
		
	std::cout << std::flush;


	// --- Plot Results --- //

	PlotResults(calculated, actualData);


	// --- Clean Up --- //
	
	fann_destroy_train(pTestData);
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNN::SaveNeuralNetwork(const std::string& fileName)
{
	nFileUtils::CreateDirectory("./nn");

	std::string path = "./nn/";
	path += fileName;

	fann_save(mpANN, path.c_str());
	std::cout << "Saved neural network topology to \'" << path << "\'." << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
//  Helper Functions
////////////////////////////////////////////////////////////////////////////////

std::string cFunctionFitNN::ConvertDataFileToNNFile(const std::vector<std::string>& files)
{
	std::string fileName = "./data/training.nndat";
	if( files.size() == 1 )
	{
		fileName = files[0] + ".nndat";
	}
	
	std::ofstream outFile(fileName.c_str());

	// This just reserves space at the beginning of the file to write header information.
	outFile << "             " << std::endl;

	int numDataPoints = 0;
	for( std::size_t i = 0; i < files.size(); ++i )
	{
		cDataFileReader df(files[i]);

		std::vector<biotac_tune_data> data = df.GetData();

		outFile.precision(20);
		for( std::size_t j = 0; j < data.size(); ++j )
		{
			if( j < 100 )
				continue;
			
			outFile // << static_cast<float>(data[j].mData.pac / 4096.0) << " "
					// << static_cast<float>(data[j].mData.pdc / 4096.0) << " "
				    // << static_cast<float>(data[j].mData.tac / 4096.0) << " "
					<< static_cast<float>(data[j].mData.tdc / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e1 / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e2 / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e3 / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e4 / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e5 / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e6 / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e7 / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e8 / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e9 / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e10 / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e11 / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e12 / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e13 / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e14 / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e15 / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e16 / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e17 / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e18 / 4096.0) << " "
					<< static_cast<float>(data[j].mData.e19 / 4096.0) << " "
					<< std::endl;

			outFile << static_cast<float>(data[j].mLoadCell_g / MAX_BIOTAC_LOAD_G) << std::endl;

			++numDataPoints;
		}
	}

	// Before we exit, we need to write out the number of data points as the first entry of the file.
	outFile.seekp(0);
	outFile << numDataPoints << " " << mNumInputs << " " << mNumOutputs << std::flush;

	outFile.close();

	return fileName;
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNN::PlotResults(const std::vector<float>& calculated, const std::vector<float>& actual )
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
	PLFLT* pError = new PLFLT[calculated.size()];
	for( std::size_t i = 0; i < calculated.size(); ++i )
	{
		pX[i] = i;
		pCalculated[i] = calculated[i];
		pActual[i] = actual[i];
		pError[i] = fabs(calculated[i] - actual[i]);
		
		ymax = std::max(calculated[i], actual[i]);
		ymax = std::max<PLFLT>(ymax, calculated[i] - actual[i]);

		ymin = std::min(calculated[i], actual[i]);
		ymin = std::min<PLFLT>(ymin, calculated[i] - actual[i]);
	}


	// --- Generate Plot --- //

	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.env(0, calculated.size(), ymin, ymax*1.001, 0, 0);
	plottingStream.lab("Samples", "Magnitude", "Neural Network Analysis");

	plottingStream.col0(nUtils::enumPLplotColor_BLUE);
	plottingStream.line(calculated.size(), pX, pCalculated);

	plottingStream.col0(nUtils::enumPLplotColor_GREEN);
	plottingStream.line(calculated.size(), pX, pActual);

	plottingStream.col0(nUtils::enumPLplotColor_MAGENTA);
	plottingStream.line(calculated.size(), pX, pError);


	// --- Create Legend --- //
	
	PLINT optArray[3];
	PLINT textColors[3];
	PLINT lineColors[3];
	PLINT lineStyles[3];
	PLFLT lineWidths[3];
	const char* text[3];

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

	optArray[2] = PL_LEGEND_LINE;
	textColors[2] = nUtils::enumPLplotColor_MAGENTA;
	lineColors[2] = nUtils::enumPLplotColor_MAGENTA;
	text[2] = "Error";
	lineStyles[2] = 1;
	lineWidths[2] = 1.0;
	
	PLFLT legendWidth, legendHeight;
	plottingStream.legend( &legendWidth, &legendHeight,
							PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX,
							PL_POSITION_RIGHT | PL_POSITION_BOTTOM,
							0.0, 0.0, 0.01,
							15, 1, 1, 0, 0,
							3, optArray,
							0.5, 0.5, 1.0, 0.0,
							textColors, (const char* const *) text,
							NULL, NULL, NULL, NULL,
							lineColors, lineStyles, lineWidths,
							NULL, NULL, NULL, NULL );
}


