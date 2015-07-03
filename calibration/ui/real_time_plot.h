#ifndef INCLUDED_READ_TIME_PLOT_H
#define INCLUDED_READ_TIME_PLOT_H


#include <plplot/plplot.h>
#include <plplot/plstream.h>
#include <pthread.h>
#include <deque>
#include <string>


class cRealTimePlot
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//

	// Constructor.  Enables the plotter to accept and plot data.
	cRealTimePlot(const std::string& title, const std::string& xAxisLabel, const std::string& yAxisLabel,
				  const std::string& line1Legend, const std::string& line2Legend = "",
				  const std::string& line3Legend = "", const std::string& line4Legend = "");

	// Desturctor.  Stops plotting and cleans up.
	~cRealTimePlot();


	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//

	// Adds a data point to be plotted when only a single point is being plotted.
	//
	// Params:
	// pt - The data point to plot.
	void AddDataPoint(double pt);

	// Adds two values to be plotted as the next data point.  Assumes only two
	// data points are being plotted for this plot.
	//
	// Params:
	// pt1 - The next data point for the first plot line.
	// pt2 - The next data point for the second plot line.
	void AddDataPoint(double pt1, double pt2);
	
	// Adds three values to be plotted as the next data point.  Assumes only three
	// data points are being plotted for this plot.
	//
	// Params:
	// pt1 - The next data point for the first plot line.
	// pt2 - The next data point for the second plot line.
	// pt3 - The next data point for the third plot line.
	void AddDataPoint(double pt1, double pt2, double pt3);

	// Adds four values to be plotted as the next data point.  Assumes only four
	// data points are being plotted for this plot.
	//
	// Params:
	// pt1 - The next data point for the first plot line.
	// pt2 - The next data point for the second plot line.
	// pt3 - The next data point for the third plot line.
	// pt4 - The next data point for the fourth plot line.
	void AddDataPoint(double pt1, double pt2, double pt3, double pt4);

	
private:

	//--------------------------------------------------------------------------//
	//---------------------------  HELPER FUNCTIONS  ---------------------------//
	//--------------------------------------------------------------------------//

	// Does the work of actually adding a data point to the data queue for plotting.
	//
	// Params:
	// p1 - The data point for the first plot line.
	// p2 - The data point for the second plot line.
	// p3 - The data point for the third plot line.
	// p4 - The data point for the fourth plot line.
	void EnqueueDataPoint(double p1, double p2, double p3, double p4);
	
	
	//--------------------------------------------------------------------------//
	//--------------------------  THREADED FUNCTIONS  --------------------------//
	//--------------------------------------------------------------------------//

	// The threaded function that does the plotting.  The parameter is expted to
	// be a pointer to the instance of this class.
	static void* ThreadFunc(void* pIn);


	//--------------------------------------------------------------------------//
	//-----------------------------  NESTED STRUCT  ----------------------------//
	//--------------------------------------------------------------------------//

	struct sDataPoint
	{
		PLFLT mPoint1;
		PLFLT mPoint2;
		PLFLT mPoint3;
		PLFLT mPoint4;
	};
	

	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	// The stream object used for plotting with PLplot.
	plstream mPlotStream;

	// The identifier for the this plot.
	PLINT mPlotId;

	// Number of active plots.  Maximum of 4.  Will affect how everything is drawn.
	int mNumActivePlots;

	// The handle to the thread that will be doing the plotting in real-time.
	pthread_t mPlottingThreadHandle;

	// Mutex protecting access to the data queue.
	pthread_mutex_t mDataMutex;

	// A list of data points waiting to be plotted.
	std::deque<sDataPoint> mBufferedPoints;

	// A boolean used to control whether the separate thread should continue plotting.
	bool mContinuePlotting;
};


#endif  // INCLUDED_READ_TIME_PLOT_H

