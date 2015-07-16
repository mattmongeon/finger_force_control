#include "real_time_plot.h"
#include "utils.h"
#include <iostream>


#ifdef WIN32
#define PLOT_DRIVER "wingcc"
#else
#define PLOT_DRIVER "xcairo"
#endif


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cRealTimePlot::cRealTimePlot(const std::string& title, const std::string& xAxisLabel,
							 const std::string& yAxisLabel,
							 const std::string& line1Legend, const std::string& line2Legend,
							 const std::string& line3Legend, const std::string& line4Legend,
							 PLFLT xAxisMax, PLFLT yAxisMin, PLFLT yAxisMax)
	: mPlotStream( 1, 1, 255, 255, 255, PLOT_DRIVER ),
	  mNumActivePlots( 1 )
{
	// --- Set Up Plotting --- //
	
	PLFLT ymin = yAxisMin, ymax = yAxisMax;
	PLFLT xmin = 0.0, xmax = xAxisMax, xjump_pct = 0.75;

	PLINT colbox = 1, collab = 3;

	// Line styles - solid
	PLINT styline[4];
	styline[0] = 1;
	styline[1] = 1;
	styline[2] = 1;
	styline[3] = 1;

	// Pen colors
	PLINT colline[4];
	colline[0] = nUtils::enumPLplotColor_RED;
	colline[1] = nUtils::enumPLplotColor_GREEN;
	colline[2] = nUtils::enumPLplotColor_BLUE;
	colline[3] = nUtils::enumPLplotColor_MAGENTA;
		
	if( !line2Legend.empty() )
		++mNumActivePlots;

	if( !line3Legend.empty() )
		++mNumActivePlots;

	if( !line4Legend.empty() )
		++mNumActivePlots;

	// Strings for plot legend
	const char* legline[4];
	legline[0] = line1Legend.c_str();
	legline[1] = line2Legend.c_str();
	legline[2] = line3Legend.c_str();
	legline[3] = line4Legend.c_str();

	PLFLT xlab = 0.0;
	PLFLT ylab = 1.0;

	bool autoy = true, acc = false;

	mPlotStream.init();

	mPlotStream.adv(0);
	mPlotStream.vsta();

	mPlotStream.stripc( &mPlotId, "bcnst", "bcnstv",
						xmin, xmax, xjump_pct, ymin, ymax,
						xlab, ylab,
						autoy, acc,
						colbox, collab,
						colline, styline, legline,
						xAxisLabel.c_str(), yAxisLabel.c_str(), title.c_str() );


	// --- Set Up Thread-Related Stuff --- //

	mContinuePlotting = true;
	mNewPointReady = false;
	mPlottingThread = boost::thread(ThreadFunc, this);
}

////////////////////////////////////////////////////////////////////////////////

cRealTimePlot::~cRealTimePlot()
{
	// It is possible that there is a point left to plot.  Wait until the other
	// thread has plotted it.
	while(mNewPointReady)
	{
		;
	}
	
	mContinuePlotting = false;
	mPlottingThread.join();

	std::cout << std::endl;
	std::cout << std::endl;
	#ifdef WIN32
	std::cout << "Showing data plot.  Close window to continue." << std::endl;
	#else
	std::cout << "Showing data plot.  Right-click plot to close and continue." << std::endl;
	#endif
	mPlotStream.stripd( mPlotId );
}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////

void cRealTimePlot::AddDataPoint(double pt)
{
	EnqueueDataPoint(pt, pt, pt, pt);
}

////////////////////////////////////////////////////////////////////////////////

void cRealTimePlot::AddDataPoint(double pt1, double pt2)
{
	EnqueueDataPoint(pt1, pt2, pt2, pt2);
}

////////////////////////////////////////////////////////////////////////////////
	
void cRealTimePlot::AddDataPoint(double pt1, double pt2, double pt3)
{
	EnqueueDataPoint(pt1, pt2, pt3, pt3);
}

////////////////////////////////////////////////////////////////////////////////

void cRealTimePlot::AddDataPoint(double pt1, double pt2, double pt3, double pt4)
{
	EnqueueDataPoint(pt1, pt2, pt3, pt4);
}

////////////////////////////////////////////////////////////////////////////////
//  Helper Functions
////////////////////////////////////////////////////////////////////////////////

void cRealTimePlot::EnqueueDataPoint(double p1, double p2, double p3, double p4)
{
	sDataPoint p;
	p.mPoint1 = p1;
	p.mPoint2 = p2;
	p.mPoint3 = p3;
	p.mPoint4 = p4;

	mDataMutex.lock();
	mNewPointReady = true;
	mBufferedPoint = p;
	mDataMutex.unlock();
}

////////////////////////////////////////////////////////////////////////////////

void cRealTimePlot::PlotPoints( const sDataPoint& p, int pointNum, int numPlots )
{
	switch(numPlots)
	{
	case 1:
		mPlotStream.stripa( mPlotId, 0, pointNum, p.mPoint1 );
		break;

	case 2:
		mPlotStream.stripa( mPlotId, 0, pointNum, p.mPoint1 );
		mPlotStream.stripa( mPlotId, 1, pointNum, p.mPoint2 );
		break;

	case 3:
		mPlotStream.stripa( mPlotId, 0, pointNum, p.mPoint1 );
		mPlotStream.stripa( mPlotId, 1, pointNum, p.mPoint2 );
		mPlotStream.stripa( mPlotId, 2, pointNum, p.mPoint3 );
		break;

	case 4:
		mPlotStream.stripa( mPlotId, 0, pointNum, p.mPoint1 );
		mPlotStream.stripa( mPlotId, 1, pointNum, p.mPoint2 );
		mPlotStream.stripa( mPlotId, 2, pointNum, p.mPoint3 );
		mPlotStream.stripa( mPlotId, 3, pointNum, p.mPoint4 );
		break;

	default:
		break;
	}
}


////////////////////////////////////////////////////////////////////////////////
//  Threaded Functions
////////////////////////////////////////////////////////////////////////////////

void cRealTimePlot::ThreadFunc(cRealTimePlot* pThis)
{
	sDataPoint p;
	bool plot = false;
	int pointNum = 0;
	while(pThis->mContinuePlotting)
	{
		// Try to grab the point.
		pThis->mDataMutex.lock();
		if( pThis->mNewPointReady )
		{
			p = pThis->mBufferedPoint;
			plot = true;
			pThis->mNewPointReady = false;
		}
		pThis->mDataMutex.unlock();

		// Plot it.
		if( plot )
		{
			pThis->PlotPoints( p, pointNum, pThis->mNumActivePlots );
			++pointNum;
			plot = false;
		}
	}
}
