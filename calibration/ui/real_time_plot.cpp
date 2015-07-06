#include "real_time_plot.h"
#include "utils.h"
#include <iostream>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cRealTimePlot::cRealTimePlot(const std::string& title, const std::string& xAxisLabel,
							 const std::string& yAxisLabel,
							 const std::string& line1Legend, const std::string& line2Legend,
							 const std::string& line3Legend, const std::string& line4Legend)
	: mPlotStream( 1, 1, 255, 255, 255, "xcairo" ),
	  mNumActivePlots( 1 )
{
	// --- Set Up Plotting --- //
	
	PLFLT ymin = 1000000.0, ymax = 1.0;
	PLFLT xmin = 0.0, xmax = 500.0, xjump_pct = 0.5;

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

	bool autoy = true, acc = true;

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
	mBufferedPoints.clear();
	pthread_mutex_init(&mDataMutex, NULL);
	pthread_create(&mPlottingThreadHandle, NULL, ThreadFunc, (void*)this);
}

////////////////////////////////////////////////////////////////////////////////

cRealTimePlot::~cRealTimePlot()
{
	// It is possible that there are some points left to plot.  Wait until
	// they are all plotted by the other thread.
	while(!mBufferedPoints.empty())
	{
		;
	}
	
	mContinuePlotting = false;
	pthread_join(mPlottingThreadHandle, NULL);
	pthread_mutex_destroy(&mDataMutex);

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "Showing data plot.  Right-click plot to close and continue." << std::endl;
	mPlotStream.stripd( mPlotId );
}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
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

	pthread_mutex_lock(&mDataMutex);
	mBufferedPoints.push_back(p);
	pthread_mutex_unlock(&mDataMutex);
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

void* cRealTimePlot::ThreadFunc(void* pIn)
{
	cRealTimePlot* pThis = reinterpret_cast<cRealTimePlot*>(pIn);

	sDataPoint p;
	bool plot = false;
	int pointNum = 0;
	while(pThis->mContinuePlotting)
	{
		// Try to grab the point.
		pthread_mutex_lock(&(pThis->mDataMutex));
		if( !pThis->mBufferedPoints.empty() )
		{
			p = pThis->mBufferedPoints.front();
			pThis->mBufferedPoints.pop_front();
			plot = true;
		}
		pthread_mutex_unlock(&(pThis->mDataMutex));

		// Plot it.
		if( plot )
		{
			pThis->PlotPoints( p, pointNum, pThis->mNumActivePlots );
			++pointNum;
			plot = false;
		}
	}

	return NULL;
}
