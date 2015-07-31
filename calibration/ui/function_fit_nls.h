#ifndef INCLUDED_FUNCTION_FIT_NLS_H
#define INCLUDED_FUNCTION_FIT_NLS_H

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

	void TrainAgainstDataFiles(const std::vector<std::string>& files);

	void TestAgainstDataFiles(const std::vector<std::string>& files);
	
	
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

	void FillStructMember( std::vector< std::pair<double, double> >& structData,
						   const std::map< int, std::vector<uint16_t> >& rawData );

	void TrainElectrodeData( const std::vector< std::pair<double, double> >& data,
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

	static bool mLoggingInitialized;
};


#endif  // INCLUDED_FUNCTION_FIT_NLS_H

