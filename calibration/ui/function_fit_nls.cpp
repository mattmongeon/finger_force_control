#include "function_fit_nls.h"
#include "data_file_reader.h"
#include "utils.h"
#include <fstream>
#include <map>
#include <numeric>
#include <iostream>
#include <plplot/plplot.h>
#include <plplot/plstream.h>
#include <ceres/ceres.h>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cFunctionFitNLS::cFunctionFitNLS()
{
	
}

////////////////////////////////////////////////////////////////////////////////

cFunctionFitNLS::~cFunctionFitNLS()
{

}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

std::vector<cElectrodeTdcCompensator> cFunctionFitNLS::TrainAgainstDataFiles(const std::vector<std::string>& files)
{
	// --- Parse Data Files --- //
	
	sTdcElectrodeData data = ParseFiles(files);


	double a1[5] = {664.0,0.1,1,1,1};
	double b1[5] = {-911.0,1,1,1,1};
	double c1[5] = {-0.001,1,1,1,1};
	double d1[5] = {2568.0,1,1,1,1};

	double a2[5] = {520.0,0.1,1,1,1};
	double b2[5] = {-762.0,1,1,1,1};
	double c2[5] = {-0.0023,1,1,1,1};
	double d2[5] = {3004.0,1,1,1,1};

	double a3[5] = {522.0,0.1,1,1,1};
	double b3[5] = {-761.0,1,1,1,1};
	double c3[5] = {-0.002,1,1,1,1};
	double d3[5] = {3000.0,1,1,1,1};

	double a4[5] = {655.0,0.1,1,1,1};
	double b4[5] = {-750.0,1,1,1,1};
	double c4[5] = {-0.003,1,1,1,1};
	double d4[5] = {2724.0,1,1,1,1};

	double a5[5] = {516.0,0.1,1,1,1};
	double b5[5] = {-784.0,1,1,1,1};
	double c5[5] = {-0.002,1,1,1,1};
	double d5[5] = {3000.0,1,1,1,1};

	double a6[5] = {654.0,0.1,1,1,1};
	double b6[5] = {-765.0,1,1,1,1};
	double c6[5] = {-0.003,1,1,1,1};
	double d6[5] = {2716.0,1,1,1,1};

	double a7[5] = {817.0,0.1,1,1,1};
	double b7[5] = {-816.0,1,1,1,1};
	double c7[5] = {-0.001,1,1,1,1};
	double d7[5] = {2594.0,1,1,1,1};

	double a8[5] = {725.0,0.1,1,1,1};
	double b8[5] = {-764.0,1,1,1,1};
	double c8[5] = {-0.001,1,1,1,1};
	double d8[5] = {2677.0,1,1,1,1};

	double a9[5] = {756.0,0.1,1,1,1};
	double b9[5] = {-732.0,1,1,1,1};
	double c9[5] = {-0.001,1,1,1,1};
	double d9[5] = {2680.0,1,1,1,1};

	double a10[5] = {672.0,0.1,1,1,1};
	double b10[5] = {-737.0,1,1,1,1};
	double c10[5] = {-0.001,1,1,1,1};
	double d10[5] = {2773.0,1,1,1,1};

	double a11[5] = {807.0,0.1,1,1,1};
	double b11[5] = {-923.0,1,1,1,1};
	double c11[5] = {-0.001,1,1,1,1};
	double d11[5] = {2548.0,1,1,1,1};

	double a12[5] = {606.0,0.1,1,1,1};
	double b12[5] = {-691.0,1,1,1,1};
	double c12[5] = {-0.002,1,1,1,1};
	double d12[5] = {2917.0,1,1,1,1};

	double a13[5] = {633.0,0.1,1,1,1};
	double b13[5] = {-683.0,1,1,1,1};
	double c13[5] = {-0.002,1,1,1,1};
	double d13[5] = {2876.0,1,1,1,1};

	double a14[5] = {480.0,0.1,1,1,1};
	double b14[5] = {-864.0,1,1,1,1};
	double c14[5] = {-0.002,1,1,1,1};
	double d14[5] = {2867.0,1,1,1,1};

	double a15[5] = {803.0,-0.1,1,1,1};
	double b15[5] = {-626.0,1,1,1,1};
	double c15[5] = {-0.002,1,1,1,1};
	double d15[5] = {2674.0,1,1,1,1};

	double a16[5] = {489.0,0.1,1,1,1};
	double b16[5] = {-870.0,1,1,1,1};
	double c16[5] = {-0.002,1,1,1,1};
	double d16[5] = {2857.0,1,1,1,1};

	double a17[5] = {588.0,0.1,1,1,1};
	double b17[5] = {-734.0,1,1,1,1};
	double c17[5] = {-0.002,1,1,1,1};
	double d17[5] = {2897.0,1,1,1,1};

	double a18[5] = {466.0,0.1,1,1,1};
	double b18[5] = {-811.0,1,1,1,1};
	double c18[5] = {-0.002,1,1,1,1};
	double d18[5] = {3037.0,1,1,1,1};

	double a19[5] = {610.0,0.1,1,1,1};
	double b19[5] = {-720.0,1,1,1,1};
	double c19[5] = {-0.002,1,1,1,1};
	double d19[5] = {2840.0,1,1,1,1};

			
// --- Fit Against Data --- //

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			std::cout << "E1:" << std::endl;
			FitToElectrodeData(data.mE1, a1, b1, c1, d1);
			std::cout << std::endl;
		}

		#pragma omp section
		{
			std::cout << "E2:" << std::endl;
			FitToElectrodeData(data.mE2, a2, b2, c2, d2);
			std::cout << std::endl;
		}

		#pragma omp section
		{
			std::cout << "E3:" << std::endl;
			FitToElectrodeData(data.mE3, a3, b3, c3, d3);
			std::cout << std::endl;
		}

		#pragma omp section
		{
			std::cout << "E4:" << std::endl;
			FitToElectrodeData(data.mE4, a4, b4, c4, d4);
			std::cout << std::endl;
		}

		#pragma omp section
		{
			std::cout << "E5:" << std::endl;
			FitToElectrodeData(data.mE5, a5, b5, c5, d5);
			std::cout << std::endl;
		}

		#pragma omp section
		{
			std::cout << "E6:" << std::endl;
			FitToElectrodeData(data.mE6, a6, b6, c6, d6);
			std::cout << std::endl;
		}

		#pragma omp section
		{
			std::cout << "E7:" << std::endl;
			FitToElectrodeData(data.mE7, a7, b7, c7, d7);
			std::cout << std::endl;
		}

		#pragma omp section
		{
			std::cout << "E8:" << std::endl;
			FitToElectrodeData(data.mE8, a8, b8, c8, d8);
			std::cout << std::endl;
		}

		#pragma omp section
		{
			std::cout << "E9:" << std::endl;
			FitToElectrodeData(data.mE9, a9, b9, c9, d9);
			std::cout << std::endl;
		}

		#pragma omp section
		{
			std::cout << "E10:" << std::endl;
			FitToElectrodeData(data.mE10, a10, b10, c10, d10);
			std::cout << std::endl;
		}

		#pragma omp section
		{
			std::cout << "E11:" << std::endl;
			FitToElectrodeData(data.mE11, a11, b11, c11, d11);
			std::cout << std::endl;
		}

		#pragma omp section
		{
			std::cout << "E12:" << std::endl;
			FitToElectrodeData(data.mE12, a12, b12, c12, d12);
			std::cout << std::endl;
		}

		#pragma omp section
		{
			std::cout << "E13:" << std::endl;
			FitToElectrodeData(data.mE13, a13, b13, c13, d13);
			std::cout << std::endl;
		}

		#pragma omp section
		{
			std::cout << "E14:" << std::endl;
			FitToElectrodeData(data.mE14, a14, b14, c14, d14);
			std::cout << std::endl;
		}

		#pragma omp section
		{
			std::cout << "E15:" << std::endl;
			FitToElectrodeData(data.mE15, a15, b15, c15, d15);
			std::cout << std::endl;
		}

		#pragma omp section
		{
			std::cout << "E16:" << std::endl;
			FitToElectrodeData(data.mE16, a16, b16, c16, d16);
			std::cout << std::endl;
		}

		#pragma omp section
		{
			std::cout << "E17:" << std::endl;
			FitToElectrodeData(data.mE17, a17, b17, c17, d17);
			std::cout << std::endl;
		}

		#pragma omp section
		{
			std::cout << "E18:" << std::endl;
			FitToElectrodeData(data.mE18, a18, b18, c18, d18);
			std::cout << std::endl;
		}

		#pragma omp section
		{
			std::cout << "E19:" << std::endl;
			FitToElectrodeData(data.mE19, a19, b19, c19, d19);
			std::cout << std::endl;
		}
	}

	std::cout << "POW TDC, EXP TDC, CONSTANT" << std::endl;

	std::vector<cElectrodeTdcCompensator> retVal;
	retVal.push_back(cElectrodeTdcCompensator(a1, b1, c1, d1));
	retVal.push_back(cElectrodeTdcCompensator(a2, b2, c2, d2));
	retVal.push_back(cElectrodeTdcCompensator(a3, b3, c3, d3));
	retVal.push_back(cElectrodeTdcCompensator(a4, b4, c4, d4));
	retVal.push_back(cElectrodeTdcCompensator(a5, b5, c5, d5));
	retVal.push_back(cElectrodeTdcCompensator(a6, b6, c6, d6));
	retVal.push_back(cElectrodeTdcCompensator(a7, b7, c7, d7));
	retVal.push_back(cElectrodeTdcCompensator(a8, b8, c8, d8));
	retVal.push_back(cElectrodeTdcCompensator(a9, b9, c9, d9));
	retVal.push_back(cElectrodeTdcCompensator(a10, b10, c10, d10));
	retVal.push_back(cElectrodeTdcCompensator(a11, b11, c11, d11));
	retVal.push_back(cElectrodeTdcCompensator(a12, b12, c12, d12));
	retVal.push_back(cElectrodeTdcCompensator(a13, b13, c13, d13));
	retVal.push_back(cElectrodeTdcCompensator(a14, b14, c14, d14));
	retVal.push_back(cElectrodeTdcCompensator(a15, b15, c15, d15));
	retVal.push_back(cElectrodeTdcCompensator(a16, b16, c16, d16));
	retVal.push_back(cElectrodeTdcCompensator(a17, b17, c17, d17));
	retVal.push_back(cElectrodeTdcCompensator(a18, b18, c18, d18));
	retVal.push_back(cElectrodeTdcCompensator(a19, b19, c19, d19));
	
	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::TestAgainstDataFiles(const std::vector<std::string>& files,
										   const std::vector<cElectrodeTdcCompensator>& compensators)
{
	std::vector<double> errors[19];
	for( std::size_t i = 0; i < 19; ++i )
		errors[i] = std::vector<double>();
	
	for( std::size_t i = 0; i < files.size(); ++i )
	{
		cDataFileReader reader(files[i]);
		for( std::size_t j = 0; j < 19; ++j )
			errors[j] = std::vector<double>();


		// --- Test Againts All Data Points --- //
		
		std::vector<biotac_tune_data> data = reader.GetData();
		for( std::size_t dataIndex = 0; dataIndex < reader.GetNumDataPoints(); ++dataIndex )
		{
			errors[0].push_back(compensators[0].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e1));
			errors[1].push_back(compensators[1].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e2));
			errors[2].push_back(compensators[2].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e3));
			errors[3].push_back(compensators[3].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e4));
			errors[4].push_back(compensators[4].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e5));
			errors[5].push_back(compensators[5].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e6));
			errors[6].push_back(compensators[6].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e7));
			errors[7].push_back(compensators[7].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e8));
			errors[8].push_back(compensators[8].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e9));
			errors[9].push_back(compensators[9].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e10));
			errors[10].push_back(compensators[10].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e11));
			errors[11].push_back(compensators[11].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e12));
			errors[12].push_back(compensators[12].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e13));
			errors[13].push_back(compensators[13].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e14));
			errors[14].push_back(compensators[14].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e15));
			errors[15].push_back(compensators[15].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e16));
			errors[16].push_back(compensators[16].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e17));
			errors[17].push_back(compensators[17].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e18));
			errors[18].push_back(compensators[18].CompensateTdc(data[dataIndex].mData.tdc, data[dataIndex].mData.pdc, data[dataIndex].mData.e19));
		}


		// --- Plot Errors --- //

		std::cout << "Plotting file \'" << files[i] << "\'" << std::endl << std::endl;
		PlotTestingErrors(errors);
	}
	
	std::cout << std::endl << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
