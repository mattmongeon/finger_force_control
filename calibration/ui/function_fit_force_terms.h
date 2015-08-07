#ifndef INCLUDED_FUNCTION_FIT_FORCE_TERMS_H
#define INCLUDED_FUNCTION_FIT_FORCE_TERMS_H


#include "function_fit_nls.h"
#include "electrode_tdc_compensator.h"
#include "biotac_force_curve.h"
#include <cmath>


class cFunctionFitForceTerms
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//

	cFunctionFitForceTerms(const std::vector<cElectrodeTdcCompensator>& compensators);

	~cFunctionFitForceTerms();


	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//

    cBioTacForceCurve TrainAgainstDataFiles(const std::vector<std::string>& files);

    void TestAgainstDataFiles(const std::vector<std::string>& files, const cBioTacForceCurve& curve);
	
	
private:

	//--------------------------------------------------------------------------//
	//---------------------------  HELPER FUNCTIONS  ---------------------------//
	//--------------------------------------------------------------------------//

	// Takes in the reference (groundtruth) data and calibration curve output and
	// plots them along with the error.
	//
	// Parameters:
	// calculated - The output of the calibration curve after passing in BioTac data.
	// actual - Reference groundtruth value for comparison with calibration curve results.
	void PlotResults(const std::vector<float>& calculated, const std::vector<float>& actual );
	
	
	//--------------------------------------------------------------------------//
	//----------------------------  CERES CLASSES  -----------------------------//
	//--------------------------------------------------------------------------//

	class cTrainingResidual
	{
	public:
		cTrainingResidual(const std::vector<uint16_t>& electrodes,
						  uint16_t tdc, uint16_t tac, double force,
						  const std::vector<cElectrodeTdcCompensator>* const pCompensators)
			: mpCompensators(pCompensators), mElectrodes(electrodes), mTDC(tdc), mTAC(tac), mForce(force)
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

		template<typename T> bool operator()( const T* const s1,
											  const T* const s2,
											  const T* const s3,
											  T* residual ) const
			{
				double x(0.0), y(0.0), z(0.0);
				for( std::size_t i = 0; i < 19; ++i )
				{
					x += mElectrodeNormalVectors[i][0] * (*mpCompensators)[i].CompensateTdc(mTDC, mTAC, mElectrodes[i]);
					y += mElectrodeNormalVectors[i][1] * (*mpCompensators)[i].CompensateTdc(mTDC, mTAC, mElectrodes[i]);
					z += mElectrodeNormalVectors[i][2] * (*mpCompensators)[i].CompensateTdc(mTDC, mTAC, mElectrodes[i]);
				}

				T result = T(sqrt(s1[0]*s1[0]*x*x + s2[0]*s2[0]*y*y + s3[0]*s3[0]*z*z));

				residual[0] = T(mForce) - result;

				return true;
			}

	private:
		const std::vector<cElectrodeTdcCompensator>* const mpCompensators;
		double mElectrodeNormalVectors[19][3];
		std::vector<uint16_t> mElectrodes;
		uint16_t mTDC;
		uint16_t mTAC;
		double mForce;
	};

	
	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	std::vector<cElectrodeTdcCompensator> mCompensators;
	double mElectrodeNormalVectors[19][3];
	
};


#endif  // INCLUDED_FUNCTION_FIT_FORCE_TERMS_H

