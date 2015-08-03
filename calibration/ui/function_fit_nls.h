#ifndef INCLUDED_FUNCTION_FIT_NLS_H
#define INCLUDED_FUNCTION_FIT_NLS_H

#include "electrode_tdc_compensator.h"
#include <vector>
#include <string>
#include <utility>
#include <map>
#include <stdint.h>
#include <cmath>


class cFunctionFitNLS
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//

	cFunctionFitNLS();

	~cFunctionFitNLS();
	

	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//

    std::vector<cElectrodeTdcCompensator> TrainAgainstDataFiles(const std::vector<std::string>& files);

    void TestAgainstDataFiles(const std::vector<std::string>& files,
							  const std::vector<cElectrodeTdcCompensator>& compensators);
	
	
private:

	//--------------------------------------------------------------------------//
	//----------------------------  HELPER CLASSES  ----------------------------//
	//--------------------------------------------------------------------------//
	
	struct sTdcElectrodeData
	{
		std::vector< std::pair<double, double> > mE1;
		std::vector< std::pair<double, double> > mE2;
		std::vector< std::pair<double, double> > mE3;
		std::vector< std::pair<double, double> > mE4;
		std::vector< std::pair<double, double> > mE5;
		std::vector< std::pair<double, double> > mE6;
		std::vector< std::pair<double, double> > mE7;
		std::vector< std::pair<double, double> > mE8;
		std::vector< std::pair<double, double> > mE9;
		std::vector< std::pair<double, double> > mE10;
		std::vector< std::pair<double, double> > mE11;
		std::vector< std::pair<double, double> > mE12;
		std::vector< std::pair<double, double> > mE13;
		std::vector< std::pair<double, double> > mE14;
		std::vector< std::pair<double, double> > mE15;
		std::vector< std::pair<double, double> > mE16;
		std::vector< std::pair<double, double> > mE17;
		std::vector< std::pair<double, double> > mE18;
		std::vector< std::pair<double, double> > mE19;
	};


	class cExponentialResidual
	{
	public:
		cExponentialResidual(double x, double y)
			: mX(x), mY(y) {}

		template<typename T> bool operator()( const T* const a,
											  const T* const b,
											  const T* const c,
											  const T* const d,
											  T* residual ) const
			{
				residual[0] = T(mY) - (a[0]*pow(T(mX)+b[0], c[0]) + d[0]);
				return true;
			}

	private:
		const double mX;
		const double mY;
	};
	
	
	//--------------------------------------------------------------------------//
	//---------------------------  HELPER FUNCTIONS  ---------------------------//
	//--------------------------------------------------------------------------//

	// Takes in a list of files and generates data for training or testing.
	//
	// Params:
	// files - List of files to be parsed into usable data.
	//
	// Return - Structure containing all of the data from the file.
	sTdcElectrodeData ParseFiles(const std::vector<std::string>& files);

	// Takes in a container of raw data that maps TDC values to values for a particular
	// electrode, averages them per TDC value, and adds them to the parameter object that
	// is part of the sTdcElectrodeData struct.
	//
	// Params:
	// structData - The container that is part of sTdcElectrodeData that will be filled.
	// rawData - The measured data to be processed and added to structData.
	void FillStructMember( std::vector< std::pair<double, double> >& structData,
						   const std::map< int, std::vector<uint16_t> >& rawData );

	// Uses the Ceres library and the parameter data vector to fit a function and return
	// the constants.  The function to be fit is of the form a*(TDC+b)^c + d.
	//
	// Params:
	// data - The container of all of the data points to be used for fitting.
	// a - [OUT] - The scalar applied to the polynomial term.
	// b - [OUT] - The offset applied to the TDC value before being raised to some power.
	// c - [OUT] - The exponential term.
	// d - [OUT] - An offset applied to the entire result.
	void FitToElectrodeData( const std::vector< std::pair<double, double> >& data,
							 double& a, double& b, double& c, double& d );
	
	// Takes in the reference (groundtruth) data and the function  output and
	// plots them along with the error.
	//
	// Parameters:
	// calculated - The output of the neural network after passing in BioTac data.
	// actual - Reference groundtruth value for comparison with function results.
	void PlotResults(const std::vector<float>& calculated, const std::vector<float>& actual );
	

	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	// A flag to protect against calling google::InitGoogleLogging() more than once.
	static bool mLoggingInitialized;
};


#endif  // INCLUDED_FUNCTION_FIT_NLS_H

