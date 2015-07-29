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
	  mNumHiddenNeurons(65),
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

	float desiredError = 0.0000015;
	unsigned int maxEpochs = 10000;
	unsigned int epochsBetweenReports = 100;
	float bitFailLimit = 0.004f;

	std::ostringstream summary;
	summary << "19 Electrodes + TDC"
			<< ", layers: "<< mNumLayers 
			<< ", limit: " << bitFailLimit
			<< ", hidden: " << mNumHiddenNeurons;
	std::cout << summary.str() << std::endl;
	
	fann_train_data* pTrainData = fann_read_train_from_file( ConvertDataFilesToNNFile(files).c_str() );
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

	fann_train_data* pTestData = fann_read_train_from_file( ConvertDataFilesToTestFile(files).c_str() );

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

std::string cFunctionFitNN::ConvertDataFilesToNNFile(const std::vector<std::string>& files)
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
		unsigned long long tdc = 0;
		unsigned long long e1 = 0;
		unsigned long long e2 = 0;
		unsigned long long e3 = 0;
		unsigned long long e4 = 0;
		unsigned long long e5 = 0;
		unsigned long long e6 = 0;
		unsigned long long e7 = 0;
		unsigned long long e8 = 0;
		unsigned long long e9 = 0;
		unsigned long long e10 = 0;
		unsigned long long e11 = 0;
		unsigned long long e12 = 0;
		unsigned long long e13 = 0;
		unsigned long long e14 = 0;
		unsigned long long e15 = 0;
		unsigned long long e16 = 0;
		unsigned long long e17 = 0;
		unsigned long long e18 = 0;
		unsigned long long e19 = 0;
		unsigned long long loadCell_g = 0;
		unsigned long long numSamples = 0;
		for( std::size_t j = 0; j < data.size(); ++j )
		{
			if( j < 100 )
				continue;

			tdc += data[j].mData.tdc;
			e1 += data[j].mData.e1;
			e2 += data[j].mData.e2;
			e3 += data[j].mData.e3;
			e4 += data[j].mData.e4;
			e5 += data[j].mData.e5;
			e6 += data[j].mData.e6;
			e7 += data[j].mData.e7;
			e8 += data[j].mData.e8;
			e9 += data[j].mData.e9;
			e10 += data[j].mData.e10;
			e11 += data[j].mData.e11;
			e12 += data[j].mData.e12;
			e13 += data[j].mData.e13;
			e14 += data[j].mData.e14;
			e15 += data[j].mData.e15;
			e16 += data[j].mData.e16;
			e17 += data[j].mData.e17;
			e18 += data[j].mData.e18;
			e19 += data[j].mData.e19;
			loadCell_g += data[j].mLoadCell_g;
			++numSamples;
		}

		tdc /= numSamples;
		e1 /= numSamples;
		e2 /= numSamples;
		e3 /= numSamples;
		e4 /= numSamples;
		e5 /= numSamples;
		e6 /= numSamples;
		e7 /= numSamples;
		e8 /= numSamples;
		e9 /= numSamples;
		e10 /= numSamples;
		e11 /= numSamples;
		e12 /= numSamples;
		e13 /= numSamples;
		e14 /= numSamples;
		e15 /= numSamples;
		e16 /= numSamples;
		e17 /= numSamples;
		e18 /= numSamples;
		e19 /= numSamples;
		loadCell_g /= numSamples;
			
		outFile // << static_cast<float>(data[j].mData.pac / 4096.0) << " "
			// << static_cast<float>(data[j].mData.pdc / 4096.0) << " "
			// << static_cast<float>(data[j].mData.tac / 4096.0) << " "
			<< static_cast<float>(tdc / 4096.0) << " "
			<< static_cast<float>(e1 / 4096.0) << " "
			<< static_cast<float>(e2 / 4096.0) << " "
			<< static_cast<float>(e3 / 4096.0) << " "
			<< static_cast<float>(e4 / 4096.0) << " "
			<< static_cast<float>(e5 / 4096.0) << " "
			<< static_cast<float>(e6 / 4096.0) << " "
			<< static_cast<float>(e7 / 4096.0) << " "
			<< static_cast<float>(e8 / 4096.0) << " "
			<< static_cast<float>(e9 / 4096.0) << " "
			<< static_cast<float>(e10 / 4096.0) << " "
			<< static_cast<float>(e11 / 4096.0) << " "
			<< static_cast<float>(e12 / 4096.0) << " "
			<< static_cast<float>(e13 / 4096.0) << " "
			<< static_cast<float>(e14 / 4096.0) << " "
			<< static_cast<float>(e15 / 4096.0) << " "
			<< static_cast<float>(e16 / 4096.0) << " "
			<< static_cast<float>(e17 / 4096.0) << " "
			<< static_cast<float>(e18 / 4096.0) << " "
			<< static_cast<float>(e19 / 4096.0) << " "
			<< std::endl;

		outFile << static_cast<float>(loadCell_g / MAX_BIOTAC_LOAD_G) << std::endl;

		++numDataPoints;
	}

	// Before we exit, we need to write out the number of data points as the first entry of the file.
	outFile.seekp(0);
	outFile << numDataPoints << " " << mNumInputs << " " << mNumOutputs << std::flush;

	outFile.close();

	return fileName;
}

////////////////////////////////////////////////////////////////////////////////

std::string cFunctionFitNN::ConvertDataFilesToTestFile(const std::vector<std::string>& files)
{
	std::string fileName = "./data/test/testing.nndat";
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