//  Helper Functions
////////////////////////////////////////////////////////////////////////////////

cFunctionFitNLS::sTdcElectrodeData cFunctionFitNLS::ParseFiles(const std::vector<std::string>& files)
{
	std::vector<sDataPoint> e1, e2, e3, e4, e5, e6, e7, e8, e9, e10,
		e11, e12, e13, e14, e15, e16, e17, e18, e19;
	sTdcElectrodeData data;

	// Read all of the data from file. For each electrode associate individual TDC values
	// with a vector of electrode readings.  All files will be parsed first and sorted in
	// this way before moving on.
	for( std::size_t i = 0; i < files.size(); ++i )
	{
		// --- Prepare Data Containers --- //

		e1.clear();
		e2.clear();
		e3.clear();
		e4.clear();
		e5.clear();
		e6.clear();
		e7.clear();
		e8.clear();
		e9.clear();
		e10.clear();
		e11.clear();
		e12.clear();
		e13.clear();
		e14.clear();
		e15.clear();
		e16.clear();
		e17.clear();
		e18.clear();
		e19.clear();
		
		
		// --- Parse File --- //
		
		cDataFileReader reader(files[i]);
		std::vector<biotac_tune_data> readerData = reader.GetData();
		std::size_t numDataPoints = readerData.size();
		for( std::size_t dataIndex = 0; dataIndex < numDataPoints; ++dataIndex )
		{
			e1.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e1) );
			e2.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e2) );
			e3.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e3) );
			e4.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e4) );
			e5.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e5) );
			e6.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e6) );
			e7.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e7) );
			e8.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e8) );
			e9.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e9) );
			e10.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e10) );
			e11.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e11) );
			e12.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e12) );
			e13.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e13) );
			e14.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e14) );
			e15.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e15) );
			e16.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e16) );
			e17.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e17) );
			e18.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e18) );
			e19.push_back( sDataPoint( readerData[dataIndex].mData.tdc, readerData[dataIndex].mData.pdc, readerData[dataIndex].mData.e19) );
		}


		// --- Generate Data Points --- //

		FillStructMember( data.mE1, e1 );
		FillStructMember( data.mE2, e2 );
		FillStructMember( data.mE3, e3 );
		FillStructMember( data.mE4, e4 );
		FillStructMember( data.mE5, e5 );
		FillStructMember( data.mE6, e6 );
		FillStructMember( data.mE7, e7 );
		FillStructMember( data.mE8, e8 );
		FillStructMember( data.mE9, e9 );
		FillStructMember( data.mE10, e10 );
		FillStructMember( data.mE11, e11 );
		FillStructMember( data.mE12, e12 );
		FillStructMember( data.mE13, e13 );
		FillStructMember( data.mE14, e14 );
		FillStructMember( data.mE15, e15 );
		FillStructMember( data.mE16, e16 );
		FillStructMember( data.mE17, e17 );
		FillStructMember( data.mE18, e18 );
		FillStructMember( data.mE19, e19 );		
	}

	return data;
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::FillStructMember( std::vector<cFunctionFitNLS::sDataPoint>& structData,
					   const std::vector<cFunctionFitNLS::sDataPoint>& rawData )
{
	// First sort all of the data according to the TDC value.
	std::map<int, std::vector<sDataPoint> > eData;
	for( std::vector<cFunctionFitNLS::sDataPoint>::const_iterator it = rawData.begin();
		 it != rawData.end();
		 ++it )
	{
		eData[it->mTDC].push_back( sDataPoint(it->mTDC, it->mAvgPDC, it->mAvgElectrode) );
	}
	

	// Now generate the actual data points by averaging the vector of associated TAC and electrode values.
	for( std::map<int, std::vector<sDataPoint> >::iterator it = eData.begin();
		 it != eData.end();
		 ++it )
	{
		// Calculate the average of all of the electrode and TAC values per TDC value.
		double pdcSum = 0, electrodeSum = 0;
		for( std::size_t i = 0; i < it->second.size(); ++i )
		{
			pdcSum += it->second[i].mAvgPDC;
			electrodeSum += it->second[i].mAvgElectrode;
		}
		
		// Generate a data point which consists of TDC value and electrode average.
		sDataPoint p;
		p.mTDC = static_cast<double>(it->first);
		p.mAvgPDC = pdcSum / static_cast<double>(it->second.size());
		p.mAvgElectrode = electrodeSum / static_cast<double>(it->second.size());
		structData.push_back(p);
	}
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::FitToElectrodeData( const std::vector<cFunctionFitNLS::sDataPoint>& data,
										  double* pA, double* pB, double* pC, double* pD )
{
	ceres::Problem problem;
	int numDataPoints = data.size();
	for( int index = 0; index < numDataPoints; ++index )
	{
		problem.AddResidualBlock(
			new ceres::AutoDiffCostFunction<cPolynomialResidual, 1, 5, 5, 5, 5>(
				new cPolynomialResidual( data[index].mTDC, data[index].mAvgPDC, data[index].mAvgElectrode ) ),
			NULL,
			pA, pB, pC, pD );
	}

	ceres::Solver::Options options;
	options.max_num_iterations = 10000;
	options.linear_solver_type = ceres::DENSE_QR;
	options.minimizer_progress_to_stdout = true;

	std::cout << "Solving... " << std::endl;
	ceres::Solver::Summary summary;
	ceres::Solve(options, &problem, &summary);
	std::cout << summary.FullReport() << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::PlotTestingErrors(const std::vector<double>* errors )
{
#ifdef WIN32
	std::string plotDriver = "wingcc";
#else
	std::string plotDriver = "xcairo";
#endif

	// --- Initialize --- //
	
	plstream plottingStream(1, 1, 255, 255, 255, plotDriver.c_str());
	plottingStream.star(2, 2);

	PlotE1ThroughE5(errors, plottingStream);
	PlotE6ThroughE10(&(errors[5]), plottingStream);
	PlotE11ThroughE15(&(errors[10]), plottingStream);
	PlotE16ThroughE19(&(errors[15]), plottingStream);
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::PlotE1ThroughE5(const std::vector<double>* errors, plstream& plottingStream)
{
	PLFLT* pX = new PLFLT[errors[0].size()];
	PLFLT* pE1 = new PLFLT[errors[0].size()];
	PLFLT* pE2 = new PLFLT[errors[1].size()];
	PLFLT* pE3 = new PLFLT[errors[2].size()];
	PLFLT* pE4 = new PLFLT[errors[3].size()];
	PLFLT* pE5 = new PLFLT[errors[4].size()];

	PLFLT ymin = 1000000.0, ymax = -1000000.0;
	for( std::size_t i = 0; i < errors[0].size(); ++i )
	{
		pX[i] = i / 100.0;
		pE1[i] = errors[0][i];
		pE2[i] = errors[1][i];
		pE3[i] = errors[2][i];
		pE4[i] = errors[3][i];
		pE5[i] = errors[4][i];

		ymax = std::max(pE1[i], ymax);
		ymax = std::max(pE2[i], ymax);
		ymax = std::max(pE3[i], ymax);
		ymax = std::max(pE4[i], ymax);
		ymax = std::max(pE5[i], ymax);

		ymin = std::min(pE1[i], ymin);
		ymin = std::min(pE2[i], ymin);
		ymin = std::min(pE3[i], ymin);
		ymin = std::min(pE4[i], ymin);
		ymin = std::min(pE5[i], ymin);
	}


	// --- Generate Plot --- //

	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.env(0, errors[0].size() / 100.0, ymin*0.99, ymax*1.01, 0, 0);
	plottingStream.lab("Time (s)", "Error", "Electrodes 1-5");

	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.line(errors[0].size(), pX, pE1);

	plottingStream.col0(nUtils::enumPLplotColor_YELLOW);
	plottingStream.line(errors[0].size(), pX, pE2);

	plottingStream.col0(nUtils::enumPLplotColor_GREEN);
	plottingStream.line(errors[0].size(), pX, pE3);

	plottingStream.col0(nUtils::enumPLplotColor_AQUAMARINE);
	plottingStream.line(errors[0].size(), pX, pE4);

	plottingStream.col0(nUtils::enumPLplotColor_PINK);
	plottingStream.line(errors[0].size(), pX, pE5);


	// --- Create Legend --- //

	PLINT optArray[5];
	PLINT textColors[5];
	PLINT lineColors[5];
	PLINT lineStyles[5];
	PLFLT lineWidths[5];
	const char* text[5];

	optArray[0] = PL_LEGEND_LINE;
	textColors[0] = nUtils::enumPLplotColor_RED;
	lineColors[0] = nUtils::enumPLplotColor_RED;
	text[0] = "E1";
	lineStyles[0] = 1;
	lineWidths[0] = 1.0;

	optArray[1] = PL_LEGEND_LINE;
	textColors[1] = nUtils::enumPLplotColor_YELLOW;
	text[1] = "E2";
	lineColors[1] = nUtils::enumPLplotColor_YELLOW;
	lineStyles[1] = 1;
	lineWidths[1] = 1.0;

	optArray[2] = PL_LEGEND_LINE;
	textColors[2] = nUtils::enumPLplotColor_GREEN;
	lineColors[2] = nUtils::enumPLplotColor_GREEN;
	text[2] = "E3";
	lineStyles[2] = 1;
	lineWidths[2] = 1.0;

	optArray[3] = PL_LEGEND_LINE;
	textColors[3] = nUtils::enumPLplotColor_AQUAMARINE;
	text[3] = "E4";
	lineColors[3] = nUtils::enumPLplotColor_AQUAMARINE;
	lineStyles[3] = 1;
	lineWidths[3] = 1.0;

	optArray[4] = PL_LEGEND_LINE;
	textColors[4] = nUtils::enumPLplotColor_PINK;
	lineColors[4] = nUtils::enumPLplotColor_PINK;
	text[4] = "E5";
	lineStyles[4] = 1;
	lineWidths[4] = 1.0;

	PLFLT legendWidth, legendHeight;
	plottingStream.legend( &legendWidth, &legendHeight,
						   PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX,
						   PL_POSITION_RIGHT | PL_POSITION_TOP,
						   0.0, 0.0, 0.01,
						   15, 1, 1, 0, 0,
						   5, optArray,
						   0.5, 0.5, 1.0, 0.0,
						   textColors, (const char* const *) text,
						   NULL, NULL, NULL, NULL,
						   lineColors, lineStyles, lineWidths,
						   NULL, NULL, NULL, NULL );
		
	delete[] pX;
	delete[] pE1;
	delete[] pE2;
	delete[] pE3;
	delete[] pE4;
	delete[] pE5;
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::PlotE6ThroughE10(const std::vector<double>* errors, plstream& plottingStream)
{
	PLFLT* pX = new PLFLT[errors[0].size()];
	PLFLT* pE6 = new PLFLT[errors[0].size()];
	PLFLT* pE7 = new PLFLT[errors[1].size()];
	PLFLT* pE8 = new PLFLT[errors[2].size()];
	PLFLT* pE9 = new PLFLT[errors[3].size()];
	PLFLT* pE10 = new PLFLT[errors[4].size()];

	PLFLT ymin = 1000000.0, ymax = -1000000.0;
	for( std::size_t i = 0; i < errors[0].size(); ++i )
	{
		pX[i] = i / 100.0;
		pE6[i] = errors[0][i];
		pE7[i] = errors[1][i];
		pE8[i] = errors[2][i];
		pE9[i] = errors[3][i];
		pE10[i] = errors[4][i];

		ymax = std::max(pE6[i], ymax);
		ymax = std::max(pE7[i], ymax);
		ymax = std::max(pE8[i], ymax);
		ymax = std::max(pE9[i], ymax);
		ymax = std::max(pE10[i], ymax);

		ymin = std::min(pE6[i], ymin);
		ymin = std::min(pE7[i], ymin);
		ymin = std::min(pE8[i], ymin);
		ymin = std::min(pE9[i], ymin);
		ymin = std::min(pE10[i], ymin);
	}


	// --- Generate Plot --- //

	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.env(0, errors[0].size() / 100.0, ymin*0.99, ymax*1.01, 0, 0);
	plottingStream.lab("Time (s)", "Error", "Electrodes 6-10");

	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.line(errors[0].size(), pX, pE6);

	plottingStream.col0(nUtils::enumPLplotColor_YELLOW);
	plottingStream.line(errors[0].size(), pX, pE7);

	plottingStream.col0(nUtils::enumPLplotColor_GREEN);
	plottingStream.line(errors[0].size(), pX, pE8);

	plottingStream.col0(nUtils::enumPLplotColor_AQUAMARINE);
	plottingStream.line(errors[0].size(), pX, pE9);

	plottingStream.col0(nUtils::enumPLplotColor_PINK);
	plottingStream.line(errors[0].size(), pX, pE10);


	// --- Create Legend --- //

	PLINT optArray[5];
	PLINT textColors[5];
	PLINT lineColors[5];
	PLINT lineStyles[5];
	PLFLT lineWidths[5];
	const char* text[5];

	optArray[0] = PL_LEGEND_LINE;
	textColors[0] = nUtils::enumPLplotColor_RED;
	lineColors[0] = nUtils::enumPLplotColor_RED;
	text[0] = "E6";
	lineStyles[0] = 1;
	lineWidths[0] = 1.0;

	optArray[1] = PL_LEGEND_LINE;
	textColors[1] = nUtils::enumPLplotColor_YELLOW;
	text[1] = "E7";
	lineColors[1] = nUtils::enumPLplotColor_YELLOW;
	lineStyles[1] = 1;
	lineWidths[1] = 1.0;

	optArray[2] = PL_LEGEND_LINE;
	textColors[2] = nUtils::enumPLplotColor_GREEN;
	lineColors[2] = nUtils::enumPLplotColor_GREEN;
	text[2] = "E8";
	lineStyles[2] = 1;
	lineWidths[2] = 1.0;

	optArray[3] = PL_LEGEND_LINE;
	textColors[3] = nUtils::enumPLplotColor_AQUAMARINE;
	text[3] = "E9";
	lineColors[3] = nUtils::enumPLplotColor_AQUAMARINE;
	lineStyles[3] = 1;
	lineWidths[3] = 1.0;

	optArray[4] = PL_LEGEND_LINE;
	textColors[4] = nUtils::enumPLplotColor_PINK;
	lineColors[4] = nUtils::enumPLplotColor_PINK;
	text[4] = "E10";
	lineStyles[4] = 1;
	lineWidths[4] = 1.0;

	PLFLT legendWidth, legendHeight;
	plottingStream.legend( &legendWidth, &legendHeight,
						   PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX,
						   PL_POSITION_RIGHT | PL_POSITION_TOP,
						   0.0, 0.0, 0.01,
						   15, 1, 1, 0, 0,
						   5, optArray,
						   0.5, 0.5, 1.0, 0.0,
						   textColors, (const char* const *) text,
						   NULL, NULL, NULL, NULL,
						   lineColors, lineStyles, lineWidths,
						   NULL, NULL, NULL, NULL );
	
	delete[] pX;
	delete[] pE6;
	delete[] pE7;
	delete[] pE8;
	delete[] pE9;
	delete[] pE10;
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::PlotE11ThroughE15(const std::vector<double>* errors, plstream& plottingStream)
{
	PLFLT* pX = new PLFLT[errors[0].size()];
	PLFLT* pE11 = new PLFLT[errors[0].size()];
	PLFLT* pE12 = new PLFLT[errors[1].size()];
	PLFLT* pE13 = new PLFLT[errors[2].size()];
	PLFLT* pE14 = new PLFLT[errors[3].size()];
	PLFLT* pE15 = new PLFLT[errors[4].size()];

	PLFLT ymin = 1000000.0, ymax = -1000000.0;
	for( std::size_t i = 0; i < errors[0].size(); ++i )
	{
		pX[i] = i / 100.0;
		pE11[i] = errors[0][i];
		pE12[i] = errors[1][i];
		pE13[i] = errors[2][i];
		pE14[i] = errors[3][i];
		pE15[i] = errors[4][i];

		ymax = std::max(pE11[i], ymax);
		ymax = std::max(pE12[i], ymax);
		ymax = std::max(pE13[i], ymax);
		ymax = std::max(pE14[i], ymax);
		ymax = std::max(pE15[i], ymax);

		ymin = std::min(pE11[i], ymin);
		ymin = std::min(pE12[i], ymin);
		ymin = std::min(pE13[i], ymin);
		ymin = std::min(pE14[i], ymin);
		ymin = std::min(pE15[i], ymin);
	}


	// --- Generate Plot --- //

	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.env(0, errors[0].size() / 100.0, ymin*0.99, ymax*1.01, 0, 0);
	plottingStream.lab("Time (s)", "Error", "Electrodes 11-15");

	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.line(errors[0].size(), pX, pE11);

	plottingStream.col0(nUtils::enumPLplotColor_YELLOW);
	plottingStream.line(errors[0].size(), pX, pE12);

	plottingStream.col0(nUtils::enumPLplotColor_GREEN);
	plottingStream.line(errors[0].size(), pX, pE13);

	plottingStream.col0(nUtils::enumPLplotColor_AQUAMARINE);
	plottingStream.line(errors[0].size(), pX, pE14);

	plottingStream.col0(nUtils::enumPLplotColor_PINK);
	plottingStream.line(errors[0].size(), pX, pE15);


	// --- Create Legend --- //

	PLINT optArray[5];
	PLINT textColors[5];
	PLINT lineColors[5];
	PLINT lineStyles[5];
	PLFLT lineWidths[5];
	const char* text[5];

	optArray[0] = PL_LEGEND_LINE;
	textColors[0] = nUtils::enumPLplotColor_RED;
	lineColors[0] = nUtils::enumPLplotColor_RED;
	text[0] = "E11";
	lineStyles[0] = 1;
	lineWidths[0] = 1.0;

	optArray[1] = PL_LEGEND_LINE;
	textColors[1] = nUtils::enumPLplotColor_YELLOW;
	text[1] = "E12";
	lineColors[1] = nUtils::enumPLplotColor_YELLOW;
	lineStyles[1] = 1;
	lineWidths[1] = 1.0;

	optArray[2] = PL_LEGEND_LINE;
	textColors[2] = nUtils::enumPLplotColor_GREEN;
	lineColors[2] = nUtils::enumPLplotColor_GREEN;
	text[2] = "E13";
	lineStyles[2] = 1;
	lineWidths[2] = 1.0;

	optArray[3] = PL_LEGEND_LINE;
	textColors[3] = nUtils::enumPLplotColor_AQUAMARINE;
	text[3] = "E14";
	lineColors[3] = nUtils::enumPLplotColor_AQUAMARINE;
	lineStyles[3] = 1;
	lineWidths[3] = 1.0;

	optArray[4] = PL_LEGEND_LINE;
	textColors[4] = nUtils::enumPLplotColor_PINK;
	lineColors[4] = nUtils::enumPLplotColor_PINK;
	text[4] = "E115";
	lineStyles[4] = 1;
	lineWidths[4] = 1.0;

	PLFLT legendWidth, legendHeight;
	plottingStream.legend( &legendWidth, &legendHeight,
						   PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX,
						   PL_POSITION_RIGHT | PL_POSITION_TOP,
						   0.0, 0.0, 0.01,
						   15, 1, 1, 0, 0,
						   5, optArray,
						   0.5, 0.5, 1.0, 0.0,
						   textColors, (const char* const *) text,
						   NULL, NULL, NULL, NULL,
						   lineColors, lineStyles, lineWidths,
						   NULL, NULL, NULL, NULL );
	
	delete[] pX;
	delete[] pE11;
	delete[] pE12;
	delete[] pE13;
	delete[] pE14;
	delete[] pE15;
}

////////////////////////////////////////////////////////////////////////////////

void cFunctionFitNLS::PlotE16ThroughE19(const std::vector<double>* errors, plstream& plottingStream)
{
	PLFLT* pX = new PLFLT[errors[0].size()];
	PLFLT* pE16 = new PLFLT[errors[0].size()];
	PLFLT* pE17 = new PLFLT[errors[1].size()];
	PLFLT* pE18 = new PLFLT[errors[2].size()];
	PLFLT* pE19 = new PLFLT[errors[3].size()];

	PLFLT ymin = 1000000.0, ymax = -1000000.0;
	for( std::size_t i = 0; i < errors[0].size(); ++i )
	{
		pX[i] = i / 100.0;
		pE16[i] = errors[0][i];
		pE17[i] = errors[1][i];
		pE18[i] = errors[2][i];
		pE19[i] = errors[3][i];

		ymax = std::max(pE16[i], ymax);
		ymax = std::max(pE17[i], ymax);
		ymax = std::max(pE18[i], ymax);
		ymax = std::max(pE19[i], ymax);

		ymin = std::min(pE16[i], ymin);
		ymin = std::min(pE17[i], ymin);
		ymin = std::min(pE18[i], ymin);
		ymin = std::min(pE19[i], ymin);
	}


	// --- Generate Plot --- //

	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.env(0, errors[0].size() / 100.0, ymin*0.99, ymax*1.01, 0, 0);
	plottingStream.lab("Time (s)", "Error", "Electrodes 16-19");

	plottingStream.col0(nUtils::enumPLplotColor_RED);
	plottingStream.line(errors[0].size(), pX, pE16);

	plottingStream.col0(nUtils::enumPLplotColor_YELLOW);
	plottingStream.line(errors[0].size(), pX, pE17);

	plottingStream.col0(nUtils::enumPLplotColor_GREEN);
	plottingStream.line(errors[0].size(), pX, pE18);

	plottingStream.col0(nUtils::enumPLplotColor_AQUAMARINE);
	plottingStream.line(errors[0].size(), pX, pE19);


	// --- Create Legend --- //

	PLINT optArray[4];
	PLINT textColors[4];
	PLINT lineColors[4];
	PLINT lineStyles[4];
	PLFLT lineWidths[4];
	const char* text[4];

	optArray[0] = PL_LEGEND_LINE;
	textColors[0] = nUtils::enumPLplotColor_RED;
	lineColors[0] = nUtils::enumPLplotColor_RED;
	text[0] = "E16";
	lineStyles[0] = 1;
	lineWidths[0] = 1.0;

	optArray[1] = PL_LEGEND_LINE;
	textColors[1] = nUtils::enumPLplotColor_YELLOW;
	text[1] = "E17";
	lineColors[1] = nUtils::enumPLplotColor_YELLOW;
	lineStyles[1] = 1;
	lineWidths[1] = 1.0;

	optArray[2] = PL_LEGEND_LINE;
	textColors[2] = nUtils::enumPLplotColor_GREEN;
	lineColors[2] = nUtils::enumPLplotColor_GREEN;
	text[2] = "E18";
	lineStyles[2] = 1;
	lineWidths[2] = 1.0;

	optArray[3] = PL_LEGEND_LINE;
	textColors[3] = nUtils::enumPLplotColor_AQUAMARINE;
	text[3] = "E19";
	lineColors[3] = nUtils::enumPLplotColor_AQUAMARINE;
	lineStyles[3] = 1;
	lineWidths[3] = 1.0;

	PLFLT legendWidth, legendHeight;
	plottingStream.legend( &legendWidth, &legendHeight,
						   PL_LEGEND_BACKGROUND | PL_LEGEND_BOUNDING_BOX,
						   PL_POSITION_RIGHT | PL_POSITION_TOP,
						   0.0, 0.0, 0.01,
						   15, 1, 1, 0, 0,
						   4, optArray,
						   0.5, 0.5, 1.0, 0.0,
						   textColors, (const char* const *) text,
						   NULL, NULL, NULL, NULL,
						   lineColors, lineStyles, lineWidths,
						   NULL, NULL, NULL, NULL );
	
	delete[] pX;
	delete[] pE16;
	delete[] pE17;
	delete[] pE18;
	delete[] pE19;
}

