#ifndef INCLUDED_READ_TIME_PLOT_H
#define INCLUDED_READ_TIME_PLOT_H


#include <plplot/plplot.h>
#include <plplot/plstream.h>
#include <pthread.h>
#include <string>


// Performs real-time plotting of up to four plots.  Performance becomes an issue as
// the number of plots increases.  Beware that at times the plotting may be too slow
// to keep up with the data being sent to it, so it might skip data points.
class cRealTimePlot
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//

	// Constructor.  Enables the plotter to accept and plot data.  Can plot 1-4 plots
	// simultaneously.  Number of plots is set by setting the names of the plots.
	// If the name is an empty string, that plot will not exist.
	//
	// Params:
	// title - The title to be displayed above the plot.
	// xAxisLabel - The label to be displayed along the x-axis.
	// yAxisLabel - The label to be displayed along the y-axis.
	// line1Legend - The string identifying plot 1.
	// line2Legend - The string identifying plot 2.  Set to empty string to disable.
	// line3Legend - The string identifying plot 3.  Set to empty string to disable.
	// line4Legend - The string identifying plot 4.  Set to empty string to disable.
	// xAxisMax - Specifies the width of the x-axis in generic units.
	cRealTimePlot(const std::string& title, const std::string& xAxisLabel, const std::string& yAxisLabel,
				  const std::string& line1Legend, const std::string& line2Legend = "",
				  const std::string& line3Legend = "", const std::string& line4Legend = "",
				  PLFLT xAxisMax = 2500.0);

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
	//-----------------------------  NESTED STRUCT  ----------------------------//
	//--------------------------------------------------------------------------//

	// Contains the data to be plotted as the next data point.
	struct sDataPoint
	{
		PLFLT mPoint1;
		PLFLT mPoint2;
		PLFLT mPoint3;
		PLFLT mPoint4;
	};


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

	// Does the work of plotting a data point.  Takes the number of data points into
	// account to try to make plotting more efficient when dealing with fewer plots.
	//
	// Params:
	// p - The data point to be plotted.
	// pointNum - Identifies the x-coordinate of the point.
	// numPlots - The number of plots being tracked in this plot.
	void PlotPoints( const sDataPoint& p, int pointNum, int numPlots );

	
	//--------------------------------------------------------------------------//
	//--------------------------  THREADED FUNCTIONS  --------------------------//
	//--------------------------------------------------------------------------//

	// The threaded function that does the plotting.  The parameter is expted to
	// be a pointer to the instance of this class.
	static void* ThreadFunc(void* pIn);


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

	// The data point waiting to be plotted.
	sDataPoint mBufferedPoint;

	// A flag indicating that a new data point is ready to be plotted.
	bool mNewPointReady;

	// A boolean used to control whether the separate thread should continue plotting.
	bool mContinuePlotting;
};


#endif  // INCLUDED_READ_TIME_PLOT_H

