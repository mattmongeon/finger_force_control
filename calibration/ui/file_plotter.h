#ifndef INCLUDED_FILE_PLOTTER_H
#define INCLUDED_FILE_PLOTTER_H


#include "../common/biotac_comm.h"
#include <vector>
#include <string>
#include <plplot/plplot.h>
#include <plplot/plstream.h>


class cFilePlotter
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//

	// Constructor.  Opens the parameter file and plots its contents.
	explicit cFilePlotter(const std::string& file);

	// Destructor.  Cleans up the plot.
	~cFilePlotter();


private:

	//--------------------------------------------------------------------------//
	//---------------------------  HELPER FUNCTIONS  ---------------------------//
	//--------------------------------------------------------------------------//

	void ConfigureAndPlotForce(const std::vector<biotac_tune_data>& data, const std::string& fileName);

	void ConfigureAndPlotPressTemp(const std::vector<biotac_tune_data>& data, const std::string& fileName);

	void ConfigureAndPlotElectrodes1(const std::vector<biotac_tune_data>& data, const std::string& fileName);

	void ConfigureAndPlotElectrodes2(const std::vector<biotac_tune_data>& data, const std::string& fileName);
	
	
	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	// The stream object used for plotting with PLplot.
	plstream mPlottingStream;

};

#endif  // INCLUDED_FILE_PLOTTER_H

