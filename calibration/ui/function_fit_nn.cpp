#include "function_fit_nn.h"
#include "data_file_reader.h"
#include "../common/biotac_comm.h"
#include <fstream>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cFunctionFitNN::cFunctionFitNN()
	: mNumLayers(3),
	  mNumInputs(23),
	  mNumHiddenNeurons(25),
	  mNumOutputs(1)
{
	mpANN = fann_create_standard(mNumLayers, mNumInputs, mNumHiddenNeurons, mNumOutputs);

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
	float desiredError = 0.001;
	unsigned int maxEpochs = 5000;
	unsigned int epochsBetweenReports = 1000;
	for( std::size_t i = 0; i < files.size(); ++i )
	{
		fann_train_on_file(mpANN,
						   ConvertDataFileToNNFile(files[i]).c_str(),
						   maxEpochs,
						   epochsBetweenReports,
						   desiredError);
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
		
	for( std::size_t i = 0; i < data.size(); ++i )
	{
		outFile << static_cast<float>(data[i].mData.pac) << " "
				<< static_cast<float>(data[i].mData.pdc) << " "
				<< static_cast<float>(data[i].mData.tac) << " "
				<< static_cast<float>(data[i].mData.tdc) << " "
				<< static_cast<float>(data[i].mData.e1) << " "
				<< static_cast<float>(data[i].mData.e2) << " "
				<< static_cast<float>(data[i].mData.e3) << " "
				<< static_cast<float>(data[i].mData.e4) << " "
				<< static_cast<float>(data[i].mData.e5) << " "
				<< static_cast<float>(data[i].mData.e6) << " "
				<< static_cast<float>(data[i].mData.e7) << " "
				<< static_cast<float>(data[i].mData.e8) << " "
				<< static_cast<float>(data[i].mData.e9) << " "
				<< static_cast<float>(data[i].mData.e10) << " "
				<< static_cast<float>(data[i].mData.e11) << " "
				<< static_cast<float>(data[i].mData.e12) << " "
				<< static_cast<float>(data[i].mData.e13) << " "
				<< static_cast<float>(data[i].mData.e14) << " "
				<< static_cast<float>(data[i].mData.e15) << " "
				<< static_cast<float>(data[i].mData.e16) << " "
				<< static_cast<float>(data[i].mData.e17) << " "
				<< static_cast<float>(data[i].mData.e18) << " "
				<< static_cast<float>(data[i].mData.e19) << " "
				<< std::endl;

		outFile << static_cast<float>(data[i].mLoadCell_g) << std::endl;
	}

	outFile.close();

	return outFileName;
}

