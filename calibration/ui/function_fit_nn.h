#ifndef INCLUDED_FUNCTION_FIT_NN_H
#define INCLUDED_FUNCTION_FIT_NN_H


#include <fann.h>
#include <string>
#include <vector>


class cFunctionFitNN
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//

	cFunctionFitNN();

	~cFunctionFitNN();


	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//

	// Loads a saved neural network configuration from file and configures this
	// object to match it.  The parameter should only be a file name.  The location
	// of neural network files is fixed.
	//
	// Params:
	// fileName - The name of the saved neural network setup.
	void LoadNeuralNetwork(const std::string& fileName);

	// Trains this neural network to the parameter files.  If the files are of
	// extension ".data", the files will first be converted to the necessary
	// format required by the FANN library.
	//
	// Params:
	// files - List of paths to files that will be used for training.
	void TrainAgainstDataFiles(const std::vector<std::string>& files);

	// Runs passes the parameter data files through this neural network and tests
	// its output against the groundtruth they contain.  This function will print
	// results to the console.
	//
	// Params:
	// files - List of paths to files that will be used for testing.
	void TestAgainstDataFiles(const std::vector<std::string>& files);

	// Saves the setup of this neural network to file using the parameter file name.
	// The path is a predetermined location.
	//
	// Params:
	// fileName - The name the save file.
	void SaveNeuralNetwork(const std::string& fileName);

	
private:

	//--------------------------------------------------------------------------//
	//---------------------------  HELPER FUNCTIONS  ---------------------------//
	//--------------------------------------------------------------------------//

	// Takes in a string that represents a data file as generated by the cDataLogger
	// class and converts it to the format required by the FANN library.
	//
	// Params:
	// filePath - The path to the file to be converted.
	//
	// Return - The name of the newly-generated file.
	std::string ConvertDataFileToNNFile(const std::string& filePath);
	
	
	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	// Pointer to the neural network object used by this class.
	fann* mpANN;

	// These variables define the structure of our neural network.
	const unsigned int mNumLayers;
	const unsigned int mNumInputs;
	const unsigned int mNumHiddenNeurons;
	const unsigned int mNumOutputs;

};


#endif  // INCLUDED_FUNCTION_FIT_NN_H

