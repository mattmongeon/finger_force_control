#include "function_fit_force_terms.h"
#include "data_file_reader.h"
#include <ceres/ceres.h>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cFunctionFitForceTerms::cFunctionFitForceTerms(const std::vector<cElectrodeTdcCompensator>& compensators)
	: mCompensators(compensators)
{
	mElectrodeNormalVectors[0][0] = 0.196;
	mElectrodeNormalVectors[0][1] = -0.953;
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

cFunctionFitForceTerms::~cFunctionFitForceTerms()
{

}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

void cFunctionFitForceTerms::TrainAgainstDataFiles(const std::vector<std::string>& files)
{
	// --- Parse Files --- //

	double s1 = 1.0, s2 = 1.0, s3 = 1.0;
	ceres::Problem problem;
	
	// We will parse the files to set up all of our training data, and then we will start
	// the training session.
	for( std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it )
	{
		cDataFileReader reader(*it);

		std::vector<biotac_tune_data> data = reader.GetData();
		for( std::size_t i = 0; i < data.size(); ++i )
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
										  data[i].mLoadCell_g,
										  &mCompensators)),
				NULL,
				&s1, &s2, &s3 );
		}
	}
	

	// --- Start Training --- //

	ceres::Solver::Options options;
	options.max_num_iterations = 1000;
	options.linear_solver_type = ceres::DENSE_QR;
	options.minimizer_progress_to_stdout = true;

	std::cout << "Solving... " << std::endl;
	ceres::Solver::Summary summary;
	ceres::Solve(options, &problem, &summary);
	std::cout << summary.FullReport() << std::endl;
	std::cout << "Final s1: " << s1 << "  s2: " << s2 << "  s3: " << s3 << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitForceTerms::TestAgainstDataFiles(const std::vector<std::string>& files)
{

}


