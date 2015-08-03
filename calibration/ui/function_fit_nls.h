#ifndef INCLUDED_FUNCTION_FIT_NLS_H
#define INCLUDED_FUNCTION_FIT_NLS_H

#include "electrode_tdc_compensator.h"
#include <vector>
#include <string>
#include <utility>
#include <map>
#include <stdint.h>
#include <cmath>


class plstream;


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
	//----------------------------  CERES CLASSES  -----------------------------//
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


	class cPolynomialResidual
	{
	public:
		cPolynomialResidual(double x, double y)
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

	// Takes in an array of vectors (one for each electrode) and plots the errors
	// at each measurement point.
	//
	// Parameters:
	// errors - Array of vectors containing error differences at each point.
	void PlotTestingErrors(const std::vector<double>* errors );

	// Plots the errors of electrodes 1 through 5 as a sub-plot of the parameter
	// plot stream.
	//
	// Params:
	// errors - Array of vectors containing the errors for electrodes 1-5.
	// plottingStream - The streaming plotting device.
	void PlotE1ThroughE5(const std::vector<double>* errors, plstream& plottingStream);

	// Plots the errors of electrodes 6 through 10 as a sub-plot of the parameter
	// plot stream.
	//
	// Params:
	// errors - Array of vectors containing the errors for electrodes 6-10.
	// plottingStream - The streaming plotting device.
	void PlotE6ThroughE10(const std::vector<double>* errors, plstream& plottingStream);

	// Plots the errors of electrodes 11 through 15 as a sub-plot of the parameter
	// plot stream.
	//
	// Params:
	// errors - Array of vectors containing the errors for electrodes 11-15.
	// plottingStream - The streaming plotting device.
	void PlotE11ThroughE15(const std::vector<double>* errors, plstream& plottingStream);

	// Plots the errors of electrodes 16 through 19 as a sub-plot of the parameter
	// plot stream.
	//
	// Params:
	// errors - Array of vectors containing the errors for electrodes 16-19.
	// plottingStream - The streaming plotting device.
	void PlotE16ThroughE19(const std::vector<double>* errors, plstream& plottingStream);
	

	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	// A flag to protect against calling google::InitGoogleLogging() more than once.
	static bool mLoggingInitialized;
};


#endif  // INCLUDED_FUNCTION_FIT_NLS_H
