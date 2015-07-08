#ifndef INCLUDED_FILE_PLOTTER_H
#define INCLUDED_FILE_PLOTTER_H


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


	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//
	
private:
	
	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	// The stream object used for plotting with PLplot.
	plstream mPlotStream;

};

#endif  // INCLUDED_FILE_PLOTTER_H

