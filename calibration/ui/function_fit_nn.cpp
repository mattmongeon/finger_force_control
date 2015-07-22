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


#define MAX_BIOTAC_LOAD_G 5098.581064889641  // 50 N


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cFunctionFitNN::cFunctionFitNN()
	: mNumLayers(4),
	  mNumInputs(2),
	  mNumHiddenNeurons(25),
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

	float desiredError = 0.001;
	unsigned int maxEpochs = 10000;
	unsigned int epochsBetweenReports = 100;

	fann_train_data* pTrainData = fann_read_train_from_file( ConvertDataFileToNNFile(files).c_str() );
	// fann_set_training_algorithm(mpANN, FANN_TRAIN_INCREMENTAL);
	// fann_set_activation_steepness_hidden(mpANN, 1);
	// fann_set_activation_steepness_hidden(mpANN, 1);

	fann_set_train_stop_function(mpANN, FANN_STOPFUNC_BIT);
	fann_set_bit_fail_limit(mpANN, 0.01f);
		
	fann_set_training_algorithm(mpANN, FANN_TRAIN_RPROP);

	fann_init_weights(mpANN, pTrainData);

	fann_train_on_data(mpANN, pTrainData, maxEpochs, epochsBetweenReports, desiredError);

	std::vector<float> values;
	for(unsigned int j = 0; j < fann_length_train_data(pTrainData); j++)
	{
		fann_type* calc_out = fann_run(mpANN, pTrainData->input[j]);
		std::cout << "Point " << j << " -> "
				  << calc_out[0] * MAX_BIOTAC_LOAD_G << ", should be " << pTrainData->output[j][0] * MAX_BIOTAC_LOAD_G
				  << ", difference=" << fann_abs(calc_out[0] - pTrainData->output[j][0]) * MAX_BIOTAC_LOAD_G << "\r\n";

		values.push_back(calc_out[0]);
	}

	std::sort(values.begin(), values.end());
	float sum = std::accumulate(values.begin(), values.end(), 0.0f);
	std::cout << "Total training points: " << fann_length_train_data(pTrainData) << "\r\n"
			  << "Total error: " << sum * MAX_BIOTAC_LOAD_G << "\r\n"
			  << "Average error: " << (sum / fann_length_train_data(pTrainData)) * MAX_BIOTAC_LOAD_G << "\r\n"
			  << "Max error: " << values[values.size() - 1] * MAX_BIOTAC_LOAD_G << "\r\n"
			  << "Min error: " << values[0] * MAX_BIOTAC_LOAD_G << "\r\n"
			  << "Mode: " << values[values.size() / 2] * MAX_BIOTAC_LOAD_G << "\r\n";
		
	std::cout << std::flush;

	fann_destroy_train(pTrainData);
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNN::TestAgainstDataFiles(const std::vector<std::string>& files)
{
	nUtils::ClearConsole();
	std::cout << "Testing against " << files.size() << " total file(s)." << std::endl;

	fann_train_data* pTrainData = fann_read_train_from_file( ConvertDataFileToNNFile(files).c_str() );

	unsigned int dataLength = fann_length_train_data(pTrainData);
	for( unsigned int j = 0; j < dataLength; ++j )
	{
		fann_reset_MSE(mpANN);

		fann_type* calc_out = fann_test(mpANN, pTrainData->input[j], pTrainData->output[j]);

		std::cout << "Point " << j << " -> "
				  << calc_out[0] * MAX_BIOTAC_LOAD_G << ", should be " << pTrainData->output[j][0] * MAX_BIOTAC_LOAD_G
				  << ", difference=" << fann_abs(calc_out[0] - pTrainData->output[j][0]) * MAX_BIOTAC_LOAD_G << "\r\n";
	}

	fann_destroy_train(pTrainData);
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
		numDataPoints += data.size();

		outFile.precision(20);
		for( std::size_t j = 0; j < data.size(); ++j )
		{
			outFile // << static_cast<float>(data[j].mData.pac / 4096.0) << " "
					<< static_cast<float>(data[j].mData.pdc / 4096.0) << " "
				// << static_cast<float>(data[j].mData.tac / 4096.0) << " "
					<< static_cast<float>(data[j].mData.tdc / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e1 / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e2 / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e3 / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e4 / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e5 / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e6 / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e7 / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e8 / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e9 / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e10 / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e11 / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e12 / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e13 / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e14 / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e15 / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e16 / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e17 / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e18 / 4096.0) << " "
				// << static_cast<float>(data[j].mData.e19 / 4096.0) << " "
					<< std::endl;

			outFile << static_cast<float>(data[j].mLoadCell_g / MAX_BIOTAC_LOAD_G) << std::endl;
		}
	}

	// Before we exit, we need to write out the number of data points as the first entry of the file.
	outFile.seekp(0);
	outFile << numDataPoints << " " << mNumInputs << " " << mNumOutputs << std::flush;

	outFile.close();

	return fileName;
}

