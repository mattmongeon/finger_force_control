#include "function_fit_nn.h"
#include "data_file_reader.h"
#include "utils.h"
#include "../common/biotac_comm.h"
#include <fstream>
#include <iostream>


#define MAX_BIOTAC_LOAD_G 5098.581064889641  // 50 N


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cFunctionFitNN::cFunctionFitNN()
	: mNumLayers(4),
	  mNumInputs(4),
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

void cFunctionFitNN::LoadNeuralNetwork(const std::string& filePath)
{
	
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNN::TrainAgainstDataFiles(const std::vector<std::string>& files)
{
	nUtils::ClearConsole();
	std::cout << "Training on " << files.size() << " total files." << std::endl;

	float desiredError = 0.001;
	unsigned int maxEpochs = 10000;
	unsigned int epochsBetweenReports = 1000;
	for( std::size_t i = 0; i < files.size(); ++i )
	{
		std::cout << std::endl;
		std::cout << "Training on " << files[i] << std::endl;
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

		std::cout << "Inputs:  PDC, PAC, TDC, TAC" << std::endl;
		for(unsigned int j = 0; j < fann_length_train_data(pTrainData); j++)
		{
			fann_type* calc_out = fann_run(mpANN, pTrainData->input[j]);
			std::cout << "("
					  << pTrainData->input[j][0] << ", "
					  << pTrainData->input[j][1] << ", "
					  << pTrainData->input[j][2] << ", "
					  << pTrainData->input[j][3] << ")"
					  << " -> " << calc_out[0] * MAX_BIOTAC_LOAD_G << ", should be " << pTrainData->output[j][0] * MAX_BIOTAC_LOAD_G
					  << ", difference=" << fann_abs(calc_out[0] - pTrainData->output[j][0]) * MAX_BIOTAC_LOAD_G << "\r\n";
		}
		std::cout << std::flush;
	}
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNN::TestAgainstDataFiles(const std::vector<std::string>& files)
{
	
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNN::SaveNeuralNetwork(const std::string& filePath)
{

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
		outFile << static_cast<float>(data[i].mData.pdc / 4096.0) << " "
				<< static_cast<float>(data[i].mData.pac / 4096.0) << " "
				<< static_cast<float>(data[i].mData.tdc / 4096.0) << " "
				<< static_cast<float>(data[i].mData.tac / 4096.0) << " "
				<< std::endl;

		outFile << static_cast<float>(data[i].mLoadCell_g / MAX_BIOTAC_LOAD_G) << std::endl;
	}

	outFile.close();

	return outFileName;
}

