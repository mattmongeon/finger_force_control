#include "function_fit_nn.h"
#include "data_file_reader.h"
#include "file_utils.h"
#include "utils.h"
#include "../common/biotac_comm.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>


#define MAX_BIOTAC_LOAD_G 5098.581064889641  // 50 N


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cFunctionFitNN::cFunctionFitNN()
	: mNumLayers(4),
	  mNumInputs(3),
	  mNumHiddenNeurons(25),
	  mNumOutputs(1)
{
	mpANN = fann_create_standard(mNumLayers, mNumInputs, mNumHiddenNeurons, mNumHiddenNeurons, mNumOutputs);

	fann_set_activation_function_hidden(mpANN, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output(mpANN, FANN_SIGMOID_SYMMETRIC);
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
	unsigned int epochsBetweenReports = 1000;
	for( std::size_t i = 0; i < files.size(); ++i )
	{
		std::cout << std::endl;
		std::cout << "Training on " << files[i] << " (File " << i+1 << " of " << files.size() << ")" << std::endl;
		std::cout << std::endl;

		fann_train_data* pTrainData = fann_read_train_from_file( ConvertDataFileToNNFile(files[i]).c_str() );
		fann_set_training_algorithm(mpANN, FANN_TRAIN_INCREMENTAL);
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
		std::cout << "Total error: " << sum << "\r\n"
				  << "Average error: " << sum / fann_length_train_data(pTrainData) << "\r\n"
				  << "Max error: " << values[values.size() - 1] << "\r\n"
				  << "Min error: " << values[0] << "\r\n"
				  << "Mode: " << values[values.size() / 2] << "\r\n";
		
		std::cout << std::flush;

		fann_destroy_train(pTrainData);
	}
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNN::TestAgainstDataFiles(const std::vector<std::string>& files)
{
	nUtils::ClearConsole();
	std::cout << "Testing against " << files.size() << " total file(s)." << std::endl;

	for( std::size_t i = 0; i < files.size(); ++i )
	{
		std::cout << std::endl;
		std::cout << "Testing on " << files[i] << " (File " << i+1 << " of " << files.size() << ")" << std::endl;
		std::cout << std::endl;
		
		std::string fileName = files[i];
		fileName += ".nndat";
		fann_train_data* pTrainData = fann_read_train_from_file( fileName.c_str() );

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

std::string cFunctionFitNN::ConvertDataFileToNNFile(const std::string& filePath)
{
	cDataFileReader df(filePath);
	std::string outFileName(filePath + ".nndat");
	std::ofstream outFile(outFileName.c_str());

	std::vector<biotac_tune_data> data = df.GetData();
	outFile << data.size() << " " << mNumInputs << " " << mNumOutputs << std::endl;

	outFile.precision(20);
	for( std::size_t i = 0; i < data.size(); ++i )
	{
		outFile << static_cast<float>(data[i].mData.pac / 4096.0) << " "
				<< static_cast<float>(data[i].mData.pdc / 4096.0) << " "
			// << static_cast<float>(data[i].mData.tac / 4096.0) << " "
				<< static_cast<float>(data[i].mData.tdc / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e1 / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e2 / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e3 / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e4 / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e5 / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e6 / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e7 / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e8 / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e9 / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e10 / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e11 / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e12 / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e13 / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e14 / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e15 / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e16 / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e17 / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e18 / 4096.0) << " "
				// << static_cast<float>(data[i].mData.e19 / 4096.0) << " "
				<< std::endl;

		outFile << static_cast<float>(data[i].mLoadCell_g / MAX_BIOTAC_LOAD_G) << std::endl;
	}

	outFile.close();

	return outFileName;
}

